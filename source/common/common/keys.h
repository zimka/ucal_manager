#ifndef UCAL2_COMMON_KEYS_H
#define UCAL2_COMMON_KEYS_H
#include "single_header/enum.h"

namespace common{
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
}
#endif//UCAL2_COMMON_KEYS_H
