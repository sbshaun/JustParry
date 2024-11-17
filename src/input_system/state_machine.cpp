#include "state_machine.hpp"
#include "../ecs/ecs_registry.hpp"
#include "../constants.hpp"
#include "../world/world_system.hpp"

bool StateMachine::transition(Entity entity, PlayerState newState)
{
	PlayerCurrentState& playerState = registry.playerCurrentStates.get(entity);
	if (playerState.currentState == newState)
		return false;
	// check if the new state is the same as current
	if (states[newState] == nullptr)
		return false;
	// check if the new state is valid to transition to
	if (!states[playerState.currentState]->canTransitionTo(entity, newState))
		return false;

	states[playerState.currentState]->exit(entity, *this);  // 1. exit the current state
	previousState = currentState;               // 2. set previous to current
	playerState.currentState = newState;                    // 2. set current to new state
	states[playerState.currentState]->enter(entity, *this); // 3. enter new state
	return true;
}

void StateMachine::update(Entity entity, float elapsed_ms)
{
	PlayerCurrentState& playerState = registry.playerCurrentStates.get(entity);
	// for some reason, the currentState in state machine transitions to IDLE immediately after STUNNED using the playerState.currentState to check for the current state instead.
	// call update on the state object of the current state.
	Animation& animation = registry.animations.get(entity);
	animation.currentFrame = (animation.currentFrame + 1) % 60;
	states[playerState.currentState]->update(entity, elapsed_ms, *this);
}

// TODO: implement concrete state classes
void IdleState::enter(Entity entity, StateMachine& stateMachine)
{
	Fighters fighter = registry.players.get(entity).current_char;
	const FighterConfig& fighterConfig = FighterManager::getFighterConfig(fighter);
}

void IdleState::exit(Entity entity, StateMachine& stateMachine)
{
	// TODO: Clean up animation data? Each state should be fine to reset it on enter
}

void IdleState::update(Entity entity, float elapsed_ms, StateMachine& stateMachine)
{
	Fighters fighter = registry.players.get(entity).current_char;
	const FighterConfig& fighterConfig = FighterManager::getFighterConfig(fighter);
	Animation& animation = registry.animations.get(entity);
	int frameIndex = (animation.currentFrame / 12) % 4;
	switch (frameIndex) {
	case 0: animation.currentTexture = fighterConfig.m_bird_idle_f4_texture; break;
	case 1: animation.currentTexture = fighterConfig.m_bird_idle_f3_texture; break;
	case 2: animation.currentTexture = fighterConfig.m_bird_idle_f2_texture; break;
	default: animation.currentTexture = fighterConfig.m_bird_idle_f1_texture; break;
	}
}

bool IdleState::canTransitionTo(Entity entity, PlayerState newState)
{
	return newState != PlayerState::IDLE;
}

void WalkingState::enter(Entity entity, StateMachine& stateMachine)
{
	Fighters fighter = registry.players.get(entity).current_char;
	const FighterConfig& fighterConfig = FighterManager::getFighterConfig(fighter);
	Animation& animation = registry.animations.get(entity);
}

void WalkingState::exit(Entity entity, StateMachine& stateMachine)
{
	Fighters fighter = registry.players.get(entity).current_char;
	const FighterConfig& fighterConfig = FighterManager::getFighterConfig(fighter);
	Animation& animation = registry.animations.get(entity);
	//animation.currentTexture = fighterConfig.m_bird_idle_f2_texture;
}

void WalkingState::update(Entity entity, float elapsed_ms, StateMachine& stateMachine)
{
	Fighters fighter = registry.players.get(entity).current_char;
	const FighterConfig& fighterConfig = FighterManager::getFighterConfig(fighter);
	Animation& animation = registry.animations.get(entity);
	int frameIndex = (animation.currentFrame / 12) % 4;

	switch (frameIndex) {
	case 0: animation.currentTexture = fighterConfig.m_bird_walk_f4_texture; break;
	case 1: animation.currentTexture = fighterConfig.m_bird_walk_f3_texture; break;
	case 2: animation.currentTexture = fighterConfig.m_bird_walk_f2_texture; break;
	default: animation.currentTexture = fighterConfig.m_bird_walk_f1_texture; break;
	}
}

