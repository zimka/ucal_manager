#include "timer.h"
using namespace device;

DeviceTimer::DeviceTimer(common::TimeUnit step){}
common::TimeStamp DeviceTimer::getStamp(){
	return common::TimeStamp(0, 0);
};
bool DeviceTimer::setStep(common::TimeUnit step){
	return false;
};
bool DeviceTimer::run(){
	return false;
};
bool DeviceTimer::stop(){
	return false;
};
bool DeviceTimer::setOverdue(common::TimeUnit t){
	return false;
};
bool DeviceTimer::isOverdue() const{
	return false;
};
