#ifndef UCAL2_COMMON_KEYS_H
#define UCAL2_COMMON_KEYS_H

#include "better_enums/enum.h"

namespace common {
    /*!
    * Key to label signal read from device.
    */
    BETTER_ENUM(SignalKey, char, Uref, Umod, Utpl, Uhtr, Uaux, Undefined);

    /*!
    * Key to label signal written to device.
    */
    BETTER_ENUM(ControlKey, char, Vg, Vm, Undefined);

    /*!
    * Key to identify AcquisitionDevice
    */
    BETTER_ENUM(DeviceId, char, Daqboard, Mock, Undefined);


    /*!
    * Key to indentify string config field
    */
    BETTER_ENUM(ConfigStringKey, char, BoardId, Undefined);

    /*!
    * Key to indentify double config field
    */
    BETTER_ENUM(ConfigDoubleKey, char, SamplingFreq, TimeUnitSize, Undefined);
}
#endif//UCAL2_COMMON_KEYS_H
