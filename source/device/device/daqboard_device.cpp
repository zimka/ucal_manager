#include "device.h"
#define WIN32_LEAN_AND_MEAN
#include "common/exceptions.h"
#include "storage/signal.h"
#include <windows.h>
#include "daqx/Daqx.h"
#include "json/single_include/nlohmann/json.hpp"
#include <string>

using namespace device;
using nlohmann::json;

//====================PUBLIC====================
std::unique_ptr<IDevice> device::acquireDevice(std::string name) {
    if (!name.size()) {
        return std::make_unique<DaqboardDevice>();
    }
    else {
        return std::make_unique<DaqboardDevice>(name);
    }
}

DaqboardDevice::DaqboardDevice(std::string name) : timer_(DEFAULT_DAQBOARD_TIMER_STEP_TU) {
    daqSetDefaultErrorHandler(0); // no error handling from DaqX, handle them manually
    handle_ = daqOpen(const_cast<char *>(name.c_str()));
    if (handle_ == -1) {
        throw common::DeviceError("Can't open Daqboard device with name:" + name);
    }
    setChannelsToZero();
};

DaqboardDevice::~DaqboardDevice() {
    stopDevice();
}

void DaqboardDevice::setProfiles(const ProfileSetup& profiles, common::TimeUnit profile_length) {
    checkState({DeviceState::CanSet});
    auto keys = {common::ControlKey::C0, common::ControlKey::C1};
    if (profiles.size() > 1) {
        // Check that profiles size is the same
        if (profiles.at(common::ControlKey::C0).size() != profiles.at(common::ControlKey::C1).size()) {
            throw common::DeviceError("All channel profiles must have same size");
        }
    }
    // Map MilliVolts(-10; 10) to Counts(0, 65K)
    for (auto pair : profiles) {
        std::vector<WORD> buffer;
        for (auto v : pair.second) {
            buffer.push_back(toCount(v));
        }
        profiles_[pair.first] = buffer;
    }
    profile_length_s_ = timer_.unitsToMilliseconds(profile_length) / 1000;
}

void DaqboardDevice::setDuration(common::TimeUnit total_duration) {
    checkState({DeviceState::CanSet});
    total_duration_s_ = timer_.unitsToMilliseconds(total_duration)/1000;
}

void DaqboardDevice::setTimer(DeviceTimer timer) {
    checkState({DeviceState::CanSet});
    timer_ = timer;
}

void DaqboardDevice::setReadingSampling(common::TimeUnit step, uint16_t per_points) {
    auto freq_hz = double(per_points) / (timer_.unitsToMilliseconds(step) / 1000);
    if (freq_hz == 0) {
        throw common::DeviceError("Reading sampling rate must be higher than 0");
    }
    sampling_frequency_hz_ = freq_hz;
}

std::string DaqboardDevice::getSetup() const {
    std::map < std::string, std::string > setup = {
        {"sampling_rate_hz", std::to_string(sampling_frequency_hz_)},
        {"profile_length_s", std::to_string(profile_length_s_) },
        {"total_duration_s", std::to_string(total_duration_s_)}
    };
    // TODO: if VoltageProfile and SignalData became the same, next code would be much simpler
    for (auto key : common::ControlKey::_values()) {
        if (profiles_.count(key)) {
            std::string repr = "";
            for (auto v : profiles_.at(key)) {
                repr += std::to_string(v);
                repr += " ";
            }
            std::string setup_key = "profile_";
            setup_key += key._to_string();
            setup[setup_key] = repr;
        }
    }
    return json(setup).dump();
}

void DaqboardDevice::prepare() {
    checkState({DeviceState::CanSet});
    // TODO: must use time units to sec, not 1000
    timer_.setStep(timer_.millisecondsToUnits(1000/sampling_frequency_hz_));
    prepareDeviceRead();
    prepareDeviceWrite();
    state_ = DeviceState::Prepared;
}

void DaqboardDevice::run() {
    checkState({DeviceState::Prepared});
    int16_t err_code = startDevice();
    if (err_code < 0) {
        throw common::DeviceError("Read configuration Daqboard Error; code: " + std::to_string(-err_code));
    }
    if (err_code > 0) {
        throw common::DeviceError("Write configuration Daqboard Error; code: " + std::to_string(err_code));
    }

    timer_.run();
    state_ = DeviceState::Running;
}

void DaqboardDevice::stop() {
    checkState({DeviceState::Prepared, DeviceState::Running});
    stopDevice();
    timer_.stop();
    state_ = DeviceState::CanSet;
}

