#ifndef UCAL2_DEVICE_DEVICE_H
#define UCAL2_DEVICE_DEVICE_H

#include "common/measures.h"
#include "common/keys.h"
#include "better_enums/enum.h"
#include "storage/frame.h"
#include "timer.h"
#include <stdint.h>
#include <vector>
#include <map>
#include <string>


namespace device {
    using VoltageProfile = std::vector<common::MilliVolt>;
    using ProfileSetup = std::map<common::ControlKey, VoltageProfile>;
    BETTER_ENUM(DeviceState, char, CanSet, Prepared, Running);
    constexpr static const char* DEFAULT_DAQBOARD_NAME = "DaqBoard3001USB";
    constexpr double DEFAULT_DAQBOARD_SAMPLING_RATE_HZ = 50;
    constexpr uint32_t DEFAULT_DAQBOARD_BUFFER_SIZE_PER_HZ = 100;
    constexpr double VABSMAX = 10000.; //measured in mV
    constexpr uint32_t COUNTSMAX = 65535;
    constexpr size_t DAQBOARD_SIGNALS_NUMBER = 5;
    constexpr size_t DAQBOARD_CONTROLS_NUMBER = 2;

    /*!
    Represents interface to hardware. Hardware may have 3 states: 
    - CanSet (configuration changes can be made)
    - Prepared (configuration is validated and hardware is ready to start) 
    - Running (hardware is taking data)
    Supported configuration features are determined by "setSomething" methods.
    */
    class IDevice {
    public:
        virtual void setProfiles(ProfileSetup const& profiles, common::TimeUnit profile_length) = 0;

        virtual void setDuration(common::TimeUnit total_duration) = 0;

        virtual void setReadingSampling(common::TimeUnit step, uint16_t per_points = 1) = 0;

        virtual void setTimer(DeviceTimer timer) = 0;

        virtual std::string getSetup() const = 0;

        virtual void prepare() = 0;

        virtual void run() = 0;

        virtual void stop() = 0;

        virtual DeviceState getState() const = 0;

        virtual storage::Frame getData() = 0;

        virtual ~IDevice() = default;
    };

    /*!
    Fake device to use in tests in Linux builds.
    */
    class MockDevice : public IDevice {
    public:
        MockDevice(std::string name = DEFAULT_DAQBOARD_NAME);

        void setProfiles(ProfileSetup const& profiles, common::TimeUnit profile_length) override;

        void setDuration(common::TimeUnit total_duration) override;

        void setReadingSampling(common::TimeUnit step, uint16_t per_points = 1) override;

        void setTimer(DeviceTimer timer) override;

        std::string getSetup() const override;

        void prepare() override;

        void run() override;

        void stop() override;

        DeviceState getState() const override;

        storage::Frame getData() override;

        virtual ~MockDevice() = default;

    private:
        mutable DeviceState state_ = DeviceState::CanSet;
        mutable DeviceTimer timer_;
        double reading_sampling_ = 1;
        uint32_t returned_points_ = 0;

        void checkState(DeviceState required);

        void checkDue() const;
    };

    /*!
    Device for Daqboard3001USB hardware. Uses DAQX api.
    */
    class DaqboardDevice : public IDevice {
    public:
        DaqboardDevice(std::string name = DEFAULT_DAQBOARD_NAME);

        void setProfiles(ProfileSetup const&, common::TimeUnit profile_length) override;

        void setDuration(common::TimeUnit total_duration) override;

        void setReadingSampling(common::TimeUnit step, uint16_t per_points = 1) override;

        void setTimer(DeviceTimer timer) override;

        std::string getSetup() const override;

        void prepare() override;

        void run() override;

        void stop() override;

        virtual DeviceState getState() const override;

        storage::Frame getData() override;

        virtual ~DaqboardDevice();

    private:
        int handle_; //DaqHandleT is typedef int actually
        DeviceTimer timer_;
        mutable DeviceState state_ = DeviceState::CanSet;
        std::map<common::ControlKey, std::vector<unsigned short>> profiles_ = {};
        common::TimeUnit profile_length_ = 0;
        common::TimeUnit total_duration_ = 0;
        double sampling_frequency_hz_ = DEFAULT_DAQBOARD_SAMPLING_RATE_HZ;

        uint32_t toCount(common::MilliVolt v) const;

        common::MilliVolt fromCount(uint32_t c) const;

        void prepareDeviceRead();

        void prepareDeviceWrite();

        uint32_t getSignalChannelId(common::SignalKey key) const;

        uint32_t getControlChannelId(common::ControlKey key) const;

        void prepareChannelWrite(common::ControlKey key);

        void startDevice();

        void stopDevice();

        void setChannelsToZero();

        void checkState(std::vector<DeviceState> allowed_states);

        uint32_t getBufferSizePerHz() const;
    };

}
#endif//UCAL2_DEVICE_DEVICE_H
