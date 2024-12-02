#include "world_system.hpp"
#include "world_init.hpp"
#include "../state/game.hpp"
#include "../physics/physics_system.hpp"
#include "../constants.hpp"
#include "../input_system/input_utils.hpp"
#include "../input_system/controller_handler.hpp"

Mix_Music *WorldSystem::background_music = nullptr;
Mix_Chunk *WorldSystem::punch_sound = nullptr;
Mix_Chunk *WorldSystem::walk_sound = nullptr;
Mix_Chunk *WorldSystem::parry_sound = nullptr;

Mix_Chunk *WorldSystem::crouch_sound = nullptr;
Mix_Chunk *WorldSystem::parry_blocked_sound = nullptr;
Mix_Chunk *WorldSystem::perfect_parry_sound = nullptr;
Mix_Chunk *WorldSystem::hurt_sound = nullptr;
Mix_Chunk *WorldSystem::kick_sound = nullptr;
Mix_Chunk *WorldSystem::menu_select_sound = nullptr;
Mix_Chunk *WorldSystem::menu_confirm_sound = nullptr;
Mix_Chunk *WorldSystem::game_count_down_sound = nullptr;
bool WorldSystem::isPlayerWalking = false;
float WorldSystem::walkStopTimer = 0.f;
static Entity player1;
static Entity player2;

/* notes:
1. a helper function to check if player is movable.
2. static: scoped to this file, since it's just a small helper function
3. placed at the top for it to be declared before being used, otherwise compiler error.
*/
static bool canMove(PlayerState state)
{
    return state != PlayerState::ATTACKING && state != PlayerState::KICKING && state != PlayerState::STUNNED && state != PlayerState::BLOCKSTUNNED && state != PlayerState::RECOVERING && state != PlayerState::PARRYING && state != PlayerState::PERFECT_PARRYING && state != PlayerState::COUNTER_ATTACKING;
}

/*
helper funciton to apply damage to a player
*/
static void applyDamage(Entity player, float damage)
{
    Health &health = registry.healths.get(player);

    health.currentHealth -= damage;

    if (health.currentHealth <= 0)
    {
        health.currentHealth = 0;
    }
    WorldSystem::playHurtSound();
}

WorldSystem::WorldSystem() {};

WorldSystem::~WorldSystem()
{
    // Clean up all shaders first
    if (renderer)
    {
        cleanupShaders(renderer->m_player1);
        cleanupShaders(renderer->m_player2);
        this->renderer = nullptr;
    }

    if (player1Motion)
    {
        this->player1Motion = nullptr;
    }
    if (player2Motion)
    {
        this->player2Motion = nullptr;
    }

    if (player1CollisionBox)
    {
        this->player1CollisionBox = nullptr;
    }

    if (player2CollisionBox)
    {
        this->player2CollisionBox = nullptr;
    }

    // Clear the input handlers and state machines
    player1InputHandler.reset(); // std::unique_ptr automatically frees memory, but it's good practice to reset
    player2InputHandler.reset();
    player1StateMachine.reset();
    player2StateMachine.reset();

    // Clear all components
    registry.clear_all_components();

    if (background_music != nullptr)
    {
        Mix_FreeMusic(background_music);
        background_music = nullptr;
    }
    if (punch_sound != nullptr)
    {
        Mix_FreeChunk(punch_sound);
        punch_sound = nullptr;
    }
    if (walk_sound != nullptr)
    {
        Mix_FreeChunk(walk_sound);
        walk_sound = nullptr;
    }
    if (parry_sound != nullptr)
    {
        Mix_FreeChunk(parry_sound);
        parry_sound = nullptr;
    }
    if (crouch_sound != nullptr)
    {
        Mix_FreeChunk(crouch_sound);
        crouch_sound = nullptr;
    }
    if (parry_blocked_sound != nullptr)
    {
        Mix_FreeChunk(parry_blocked_sound);
        parry_blocked_sound = nullptr;
    }
    if (perfect_parry_sound != nullptr)
    {
        Mix_FreeChunk(perfect_parry_sound);
        perfect_parry_sound = nullptr;
    }
    if (hurt_sound != nullptr)
    {
        Mix_FreeChunk(hurt_sound);
        hurt_sound = nullptr;
    }
    if (kick_sound != nullptr)
    {
        Mix_FreeChunk(kick_sound);
        kick_sound = nullptr;
    }
    if (menu_select_sound != nullptr)
    {
        Mix_FreeChunk(menu_select_sound);
        menu_select_sound = nullptr;
    }
    if (menu_confirm_sound != nullptr)
    {
        Mix_FreeChunk(menu_confirm_sound);
        menu_confirm_sound = nullptr;
    }
    if (game_count_down_sound != nullptr)
    {
        Mix_FreeChunk(game_count_down_sound);
        game_count_down_sound = nullptr;
    }

    std::cout << "WorldSystem cleaned up." << std::endl;
}