DeviceState DaqboardDevice::getState() const {
    return state_;
}

storage::Frame DaqboardDevice::getData() {
    checkState({DeviceState::Running});
    DWORD active;
    DWORD ready_scans_number;
    DWORD get_scans_number;
    daqAdcTransferGetStat(handle_, &active, &ready_scans_number);
    auto frame = storage::Frame();
    // check if DaqBoard keeps acquiring data
    bool is_running = active & DaafAcqActive;
    if (ready_scans_number) {
        frame.setTs(timer_.getStamp());
        std::vector<uint16_t> buffer(2 * ready_scans_number * DAQBOARD_SIGNALS_NUMBER);
        // Get all data in counts
        daqAdcTransferBufData(handle_, (PWORD)(buffer.data()), ready_scans_number, DabtmRetAvail, &get_scans_number);
        std::vector<storage::SignalValue> buffer_signal;
        // Map counts to MilliVolts
        for (auto i = 0; i < get_scans_number * DAQBOARD_SIGNALS_NUMBER; i++) {
            buffer_signal.push_back(fromCount(buffer[i]));
        }
        // Parse buffer from [Ua0, Ub0, Uc0, Ua1, Ub1, Uc1...] to Ua[], Ub[], Uc[]
        for (auto key : common::SignalKey::_values()) {
            if (key != +common::SignalKey::Undefined) {
                frame[key] = storage::SignalData(
                    buffer_signal.data() + getSignalChannelId(key),
                    get_scans_number,
                    DAQBOARD_SIGNALS_NUMBER
                );
            }
        }
    }
    // Daqboard can stop itself if all scans are taken
    if ((ready_scans_number) && (!is_running)) {
        // It occurs only when total_duration_ is non zero (for finite scan)
        if (!total_duration_s_) {
            throw common::AssertionError("Device is not running and has data, but does not have total_duration");
        }
        // We gave all the data and now can switch state to CanSet
        stop();
    }
    return frame;
}
//====================PRIVATE====================

uint32_t DaqboardDevice::getBufferSizePerHz() const {
    return DEFAULT_DAQBOARD_BUFFER_SIZE_PER_HZ;
}

void DaqboardDevice::checkState(std::vector<DeviceState> allowed_states) {
    for (auto as : allowed_states) {
        if (state_ == as)
            return;
    }
    // TODO: make exception message less cryptic
    throw common::DeviceError("Action can't be run from the current state");
}

uint32_t device::DaqboardDevice::toCount(common::MilliVolt v) const {
    bool correct = (v >= -VABSMAX) && (v <= VABSMAX);
    if (!correct) {
        throw common::DeviceError(
            "Voltage level " + std::to_string(v) + " is out of range "
        );
    }
    return COUNTSMAX / 2 * ((double(v) / VABSMAX) + 1);
}

common::MilliVolt device::DaqboardDevice::fromCount(uint32_t c) const {
    bool correct = (c >= 0) && (c <= COUNTSMAX);
    if (!correct)
        throw common::DeviceError("Voltage counts " + std::to_string(c) + " is out of range [0, 65535]");
    return common::MilliVolt(VABSMAX * ((2 * double(c) / COUNTSMAX) - 1));
}

