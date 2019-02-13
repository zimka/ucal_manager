#include "timestamp.h"
#include "defaults.h"

using namespace common;

TimeStamp::TimeStamp(TimeUnit s, TickCount c): step(s), count(c){};
