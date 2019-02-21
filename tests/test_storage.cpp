#include <utility>
#include <sstream>
#include <catch2/catch.hpp>
#include "storage/signal.h"
using namespace storage;


TEST_CASE("Signal") {
	const int len = 10;
	float values[len] = { 1. };
	std::string valid_repr = "[1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0]";
		
	SECTION("Creation"){
		SignalData s0;
		REQUIRE(s0.size() == 0);
		
		SignalData s1(values, len);
		REQUIRE(s1.size() == len);

		SignalData s2(s1);
		REQUIRE(s2.size() == len);
		REQUIRE(s1.size() == len);

		SignalData s3(std::move(s1));
		REQUIRE(s3.size() == len);
		REQUIRE(s2.size() == len);
		REQUIRE(s1.size() == 0);
	}
	SECTION("Assignment"){
		SignalData s0;
		SignalData s1(values, len);
		REQUIRE(s0.size() == 0);
		REQUIRE(s1.size() == len);

		s0 = s1;
		REQUIRE(s0.size() == len);
		REQUIRE(s1.size() == len);

		SignalData s2;
		REQUIRE(s2.size() == 0);
		s2 = std::move(s0);
		REQUIRE(s0.size() == 0);
		REQUIRE(s1.size() == len);
		REQUIRE(s2.size() == len);
	}
	SECTION("IO"){
		SignalData s0(values, len);
		std::stringstream ss;
		ss << s0;
		REQUIRE(ss.str() == valid_repr);
		REQUIRE(s0.repr() == valid_repr);
		ss << s0;
		SignalData s1;
		REQUIRE(s1.size() == 0);
		ss >> s1;
		REQUIRE(s1.size() == len);
		ss << s1;
		REQUIRE(ss.str() == valid_repr);
		REQUIRE(s1.repr() == valid_repr);
	}
	SECTION("Operations"){
		int part_len = len / 2 - 1;
		SignalData s0(values, len);
		REQUIRE(s0.size() == len);
		SignalData s1 = s0.detachBack(part_len);
		REQUIRE(s1.size() == part_len);
		REQUIRE(s0.size() == (len - part_len));
		SignalData s2(s1);
		REQUIRE(s2.size() == part_len);
		s2.attachBack(s1);
		REQUIRE(s2.size() == 2*part_len);
		REQUIRE(s1.size() == part_len);
	}
}
