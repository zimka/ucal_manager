#include <utility>
#include <sstream>
#include <catch2/catch.hpp>
#include "storage/signal.h"
using namespace storage;


TEST_CASE("Signal") {
	const int len = 10;
	SignalValue values[len] = {1.0,2.0,1.0,2.0,1.0,2.0,1.0,2.0,1.0,2.0};
	std::string valid_repr = "[1.0,2.0,1.0,2.0,1.0,2.0,1.0,2.0,1.0,2.0]";
		
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

		SignalData s4(values, len/2, 2);
		REQUIRE(s4.size() == len/2);
		REQUIRE(s4[0] == 1.);
		REQUIRE(s4[1] == 1.);

		SignalData s5(values + 1, len/2, 2);
		REQUIRE(s5.size() == len/2);
		REQUIRE(s5[0] == 2.);
		REQUIRE(s5[1] == 2.);

	}
	SECTION("Assignment"){
		SignalData s0;
		SignalData s1(values, len);

		s0 = s1;
		REQUIRE(s0.size() == len);
		REQUIRE(s1.size() == len);

		SignalData s2;
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
		ss.str("");
		ss << s1;
		REQUIRE(ss.str() == valid_repr);
		REQUIRE(s1.repr() == valid_repr);

		s0 = SignalData();
		ss.str("");
		ss << s0;
		ss >> s1;
		REQUIRE(s1.size() == 0);
	}
	SECTION("Operations"){
		int part_len = len / 2 - 1;
		SignalData s0(values, len);
		SignalData s1 = s0.detachBack(part_len);
		REQUIRE(s1.size() == part_len);
		REQUIRE(s0.size() == (len - part_len));
		SignalData s2(s1);
		s2.attachBack(s1);
		REQUIRE(s2.size() == 2*part_len);
		REQUIRE(s1.size() == part_len);

		SignalData s3 = s1.detachBack(part_len);
		REQUIRE(s1.size() == 0);
		REQUIRE(s3.size() == part_len);
		s0 = s3.detachBack(0);
		REQUIRE(s0.size() == 0);
		s3.attachBack(s0);
		REQUIRE(s3.size() == part_len);
		REQUIRE(s0.size() == 0);

		REQUIRE(s3[0] == 1.);
		REQUIRE(s3[part_len - 1] == 2.);
	}
}