bool WalkingState::canTransitionTo(Entity entity, PlayerState newState)
{
	return newState != PlayerState::WALKING;
}

//void JumpingState::enter(Entity entity, StateMachine &stateMachine)
//{
//    // Set initial jump velocity
//    Fighters fighter = registry.players.get(entity).current_char;
//    const FighterConfig& fighterConfig = FighterManager::getFighterConfig(fighter);
//    Animation& animation = registry.animations.get(entity);
//    animation.currentFrame = 0;
//    animation.currentTexture = fighterConfig.m_bird_idle_f1_texture;
//}

//void JumpingState::exit(Entity entity, StateMachine &stateMachine)
//{
//}
//
//void JumpingState::update(Entity entity, float elapsed_ms, StateMachine &stateMachine)
//{
//    // add gravity, check for landing collision
//    Motion &motion = registry.motions.get(entity);
//    if (!motion.inAir)
//    {
//        stateMachine.transition(entity, PlayerState::IDLE);
//    }
//}
//
//bool JumpingState::canTransitionTo(Entity entity, PlayerState newState)
//{
//    return newState != PlayerState::JUMPING;
//}

void AttackingState::enter(Entity entity, StateMachine& stateMachine)
{
	Player& player = registry.players.get(entity);
	std::cout << "Player " << player.id << " attacks!" << std::endl;

	// add attack animation
	Fighters fighter = registry.players.get(entity).current_char;
	const FighterConfig& fighterConfig = FighterManager::getFighterConfig(fighter);

	// 1. register a state timer
	PlayerCurrentState& playerState = registry.playerCurrentStates.get(entity);
	playerState.currentState = PlayerState::ATTACKING;
	StateTimer& playerStateTimer = registry.stateTimers.get(entity);
	playerStateTimer.reset(fighterConfig.HITBOX_DURATION);

	Motion& motion = registry.motions.get(entity);

	HitBox& hitBox = registry.hitBoxes.get(entity);
	hitBox.active = true;
	hitBox.hit = false;
	hitBox.width = 0; // fighterConfig.PUNCH_WIDTH;
	hitBox.height = fighterConfig.PUNCH_HEIGHT;
	hitBox.yOffset = fighterConfig.PUNCH_Y_OFFSET;
	float baseOffset = fighterConfig.PUNCH_X_OFFSET; // set base offset and adjust based on player direction 
	hitBox.xOffset = motion.direction ? baseOffset : -baseOffset;

    Animation& animation = registry.animations.get(entity);
    animation.currentFrame = 0;
}

void AttackingState::exit(Entity entity, StateMachine& stateMachine)
{
	// make sure the state timer is not alive 
	StateTimer& playerStateTimer = registry.stateTimers.get(entity);
	playerStateTimer.reset(0);

	PlayerCurrentState& playerState = registry.playerCurrentStates.get(entity);
	playerState.currentState = PlayerState::IDLE;

	HitBox& playerHitBox = registry.hitBoxes.get(entity);
	playerHitBox.active = false;
	playerHitBox.hit = false;

	Animation& animation = registry.animations.get(entity);
	animation.playedOnce = false;
}

void AttackingState::update(Entity entity, float elapsed_ms, StateMachine& stateMachine)
{
	// increase the width of the hitbox over time, max width is the fighter's PUNCH_WIDTH
	HitBox& hitBox = registry.hitBoxes.get(entity);
	Fighters fighter = registry.players.get(entity).current_char;
	const FighterConfig& fighterConfig = FighterManager::getFighterConfig(fighter);

	if (hitBox.width < fighterConfig.PUNCH_WIDTH)
	{
		hitBox.width += fighterConfig.PUNCH_WIDTH / (fighterConfig.HITBOX_DURATION / 4) * elapsed_ms;
	}

	Animation& animation = registry.animations.get(entity);
	if (animation.currentFrame < 12 && animation.playedOnce == false) {
		animation.currentTexture = fighterConfig.m_bird_punch_f1_texture;
	}
	else {
		animation.currentTexture = fighterConfig.m_bird_punch_f2_texture;
		animation.playedOnce = true;
	}

	animation.currentFrame = animation.currentFrame + 1;

	// when state timer is expired, transition to idle
	StateTimer& playerStateTimer = registry.stateTimers.get(entity);
	if (playerStateTimer.isAlive())
	{
		playerStateTimer.update(elapsed_ms);
	}
	else
	{
		// TODO: Transition to a winding down state, play winding down animation THEN go to idle
		stateMachine.transition(entity, PlayerState::IDLE);
	}
}

