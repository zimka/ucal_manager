#include "timestamp.h"
#include "defaults.h"
using namespace common;

TimeStamp::TimeStamp(TimeUnit s, TickCount c): step(s), count(c){};

bool TimeStamp::operator==(const TimeStamp& other) const{
	return false;
}

bool TimeStamp::operator<(const TimeStamp& other) const{
	return false;
}

bool TimeStamp::operator>(const TimeStamp& other) const{
	return false;
}

std::string TimeStamp::repr() const{
	return "";
}

TimeUnit TimeStamp::value() const{
	return 0;
}

std::ostream& common::operator<<(std::ostream& os, const TimeStamp& ts){
	return os;
}
