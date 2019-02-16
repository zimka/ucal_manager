#ifndef UCAL2_COMMON_UTILS_H
#define UCAL2_COMMON_UTILS_H
#include "defaults.h"

namespace common{
	/*!
	* Tests two values to be equal. For big values relative error is compared to ACCURACY_ERROR. 
	* For small values absolute error or absolute sum is compared to ACCURACY_ERROR.
	*/
	inline bool approxEqual(double lv, double rv){
		bool sameSign = ((lv < 0) == (rv < 0));
		double diff = abs(lv - rv);
		double sum = abs(lv + rv);

		if ((sameSign) && ((sum) < ACCURACY_ERROR))
			return true;
		if ((sameSign) && ((diff / sum) < ACCURACY_ERROR))
			return true;
		if ((!sameSign) && (diff < ACCURACY_ERROR))
			return true;
		return false;
	}
}
#endif//UCAL2_COMMON_UTILS_H
