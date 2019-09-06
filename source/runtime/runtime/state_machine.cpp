//
// Created by igor on 06.09.19.
//

#include "state_machine.h"
#include <common/exceptions.h>

#define STATE_INTERFACE_DECL \
    void runNext() override; \
    void stop() override; \
    void setPlan(DummyPlan) override; \
    DummyPlan getPlan() override; \
    //setConfig(json) override; \
    //json getConfig() override; \
    //IState& getState() override; \
    //Data getData() override;

class NoPlanState : public IState
{
public:
    NoPlanState(StateMachine* machine) : machine_(machine) {}
    STATE_INTERFACE_DECL

private:
    StateMachine* machine_;
};

class HasPlanState : public IState
{
public:
    HasPlanState(StateMachine* machine) : machine_(machine) {}
    STATE_INTERFACE_DECL

private:
    StateMachine* machine_;
};

void NoPlanState::runNext() {
    throw common::StateViolationError("Cannot run anything while in NoPlanState!");
}

void NoPlanState::stop() {
    throw common::StateViolationError("Cannot stop anything while in NoPlanState!");
}

void NoPlanState::setPlan(DummyPlan plan) {
    auto context = machine_->getContext();
    // TODO: write logic for context and plan
    machine_->setState(std::make_unique<HasPlanState>(machine_));
}

#undef STATE_INTERFACE_DECL
