#include <utility>
#include <sstream>
#include <catch2/catch.hpp>
#include <chrono>
#include <thread>
#include "common/timestamp.h"
#include "common/utils.h"
#include "device/timer.h"

using namespace device;
using namespace common;


void testSleep(common::TimeUnit t){
	std::this_thread::sleep_for(std::chrono::milliseconds((int)t));
}


TEST_CASE("DeviceTimer"){
	WARN("DeviceTimer: time consuming tests...");
	auto td = TimeUnit(10);

	float sleep_ratio = 10;
	auto sleep_duration = TimeUnit(td * sleep_ratio);


	SECTION("Stopped"){
		DeviceTimer timer(td);
		REQUIRE(timer.getStamp().value() == 0);
		REQUIRE(!timer.isOverdue());
		REQUIRE(!timer.stop());
	}
	SECTION("Running"){
		DeviceTimer timer(td);
		bool correct = true;
		timer.run();
		// IMPORTANT! At ~80 steps becomes not exactly sync because of code run time
		for (auto i = 0; i < 50; i++) {
			auto measured = timer.getStamp().value();
			auto target = i * sleep_duration;
			bool correct_current = common::approxEqual(measured, target);
			testSleep(sleep_duration);
			if (correct) {
				correct = correct && correct_current;
				if (!correct){
					std::string message = std::to_string(measured) + " instead of " + std::to_string(target);
					WARN("incorrect at step " + std::to_string(i) + "; Got " + message);
				}
			}
		}
		REQUIRE(correct);
	}
	SECTION("Overdue") {
		float ratio = 10;
		auto sleep_duration = TimeUnit(td * ratio);

		float overdue_ratio = 5;
		auto overdue_ts = sleep_duration * overdue_ratio;
		DeviceTimer timer(td);
		timer.setOverdue(overdue_ts);
		timer.run();
		size_t overdue_step = 0;
		for (auto i = 0; i < 2 * overdue_ratio; i++) {
			testSleep(sleep_duration);
			if (timer.isOverdue()){
				overdue_step = i;
				break;
			}
		}
		REQUIRE(overdue_step == overdue_ratio);
	}
	SECTION("State machine"){
		bool status = false;
		DeviceTimer timer(td);

		REQUIRE_FALSE(timer.isRunning());
		REQUIRE_FALSE(timer.isOverdue());
		REQUIRE_FALSE(timer.stop());
		REQUIRE_FALSE(timer.isRunning());
		
		REQUIRE(timer.run());
		REQUIRE(timer.isRunning());
		
		REQUIRE_FALSE(timer.run());
		
		REQUIRE_FALSE(timer.setOverdue(td));
		REQUIRE(timer.stop());
		REQUIRE_FALSE(timer.isRunning());
	}

}
