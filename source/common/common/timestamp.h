#ifndef UCAL2_COMMON_TIMESTAMP_H
#define UCAL2_COMMON_TIMESTAMP_H
#include <stdint.h>
#include <iostream>

namespace common {
	using TimeUnit = uint32_t;
	using TickCount = uint32_t;

	struct TimeStamp {
		const TimeUnit step;
		const TickCount count;

		TimeStamp(TimeUnit step, TickCount count);
	};
		
}
#endif//UCAL2_COMMON_TIMESTAMP_H