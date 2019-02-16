#ifndef UCAL2_COMMON_UTILS_H
#define UCAL2_COMMON_UTILS_H
#include <stdlib.h>
#include "defaults.h"

namespace common{
	inline bool approxEqual(double lv, double rv){
		bool sameSign = ((lv < 0) == (rv < 0));
		double diff = abs(lv - rv);
		double sum = abs(lv + rv);

		bool result = false;
		if ((sameSign) && ((diff / sum) < ACCURACY_ERROR))
			result = true;
		if ((!sameSign) && (diff < ACCURACY_ERROR))
			result = true; 
		return result;
	}
}
#endif//UCAL2_COMMON_UTILS_H