#include <catch2/catch.hpp>
#include <sstream>
#include "common/timestamp.h"
using namespace common;


TEST_CASE("TimeStamp") {
	SECTION("Creation") {
		TimeStamp ts(0, 0);
		REQUIRE(ts.step == 0);
		REQUIRE(ts.count == 0);
	}
	SECTION("Comparation"){
		TimeStamp ts1(10, 10);
		TimeStamp ts2(10, 10);

		TimeStamp ts3(10, 20);
		TimeStamp ts4(20, 10);
		
		TimeStamp ts5(0, 10);
		TimeStamp ts6(0, 10);

		TimeStamp ts7(10, 0);
		TimeStamp ts8(10, 0);
		TimeStamp ts9(20, 0);


		REQUIRE(ts1 == ts2);
		REQUIRE(ts2 == ts1);

		REQUIRE(ts3 > ts1);
		REQUIRE(ts4 > ts1);

		REQUIRE(ts2 < ts3);
		REQUIRE(ts2 < ts4);

		REQUIRE(ts3 == ts4);

		REQUIRE(ts5 < ts1);
		REQUIRE(ts6 < ts3);
		REQUIRE(ts9 < ts4);


		REQUIRE(ts5 == ts6);
		REQUIRE(ts5 == ts7);
		REQUIRE(ts7 == ts8);
		REQUIRE(ts7 == ts9);
	}
	SECTION("Representation"){
		TimeStamp ts(5, 20);
		std::string desired_repr = "TimeStamp<step:5,count:20>";
		std::stringstream ss;
		ss << ts;
		REQUIRE(ts.value() == TimeUnit(100));
		REQUIRE(ts.repr() == desired_repr);
		REQUIRE(ss.str() == desired_repr);
	}
}