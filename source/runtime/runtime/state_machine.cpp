//
// Created by igor on 06.09.19.
//

#include "state_machine.h"
#include "core.h"
#include <common/exceptions.h>

#include <sstream>
//#include <thread>

using namespace runtime;
using std::move;
using common::MachineState;
using common::MachineStateType;


StateMachine::StateMachine()
    : core_(std::make_unique<CoreState>())
{}

MachineState StateMachine::getState() {
    return state_->getState();
}

common::Config const& StateMachine::getConfig() {
    return state_->getConfig();
}

Plan const& StateMachine::getPlan() {
    return state_->getPlan();
}

storage::Storage const& StateMachine::getData() {
    return state_->getData();
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

void StateMachine::setState(StatePtr new_state) {
    state_ = move(new_state);
}

StatePtr& StateMachine::accessCore() {
    return core_;
}

// THIS CODE IS FAKE AND MUST NOT BE EXECUTED
// IT'S SOLE PURPOSE IS TO INSTANTIATE ALL NEEDED TEMPLATE VARIANTS IN COMPILE TIME
template <size_t _idx>
class GenRecursive {
public:
    static int exec() {
        constexpr auto iter = MachineState::_values();
        constexpr auto start = iter.begin() + _idx;
        constexpr auto end = iter.end();
        GenericState<start->_value>();
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

#define THROW_ERROR(fname, S) \
    std::stringstream message; \
    message << "Cannot execute " << #fname << " from " \
    << MachineState::_from_integral(S)._to_string(); \
    throw common::StateViolationError(message.str());

/*!
 * Defines member function that throws exception
 */
#define STATE_DEFAULT_THROW(retv, fname, ...) \
template <MachineStateType S> \
retv GenericState<S>::fname(__VA_ARGS__) { \
    THROW_ERROR(fname, S); \
}

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
    return machine_->accessCore()->getConfig();
}

STATE_DEFAULT_THROW(Plan const&, getPlan)

STATE_DEFAULT_THROW(storage::Storage const&, getData)

STATE_DEFAULT_THROW(void, setConfig, json const&)

STATE_DEFAULT_THROW(void, setPlan, Plan)

STATE_DEFAULT_THROW(void, runNext)

STATE_DEFAULT_THROW(void, stop)

template <>
common::Config const& GenericState<MachineState::NotReady>::getConfig() {
    THROW_ERROR(getConfig, MachineState::NotReady);
}

template <>
Plan const& GenericState<MachineState::NoPlan>::getPlan() {
    return machine_->accessCore()->getPlan();
}

template <>
void GenericState<MachineState::NoPlan>::setConfig(json const& data) {
    return machine_->accessCore()->setPlan(data);
}

template <>
void GenericState<MachineState::NoPlan>::setPlan(Plan new_plan) {
    if(!new_plan.empty())
    {
        machine_->accessCore()->setPlan(new_plan);
        machine_->setState(createState<MachineState::HasPlan>(machine_));
    }
}

template <>
void GenericState<MachineState::HasPlan>::setPlan(Plan new_plan) {
    bool empty = new_plan.empty();
    machine_->accessCore()->setPlan(move(new_plan));
    if(empty)
    {
        machine_->setState(createState<MachineState::NoPlan>(machine_));
    }
}

template <>
Plan const& GenericState<MachineState::HasPlan>::getPlan() {
    return machine_->accessCore()->getPlan();
}

template <>
void GenericState<MachineState::HasPlan>::runNext() {
    machine_->accessCore()->runNext();
    machine_->setState(createState<MachineState::Executing>(machine_));
}

template <>
Plan const& GenericState<MachineState::Executing>::getPlan() {
    return machine_->accessCore()->getPlan(); // TODO: cut alredy done blocks from plan
}

template <>
void GenericState<MachineState::Executing>::runNext() {
    return machine_->accessCore()->runNext();
}

template <>
storage::Storage const& GenericState<MachineState::Executing>::getData() {
    return machine_->accessCore()->getData();
}

template <>
storage::Storage const& GenericState<MachineState::HasPlan>::getData() {
    return machine_->accessCore()->getData();
}

template <>
void GenericState<MachineState::Executing>::stop() {
    machine_->accessCore()->stop();
    machine_->setState(createState<MachineState::HasPlan>(machine_));
}
