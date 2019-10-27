#ifndef UCAL2_DEVICE_ACQUIRE_H
#define UCAL2_DEVICE_ACQUIRE_H

#include "device/device.h"
#include "common/exceptions.h"
#define REAL_DEVICE 1;

// Do we need one more singleton?
namespace device {
    static std::unique_ptr<IDevice> acquireDevice(){
    #ifdef REAL_DEVICE
    auto& conf = common::acquireConfig();
    std::unique_ptr<IDevice> dev = std::make_unique<DaqboardDevice>(
        conf->readStr(common::ConfigStringKey::BoardId)
    );
    DeviceTimer timer(1);
    timer.reconfigure(common::acquireConfig());
    dev->setTimer(timer);
    #else
    std::unique_ptr<IDevice> dev = std::make_unique<MockDevice>();
    #endif
    return std::move(dev);
    }
}
#endif//UCAL2_DEVICE_ACQUIRE_H
