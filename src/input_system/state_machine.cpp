#include "state_machine.hpp"
#include "../ecs/ecs_registry.hpp"

bool StateMachine::transition(PlayerState newState) {
    // check if the new state is valid to transition to 
    if (!states[currentState]->canTransitionTo(newState)) return false;
    // check if the new state is the same as current 
    if (newState == currentState) return;
    if (states[newState] == nullptr) return;

    // 1. exit the current state 
    states[currentState]->exit();
    // 2. set current to new state 
    currentState = newState;
    // 3. enter new state 
    states[currentState]->enter();
}

void StateMachine::update(float dt) {
    // call update on the state object of the current state. 
    states[currentState]->update(dt);
}

// TODO: implement concrete state classes
void IdleState::enter() {
    // Set animation to idle
    std::cout << "Entering Idle State" << std::endl;
}

void IdleState::exit() {
    std::cout << "Exiting Idle State" << std::endl;
}

void IdleState::update(float dt) {
    // Check for input to transition to other states
}

bool IdleState::canTransitionTo(PlayerState newState) {
    return newState != PlayerState::IDLE;
}

void WalkingState::enter() {
    // Set animation to walking
    std::cout << "Entering Walking State" << std::endl;
}

void WalkingState::exit() {
    std::cout << "Exiting Walking State" << std::endl;
}

void WalkingState::update(float dt) {
    // Update position based on input
}

bool WalkingState::canTransitionTo(PlayerState newState) {
    return newState != PlayerState::WALKING;
}

void JumpingState::enter() {
    // Set initial jump velocity
    std::cout << "Entering Jumping State" << std::endl;
}

void JumpingState::exit() {
    std::cout << "Exiting Jumping State" << std::endl;
}

void JumpingState::update(float dt) {
    // add gravity, check for landing collision 
}

void AttackingState::enter() {
    // add attack animation
    std::cout << "Entering Attacking State" << std::endl;
}

void AttackingState::exit() {
    std::cout << "Exiting Attacking State" << std::endl;
}

void AttackingState::update(float dt) {
    // Check for hit detection, transition to recovery
}

bool AttackingState::canTransitionTo(PlayerState newState) {
    return newState != PlayerState::ATTACKING;
}

void ParryingState::enter() {
    // add parry animation
    std::cout << "Entering Parrying State" << std::endl;
}

void ParryingState::exit() {
    std::cout << "Exiting Parrying State" << std::endl;
}

void ParryingState::update(float dt) {
    // Check for successful parry, transition to counter or recovery
}

bool ParryingState::canTransitionTo(PlayerState newState) {
    return newState != PlayerState::PARRYING;
}

StateMachine createPlayerStateMachine() {
    StateMachine state_machine;
    state_machine.addState(PlayerState::IDLE, new IdleState());
    state_machine.addState(PlayerState::WALKING, new WalkingState());
    state_machine.addState(PlayerState::JUMPING, new JumpingState());
    state_machine.addState(PlayerState::ATTACKING, new AttackingState());
    state_machine.addState(PlayerState::PARRYING, new ParryingState());
    // TODO: add more states as needed. 
    return state_machine;
}