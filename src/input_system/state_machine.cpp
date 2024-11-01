#include "state_machine.hpp"
#include "../ecs/ecs_registry.hpp"
#include "../constants.hpp"

bool StateMachine::transition(Entity entity, PlayerState newState)
{
    // check if the new state is the same as current
    if (states[newState] == nullptr)
        return false;
    // check if the new state is valid to transition to
    if (!states[currentState]->canTransitionTo(entity, newState))
        return false;

    states[currentState]->exit(entity, *this);  // 1. exit the current state
    currentState = newState;                    // 2. set current to new state
    states[currentState]->enter(entity, *this); // 3. enter new state
    return true;
}

void StateMachine::update(Entity entity, float elapsed_ms)
{
    PlayerCurrentState& playerState = registry.playerCurrentStates.get(entity);
    // for some reason, the currentState in state machine transitions to IDLE immediately after STUNNED using the playerState.currentState to check for the current state instead.
    // call update on the state object of the current state.
    states[playerState.currentState]->update(entity, elapsed_ms, *this);
}

// TODO: implement concrete state classes
void IdleState::enter(Entity entity, StateMachine &stateMachine)
{
    // TODO: add animation to idle?
}

void IdleState::exit(Entity entity, StateMachine &stateMachine)
{
    // do nothing
}

void IdleState::update(Entity entity, float elapsed_ms, StateMachine &stateMachine)
{
    // do nothing
}

bool IdleState::canTransitionTo(Entity entity, PlayerState newState)
{
    return newState != PlayerState::IDLE;
}

void WalkingState::enter(Entity entity, StateMachine &stateMachine)
{
    // Set animation to walking
}

void WalkingState::exit(Entity entity, StateMachine &stateMachine)
{
}

void WalkingState::update(Entity entity, float elapsed_ms, StateMachine &stateMachine)
{
    Motion &motion = registry.motions.get(entity);
    if (motion.velocity.x == 0)
    {
        // stateMachine.transition(entity, PlayerState::IDLE);
    }
}

bool WalkingState::canTransitionTo(Entity entity, PlayerState newState)
{
    return true;
}

void JumpingState::enter(Entity entity, StateMachine &stateMachine)
{
    // Set initial jump velocity
}

void JumpingState::exit(Entity entity, StateMachine &stateMachine)
{
}

void JumpingState::update(Entity entity, float elapsed_ms, StateMachine &stateMachine)
{
    // add gravity, check for landing collision
    Motion &motion = registry.motions.get(entity);
    if (!motion.inAir)
    {
        stateMachine.transition(entity, PlayerState::IDLE);
    }
}

bool JumpingState::canTransitionTo(Entity entity, PlayerState newState)
{
    return newState != PlayerState::JUMPING;
}

void AttackingState::enter(Entity entity, StateMachine& stateMachine) {
    std::cout << "Entering Attacking State" << std::endl;
    // add attack animation
    Fighters fighter = registry.players.get(entity).current_char;
    float HITBOX_DURATION = FighterManager::getFighterConfig(fighter).HITBOX_DURATION;

    // 1. register a state timer
    PlayerCurrentState& playerState = registry.playerCurrentStates.get(entity);
    playerState.currentState = PlayerState::ATTACKING;
    StateTimer &playerStateTimer = registry.stateTimers.get(entity);
    playerStateTimer.reset(HITBOX_DURATION);

    HitBox &playerHitBox = registry.hitBoxes.get(entity);
    playerHitBox.active = true;
    playerHitBox.hit = false;
}

void AttackingState::exit(Entity entity, StateMachine &stateMachine)
{
    PlayerCurrentState &playerState = registry.playerCurrentStates.get(entity);
    playerState.currentState = PlayerState::IDLE;

    HitBox &playerHitBox = registry.hitBoxes.get(entity);
    playerHitBox.active = false;
    playerHitBox.hit = false;
}

void AttackingState::update(Entity entity, float elapsed_ms, StateMachine &stateMachine)
{
    // when state timer is expired, transition to idle
    StateTimer &playerStateTimer = registry.stateTimers.get(entity);
    if (playerStateTimer.isAlive())
    {
        playerStateTimer.update(elapsed_ms);
    }
    else
    {
        stateMachine.transition(entity, PlayerState::IDLE);
    }
}

bool AttackingState::canTransitionTo(Entity entity, PlayerState newState)
{
    StateTimer &playerStateTimer = registry.stateTimers.get(entity);
    if (playerStateTimer.isAlive())
        return false; // still in current state
    return newState != PlayerState::ATTACKING;
}

void CrouchingState::enter(Entity entity, StateMachine &stateMachine)
{
    // add crouch animation
}

void CrouchingState::exit(Entity entity, StateMachine &stateMachine)
{
}

void CrouchingState::update(Entity entity, float elapsed_ms, StateMachine &stateMachine)
{
    // Check for input to transition to other states
}

bool CrouchingState::canTransitionTo(Entity entity, PlayerState newState)
{
    return newState != PlayerState::CROUCHING;
    // && newState != PlayerState::JUMPING;
}

void ParryingState::enter(Entity entity, StateMachine &stateMachine)
{
    // add parry animation
}

void ParryingState::exit(Entity entity, StateMachine &stateMachine)
{
}

void ParryingState::update(Entity entity, float elapsed_ms, StateMachine &stateMachine)
{
    // Check for successful parry, transition to counter or recovery
}

bool ParryingState::canTransitionTo(Entity entity, PlayerState newState)
{
    return newState != PlayerState::PARRYING;
}

void StunnedState::enter(Entity entity, StateMachine& stateMachine) {
    std::cout << "Entering Stunned State" << std::endl;
}

void StunnedState::exit(Entity entity, StateMachine& stateMachine) {
    std::cout << "Exiting Stunned State" << std::endl;
}

void StunnedState::update(Entity entity, float elapsed_ms, StateMachine& stateMachine) {
    StateTimer& playerStateTimer = registry.stateTimers.get(entity);
    if (playerStateTimer.isAlive()) {
        playerStateTimer.update(elapsed_ms);
    } else {
        PlayerCurrentState& playerState = registry.playerCurrentStates.get(entity);
        playerState.currentState = PlayerState::IDLE;
        stateMachine.transition(entity, PlayerState::IDLE);
    }
}

bool StunnedState::canTransitionTo(Entity entity, PlayerState newState) {
    StateTimer& playerStateTimer = registry.stateTimers.get(entity);
    if (playerStateTimer.isAlive()) return false; // still in current state
    return newState != PlayerState::STUNNED;
}