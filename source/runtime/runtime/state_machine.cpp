//
// Created by igor on 06.09.19.
//

#include "state_machine.h"
#include <common/exceptions.h>

#include <sstream>

using namespace runtime;
using std::move;


MachineState StateMachine::getState() {
    return state_->getState();
}

common::Config const& StateMachine::getConfig() {
    return state_->getConfig();
}

Plan const& StateMachine::getPlan() {
    return state_->getPlan();
}

void StateMachine::setConfig(json const& json_cfg) {
    state_->setConfig(json_cfg);
}

void StateMachine::setPlan(Plan new_plan) {
    state_->setPlan(move(new_plan));
    //dummyContext_.plan = new_plan;
}

void StateMachine::runNext() {
    state_->runNext();
}

void StateMachine::stop() {
    state_->stop();
}

Context& StateMachine::getContext() {
    return context_;
}

void StateMachine::setState(StatePtr new_state) {
    state_ = move(new_state);
}

template <MachineStateType S>
void GenericState<S>::throwError(const char* name) {
    std::stringstream message;
    message << "Cannot execute " << name << " from "
            << MachineState::_from_integral(S)._to_string();
    throw common::StateViolationError(message.str());
}

// THIS CODE IS FAKE AND MUST NOT BE EXECUTED
// IT'S SOLE PURPOSE IS TO INSTATIATE ALL NEEDED TEMPLATE VARIANTS IN COMPILE TIME
template <size_t _idx>
class GenRecursive {
public:
    static int exec() {
        constexpr auto iter = MachineState::_values();
        constexpr auto start = iter.begin() + _idx;
        constexpr auto end = iter.end();
        GenericState<start->_value>(nullptr);
        GenRecursive<_idx + 1>::exec();
        return 0;
    }
};

template <>
class GenRecursive<MachineState::_size()> {
public:
    static int exec() {
        return 0;
    }
};

static int foo = GenRecursive<0>::exec();
// END OF FAKE CODE

/*!
 * Defines member function that throws exception
 */
#define STATE_DEFAULT_THROW(retv, fname, ...) \
template <MachineStateType S> \
retv GenericState<S>::fname(__VA_ARGS__) { throwError(#fname); }

/*!
 * Returns state, same for all functions
 * @tparam S numeric representation of MachineState
 * @return MachineState value of current state
 */
template <MachineStateType S>
MachineState GenericState<S>::getState() {
    return MachineState::_from_integral(S);
}

template <MachineStateType S>
common::Config const& GenericState<S>::getConfig() {
    return *common::acquireConfig();
}

STATE_DEFAULT_THROW(Plan const&, getPlan)

STATE_DEFAULT_THROW(void, setConfig, json const&)

STATE_DEFAULT_THROW(void, setPlan, Plan)

STATE_DEFAULT_THROW(void, runNext)

STATE_DEFAULT_THROW(void, stop)

template <>
common::Config const& GenericState<MachineState::NotReady>::getConfig() {
    throwError ("getConfig");
}

template <>
Plan const& GenericState<MachineState::NoPlan>::getPlan() {
    return machine_->getContext().plan;
}

template <>
void GenericState<MachineState::NoPlan>::setPlan(Plan new_plan) {
    if (!new_plan.empty()) {
        machine_->getContext().plan = move(new_plan);
        machine_->setState(createState<MachineState::HasPlan>(machine_));
    }
}

template <>
void GenericState<MachineState::HasPlan>::setPlan(Plan new_plan) {
    auto& stored_plan = machine_->getContext().plan;
    stored_plan = move(new_plan);
    if (stored_plan.empty()) {
        machine_->setState(createState<MachineState::NoPlan>(machine_));
    }
}

template <>
Plan const& GenericState<MachineState::HasPlan>::getPlan() {
    return machine_->getContext().plan;
}

template <>
void GenericState<MachineState::HasPlan>::runNext() {
    auto& context = machine_->getContext();
    //context.device->prepare();
    //context.device->run();
    machine_->setState(createState<MachineState::Executing>(machine_));
}

template <>
Plan const& GenericState<MachineState::Executing>::getPlan() {
    return machine_->getContext().plan; // TODO: cut alredy done blocks from plan
}

template <>
void GenericState<MachineState::Executing>::stop() {
    auto& context = machine_->getContext();
    //context.device->stop();
    machine_->setState(createState<MachineState::Executing>(machine_));
}