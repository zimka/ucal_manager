#include <catch2/catch.hpp>
#include "common/defaults.h"


TEST_CASE( "Example") {
	SECTION("Example") {
		REQUIRE(common::ACCURACY_ERROR == 0);
	};
}