void WorldSystem::init(GlRender *renderer)
{
    this->renderer = renderer;

    //////////////////////////////////////
    // Loading music and sounds with SDL
    if (SDL_Init(SDL_INIT_AUDIO) < 0)
    {
        fprintf(stderr, "Failed to initialize SDL Audio");
        exit(1);
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
    {
        fprintf(stderr, "Failed to open audio device");
        exit(1);
    }

    // Load background music
    background_music = Mix_LoadMUS(audio_path("background_music.wav").c_str());
    punch_sound = Mix_LoadWAV(audio_path("punch_sound.wav").c_str());
    walk_sound = Mix_LoadWAV(audio_path("walk_sound.wav").c_str());
    parry_sound = Mix_LoadWAV(audio_path("parry_sound.wav").c_str());
    crouch_sound = Mix_LoadWAV(audio_path("crouch_sound.wav").c_str());
    parry_blocked_sound = Mix_LoadWAV(audio_path("parry_blocked_sound.wav").c_str());
    perfect_parry_sound = Mix_LoadWAV(audio_path("perfect_parry_sound.wav").c_str());
    hurt_sound = Mix_LoadWAV(audio_path("hurt_sound.wav").c_str());
    kick_sound = Mix_LoadWAV(audio_path("kick_sound.wav").c_str());
    menu_select_sound = Mix_LoadWAV(audio_path("menu_select_sound.wav").c_str());
    menu_confirm_sound = Mix_LoadWAV(audio_path("menu_confirm_sound.wav").c_str());
    game_count_down_sound = Mix_LoadWAV(audio_path("game_count_down_sound.wav").c_str());


    if (background_music == nullptr || punch_sound == nullptr || walk_sound == nullptr || crouch_sound == nullptr ||
        parry_sound == nullptr || parry_blocked_sound == nullptr || perfect_parry_sound == nullptr || 
        hurt_sound == nullptr || kick_sound == nullptr || menu_select_sound == nullptr || 
        menu_confirm_sound == nullptr || game_count_down_sound == nullptr)
    {
        fprintf(stderr, "Failed to load sounds\n %s\n %s\n %s\n %s\n %s\n %s\n %s\n %s\n %s\n %s\n %s\n %s\n make sure the data directory is present \n",
                audio_path("background_music.wav").c_str(),
                audio_path("punch_sound.wav").c_str(),
                audio_path("walk_sound.wav").c_str(),
                audio_path("crouch_sound.wav").c_str(),
                audio_path("parry_sound.wav").c_str(),
                audio_path("parry_blocked_sound.wav").c_str(),
                audio_path("perfect_parry_sound.wav").c_str(),
                audio_path("hurt_sound.wav").c_str(),
                audio_path("kick_sound.wav").c_str(),
                audio_path("menu_select_sound.wav").c_str(),
                audio_path("menu_confirm_sound.wav").c_str(),
                audio_path("game_count_down_sound.wav").c_str());
        exit(1);
    }
    else
    {
        std::cout << "Sounds loaded" << std::endl;
    }

    // Initialize audio state based on settings
    updateVolume();
    updateAudioState();

    // Create entities
    Fighters fighter1 =  Game::getP1SelectedCharacter();
    Fighters fighter2 =  Game::getP2SelectedCharacter();
    std::cout << "Player 1: " << (int)fighter1 << std::endl;
    std::cout << "Player 2: " << (int)fighter2 << std::endl;
    player1 = createPlayer1(renderer, {-1.25, FLOOR_Y + birdman.NDC_HEIGHT}, fighter1);
    player2 = createPlayer2(renderer, {1.25, FLOOR_Y + birdmanConfig.NDC_HEIGHT}, fighter2);

    renderer->m_player1 = player1;
    renderer->m_player2 = player2;

    Entity boundaryRight = createBoundary(BOUND_SIZE_R, RIGHT);
    Entity boundaryLeft = createBoundary(BOUND_SIZE_L, LEFT);
    Entity boundaryFloor = createFloor(FLOOR_Y, FLOOR);

    // TODO: Figure out height dimensions correctly, floor_y value appears to shrink playable height too much
    Entity playableArea = createPlayableArea({0, 0.125}, M_WINDOW_WIDTH_PX * 0.9f, M_WINDOW_HEIGHT_PX * 0.875);
    renderer->m_playableArea = playableArea;

    // 1. init input handlers and state machines
    initInputHandlers();
    initStateMachines();

    Motion &p1 = registry.motions.get(renderer->m_player1);
    Motion &p2 = registry.motions.get(renderer->m_player2);
    this->player1Motion = &p1;
    this->player2Motion = &p2;

    CollisionBox &p1CollisionBox = registry.collisionBoxes.get(renderer->m_player1);
    CollisionBox &p2CollisionBox = registry.collisionBoxes.get(renderer->m_player2);
    this->player1CollisionBox = &p1CollisionBox;
    this->player2CollisionBox = &p2CollisionBox;

    bloodSystem = BloodParticleSystem();
    smokeSystem = SmokeParticleSystem();
	sparkleSystem = SparkleParticleSystem();
}
void WorldSystem::step(float elapsed_ms)
{
    // PARTICLES: ADD ALL RENDERS HERE
    bloodSystem.update(elapsed_ms);
    smokeSystem.update(elapsed_ms);
    sparkleSystem.update(elapsed_ms);
}

void WorldSystem::emitBloodParticles(float x, float y, float z, bool direction)
{
    bloodSystem.emit(x, y, z, direction);
}

void WorldSystem::emitSmokeParticles(float x, float y, float z)
{
    smokeSystem.emit(x, y, z, false);
}

void WorldSystem::emitSparkleParticles(float x, float y, float z)
{
	sparkleSystem.emit(x, y, z, false);
}

void WorldSystem::renderParticles()
{
    // PARTICLES: ADD ALL RENDERS HERE
    bloodSystem.render(renderer->m_worldModel);
    smokeSystem.render(renderer->m_worldModel);
	sparkleSystem.render(renderer->m_worldModel);
}

void WorldSystem::initInputHandlers()
{
    std::cout << "\nInitializing controls:" << std::endl;

    // Print Player 1 controls
    std::cout << "\nPlayer 1 Controls:" << std::endl;
    std::cout << "Up: " << Settings::getKeyName(Settings::p1Controls.up) << std::endl;
    std::cout << "Crouch: " << Settings::getKeyName(Settings::p1Controls.down) << std::endl;
    std::cout << "Move Left: " << Settings::getKeyName(Settings::p1Controls.left) << std::endl;
    std::cout << "Move Right: " << Settings::getKeyName(Settings::p1Controls.right) << std::endl;
    std::cout << "Punch: " << Settings::getKeyName(Settings::p1Controls.punch) << std::endl;
    std::cout << "Parry: " << Settings::getKeyName(Settings::p1Controls.parry) << std::endl;

    // Print Player 2 controls
    std::cout << "\nPlayer 2 Controls:" << std::endl;
    std::cout << "Up: " << Settings::getKeyName(Settings::p2Controls.up) << std::endl;
    std::cout << "Crouch: " << Settings::getKeyName(Settings::p2Controls.down) << std::endl;
    std::cout << "Move Left: " << Settings::getKeyName(Settings::p2Controls.left) << std::endl;
    std::cout << "Move Right: " << Settings::getKeyName(Settings::p2Controls.right) << std::endl;
    std::cout << "Punch: " << Settings::getKeyName(Settings::p2Controls.punch) << std::endl;
    std::cout << "Parry: " << Settings::getKeyName(Settings::p2Controls.parry) << std::endl;

    // Player 1 controls using Settings
    std::unique_ptr<InputMapping> player1InputMapping = std::make_unique<InputMapping>();

    player1InputMapping->bindKeyToAction(Settings::p1Controls.up, Action::JUMP);
    player1InputMapping->bindKeyToAction(Settings::p1Controls.down, Action::CROUCH);
    player1InputMapping->bindKeyToAction(Settings::p1Controls.left, Action::MOVE_LEFT);
    player1InputMapping->bindKeyToAction(Settings::p1Controls.right, Action::MOVE_RIGHT);
    player1InputMapping->bindKeyToAction(Settings::p1Controls.punch, Action::PUNCH);
    player1InputMapping->bindKeyToAction(Settings::p1Controls.parry, Action::PARRY);

    std::unique_ptr<ControllerMapping> player1ControllerMapping = std::make_unique<ControllerMapping>(0); //HAVE TO SOME HOW MAKE THESE CID's NOT HARD CODED 

    player1ControllerMapping->bindKeyToAction(0, Action::MOVE_LEFT);
    player1ControllerMapping->bindKeyToAction(1, Action::MOVE_RIGHT);
    player1ControllerMapping->bindKeyToAction(3, Action::JUMP);
    player1ControllerMapping->bindKeyToAction(2, Action::CROUCH);
    player1ControllerMapping->bindKeyToAction(4, Action::PUNCH);
    player1ControllerMapping->bindKeyToAction(5, Action::PARRY);

    std::unique_ptr<ControllerMapping> player2ControllerMapping = std::make_unique<ControllerMapping>(1);

    player2ControllerMapping->bindKeyToAction(0, Action::MOVE_LEFT);
    player2ControllerMapping->bindKeyToAction(1, Action::MOVE_RIGHT);
    player2ControllerMapping->bindKeyToAction(3, Action::JUMP);
    player2ControllerMapping->bindKeyToAction(2, Action::CROUCH);
    player2ControllerMapping->bindKeyToAction(4, Action::PUNCH);
    player2ControllerMapping->bindKeyToAction(5, Action::PARRY);

    // Player 2 controls using Settings
    std::unique_ptr<InputMapping> player2InputMapping = std::make_unique<InputMapping>();
    player2InputMapping->bindKeyToAction(Settings::p2Controls.up, Action::JUMP);
     player2InputMapping->bindKeyToAction(Settings::p2Controls.down, Action::CROUCH);
    player2InputMapping->bindKeyToAction(Settings::p2Controls.left, Action::MOVE_LEFT);
    player2InputMapping->bindKeyToAction(Settings::p2Controls.right, Action::MOVE_RIGHT);
    player2InputMapping->bindKeyToAction(Settings::p2Controls.punch, Action::PUNCH);
    player2InputMapping->bindKeyToAction(Settings::p2Controls.parry, Action::PARRY);

    // Initialize input handlers with the mappings
    // player1InputHandler = std::make_unique<InputHandler>(std::move(player1InputMapping), std::move(player1ControllerMapping));
    // player2InputHandler = std::make_unique<InputHandler>(std::move(player2InputMapping), std::move(player1ControllerMapping));        
    int p1_cid = registry.players.get(player1).controller_id;
    int p2_cid = registry.players.get(player2).controller_id;
    if (p1_cid != -1)
    {
        player1InputHandler = std::make_unique<ControllerHandler>(std::move(player1ControllerMapping));
    } else {
        player1InputHandler = std::make_unique<InputHandler>(std::move(player1InputMapping), nullptr);
    }

    if (p2_cid != -1)
    {
        player2InputHandler = std::make_unique<ControllerHandler>(std::move(player2ControllerMapping));
    } else {
        player2InputHandler = std::make_unique<InputHandler>(std::move(player2InputMapping), nullptr);
    }

    //the Input Handler definition is to be done based on controller assignment

    player1InputHandler->initDefaultActionToCommandMapping();
    player2InputHandler->initDefaultActionToCommandMapping();

    std::cout << "\nControls initialized successfully\n"
              << std::endl;
}

void WorldSystem::initStateMachines()
{
    player1StateMachine = std::make_unique<StateMachine>();
    player2StateMachine = std::make_unique<StateMachine>();

    player1StateMachine->addState(PlayerState::IDLE, std::make_unique<IdleState>());
    player1StateMachine->addState(PlayerState::WALKING, std::make_unique<WalkingState>());
    // player1StateMachine->addState(PlayerState::JUMPING, std::make_unique<JumpingState>());
    player1StateMachine->addState(PlayerState::ATTACKING, std::make_unique<AttackingState>());
    player1StateMachine->addState(PlayerState::KICKING, std::make_unique<KickingState>());

    player1StateMachine->addState(PlayerState::CROUCHING, std::make_unique<CrouchingState>());
    player1StateMachine->addState(PlayerState::PARRYING, std::make_unique<ParryingState>());
    player1StateMachine->addState(PlayerState::STUNNED, std::make_unique<StunnedState>());
    player1StateMachine->addState(PlayerState::BLOCKSTUNNED, std::make_unique<BlockStunnedState>());

    player2StateMachine->addState(PlayerState::IDLE, std::make_unique<IdleState>());
    player2StateMachine->addState(PlayerState::WALKING, std::make_unique<WalkingState>());
    // player2StateMachine->addState(PlayerState::JUMPING, std::make_unique<JumpingState>());
    player2StateMachine->addState(PlayerState::ATTACKING, std::make_unique<AttackingState>());
    player2StateMachine->addState(PlayerState::KICKING, std::make_unique<KickingState>());

    player2StateMachine->addState(PlayerState::CROUCHING, std::make_unique<CrouchingState>());
    player2StateMachine->addState(PlayerState::PARRYING, std::make_unique<ParryingState>());
    player2StateMachine->addState(PlayerState::STUNNED, std::make_unique<StunnedState>());
    player2StateMachine->addState(PlayerState::BLOCKSTUNNED, std::make_unique<BlockStunnedState>());
}

// IN THE FUTURE WE SHOULD MAKE THE ENTITY LOOPING A SINGLE FUNCTION AND ALL THE PROCESSING PER LOOP HELPERS SO WE ONLY ITERATE THROUGH THE ENTITIES ONCE PER GAME CYCLE

void WorldSystem::handleInput(int currentLevel, bool dummy)
{
    // Player 1's input is always handled
    player1InputHandler->handleInput(renderer->m_player1, *player1StateMachine);

    // For Player 2, either handle manual input or bot input
    if (dummy) {
        return;
    }
    else if (!botEnabled)
    {
        player2InputHandler->handleInput(renderer->m_player2, *player2StateMachine);
    }
    else
    {
        // Process bot's inputs through the state machine

        PlayerInput &p2Input = registry.playerInputs.get(renderer->m_player2);

        // Convert bot inputs to state machine transitions
        if (p2Input.left || p2Input.right)
            player2StateMachine->transition(renderer->m_player2, PlayerState::WALKING);
        /*if (p2Input.up)
            player2StateMachine->transition(renderer->m_player2, PlayerState::JUMPING);*/
        if (p2Input.punch || p2Input.kick)
            player2StateMachine->transition(renderer->m_player2, PlayerState::ATTACKING);

        // Update motion based on inputs  
        float P2_movespeed = FighterManager::getFighterConfig(registry.players.get(renderer->m_player2).current_char).MOVESPEED; //why does this only exist for p2
        
        if (p2Input.left)
            player2Motion->velocity.x = -P2_movespeed;
        else if (p2Input.right)
            player2Motion->velocity.x = P2_movespeed;
        else
            player2Motion->velocity.x = 0;
        botInstance.pollBotRng(*renderer, *player2StateMachine, currentLevel);

    }
}

void WorldSystem::inputProcessing()
{
    // check input queue and resolve
    PlayerInput &player1Input = registry.playerInputs.get(renderer->m_player1);
    PlayerInput &player2Input = registry.playerInputs.get(renderer->m_player2);

    // Process player 1's input
    if (player1Input.punch)
        player1StateMachine->transition(renderer->m_player1, PlayerState::ATTACKING);

    // Only process player 2's manual input if bot is disabled
    // if (!botEnabled)
    // {
    //     if (player2Input.punch)
    //         player2StateMachine->transition(renderer->m_player2, PlayerState::ATTACKING);
    //     if (player2Input.kick)
    //         player2StateMachine->transition(renderer->m_player2, PlayerState::ATTACKING);
    // }
}

void WorldSystem::movementProcessing()
{
    player1Motion->lastPos = player1Motion->position;
    player2Motion->lastPos = player2Motion->position;

    PlayerCurrentState &player1State = registry.playerCurrentStates.get(renderer->m_player1);
    PlayerCurrentState &player2State = registry.playerCurrentStates.get(renderer->m_player2);

    bool isPlayerMoving = player1Motion->velocity.x != 0 || player2Motion->velocity.x != 0;
    if (isPlayerMoving)
    {
        walkStopTimer = 0.f;
        if (!isPlayerWalking && Settings::audioSettings.enable_sound_effects)
        {
            // std::cout << "Playing walk sound" << std::endl;
            Mix_PlayChannel(WALK_SOUND_CHANNEL, walk_sound, -1);
            isPlayerWalking = true;
        }
    }
    else
    {
        walkStopTimer += PLAYER_STATE_TIMER_STEP;
        if (walkStopTimer >= WALK_SOUND_TIMEOUT && isPlayerWalking)
        {
            Mix_HaltChannel(WALK_SOUND_CHANNEL);
            isPlayerWalking = false;
        }
    }

    if (canMove(player1State.currentState))
    {
        // player can only move if not in these non-moveable states.
        player1Motion->position += player1Motion->velocity;
    }
    else
    {
        player1Motion->position.y += player1Motion->velocity.y;
        // std::cout << "Player 1 cannot move, current state: " << PlayerStateToString(player1State.currentState) << std::endl;
    }

    if (canMove(player2State.currentState))
    {
        // player can only move if not in these non-moveable states.
        player2Motion->position += player2Motion->velocity;
    }
    else
    {
        player2Motion->position.y += player2Motion->velocity.y;
        // std::cout << "Player 2 cannot move, current state: " << PlayerStateToString(player2State.currentState) << std::endl;
    }
}

void WorldSystem::updateStateTimers(float elapsed_ms)
{
    player1StateMachine->update(renderer->m_player1, elapsed_ms);
    player2StateMachine->update(renderer->m_player2, elapsed_ms);
}

mat3 createProjectionMatrix()
{
    // Fake projection matrix, scales with respect to window coordinates
    float left = -1.f;
    float bottom = -1.f;

    gl_has_errors();
    float right = 1.f;
    float top = 1.f;

    float sx = 2.f / (right - left);
    float sy = 2.f / (top - bottom);
    float tx = -(right + left) / (right - left);
    float ty = -(top + bottom) / (top - bottom);
    return {{sx, 0.f, 0.f}, {0.f, sy, 0.f}, {tx, ty, 1.f}};
}

bool WorldSystem::checkHitBoxMeshCollision(float hitBoxLeft, float hitBoxRight, float hitBoxTop,
                                           float hitBoxBottom, ObjectMesh *mesh, Motion &hurtMotion)
{

    std::vector<vec3> originalPositions;
    for (const ColoredVertex &vertex : mesh->vertices)
    {
        originalPositions.push_back(vertex.position);
    }

    mat3 projection = createProjectionMatrix();

    Transform transform;
    transform.rotate(hurtMotion.angle);
    if (hurtMotion.direction)
    {
        transform.translate({hurtMotion.position.x - 0.086637, hurtMotion.position.y});
        transform.rotate(hurtMotion.angle);
        transform.scale(vec2{1, 1});
    }
    else
    {
        transform.translate({hurtMotion.position.x + 0.086637, hurtMotion.position.y});
        transform.rotate(hurtMotion.angle);
        transform.scale(vec2{-1, 1});
    }

    vec3 transformedVertexFirst = projection * transform.mat * vec3(originalPositions[0].x, originalPositions[0].y, 1.0);

    /*std::cout << "transformed vertex last" << transformedVertexFirst.x << transformedVertexFirst.y << std::endl;
    std::cout << "transformed vertex first" << transformedVertexLast.x << transformedVertexLast.y << std::endl;

    std::cout << "hitbox top" << hitBoxTop << std::endl;
    std::cout << "hitbox bot" << hitBoxBottom << std::endl;*/

    float farthestUp = transformedVertexFirst.y;
    float farthestBot = transformedVertexFirst.y;
    float farthestLeft = transformedVertexFirst.x;
    float farthestRight = transformedVertexFirst.x;

    for (const vec3 &originalPos : originalPositions)
    {
        vec3 transformedVertex = transform.mat * vec3(originalPos.x, originalPos.y, 1.0);

        // std::cout << "transformed vertex" << transformedVertex.x << transformedVertex.y << std::endl;

        bool rightCheck = transformedVertex.x < hitBoxRight;
        bool leftCheck = transformedVertex.x > hitBoxLeft;
        bool topCheck = transformedVertex.y < hitBoxTop;
        bool bottomCheck = transformedVertex.y > hitBoxBottom;

        // std::cout << rightCheck << leftCheck << topCheck << bottomCheck << std::endl;

        if (transformedVertex.y > farthestUp)
        {
            farthestUp = transformedVertex.y;
        }
        if (transformedVertex.y < farthestBot)
        {
            farthestBot = transformedVertex.y;
        }
        if (transformedVertex.x < farthestLeft)
        {
            farthestLeft = transformedVertex.x;
        }
        if (transformedVertex.x > farthestRight)
        {
            farthestRight = transformedVertex.x;
        }

        if (rightCheck && leftCheck && topCheck && bottomCheck)
        {
            return true;
        }
    }

    // std::cout << "Mesh Left:" << farthestLeft << " Mesh Right:" << farthestRight << std::endl;
    // std::cout << "Mesh Top:" << farthestUp << " Mesh Bot:" << farthestBot << std::endl;
    // std::cout << "Middle: " << (farthestLeft + farthestRight) / 2 << "," << (farthestUp + farthestBot) / 2 << "," << std::endl;
    // std::cout << "Middle: " << hurtMotion.position.x << "," << hurtMotion.position.y << std::endl;

    // temporary return
    return false;
}

bool WorldSystem::checkHitBoxCollisions(Entity playerWithHitBox, Entity playerWithHurtBox)
{
    Motion &hitPlayerMotion = registry.motions.get(playerWithHitBox);
    Motion &hurtPlayerMotion = registry.motions.get(playerWithHurtBox);
    HitBox &hitBox = registry.hitBoxes.get(playerWithHitBox);
    HurtBox &hurtBox = registry.hurtBoxes.get(playerWithHurtBox);

    // if hitbox is not active or has already hit, skip check
    if (!hitBox.active || hitBox.hit)
        return false;

    float hitBoxLeft = hitBox.getLeft(hitPlayerMotion.position, hitPlayerMotion.direction);
    float hitBoxRight = hitBox.getRight(hitPlayerMotion.position, hitPlayerMotion.direction);
    float hitBoxTop = hitBox.getTop(hitPlayerMotion.position, hitPlayerMotion.direction);
    float hitBoxBottom = hitBox.getBottom(hitPlayerMotion.position, hitPlayerMotion.direction);

    float hurtBoxLeft = hurtBox.getLeft(hurtPlayerMotion.position, hurtPlayerMotion.direction);
    float hurtBoxRight = hurtBox.getRight(hurtPlayerMotion.position, hurtPlayerMotion.direction);
    float hurtBoxTop = hurtBox.getTop(hurtPlayerMotion.position, hurtPlayerMotion.direction);
    float hurtBoxBottom = hurtBox.getBottom(hurtPlayerMotion.position, hurtPlayerMotion.direction);

    bool x_collision = hitBoxLeft < hurtBoxRight && hitBoxRight > hurtBoxLeft;
    bool y_collision = hitBoxTop > hurtBoxBottom && hitBoxBottom < hurtBoxTop;

    // if hitbox collides with parry box, the attack is parried
    if (checkParryBoxCollisions(playerWithHitBox, playerWithHurtBox))
    {
        // parried the attack, transition the attacking player to stunned state using state machine
        if (playerWithHitBox == renderer->m_player1)
        {
            if (registry.parryBoxes.get(playerWithHurtBox).perfectParry)
            {
                float hurt_x = hurtPlayerMotion.position.x;
                float hurt_y = hurtPlayerMotion.position.y;
                // bool hurt_dir = parryPlayerMotion.direction;
                emitSparkleParticles(hurt_x, hurt_y, 0.f);
                createNotification(500.f, true, renderer->m_notif_stunned);
                playPerfectParrySound();
                Player& p1 = registry.players.get(playerWithHitBox);
                p1.perfectParries++;
                player1StateMachine->transition(playerWithHitBox, PlayerState::STUNNED);
                registry.postureBars.get(playerWithHurtBox).currentBar++;
            }
            else
            {
                createNotification(500.f, false, renderer->m_notif_parried);
                playParryBlockedSound();
                Player& p1 = registry.players.get(playerWithHitBox);
                p1.parries++;
                player2StateMachine->transition(playerWithHurtBox, PlayerState::BLOCKSTUNNED);
                hitBox.active = false;
            }
        }
        else
        {
            if (registry.parryBoxes.get(playerWithHurtBox).perfectParry)
            {
                float hurt_x = hurtPlayerMotion.position.x;
                float hurt_y = hurtPlayerMotion.position.y;
                // bool hurt_dir = parryPlayerMotion.direction;
                emitSparkleParticles(hurt_x, hurt_y, 0.f);
                createNotification(500.f, false, renderer->m_notif_stunned);
                playPerfectParrySound();
                Player& p2 = registry.players.get(playerWithHitBox);
                p2.perfectParries++;
                player2StateMachine->transition(playerWithHitBox, PlayerState::STUNNED);
                registry.postureBars.get(playerWithHurtBox).currentBar++;
            }
            else
            {
                createNotification(500.f, true, renderer->m_notif_parried);
                playParryBlockedSound();
                Player& p2 = registry.players.get(playerWithHitBox);
                p2.parries++;
                player1StateMachine->transition(playerWithHurtBox, PlayerState::BLOCKSTUNNED);
                hitBox.active = false;
            }
        }
        return false;
    }

    if (x_collision && y_collision)
    {
        // std::cout << "AABB Collision Detected" << std::endl;
        // Hitbox collided with the hurtbox of the other player
        // Check if the hitbox has collided with the mesh of the other player
        ObjectMesh *otherPlayerMeshPtr = registry.objectMeshPtrs.get(playerWithHurtBox);
        std::cout << "HELLO" << std::endl;
        if (checkHitBoxMeshCollision(hitBoxLeft, hitBoxRight, hitBoxTop, hitBoxBottom,
                                     otherPlayerMeshPtr, hurtPlayerMotion))
        {
            // std::cout << "Mesh Collision Detected" << std::endl;
            Player &attacker = registry.players.get(playerWithHitBox);
            Player &defender = registry.players.get(playerWithHurtBox);

            auto otherPlayerVertices = otherPlayerMeshPtr->vertices;
            // std::cout << "Player " << attacker.id << " hits Player " << defender.id << std::endl;
            hitBox.hit = true;
            return true;
        }
        return false;
    }

    return false;
}

// check if parry box is active and if hitbox collides with it
bool WorldSystem::checkParryBoxCollisions(Entity playerWithHitBox, Entity playerWithParryBox)
{
    ParryBox &parryBox = registry.parryBoxes.get(playerWithParryBox);
    if (!parryBox.active)
        return false;

    Motion &hitPlayerMotion = registry.motions.get(playerWithHitBox);
    Motion &parryPlayerMotion = registry.motions.get(playerWithParryBox);
    HitBox &hitBox = registry.hitBoxes.get(playerWithHitBox);

    // if hitbox is not active or has already hit, skip check
    if (!hitBox.active || hitBox.hit)
        return false;

    float hitBoxLeft = hitBox.getLeft(hitPlayerMotion.position, hitPlayerMotion.direction);
    float hitBoxRight = hitBox.getRight(hitPlayerMotion.position, hitPlayerMotion.direction);
    float hitBoxTop = hitBox.getTop(hitPlayerMotion.position, hitPlayerMotion.direction);
    float hitBoxBottom = hitBox.getBottom(hitPlayerMotion.position, hitPlayerMotion.direction);

    float parryBoxLeft = parryBox.getLeft(parryPlayerMotion.position, parryPlayerMotion.direction);
    float parryBoxRight = parryBox.getRight(parryPlayerMotion.position, parryPlayerMotion.direction);
    float parryBoxTop = parryBox.getTop(parryPlayerMotion.position, parryPlayerMotion.direction);
    float parryBoxBottom = parryBox.getBottom(parryPlayerMotion.position, parryPlayerMotion.direction);

    bool x_collision = hitBoxLeft < parryBoxRight && hitBoxRight > parryBoxLeft;
    bool y_collision = hitBoxTop > parryBoxBottom && hitBoxBottom < parryBoxTop;

    if (x_collision && y_collision)
    {
        // std::cout << "AABB Collision Detected" << std::endl;
        // Hitbox collided with the hurtbox of the other player
        // Check if the hitbox has collided with the mesh of the other player
        ObjectMesh *otherPlayerMeshPtr = registry.objectMeshPtrs.get(playerWithParryBox);
        if (checkHitBoxMeshCollision(hitBoxLeft, hitBoxRight, hitBoxTop, hitBoxBottom,
                                     otherPlayerMeshPtr, parryPlayerMotion))
        {
            //float hurt_x = parryPlayerMotion.position.x;
            //float hurt_y = parryPlayerMotion.position.y;
            //// bool hurt_dir = parryPlayerMotion.direction;
            //emitSparkleParticles(hurt_x, hurt_y, 0.f);
            // if parried successfully, reset state timer to 0
            StateTimer &playerStateTimer = registry.stateTimers.get(playerWithParryBox);
            playerStateTimer.reset(0);
            return true;
        }
        return false;
    }

    return false;
}

// helper function for AABB Collision
void WorldSystem::checkAABBCollision(bool &xCollision, bool &yCollision,
                                     const Box &box1, Motion &motion1,
                                     const Box &box2, Motion &motion2)
{
    vec2 position1 = motion1.position;
    bool facingRight1 = motion1.direction;
    vec2 position2 = motion2.position;
    bool facingRight2 = motion2.direction;

    float box1Left = box1.getLeft(position1, facingRight1);
    float box1Right = box1.getRight(position1, facingRight1);
    float box1Top = box1.getTop(position1, facingRight1);
    float box1Bottom = box1.getBottom(position1, facingRight1);

    float box2Left = box2.getLeft(position2, facingRight2);
    float box2Right = box2.getRight(position2, facingRight2);
    float box2Top = box2.getTop(position2, facingRight2);
    float box2Bottom = box2.getBottom(position2, facingRight2);

    // Perform AABB collision check
    bool x_collision = box1Left < box2Right && box1Right > box2Left;
    bool y_collision = box1Top > box2Bottom && box1Bottom < box2Top;

    xCollision = x_collision;
    yCollision = y_collision;
}

/*
1. check if one player attacks the other player, and
2. apply damange if attack succeeds, i.e. hitbox hits hurtbox
3. set state to STUNNED if being hit, set state timer.
*/
void WorldSystem::hitBoxCollisions()
{
    Entity player1 = renderer->m_player1;
    Entity player2 = renderer->m_player2;
    Fighters current_char1 = registry.players.get(player1).current_char;
    Fighters current_char2 = registry.players.get(player2).current_char;

    // check if player 1 hit player 2
    if (checkHitBoxCollisions(player1, player2))
    {
        const FighterConfig &config = FighterManager::getFighterConfig(current_char1);

        // Get victim (player2) motion for particle emission
        Motion &victimMotion = registry.motions.get(player2);

        float damage = registry.hitBoxes.get(player1).damage;
        applyDamage(player2, damage);
        emitBloodParticles(victimMotion.position.x, victimMotion.position.y, 0.0f, victimMotion.direction);

        KnockBack &knockback = registry.knockbacks.get(player2);
        knockback.active = true;
        knockback.duration = config.KNOCKBACK_DURATION;

        // Use attacker (player1) direction for knockback
        float direction = player1Motion->direction ? 1.0f : -1.0f;
        knockback.force = {
            direction * config.KNOCKBACK_FORCE_X,
            config.KNOCKBACK_FORCE_Y};

        // previously if a player is attacking and it hits the parry box, it will transition to STUNNED 
        // that was ATTACKING -> STUNNED, and the stun time is lightly longer 
        // this caused that whenever player transitions from ATTACKING to STUNNED, the stun time is longer 
        // e.g. when a player starts a punch and being it before punch takes effect 
        // adding these lines to reset state back to IDLE before transitioning to STUNNED 
        StateTimer &playerStateTimer = registry.stateTimers.get(player2);
        playerStateTimer.reset(0);
        PlayerCurrentState &player2State = registry.playerCurrentStates.get(player2);
        player2StateMachine->transition(player2, PlayerState::IDLE);
        player2StateMachine->transition(player2, PlayerState::STUNNED);
        createNotification(500.f, true, renderer->m_notif_hit);
    }

    // check if player 2 hit player 1
    if (checkHitBoxCollisions(player2, player1))
    {
        const FighterConfig &config = FighterManager::getFighterConfig(current_char2);

        // Get victim (player1) motion for particle emission
        Motion &victimMotion = registry.motions.get(player1);

        float damage = registry.hitBoxes.get(player2).damage;
        applyDamage(player1, damage);
        emitBloodParticles(victimMotion.position.x, victimMotion.position.y, 0.0f, victimMotion.direction);

        KnockBack &knockback = registry.knockbacks.get(player1);
        knockback.active = true;
        knockback.duration = config.KNOCKBACK_DURATION;

        // Use attacker (player2) direction for knockback
        float direction = player2Motion->direction ? 1.0f : -1.0f;
        knockback.force = {
            direction * config.KNOCKBACK_FORCE_X,
            config.KNOCKBACK_FORCE_Y};

        // same as above: 
        // previously if a player is attacking and it hits the parry box, it will transition to STUNNED 
        // that was ATTACKING -> STUNNED, and the stun time is lightly longer 
        // this caused that whenever player transitions from ATTACKING to STUNNED, the stun time is longer 
        // e.g. when a player starts a punch and being it before punch takes effect 
        // adding these lines to reset state back to IDLE before transitioning to STUNNED 
        StateTimer &playerStateTimer = registry.stateTimers.get(player1);
        playerStateTimer.reset(0);
        PlayerCurrentState &player1State = registry.playerCurrentStates.get(player1);
        player1StateMachine->transition(player1, PlayerState::IDLE);
        player1StateMachine->transition(player1, PlayerState::STUNNED);
        createNotification(500.f, false, renderer->m_notif_hit);
    }
}

void WorldSystem::playerCollisions(GlRender *renderer)
{
    bool xCollision;
    bool yCollision;

    checkAABBCollision(xCollision, yCollision,
                       *player1CollisionBox, *player1Motion,
                       *player2CollisionBox, *player2Motion);

    if (xCollision && yCollision)
    {
        // This is the version where the players can't push each other
        if (player1Motion->velocity.x > 0 && player2Motion->velocity.x < 0) {
            player1Motion->position.x = player1Motion->position.x - 0.02f / FPS_LOGIC_FACTOR;
            player2Motion->position.x = player2Motion->position.x + 0.02f / FPS_LOGIC_FACTOR;
        }
        if (player1Motion->velocity.x > 0 && player2Motion->velocity.x >= 0) {
            player1Motion->position.x = player1Motion->position.x - 0.02f / FPS_LOGIC_FACTOR;
        }
        if (player1Motion->velocity.x <= 0 && player2Motion->velocity.x < 0) {
            player2Motion->position.x = player2Motion->position.x + 0.02f / FPS_LOGIC_FACTOR;
        }

        // This is the version where the players can push each other
        /*player1Motion->position.x = player1Motion->position.x - 0.02f / FPS_LOGIC_FACTOR;
        player2Motion->position.x = player2Motion->position.x + 0.02f / FPS_LOGIC_FACTOR;*/
    }
}

void WorldSystem::updatePlayableArea()
{
    PlayableArea &playableArea = registry.playableArea.get(renderer->m_playableArea);
    playableArea.updatePosition(player1Motion->position, player2Motion->position);
    playableArea.updateWorldModel(renderer->m_worldModel);
}