void DaqboardDevice::prepareDeviceRead() {
    daqAdcSetClockSource(handle_, DacsAdcClock);
    // 1. Configuring Channels - What Type of Channels? How Many?
    std::vector<DaqAdcGain> daq_setscan_gains;
    std::vector<uint32_t> daq_setscan_flags;
    std::vector<uint32_t> daq_setscan_channels;
    for (uint32_t i = 0; i != DAQBOARD_SIGNALS_NUMBER; i++) {
        daq_setscan_channels.push_back(i);
        daq_setscan_gains.push_back(DgainX1);
        daq_setscan_flags.push_back(DafBipolar);
    }
    daqAdcSetScan(handle_,
        (PDWORD(&daq_setscan_channels[0])),
        &(daq_setscan_gains[0]),
        (PDWORD(&daq_setscan_flags[0])),
        DAQBOARD_SIGNALS_NUMBER
    );
    //2. Configuring Acquisition Events - How Should the Acquisition Start and Stop? (stop is actually configured in set_**_voltage)
    daqSetTriggerEvent(handle_, DatsSoftware, (DaqEnhTrigSensT)NULL, 0, (DaqAdcGain)0, 0, DaqTypeAnalogLocal, 0.0, 0.0,
        DaqStartEvent);
    if (total_duration_s_ > 0) {
        // Configuring stop event at N scans
        auto scans_number = total_duration_s_ * sampling_frequency_hz_;
        daqAdcSetAcq(handle_, DaamNShot, 0, scans_number);
        daqSetTriggerEvent(handle_, DatsScanCount, (DaqEnhTrigSensT)NULL, 0, (DaqAdcGain)0, 0, DaqTypeAnalogLocal, 0.0,
            0.0, DaqStopEvent);
    }
    else {
        daqSetTriggerEvent(handle_, DatsSoftware, (DaqEnhTrigSensT)NULL, 0, (DaqAdcGain)0, 0, DaqTypeAnalogLocal, 0.0, 0.0,
            DaqStopEvent);
    }
    //3. Setting the Acquisition Rate - How Fast Should the Channels be Scanned?
    float real_scan_freq = 0;
    DaqError err = daqAdcSetRate(handle_, DarmFrequency, DaasPostTrig, sampling_frequency_hz_, &real_scan_freq);
    if (err) {
        throw common::DeviceError("Daqboard Error setting reading rate, error code: " + std::to_string(err));
    }
    //4. Setting up the Buffer Model - How Should the Data be Stored?
    auto inner_buffer_size = (unsigned int)(sampling_frequency_hz_ * DAQBOARD_SIGNALS_NUMBER * getBufferSizePerHz());
    daqAdcTransferSetBuffer(handle_, NULL, inner_buffer_size, DatmUpdateBlock | DatmDriverBuf);
    //5.Arming the Acquisition and Starting the Transfer
    daqAdcTransferStart(handle_);
    err = daqAdcArm(handle_);
    if (err) {
        throw common::DeviceError("Daqboard Error arming ADC, error code: " + std::to_string(err));
    }
}

void DaqboardDevice::prepareChannelWrite(common::ControlKey key) {
    /*!
 *  Data non-const outputting process for iotech daqboard follows next pipeline
 *  1. Configure all the output channels to be included in the waveform/pattern output as static output mode DdomStaticWave via the daqDacSetOutputMode function.
 *  2. Set the waveform/pattern output clock source for each output channel via the daqDacWaveSetClockSource function.
 *  3. If the selected clock source uses the internal waveform/pattern output pacer clock, then set the output update frequency using the daqDacWaveSetFreq function.
 *  4. Set the trigger event which will initiate the waveform/pattern output via the daqDacWaveSetTrig
 *  5. Configure the update mode and waveform/pattern output termination conditions using the daqDacWaveSetMode
 *  6. Though it is not required, it is also recommended that the transfer buffer is allocated and the transfer is enabled prior to invoking this function(daqDacWaveSetBuffer and the daqDacTransferStart functions)
 *  7. define PredefWave
 *  8. Call daqDacWaveArm()
 *  9. Terminate by daq daqDacWaveDisarm
 */
    auto& buffer = profiles_.at(key);
    if (!buffer.size()) {
        throw common::DeviceError("Empty control channel was set to device - it is forbidden!");
    }
    WORD channel_number = getControlChannelId(key);
    // 1.
    daqDacSetOutputMode(handle_, DddtLocal, channel_number, DdomStaticWave);
    //2.
    daqDacWaveSetClockSource(handle_, DddtLocal, channel_number, DdcsDacClock); //DdcsDacClock
    //3.
    if (profile_length_s_ == 0) {
        throw common::DeviceError("Profile length must be greater than zero");
    }
    float dac_frequency_hz_target = float(buffer.size()) / profile_length_s_;
    daqDacWaveSetFreq(handle_, DddtLocal, channel_number, dac_frequency_hz_target);
    float dac_frequency_hz_result = 0;
    daqDacWaveGetFreq(handle_, DddtLocal, channel_number, &dac_frequency_hz_result);
    float diff = dac_frequency_hz_result - dac_frequency_hz_target;
    if (diff < 0) {
        diff = -diff;
    }
    if (diff / dac_frequency_hz_target > 0.001) {
        std::string msg = "Can't assign freq " + std::to_string(dac_frequency_hz_target);
        msg += "; probably frequency is too low";
        throw common::DeviceError(msg);
    }
    //4.
    bool ignored_param = true; // or false, doesn't matter, blame DaqBoard API
    daqDacWaveSetTrig(handle_, DddtLocal, channel_number, DdtsSoftware, ignored_param);
    //5.
    if (total_duration_s_ > 0) {
        DWORD update_count = dac_frequency_hz_result * total_duration_s_ + 1;
        daqDacWaveSetMode(handle_, DddtLocal, channel_number, DdwmNShot, update_count);
    }
    else {
        daqDacWaveSetMode(handle_, DddtLocal, channel_number, DdwmInfinite, NULL);
    }
    //6. daqDacTransferStart?
    daqDacWaveSetBuffer(handle_, DddtLocal, channel_number, buffer.data(), buffer.size(), DdtmUserBuffer/*DdtmCycleOn*/);//??
    //7.	
    DaqError err = daqDacWaveSetUserWave(handle_, DddtLocal, channel_number);
    if (err) {
        throw common::DeviceError(
            "Error configuring control channel " + std::to_string(channel_number) + ", error code: " + std::to_string(err)
        );
    }
    daqDacTransferStart(handle_, DddtLocal, channel_number);
}