bool AttackingState::canTransitionTo(Entity entity, PlayerState newState)
{
	if (newState == PlayerState::STUNNED)
		return true; // if being parried during attacck, override and transition to STUNNED 

	StateTimer& playerStateTimer = registry.stateTimers.get(entity);
	if (playerStateTimer.isAlive())
		return false; // still in current state

	return newState != PlayerState::ATTACKING;
}

void CrouchingState::enter(Entity entity, StateMachine& stateMachine)
{
	// add crouch animation
}

void CrouchingState::exit(Entity entity, StateMachine& stateMachine)
{
}

void CrouchingState::update(Entity entity, float elapsed_ms, StateMachine& stateMachine)
{
	// Check for input to transition to other states
}

bool CrouchingState::canTransitionTo(Entity entity, PlayerState newState)
{
	return newState != PlayerState::CROUCHING;
	// && newState != PlayerState::JUMPING;
}

void ParryingState::enter(Entity entity, StateMachine& stateMachine)
{
	// similar to attacking state, register a state timer, and activate parry box 
	Player& player = registry.players.get(entity);
	std::cout << "Player " << player.id << " parries!" << std::endl;

	Fighters fighter = registry.players.get(entity).current_char;
	const FighterConfig& fighterConfig = FighterManager::getFighterConfig(fighter);
	Animation& animation = registry.animations.get(entity);
	animation.currentFrame = 0;
	animation.currentTexture = fighterConfig.m_bird_punch_f1_texture;

	float PARRY_BOX_DURATION = fighterConfig.PARRY_DURATION;

	// 1. register a state timer
	PlayerCurrentState& playerState = registry.playerCurrentStates.get(entity);
	playerState.currentState = PlayerState::PARRYING;

	ParryBox& playerParryBox = registry.parryBoxes.get(entity);
	playerParryBox.active = true;

	StateTimer& playerStateTimer = registry.stateTimers.get(entity);
	playerStateTimer.reset(PARRY_BOX_DURATION);
}

void ParryingState::exit(Entity entity, StateMachine& stateMachine)
{
	std::cout << "Exiting Parrying State" << std::endl;
	PlayerCurrentState& playerState = registry.playerCurrentStates.get(entity);
	playerState.currentState = PlayerState::IDLE;

	ParryBox& playerParryBox = registry.parryBoxes.get(entity);
	playerParryBox.active = false;
}

void ParryingState::update(Entity entity, float elapsed_ms, StateMachine& stateMachine)
{
	// Check for successful parry, transition to counter or recovery 
	StateTimer& playerStateTimer = registry.stateTimers.get(entity);

	Fighters fighter = registry.players.get(entity).current_char;
	const FighterConfig& fighterConfig = FighterManager::getFighterConfig(fighter);
	//std::cout << "Parrying State Timer: " << playerStateTimer.elapsedTime << std::endl;
	if (playerStateTimer.isAlive())
	{
		playerStateTimer.update(elapsed_ms);

		// While the parrying state is still active, check if the 
		ParryBox& playerParryBox = registry.parryBoxes.get(entity);
		if (playerStateTimer.elapsedTime < fighterConfig.PERFECT_PARRY_DURATION) {
			playerParryBox.perfectParry = true;
		}
		else {
			playerParryBox.perfectParry = false;
		}
		// std::cout << "Perfect Parry State: " << playerParryBox.perfectParry << std::endl;
	}
	else
	{
		PlayerCurrentState& playerState = registry.playerCurrentStates.get(entity);
		playerState.currentState = PlayerState::IDLE;
		stateMachine.transition(entity, PlayerState::IDLE);
	}
}

bool ParryingState::canTransitionTo(Entity entity, PlayerState newState)
{
	if (newState == PlayerState::BLOCKSTUNNED)
		return true; //if being parried during outside of perfect parry window, override and transition to BLOCKSTUNNED 

	StateTimer& playerStateTimer = registry.stateTimers.get(entity);
	if (playerStateTimer.isAlive())
		return false; // still in current state
	return newState != PlayerState::PARRYING;
}

