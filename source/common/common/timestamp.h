#ifndef UCAL2_COMMON_TIMESTAMP_H
#define UCAL2_COMMON_TIMESTAMP_H
#include <stdint.h>
#include <iostream>

namespace common{
	/*!
	* Time in units defined in defaults.h stored as int.
	*/
	using TimeUnit = uint32_t;

	/*!
	* Number of TimeUnit steps.
	*/
	using TickCount = uint32_t;

	/*!
	* Time duration expressing the difference between two moments.
	*/
	struct TimeStamp{
		TimeUnit step;
		TickCount count;

		TimeStamp(TimeUnit step, TickCount count);

		TimeUnit value() const;
		std::string repr() const;

		bool operator==(const TimeStamp& other) const;
		bool operator< (const TimeStamp& other) const;
		bool operator> (const TimeStamp& other) const;
	};		
	std::ostream& operator<<(std::ostream& os, const TimeStamp& ts);
}
#endif//UCAL2_COMMON_TIMESTAMP_H