void DaqboardDevice::prepareDeviceWrite() {
    bool at_least_one = false;
    for (auto k : common::ControlKey::_values()) {
        if (profiles_.count(k)) {
            prepareChannelWrite(k);
            at_least_one = true;
        }
    }
    if (at_least_one) {
        DaqError err = daqDacWaveArm(handle_, DddtLocal);
        if (err) {
            throw common::DeviceError("Error arming DAC, error code: " + std::to_string(err));
        }
    }
}

uint32_t device::DaqboardDevice::getSignalChannelId(common::SignalKey key) const {
    if (key == +common::SignalKey::Undefined) {
        throw common::AssertionError("'Undefined' SignalKey does not have a channel");
    }
    static const std::map<common::SignalKey, uint8_t> signal_ids = {
        {common::SignalKey::S0, 0},
        {common::SignalKey::S1, 1},
        {common::SignalKey::S2, 2},
        {common::SignalKey::S3, 3},
        {common::SignalKey::S4, 4},
    };
    if (signal_ids.size() != DAQBOARD_SIGNALS_NUMBER) {
        throw common::AssertionError("DaqboardDevice SignalKey map is incomplete");
    }
    return signal_ids.at(key);
}

uint32_t device::DaqboardDevice::getControlChannelId(common::ControlKey key) const {
    if (key == +common::ControlKey::Undefined) {
        throw common::AssertionError("'Undefined' ControlKey does not have a channel");
    }
    static const std::map<common::ControlKey, uint32_t> control_ids = {
        {common::ControlKey::C0, 0},
        {common::ControlKey::C1, 1},
    };
    if (control_ids.size() != DAQBOARD_CONTROLS_NUMBER) {
        throw common::AssertionError("DaqboardDevice ControlKey map is incomplete");
    }
    return control_ids.at(key);
}

int16_t device::DaqboardDevice::startDevice() noexcept {
    // To avoid conditional api execution at first all commands
    // are run, then if anything went wrong device is stopped
    int16_t err_code = 0;
    // execute all commands
    DaqError err_read = DerrNoError;
    DaqError err_write_0 = DerrNoError;
    DaqError err_write_1 = DerrNoError;

    uint32_t channel0;
    uint32_t channel1; 

    try{
        channel0 = getControlChannelId(common::ControlKey::C0);
        channel1 = getControlChannelId(common::ControlKey::C1);
    }
    catch (std::runtime_error){
        // to reduce lag between read and write we have to check it here
        // method is marked as noexcept
        return -1000;
    }
    {
        err_read = daqAdcSoftTrig(handle_);
        if (profiles_.size()){
            err_write_0 = daqDacWaveSoftTrig(
                handle_, DddtLocal, channel0
            );
            err_write_1 = daqDacWaveSoftTrig(
                handle_, DddtLocal, channel1
            );
        }
    }
    // check all results, if any is nonzero - save it;
    // priority rises from read write to read errors
    if (err_write_1 != DerrNoError){
        err_code = err_write_1;
    }
    if (err_write_0 != DerrNoError) {
        err_code = err_write_0;
    }
    if (err_read != DerrNoError){
        err_code = -err_read;
    }
  
    if (err_code != 0) {
        // turn off everything
        stopDevice();
    }
    return err_code;
}

void device::DaqboardDevice::stopDevice() noexcept {
    daqDacWaveDisarm(handle_, DddtLocal);
    daqAdcTransferStop(handle_);
    daqAdcDisarm(handle_);
    setChannelsToZero();
}

void device::DaqboardDevice::setChannelsToZero() noexcept {
    for (auto v : common::ControlKey::_values()) {
        if (v != +common::ControlKey::Undefined) {
            auto id = getControlChannelId(v);
            daqDacSetOutputMode(handle_, DddtLocal, id, DdomVoltage);
            daqDacWt(handle_, DddtLocal, id, toCount(0));
        }
    }
}
