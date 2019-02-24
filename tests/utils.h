#ifndef UCAL2_TESTS_UTILS_H
#define UCAL2_TESTS_UTILS_H

#include "common/keys.h"
#include "common/utils.h"
#include "storage/signal.h"

bool isEqual(storage::SignalData const& sd1, storage::SignalData const& sd2){
	if (sd1.size() != sd2.size())
		return false;
	bool same = true;
	for (size_t i=0; i<sd1.size(); i++){
		same = same && common::approxEqual(sd1[i], sd2[i]);
	}
	return same;
}
#endif//UCAL2_TESTS_UTILS_H
