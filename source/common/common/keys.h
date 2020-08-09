#ifndef UCAL2_COMMON_KEYS_H
#define UCAL2_COMMON_KEYS_H

#include "better_enums/enum.h"

namespace common {
    /*!
    * Key to label signal read from device.
    */
    BETTER_ENUM(SignalKey, char, S0, S1, S2, S3, S4, Undefined);

    /*!
    * Key to label signal written to device.
    */
    BETTER_ENUM(ControlKey, char, C0, C1, Undefined);

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
    BETTER_ENUM(ConfigDoubleKey, char, TimeUnitSize, StorageFrameSize, Undefined);

    /*!
     * Keys for indexing Runtime states
     */
    using MachineStateType = unsigned;
    BETTER_ENUM(MachineState, MachineStateType, Error, NoPlan, HasPlan, Executing)
}
#endif//UCAL2_COMMON_KEYS_H
