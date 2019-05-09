#include "timer.h"
#include "common/exceptions.h"
using namespace device;

DeviceTimer::DeviceTimer(common::TimeUnit step){
	step_ = step;
	overdue_ = 0;
	start_ = clock_.now();
};

common::TimeStamp DeviceTimer::getStamp() const{
	if (! isRunning()){
		common::AssertionError(
			"Non-running DeviceTimer is requested for TimeStamp"
		);
	}
	common::TimeUnit duration = (std::chrono::duration_cast<std::chrono::milliseconds>(clock_.now() - start_)).count();
	return common::TimeStamp(step_, duration/step_);
};

bool DeviceTimer::setStep(common::TimeUnit step){
	if (isRunning()){
		return false;
	}
	step_ = step;
	return true;
};

bool DeviceTimer::run(){
	if (is_running_){
		return false;
	}
	start_ = clock_.now();
	is_running_ = true;
	return true;
};

bool DeviceTimer::stop(){
	if (! is_running_){
		return false;
	}
	is_running_ = false;
	return true;
};

bool DeviceTimer::setOverdue(common::TimeUnit overdue){
	if (isRunning()){
		return false;
	}
	overdue_ = overdue;
	return true;
};

bool DeviceTimer::isOverdue() const{
	if (! isRunning()){
		return false;
	}
	if (!overdue_){
		return false;
	}
	common::TimeUnit duration = (std::chrono::duration_cast<std::chrono::milliseconds>(clock_.now() - start_)).count();
	return duration > overdue_;
};

bool DeviceTimer::isRunning() const{
	return is_running_;
};