#include "device.h"
#include "common/exceptions.h"
#include "storage/signal.h"

using namespace device;

MockDevice::MockDevice() :timer_(1) {};

void MockDevice::setProfiles(const ProfileSetup&, common::TimeUnit profile_length) {
	checkState(DeviceState::CanSet);
	//MockDevice always returns constant voltage 1 in all channels
}

void MockDevice::setDuration(common::TimeUnit total_duration) {
	checkState(DeviceState::CanSet);
	timer_.setOverdue(total_duration);
}

void MockDevice::setReadingSampling(common::TimeUnit step, uint16_t per_points) {
	checkState(DeviceState::CanSet);

	if (per_points != 1) {
		throw common::AssertionError("Mock device only supports integer sampling, use per_points = 1");
	}
	if (step <= 0) {
		throw common::ValueError("Time step must be greater than zero");
	}
	reading_sampling_ = step;
}

std::string MockDevice::getSetup() const {
	return std::string();
}

void MockDevice::prepare() {
	checkState(DeviceState::CanSet);
	timer_.setStep(common::TimeUnit(reading_sampling_));
	state_ = DeviceState::Prepared;
}

void MockDevice::run() {
	checkState(DeviceState::Prepared);
	state_ = DeviceState::Running;
	timer_.run();
}

void MockDevice::stop() {
	state_ = DeviceState::CanSet;
	timer_.stop();
}

DeviceState MockDevice::getState() const {
	checkDue();
	return state_;
}

storage::Frame MockDevice::getData() {
	checkDue();
	common::TimeStamp ts = timer_.getStamp();
	uint32_t available_points = uint32_t(ts.value() / reading_sampling_);
	if (available_points <= returned_points_) {
		return storage::Frame(ts);
	}
	std::vector<storage::SignalValue> values;
	uint32_t len = available_points - returned_points_;
	storage::SignalValue level = 1;
	for (size_t i = 0; i < len; ++i) {
		values.push_back(level);
	}
	storage::SignalData sd(&*values.begin(), len);
	storage::Frame f(ts);
	for (common::SignalKey k : common::SignalKey::_values()) {
		f[k] = sd;
	}
	returned_points_ += f.size();
	return std::move(f);
}

void MockDevice::checkState(DeviceState required) {
	if (state_ != required){
		std::string required_repr = required._to_string();
		std::string state_repr = state_._to_string();
		throw common::AssertionError("State is required to be " + required_repr + ", but found " + state_repr);
	}
}

void MockDevice::checkDue() const {
	if (timer_.isOverdue()) {
		state_ = DeviceState::CanSet;
		timer_.stop();
	}
}