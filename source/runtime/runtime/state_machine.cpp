//
// Created by igor on 06.09.19.
//

#include "state_machine.h"
#include <common/exceptions.h>

#include <sstream>

using namespace runtime;


MachineState StateMachine::getState() {
    return state_->getState();
}

common::Config const& StateMachine::getConfig() {
    return state_->getConfig();
}

DummyPlan StateMachine::getPlan() {
    return state_->getPlan();
}

void StateMachine::setConfig(json const& json_cfg) {
    state_->setConfig(json_cfg);
}

void StateMachine::setPlan(DummyPlan const& new_plan) {
    state_->setPlan(new_plan);
    //dummyContext_.plan = new_plan;
}

void StateMachine::runNext() {
    state_->runNext();
}

void StateMachine::stop() {
    state_->stop();
}

DummyContext& StateMachine::getContext() {
    return dummyContext_;
}

void StateMachine::setState(StatePtr new_state) {
    state_ = std::move(new_state);
}

template <MachineStateType S>
void GenericState<S>::throwError(const char* name) {
    std::stringstream message;
    message << "Cannot execute " << name << " from "
            << MachineState::_from_integral(S)._to_string();
    throw common::StateViolationError(message.str());
}

template <MachineStateType S>
StatePtr stateGenerator (StateMachine* machine) {
    return std::make_unique<GenericState<S>>(machine);
}

constexpr
MachineState::_integral f (MachineState::_value_iterable iter) {
    return iter.begin()->_value;
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

STATE_DEFAULT_THROW(DummyPlan, getPlan)

STATE_DEFAULT_THROW(void, setConfig, json const&)

STATE_DEFAULT_THROW(void, setPlan, DummyPlan const&)

STATE_DEFAULT_THROW(void, runNext)

STATE_DEFAULT_THROW(void, stop)

template <>
common::Config const& GenericState<MachineState::NotReady>::getConfig() {
    throwError ("getConfig");
}
