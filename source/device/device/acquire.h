#ifndef UCAL2_DEVICE_ACQUIRE_H
#define UCAL2_DEVICE_ACQUIRE_H

#include "device/device.h"
#include "common/exceptions.h"
#define REAL_DEVICE 1;

// Do we need one more singleton?
namespace device {
    std::unique_ptr<IDevice> acquireDevice();
}
#endif//UCAL2_DEVICE_ACQUIRE_H
