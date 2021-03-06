#ifndef UCAL2_COMMON_MEASURES_H
#define UCAL2_COMMON_MEASURES_H

#include <stdlib.h>
#include <stdint.h>

namespace common {
    /*!
    * Base time units that used in project. Either ms or us.
    */
    const char TIME_UNIT_BASE[] = "ms";

    /*!
    * Base time units multiplier. E.g. 10 with ms means that
    * to measured milliseconds value would be multiplied by 10
    * to cast it to TimeUnit.
    */
    const int TIME_UNIT_MULTIPLIER = 10; // TIME_UNIT - 0.1ms

    /*!
    * Acceptable relative error for big double to be considered as equal.
    * Acceptable absolute error for small double to be considered as equal.
    */
    const float ACCURACY_ERROR = 1e-6;

    //TODO: make magnitude of time changable by config
    /*!
    * Time in millisecond units
    */
    using TimeUnit = uint32_t;

    /*!
    * Voltage in millivoltage units
    */
    using MilliVolt = int32_t;
}
#endif//UCAL2_COMMON_MEASURES_H
