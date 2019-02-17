#ifndef UCAL2_COMMON_KEYS_H
#define UCAL2_COMMON_KEYS_H
#include "single_header/enum.h"

namespace common{
	/*!
	* Key to label signal read from device.
	*/
	BETTER_ENUM(SignalKey, char, Uref, Umod, Utpl, Uhtr, Uaux);
}
#endif //UCAL2_COMMON_KEYS_H
