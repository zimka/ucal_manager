#include "timestamp.h"
#include "utils.h"
#include "exceptions.h"

using namespace common;

double getRtlRatio(TimeStamp const& lts, TimeStamp const& rts) {
    if ((lts.step == 0) || (lts.count == 0)) {
        throw AssertionError("common::getRtlRatio got zero lts");
    }
    double steps_ratio = double(rts.step) / lts.step;
    double counts_ratio = double(rts.count) / lts.count;
    return steps_ratio * counts_ratio;
}

TimeStamp::TimeStamp(TimeUnit s, TickCount c) : step(s), count(c) {};

bool TimeStamp::operator==(TimeStamp const& other) const {
    if (this->value() == 0) {
        if (other.value() == 0) {
            return true;
        }
        else {
            return false;
        }
    }

    double ratio = getRtlRatio(*this, other);
    return approxEqual(ratio, 1.);
}

bool TimeStamp::operator<(TimeStamp const& other) const {
    if (this->value() == 0) {
        if (other.value() == 0) {
            return false;
        }
        else {
            return true;
        }
    }
    double ratio = getRtlRatio(*this, other);
    return ((!approxEqual(ratio, 1.)) && (ratio > 1.));
}

bool TimeStamp::operator>(TimeStamp const& other) const {
    if (this->value() == 0) {
        return false;
    }
    double ratio = getRtlRatio(*this, other);
    return ((!approxEqual(ratio, 1.)) && (ratio < 1.));
}

std::string TimeStamp::repr() const {
    return "TimeStamp<step:" + std::to_string(step) + ",count:" + std::to_string(count) + ">";
}

TimeUnit TimeStamp::value() const {
    return step * count;
}

std::ostream& common::operator<<(std::ostream& os, TimeStamp const& ts) {
    os << ts.repr();
    return os;
}
