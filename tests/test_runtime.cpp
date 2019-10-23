//
// Created by igor on 27.07.19.
//

#include <runtime/block.h>
#include <runtime/state_machine.h>
#include <runtime/core.h>
#include <catch2/catch.hpp>

using namespace runtime;

static void testSleep(common::TimeUnit t) {
    std::this_thread::sleep_for(std::chrono::milliseconds(t));
}

static void flushQueue(runtime::FrameQueue* queue) {
    while (queue->peek() != nullptr) {
        queue->pop();
    }
}

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
                             ", write_step_ = 10"
                             ", block_len_ = 10"
                             ", read_step_ = 1.5"
                             ", voltage_0_ = { 10, 20, 30, 40, }"
                             ", voltage_1_ = { 110, 120, 130, 140, }"
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
    using common::MachineState;
    Block block1 {
            1,
            100,
            1,
            {10, 20, 30, 40,},
            {110, 120, 130, 140,},
    };
    Block block2 {
            10,
            0,
            1,
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
        REQUIRE_NOTHROW(machine.setPlan(empty));
        REQUIRE(machine.getState()._value == MachineState::NoPlan);
        REQUIRE_NOTHROW(machine.setPlan(non_empty));
        REQUIRE(machine.getState()._value == MachineState::HasPlan);
    }

    SECTION("HasPlan") {
        StateMachine machine;
        machine.setState(createState<MachineState::HasPlan>(&machine));

        REQUIRE(machine.getState()._value == MachineState::HasPlan);
        REQUIRE_NOTHROW(machine.getConfig()); // TODO: check config?
        Plan empty;
        Plan non_empty = { block1 };
        REQUIRE_NOTHROW(machine.setPlan(non_empty));
        REQUIRE(machine.getState()._value == MachineState::HasPlan);
        REQUIRE_NOTHROW(machine.setPlan(empty));
        REQUIRE(machine.getState()._value == MachineState::NoPlan);
        REQUIRE_NOTHROW(machine.setPlan(non_empty));
        REQUIRE(machine.getState()._value == MachineState::HasPlan);
        REQUIRE(machine.getPlan() == non_empty); // TODO: must return current plan
        //REQUIRE_THROWS(machine.getData()); TODO: rewrite
        //REQUIRE_THROWS(machine.SetConfig()); TODO: rewrite
        REQUIRE_THROWS(machine.stop());
        REQUIRE_NOTHROW(machine.runNext());
        REQUIRE(machine.getState()._value == MachineState::Executing);
        REQUIRE_NOTHROW(machine.stop());
        REQUIRE(machine.getState() == +MachineState::HasPlan);
    }

    SECTION("Executing") {
        StateMachine machine;
        Plan empty;
        Plan non_empty = { block2 , block1};
        machine.setState(createState<MachineState::NoPlan>(&machine));
        REQUIRE_NOTHROW(machine.setPlan(non_empty));
        REQUIRE(machine.getState()._value == MachineState::HasPlan);
        REQUIRE(machine.getPlan() == non_empty);
        REQUIRE_NOTHROW(machine.runNext());
        REQUIRE(machine.getState()._value == MachineState::Executing);
        testSleep(150);
        REQUIRE(machine.getPlan() == non_empty);
        REQUIRE_THROWS(machine.setPlan(non_empty));
        REQUIRE_THROWS(machine.setPlan(empty));

        // TODO: complex behaviour: run next block, if current block is infinite, else throw exception
        REQUIRE_NOTHROW(machine.runNext()); // Switch from infinite to finite
        Plan tail = {block1};
        REQUIRE(machine.getPlan() == tail);
        //REQUIRE(machine.getState()._value == MachineState::HasPlan);
        REQUIRE_NOTHROW(machine.getConfig());
        //REQUIRE_THROWS(machine.getData()); TODO: rewrite TODO: must return data already collected
        //REQUIRE_THROWS(machine.SetConfig()); TODO: rewrite
        //REQUIRE_NOTHROW(machine.stop());
        testSleep(200);
        REQUIRE(machine.getState() == +MachineState::HasPlan); // FIXME: also unstable
    }
}


TEST_CASE("Worker") {
    auto queue = FrameQueue();
    std::atomic<int8_t> index {-1};
    runtime::Block inf_block {
            100,
            0,
            1.5,
            {10, 20, 30, 40,},
            {110, 120, 130, 140,},
    };
    runtime::Block short_block {
        1,
        10,
        1.5,
        {10, 20, 30, 40,},
        {110, 120, 130, 140,},
    };
    auto plan = runtime::Plan({inf_block, short_block});

    SECTION("Init turned off") {
        auto w = Worker(&index, &queue, plan);
        REQUIRE(w.finished());
        REQUIRE(queue.peek() == nullptr);
        w.doStep();
        REQUIRE(queue.peek() == nullptr);
    }

    SECTION("Init turned on") {
        index = 0;
        auto w = Worker(&index, &queue, plan);
        REQUIRE(!w.finished());
        REQUIRE(queue.peek() == nullptr);
        w.doStep();
        REQUIRE(queue.peek() == nullptr);
        testSleep(100);
        w.doStep();
        REQUIRE(queue.peek() != nullptr);
        REQUIRE(!w.finished());
    }

    SECTION("Master stop") {
        index = 0;
        auto w = Worker(&index, &queue, plan);
        w.doStep(); 
        testSleep(100);
        w.doStep();
        REQUIRE(!w.finished());
        REQUIRE(queue.peek() != nullptr);
        flushQueue(&queue);
        REQUIRE(queue.peek() == nullptr);
        index = -1;
        w.doStep();
        w.doStep();
        REQUIRE(queue.peek() == nullptr);
        REQUIRE(w.finished());
    }
    SECTION("Infinite block") {
        index = 0;
        auto w = Worker(&index, &queue, {inf_block});
        for (auto i = 0; i < 50; i++) {
            w.doStep();
            testSleep(200);
            REQUIRE(!w.finished());
        }
    }
    SECTION("Short block") {
        index = 0;
        auto w = Worker(&index, &queue, {short_block});
        int i = 0;
        for (i = 0; i < 50; i++) {
            w.doStep();
            testSleep(200);
            if (w.finished()) {
                break;
            }
        }
        REQUIRE(i<=3);
    }
}
