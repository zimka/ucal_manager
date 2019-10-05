#ifndef UCAL2_DEVICE_ACQUIRE_H
#define UCAL2_DEVICE_ACQUIRE_H

#include "device/device.h"
#include "common/exceptions.h"

// Do we need one more singleton?
namespace device {
    static std::unique_ptr<IDevice> acquireDevice(){
    #ifdef REAL_DEVICE
    return std::make_unique<DaqboardDevice>();
    #else
    return std::make_unique<DaqboardDevice>();
    return std::make_unique<MockDevice>();
    #endif
    }
}
#endif//UCAL2_DEVICE_ACQUIRE_H
