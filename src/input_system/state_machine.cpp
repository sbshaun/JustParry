#include "state_machine.hpp"
#include "../ecs/ecs_registry.hpp"

bool StateMachine::transition(Entity entity, PlayerState newState) {
    // check if the new state is the same as current 
    if (newState == currentState) return false;
    if (states[newState] == nullptr) return false;
    // check if the new state is valid to transition to 
    if (!states[currentState]->canTransitionTo(newState)) return false;

    states[currentState]->exit(entity); // 1. exit the current state 
    currentState = newState; // 2. set current to new state 
    states[currentState]->enter(entity); // 3. enter new state 
    return true;
}

void StateMachine::update(Entity entity, float dt) {
    // call update on the state object of the current state. 
    states[currentState]->update(entity, dt);
}

// TODO: implement concrete state classes
void IdleState::enter(Entity entity) {
    // Set animation to idle
    std::cout << "Entering Idle State" << std::endl;
}

void IdleState::exit(Entity entity) {
    std::cout << "Exiting Idle State" << std::endl;
}

void IdleState::update(Entity entity, float dt) {
    // Check for input to transition to other states
}

bool IdleState::canTransitionTo(PlayerState newState) {
    return newState != PlayerState::IDLE;
}

void WalkingState::enter(Entity entity) {
    // Set animation to walking
    std::cout << "Entering Walking State" << std::endl;
}

void WalkingState::exit(Entity entity) {
    std::cout << "Exiting Walking State" << std::endl;
}

void WalkingState::update(Entity entity, float dt) {
    // Update position based on input
}

bool WalkingState::canTransitionTo(PlayerState newState) {
    return newState != PlayerState::WALKING;
}

void JumpingState::enter(Entity entity) {
    // Set initial jump velocity
    std::cout << "Entering Jumping State" << std::endl;
}

void JumpingState::exit(Entity entity) {
    std::cout << "Exiting Jumping State" << std::endl;
}

void JumpingState::update(Entity entity, float dt) {
    // add gravity, check for landing collision 
}

bool JumpingState::canTransitionTo(PlayerState newState) {
    return newState != PlayerState::JUMPING;
} 

void AttackingState::enter(Entity entity) {
    // add attack animation
    std::cout << "Entering Attacking State" << std::endl;
}

void AttackingState::exit(Entity entity) {
    std::cout << "Exiting Attacking State" << std::endl;
}

void AttackingState::update(Entity entity, float dt) {
    // Check for hit detection, transition to recovery
}

bool AttackingState::canTransitionTo(PlayerState newState) {
    return newState != PlayerState::ATTACKING;
}

void CrouchingState::enter(Entity entity) {
    // add crouch animation
    std::cout << "Entering Crouching State" << std::endl;
}

void CrouchingState::exit(Entity entity) {
    std::cout << "Exiting Crouching State" << std::endl;
}

void CrouchingState::update(Entity entity, float dt) {
    // Check for input to transition to other states
}

bool CrouchingState::canTransitionTo(PlayerState newState) {
    return newState != PlayerState::CROUCHING;
}

void ParryingState::enter(Entity entity) {
    // add parry animation
    std::cout << "Entering Parrying State" << std::endl;
}

void ParryingState::exit(Entity entity) {
    std::cout << "Exiting Parrying State" << std::endl;
}

void ParryingState::update(Entity entity, float dt) {
    // Check for successful parry, transition to counter or recovery
}

bool ParryingState::canTransitionTo(PlayerState newState) {
    return newState != PlayerState::PARRYING;
}

StateMachine createPlayerStateMachine() {
    StateMachine state_machine;
    state_machine.addState(PlayerState::IDLE, std::make_unique<IdleState>());
    state_machine.addState(PlayerState::WALKING, std::make_unique<WalkingState>());
    state_machine.addState(PlayerState::JUMPING, std::make_unique<JumpingState>());
    state_machine.addState(PlayerState::ATTACKING, std::make_unique<AttackingState>());
    state_machine.addState(PlayerState::PARRYING, std::make_unique<ParryingState>());
    // TODO: add more states as needed. 
    return state_machine;
}