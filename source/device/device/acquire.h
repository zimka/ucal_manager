#include "device/device.h"
#include "common/exceptions.h"

// Do we need one more singleton?
namespace device {
    std::unique_ptr<IDevice> acquireDevice(){
    #ifdef REAL_DEVICE
    return std::make_unique<DaqboardDevice>();
    #else
    return std::make_unique<MockDevice>();
    #endif
	}
}
