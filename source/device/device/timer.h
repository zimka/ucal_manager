#ifndef UCAL2_DEVICE_TIMER_H
#define UCAL2_DEVICE_TIMER_H
#include <chrono>
#include "common/units.h"
#include "common/timestamp.h"

namespace device{
	class DeviceTimer{	
	public:
		DeviceTimer(common::TimeUnit step);
		common::TimeStamp getStamp() const;
		bool setStep(common::TimeUnit step);
		bool run();
		bool stop();
		bool setOverdue(common::TimeUnit overdue);
		bool isOverdue() const;
		bool isRunning() const;
	private:
		bool is_running_ = false;
		common::TimeUnit step_;
		common::TimeUnit overdue_;
		std::chrono::steady_clock clock_;
		std::chrono::time_point<std::chrono::steady_clock> start_;
	};
}

#endif//UCAL2_DEVICE_TIMER_H
