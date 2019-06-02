#include "interface.h"
#include "storage/signal.h"
using namespace device;


MockDevice::MockDevice():timer_(1){};
bool MockDevice::run(){
	if (is_running_){
		return false;
	}
	timer_.stop();
	timer_.run();
	is_running_ = true;
	return true;
};
bool MockDevice::stop(){
	if (! is_running_){
		return false;
	}
	timer_.stop();
	is_running_ = false;
	return true;
};
bool MockDevice::isRunning() const{
	if (!timer_.isOverdue()){
		return is_running_;
	}
	//TODO: this does not turn off timer
	// can't use stop beacuse of const
	is_running_ = false;
	return is_running_;
};
storage::Frame MockDevice::getFrame(){
	auto ts = timer_.getStamp();
	int len = ts.count - points_returned_;
	if (len <= 0){
		return storage::Frame(ts);
	}

	std::vector<storage::SignalValue> values;
	for (size_t i=0; i<len; ++i){
		values.push_back(1);
	}
	storage::SignalData sd(&*values.begin(), len);
	storage::Frame f(ts);
	for (common::SignalKey k: common::SignalKey::_values()){
		f[k] = sd;
	}
	points_returned_ += f.size();
	return std::move(f);
};
bool MockDevice::setConstVoltage(common::ControlKey id, common::VoltUnit level){
	return (!isRunning());
};
bool MockDevice::setSineVoltage(common::ControlKey id, common::VoltUnit offset, common::VoltUnit amplitude, common::TimeUnit sine_period){
	return (!isRunning());
};
bool MockDevice::setLinearVoltage(common::ControlKey id, common::VoltUnit start, common::VoltUnit end, common::TimeUnit signal_duration){
	return (!isRunning());
};
bool MockDevice::setSqrtVoltage(common::ControlKey id, common::VoltUnit start, common::VoltUnit end, common::TimeUnit signal_duration){
	return (!isRunning());
};
bool MockDevice::setSequenceVoltage(common::ControlKey id, storage::SignalData voltage_sequence, common::TimeUnit sequence_duration){
	return (!isRunning());
};
bool MockDevice::setTimeout(common::TimeUnit finish){
	if (isRunning()){
		return false;
	}
	timer_.setOverdue(finish);
	return true;
};
common::DeviceId MockDevice::getId() const{
	return common::DeviceId::Mock;
};
