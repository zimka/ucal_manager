#ifndef UCAL2_COMMON_UTILS_H
#define UCAL2_COMMON_UTILS_H

#include "measures.h"
#include <algorithm>
#include <random>

namespace common {
    /*!
    * Tests two values to be equal. For big values relative error is compared to ACCURACY_ERROR.
    * For small values absolute error or absolute sum is compared to ACCURACY_ERROR.
    */
    inline bool approxEqual(double lv, double rv) {
        bool sameSign = ((lv < 0) == (rv < 0));
        double diff = abs(lv - rv);
        double sum = abs(lv + rv);

        if ((sameSign) && ((sum) < ACCURACY_ERROR)) {
            return true;
        }
        if ((sameSign) && ((diff / sum) < ACCURACY_ERROR)) {
            return true;
        }
        if ((!sameSign) && (diff < ACCURACY_ERROR)) {
            return true;
        }
        return false;
    }

    inline std::string getRandomString(size_t len) {
        auto randchar = []() -> char {
            const char charset[] =
                    "0123456789"
                    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                    "abcdefghijklmnopqrstuvwxyz";
            std::default_random_engine rng(std::random_device {}());
            std::uniform_int_distribution<> dist(0, sizeof(charset) - 1);
            return charset[dist(rng)];
        };
        std::string str(len, 0);
        std::generate_n(str.begin(), len, randchar);
        return str;
    }
}
#endif//UCAL2_COMMON_UTILS_H
