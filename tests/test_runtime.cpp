//
// Created by igor on 27.07.19.
//

#include <runtime/block.h>
#include <runtime/state_machine.h>
#include <catch2/catch.hpp>

using namespace runtime;

TEST_CASE("Block") {
    SECTION("Creation") {
        Block block {
            10,
            10,
            1.5,
            {10, 20, 30, 40,},
            {110, 120, 130, 140,},
        };

        std::string result = "struct Block {"
                             ", pattern_len_ = 10"
                             ", block_len_ = 10"
                             ", sampling_rate_ = 1.5"
                             ", guard_ = { 10, 20, 30, 40, }"
                             ", mod_ = { 110, 120, 130, 140, }"
                             ;
        REQUIRE(block.repr() == result);
    }

    SECTION("Copy") {
        Block block1 {
                10,
                10,
                1.5,
                {10, 20, 30, 40,},
                {110, 120, 130, 140,},
        };

        {
            Block block2 (block1);
            REQUIRE(block1.repr() == block2.repr());
        }

        {
            Block block2 = block1;
            REQUIRE(block1.repr() == block2.repr());
        }

        {
            Block block2 = std::move(block1);
            REQUIRE(block1.repr() != block2.repr());
        }
    }
}

TEST_CASE("StateMachine") {
    Block block1 {
            10,
            10,
            1.5,
            {10, 20, 30, 40,},
            {110, 120, 130, 140,},
    };
    SECTION("NotReady") {
        StateMachine machine;
        machine.setState(createState<MachineState::NotReady>(&machine));

        REQUIRE(machine.getState()._value == MachineState::NotReady);
        REQUIRE_THROWS(machine.getConfig());
        REQUIRE_THROWS(machine.getPlan());
        //REQUIRE_THROWS(machine.getData()); TODO: rewrite
        //REQUIRE_THROWS(machine.SetConfig()); TODO: rewrite
        REQUIRE_THROWS(machine.setPlan(Plan()));
        REQUIRE_THROWS(machine.runNext());
        REQUIRE_THROWS(machine.stop());
    }

    SECTION("Error") {
        StateMachine machine;
        machine.setState(createState<MachineState::Error>(&machine));

        REQUIRE(machine.getState()._value == MachineState::Error);
        REQUIRE_NOTHROW(machine.getConfig()); // TODO: check config?
        REQUIRE_THROWS(machine.getPlan());
        //REQUIRE_THROWS(machine.getData()); TODO: rewrite
        //REQUIRE_THROWS(machine.SetConfig()); TODO: rewrite
        REQUIRE_THROWS(machine.setPlan(Plan()));
        REQUIRE_THROWS(machine.runNext());
        REQUIRE_THROWS(machine.stop());
    }

    SECTION("NoPlan") {
        StateMachine machine;
        machine.setState(createState<MachineState::NoPlan>(&machine));

        Plan empty;
        Plan non_empty = { block1 };
        REQUIRE(machine.getState()._value == MachineState::NoPlan);
        REQUIRE_NOTHROW(machine.getConfig()); // TODO: check config?
        REQUIRE_NOTHROW(machine.getPlan().empty()); // TODO: must return empty plan
        //REQUIRE_THROWS(machine.getData()); TODO: rewrite
        //REQUIRE_THROWS(machine.SetConfig()); TODO: rewrite
        REQUIRE_THROWS(machine.runNext());
        REQUIRE_THROWS(machine.stop());
        REQUIRE_NOTHROW((machine.setPlan(empty), machine.getState()._value == MachineState::NoPlan));
        REQUIRE_NOTHROW((machine.setPlan(non_empty), machine.getState()._value == MachineState::HasPlan));
    }

    SECTION("HasPlan") {
        StateMachine machine;
        machine.setState(createState<MachineState::HasPlan>(&machine));

        REQUIRE(machine.getState()._value == MachineState::HasPlan);
        REQUIRE_NOTHROW(machine.getConfig()); // TODO: check config?
        Plan empty;
        Plan non_empty = { block1 };
        REQUIRE_NOTHROW((machine.setPlan(non_empty), machine.getState()._value == MachineState::HasPlan));
        REQUIRE_NOTHROW((machine.setPlan(empty), machine.getState()._value == MachineState::NoPlan));
        REQUIRE_NOTHROW((machine.setPlan(non_empty), machine.getState()._value == MachineState::HasPlan));
        REQUIRE_NOTHROW(machine.getPlan() == non_empty); // TODO: must return current plan
        //REQUIRE_THROWS(machine.getData()); TODO: rewrite
        //REQUIRE_THROWS(machine.SetConfig()); TODO: rewrite
        REQUIRE_THROWS(machine.stop());
        REQUIRE_NOTHROW((machine.runNext(), machine.getState()._value == MachineState::Executing));
    }

    SECTION("Executing") {
        StateMachine machine;
        Plan empty;
        Plan non_empty = { block1 };
        machine.setState(createState<MachineState::NoPlan>(&machine));
        REQUIRE_NOTHROW((machine.setPlan(non_empty), machine.getState()._value == MachineState::HasPlan));
        REQUIRE_NOTHROW((machine.runNext(), machine.getState()._value == MachineState::Executing));

        REQUIRE(machine.getState()._value == MachineState::Executing);
        REQUIRE_NOTHROW(machine.getConfig()); // TODO: check config?
        REQUIRE_NOTHROW(machine.getPlan() == non_empty); // TODO: must return currently executed plan
        //REQUIRE_THROWS(machine.getData()); TODO: rewrite TODO: must return data already collected
        //REQUIRE_THROWS(machine.SetConfig()); TODO: rewrite
        REQUIRE_THROWS(machine.setPlan(non_empty));
        REQUIRE_THROWS(machine.setPlan(empty));
        // TODO: complex behaviour: run next block, if current block is infinite, else throw exception
        //REQUIRE_NOTHROW((machine.runNext(), machine.getState()._value == MachineState::Executing));
        REQUIRE_NOTHROW(machine.stop());
    }
}
