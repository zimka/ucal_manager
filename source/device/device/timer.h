#ifndef UCAL2_DEVICE_TIMER_H
#define UCAL2_DEVICE_TIMER_H
#include <chrono>
#include "common/units.h"
#include "common/timestamp.h"

namespace device{
	class DeviceTimer{	
	public:
		DeviceTimer(common::TimeUnit step);
		common::TimeStamp getStamp();
		bool setStep(common::TimeUnit step);
		bool run();
		bool stop();
		bool setOverdue(common::TimeUnit t);
		bool isOverdue() const;
	};
}

#endif//UCAL2_DEVICE_TIMER_H
