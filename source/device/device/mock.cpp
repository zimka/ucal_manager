#include "interface.h"
using namespace device;


MockDevice::MockDevice(){};
bool MockDevice::run(){
	return false;
};
bool MockDevice::stop(){
	return false;
};
bool MockDevice::isRunning() const{
	return false;
};
storage::Frame MockDevice::getFrame(){
	return storage::Frame();
};
bool MockDevice::setConstVoltage(common::ControlKey id, common::VoltUnit level){
	return false;
};
bool MockDevice::setSineVoltage(common::ControlKey id, common::VoltUnit offset, common::VoltUnit amplitude, common::TimeUnit sine_period){
	return false;
};
bool MockDevice::setLinearVoltage(common::ControlKey id, common::VoltUnit start, common::VoltUnit end, common::TimeUnit signal_duration){
	return false;
};
bool MockDevice::setSqrtVoltage(common::ControlKey id, common::VoltUnit start, common::VoltUnit end, common::TimeUnit signal_duration){
	return false;
};
bool MockDevice::setSequenceVoltage(common::ControlKey id, storage::SignalData voltage_sequence, common::TimeUnit sequence_duration){
	return false;
};
bool MockDevice::setTimeout(common::TimeUnit finish){
	return false;
};
common::DeviceId MockDevice::getId() const{
	return common::DeviceId::Mock;
};