void StunnedState::enter(Entity entity, StateMachine& stateMachine)
{
	Player& player = registry.players.get(entity);
	std::cout << "Player " << player.id << " is stunned!" << std::endl;

	Fighters fighter = registry.players.get(entity).current_char;
	const FighterConfig& fighterConfig = FighterManager::getFighterConfig(fighter);
	Animation& animation = registry.animations.get(entity);
	animation.currentTexture = fighterConfig.m_bird_stun_f1_texture;

	float STUN_DURATION;
	if (stateMachine.getPreviousState() == PlayerState::ATTACKING)
	{
		STUN_DURATION = FighterManager::getFighterConfig(fighter).PARRY_STUN_DURATION;
	}
	else
	{
		STUN_DURATION = FighterManager::getFighterConfig(fighter).PUNCH_STUN_DURATION;
	}

	// 1. register a state timer
	PlayerCurrentState& playerState = registry.playerCurrentStates.get(entity);
	playerState.currentState = PlayerState::STUNNED;
	StateTimer& playerStateTimer = registry.stateTimers.get(entity);
	playerStateTimer.reset(STUN_DURATION);
}

void StunnedState::exit(Entity entity, StateMachine& stateMachine)
{
	std::cout << "Exiting Stunned State" << std::endl;
}

void StunnedState::update(Entity entity, float elapsed_ms, StateMachine& stateMachine)
{
	StateTimer& playerStateTimer = registry.stateTimers.get(entity);
	if (playerStateTimer.isAlive())
	{
		playerStateTimer.update(elapsed_ms);
	}
	else
	{
		PlayerCurrentState& playerState = registry.playerCurrentStates.get(entity);
		playerState.currentState = PlayerState::IDLE;
		stateMachine.transition(entity, PlayerState::IDLE);
	}
}

bool StunnedState::canTransitionTo(Entity entity, PlayerState newState)
{
	StateTimer& playerStateTimer = registry.stateTimers.get(entity);
	if (playerStateTimer.isAlive())
		return false; // still in current state
	return newState != PlayerState::STUNNED;
}

void BlockStunnedState::enter(Entity entity, StateMachine& stateMachine)
{
	Player& player = registry.players.get(entity);
	std::cout << "Player " << player.id << " is block stunned!" << std::endl;

	Fighters fighter = registry.players.get(entity).current_char;
	const FighterConfig& fighterConfig = FighterManager::getFighterConfig(fighter);
	Animation& animation = registry.animations.get(entity);

	float STUN_DURATION;
	if (stateMachine.getPreviousState() == PlayerState::PARRYING)
	{
		STUN_DURATION = FighterManager::getFighterConfig(fighter).BLOCK_STUN_DURATION;
	}
	else
	{
		STUN_DURATION = FighterManager::getFighterConfig(fighter).BLOCK_STUN_DURATION;
	}

	// 1. register a state timer
	PlayerCurrentState& playerState = registry.playerCurrentStates.get(entity);
	playerState.currentState = PlayerState::BLOCKSTUNNED;
	StateTimer& playerStateTimer = registry.stateTimers.get(entity);
	playerStateTimer.reset(STUN_DURATION);
}

void BlockStunnedState::exit(Entity entity, StateMachine& stateMachine)
{
	std::cout << "Exiting Stunned State" << std::endl;
}

void BlockStunnedState::update(Entity entity, float elapsed_ms, StateMachine& stateMachine)
{
	StateTimer& playerStateTimer = registry.stateTimers.get(entity);
	if (playerStateTimer.isAlive())
	{
		playerStateTimer.update(elapsed_ms);
	}
	else
	{
		PlayerCurrentState& playerState = registry.playerCurrentStates.get(entity);
		playerState.currentState = PlayerState::IDLE;
		stateMachine.transition(entity, PlayerState::IDLE);
	}
}

bool BlockStunnedState::canTransitionTo(Entity entity, PlayerState newState)
{
	StateTimer& playerStateTimer = registry.stateTimers.get(entity);
	if (playerStateTimer.isAlive())
		return false; // still in current state
	return newState != PlayerState::BLOCKSTUNNED;
}
