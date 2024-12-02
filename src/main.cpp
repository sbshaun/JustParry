#define GL3W_IMPLEMENTATION
#include <gl3w.h>
#include "bot/bot.hpp"
#include "window.hpp"
#include "graphics/renderer.hpp"
#include <assert.h>
#include "constants.hpp"
#include "physics/physics_system.hpp"
#include "ecs/ecs_registry.hpp"
#include "world/world_system.hpp"
#include "interp/linearinterp.hpp"
#include "state/game.hpp"
#include "fps/fps.hpp"
#include "input_system/input_handler.hpp"
#include "input_system/state_machine.hpp"
#include "input_system/utility_inputs.hpp"
// #include "state/settings_menu.hpp"
#include "SDL.h"
#include <SDL.h>
#define SDL_MAIN_HANDLED

int timer = timer_length;
static bool roundEnded = false;
auto last_time = std::chrono::high_resolution_clock::now();

int generateUI(GlRender &renderer)
{
    auto current_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = current_time - last_time;

    if (elapsed.count() >= 1.0 && timer > 0)
    {
        last_time = current_time;
        timer--;
    }

    if (timer <= 0)
    {
        return 1;
    }

    renderer.renderUI(timer);
    return 0;
}

void checkIsRoundOver(GlRender &renderer, Bot &botInstance, WorldSystem &worldSystem, Game &game, bool &botEnabled, StateMachine &botStateMachine)
{
    Health &h1 = registry.healths.get(renderer.m_player1);
    Health &h2 = registry.healths.get(renderer.m_player2);
    PlayerInput &p1 = registry.playerInputs.get(renderer.m_player1);
    PlayerInput &p2 = registry.playerInputs.get(renderer.m_player2);

    // Check if round is over due to health or timer
    if (h1.currentHealth <= 0 || h2.currentHealth <= 0 || generateUI(renderer) == 1)
    {
        // Disable player inputs during round over
        p1 = PlayerInput();
        p2 = PlayerInput();

        // Always render round over animation with winner text
        renderer.renderRoundOver(1);
        WorldSystem::stopAllSounds();

        // Update scores and set round over state only once
        if (!roundEnded)
        {
            game.updateScores(h1, h2);
            roundEnded = true;
            if (game.getPlayer1Score() == 2 || game.getPlayer2Score() == 2)
            {
                if (game.isVersusMode())
                {
                    game.setState(GameState::MATCH_OVER);
                }
                else
                {
                    game.setState(GameState::LEVEL_OVER);
                }
            }
            else
            {
                game.setState(GameState::ROUND_OVER);
                if (h1.currentHealth == h2.currentHealth)
                {
                    // do not increment round if health is equal
                }
                else
                {
                    game.incrementRound();
                }
            }
        }
    }
    else
    {
        roundEnded = false;
        if (botEnabled)
        {
            botInstance.pollBotRng(renderer, botStateMachine, game.getCurrentLevel());
        }
        renderer.renderUI(timer);
    }
}

int main()
{
    //// START INITS ////
    GLWindow glWindow(M_WINDOW_WIDTH_PX, M_WINDOW_HEIGHT_PX);
    assert(gl3w_init() == 0);
    // assert(is_fine == 0);

    glfwSwapInterval(1);              // Enable vsync
    if (SDL_Init(SDL_INIT_AUDIO) < 0) // Init SDL joystick
    {
        std::cerr << "Failed to init SDL joy handler" << std::endl;
        return EXIT_FAILURE;
    }

    // init fighters config
    FighterManager::init();

    GlRender renderer;
    renderer.initialize();

    // Load settings before initializing world system
    Settings::loadSettings();

    WorldSystem worldSystem; // This will now use the loaded settings

    PhysicsSystem physicsSystem;
    Bot botInstance;

    StateMachine botStateMachine;
    botStateMachine.addState(PlayerState::IDLE, std::make_unique<IdleState>());
    botStateMachine.addState(PlayerState::WALKING, std::make_unique<WalkingState>());
    botStateMachine.addState(PlayerState::ATTACKING, std::make_unique<AttackingState>());
    botStateMachine.addState(PlayerState::KICKING, std::make_unique<KickingState>());
    botStateMachine.addState(PlayerState::PARRYING, std::make_unique<ParryingState>());
    botStateMachine.addState(PlayerState::STUNNED, std::make_unique<StunnedState>());
    botStateMachine.addState(PlayerState::BLOCKSTUNNED, std::make_unique<BlockStunnedState>());

    // Initialize game state
    Game game;
    renderer.setGameInstance(&game);
    FPSCounter fpsCounter(60);

    // Initialize states from game instance instead of directly from settings
    bool showFPS = game.getShowFPS();
    bool botEnabled = false;
    renderer.debugMode = Settings::windowSettings.enable_debug;

    game.loadArcadeState();

    // Toggle states
    bool fKeyPressed = false;
    bool bKeyPressed = false;
    bool hKeyPressed = false;
    bool shouldExit = false;

    const float targetLogicDuration = 1000 / TARGET_LOGIC_RATE;      // denominator is logic+input checks per second
    const float FramesPerLogicLoop = TARGET_LOGIC_RATE / TARGET_FPS; // The number of logic loops that would result in 60fps

    int loopsSinceLastFrame = 0;
    //// END INITS ////

    // font setup and initialization.
    std::string font_filename = "Kenney_Mini_Square.ttf";
    unsigned int font_default_size = 100;
    if (!renderer.fontInit(font_filename, font_default_size))
    {
        std::cerr << "Failed to initialize fonts. Exiting." << std::endl;
        return EXIT_FAILURE;
    }

    // Main loop
    bool p1Ready = false;
    bool p2Ready = false;
    static bool p1KeyPressed = false; // Track the state of the key for player 1
    static bool p2KeyPressed = false; // Track the state of the key for player 2
    bool goDown1 = false;
    bool goUp1 = false;
    bool goDown2 = false;
    bool goUp2 = false;

    float offsetY1 = 0.0f;
    float offsetY2 = 0.0f;

    auto t = std::chrono::high_resolution_clock::now();
    while (!glWindow.shouldClose())
    {
        // start a timer for each loop
        auto start = std::chrono::steady_clock::now();

        auto now = std::chrono::high_resolution_clock::now();
        float elapsed_ms =
            (float)(std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count() / 1000;
        t = now;

        switch (game.getState())
        {
        case GameState::INIT:
            worldSystem.init(&renderer);
            game.setState(GameState::MENU);
            break;
        case GameState::MENU:
            p1Ready = false;
            p2Ready = false;
            glfwSwapInterval(1); // enable vsync

            WorldSystem::stopAllSounds(); // Stop sounds in menu
            WorldSystem::stopBackgroundMusic();
            game.renderMenu(renderer );
            if (game.handleMenuInput(glWindow.window, renderer, shouldExit))
            {
                std::cout << "Entered Character Select Stage" << std::endl;
                game.setState(GameState::CHARACTER_SELECT);
            }
            if (Settings::windowSettings.show_fps)
            {
                fpsCounter.update(renderer, false);
                renderer.renderFPS(fpsCounter.getFPS(), true);
            }
            // Handle ESC = close game when in menu
            if (isKeyPressed(GLFW_KEY_ESCAPE))
            {
                shouldExit = true;
            }

            glWindow.windowSwapBuffers();
            break;
        case GameState::ARCADE_PREFIGHT:
            // Enable bot for arcade
            botEnabled = true;
            worldSystem.botEnabled = true;

            /*game.handleArcadePrefightInputs(glWindow, p1KeyPressed, p1Ready, goDown1, goUp1, offsetY1);
            game.renderArcadePrefight(renderer, offsetY1, p1Ready);
            game.renderReadyText(renderer, p1Ready, true, game);
            if (Settings::windowSettings.show_fps)
            {
                fpsCounter.update(renderer, false);
                renderer.renderFPS(fpsCounter.getFPS(), true);
            }
            glWindow.windowSwapBuffers();*/
            game.startArcade(renderer);
            game.setState(GameState::ARCADE_MENU);
            break;
        case GameState::ARCADE_MENU:
            game.renderArcadeMenu(renderer);
            if (game.handleArcadeMenuInput(glWindow.window))
            {
                game.setState(GameState::ARCADE_STORY);
            }
            if (Settings::windowSettings.show_fps)
            {
                fpsCounter.update(renderer, false);
                renderer.renderFPS(fpsCounter.getFPS(), true);
            }

            glWindow.windowSwapBuffers();
            break;
        case GameState::ARCADE_STORY:
            game.renderArcadeStory(renderer);
            if (game.handleArcadeStoryInput(glWindow.window))
            {
                WorldSystem::playGameCountDownSound();
                game.setState(GameState::ROUND_START);
            }
            if (Settings::windowSettings.show_fps)
            {
                fpsCounter.update(renderer, false);
                renderer.renderFPS(fpsCounter.getFPS(), true);
            }

            glWindow.windowSwapBuffers();
            break;
        case GameState::HELP:
        {
            if (botEnabled)
            {
                worldSystem.botEnabled = true;
            }
            if (game.handleHelpInput(glWindow.window))
            {
                game.resetGame(renderer, worldSystem);
                game.setState(GameState::MENU);
            }
            if (loopsSinceLastFrame == FramesPerLogicLoop)
            {
                loopsSinceLastFrame = 0;

                // Resume sounds and check music when returning to playing state
                if (game.getPreviousState() == GameState::PAUSED ||
                    game.getPreviousState() == GameState::SETTINGS)
                {
                    WorldSystem::resumeSounds();
                    WorldSystem::updateAudioState(); // Make sure music is playing if enabled
                }

                handleUtilityInputs(renderer, showFPS,
                                    fKeyPressed, bKeyPressed, hKeyPressed,
                                    glWindow, fpsCounter, shouldExit,
                                    worldSystem);

                // worldSystem.emitSmokeParticles(0.1f, 0.1f, 0.0f);
                // Do all rendering here, only once
                renderer.render();

                game.renderHelpScreen(renderer, botEnabled);
                worldSystem.renderParticles();
                renderer.handleNotifications(elapsed_ms);

                if (Settings::windowSettings.show_fps)
                {
                    renderer.renderFPS(fpsCounter.getFPS(), true);
                }
                glWindow.windowSwapBuffers();
            }

            loopsSinceLastFrame++;

            // Update center for playable area
            worldSystem.movementProcessing(); // PROCESS MOVEMENTS BASED ON THE DECISIONS MADE BY FRAME BUFFER
            worldSystem.playerCollisions(&renderer);
            worldSystem.hitBoxCollisions();
            worldSystem.updatePlayableArea();
            physicsSystem.step();

            // NEEDS TO BE ELAPSED_MS / 1000.0f FOR PARTICLES TO RENDER!!
            // OTHERWISE THEY DIE OUT TOO FAST
            worldSystem.step(elapsed_ms / 1000.0f);

            worldSystem.updateStateTimers(PLAYER_STATE_TIMER_STEP);
            // time the next logic check
            auto end = std::chrono::steady_clock::now();
            std::chrono::duration<double, std::milli> FastLoopIterTime = end - start;

            // Calculate the remaining time to sleep
            int sleepDuration = static_cast<int>(targetLogicDuration) - static_cast<int>(FastLoopIterTime.count());
            // std::cout << "i wanna sleep for " << sleepDuration << std::endl;
            if (sleepDuration > 0)
            {
                auto sleepEnd = std::chrono::steady_clock::now() + std::chrono::milliseconds(sleepDuration);
                while (std::chrono::steady_clock::now() < sleepEnd)
                {
                    if (game.getCurrentTutorialPage() < 2)
                    {
                        worldSystem.handleInput(0, true); // this sets player inputs #3
                    }
                    else
                    {
                        worldSystem.handleInput(0, false); // this sets player inputs #3
                    }
                } // Do input polling during wait time maybe and input conflict resoltion each logic step rather than each frame
            }
        }
        break;

        case GameState::SETTINGS:
            WorldSystem::stopAllSounds();      // Stop sounds when entering settings
            game.renderSettingsMenu(renderer); // render settings overlay

            // handle user input
            if (game.handleSettingsInput(renderer, glWindow.window))
            {
                // save changes and update game to new inputs.
                game.setState(GameState::SETTINGS_EXIT);
            }

            if (Settings::windowSettings.show_fps)
            {
                fpsCounter.update(renderer, false);
                renderer.renderFPS(fpsCounter.getFPS(), true);
            }

            // Handle ESC = go back to pause when in playing
            if (isKeyPressed(GLFW_KEY_ESCAPE))
            {
                if (game.getPreviousState() == GameState::PAUSED)
                {
                    game.setState(GameState::PAUSED);
                }
            }

            glWindow.windowSwapBuffers();
            break;
        case GameState::SETTINGS_EXIT:
            // Reinitialize input handlers after updating settings
            worldSystem.initInputHandlers();

            // Return to previous state
            // previous state is saved before settings were opened.
            if (game.getPreviousState() == GameState::PAUSED)
            {
                game.setState(GameState::PAUSED);
            }
            else if (game.getPreviousState() == GameState::MENU)
            {
                registry.players.get(renderer.m_player1).parries = 0;
                registry.players.get(renderer.m_player1).perfectParries = 0;
                registry.players.get(renderer.m_player2).parries = 0;
                registry.players.get(renderer.m_player2).perfectParries = 0;
                game.setState(GameState::MENU);
            }
            break;
        case GameState::CHARACTER_SELECT:
            botEnabled = false;
            worldSystem.botEnabled = false;
            game.setVersusMode(true);
            game.handleCharacterInputs(renderer, glWindow, p1KeyPressed, p1Ready, p2KeyPressed, p2Ready, goDown1, goDown2, goUp1, goUp2, offsetY1, offsetY2);
            game.renderCharacterSelect(renderer, offsetY1, offsetY2, p1Ready, p2Ready);
            renderer.renderSelectorTriangleP1(380, 670, 40, 40, true);
            renderer.renderSelectorTriangleP2(83, 670, 40, 40, true);

            renderer.renderSelectorTriangleP1(900, 670, 40, 40, true);
            renderer.renderSelectorTriangleP2(605, 670, 40, 40, true);
            game.renderReadyText(renderer, p1Ready, p2Ready, game);
            if (Settings::windowSettings.show_fps)
            {
                fpsCounter.update(renderer, false);
                renderer.renderFPS(fpsCounter.getFPS(), true);
            }

            glWindow.windowSwapBuffers();
            break;
        case GameState::ROUND_START:
        {
            if (loopsSinceLastFrame == FramesPerLogicLoop)
            {
                loopsSinceLastFrame = 0;

                worldSystem.updatePlayableArea();
                renderer.render();
                renderer.renderUI(timer);
                game.renderPauseButton(renderer);

                interp_moveEntitesToScreen(renderer, game);

                if (!isLoading)
                {
                    // Only play music if it's enabled in settings
                    if (Settings::audioSettings.enable_music)
                    {
                        WorldSystem::stopBackgroundMusic(); // Stop any existing music first
                        WorldSystem::playBackgroundMusic(); // Start fresh
                        WorldSystem::updateAudioState();    // Make sure volume is correct
                    }
                    game.setState(GameState::PLAYING);
                }

                if (Settings::windowSettings.show_fps)
                {
                    fpsCounter.update(renderer, false);
                    renderer.renderFPS(fpsCounter.getFPS(), true);
                }

                glWindow.windowSwapBuffers();
            }

            loopsSinceLastFrame++;

            // Time the next logic check
            auto end = std::chrono::steady_clock::now();
            std::chrono::duration<double, std::milli> FastLoopIterTime = end - start;

            // Calculate the remaining time to sleep
            int sleepDuration = static_cast<int>(targetLogicDuration) - static_cast<int>(FastLoopIterTime.count());
            if (sleepDuration > 0)
            {
                auto sleepEnd = std::chrono::steady_clock::now() + std::chrono::milliseconds(sleepDuration);
                while (std::chrono::steady_clock::now() < sleepEnd)
                {
                    // worldSystem.handleInput(game.getCurrentLevel());
                }
            }
        }
            glfwSwapInterval(0); // disable vsync
            break;

        case GameState::PAUSED:
            WorldSystem::stopAllSounds(); // Stop sounds when paused
            // Continue rendering the game state in the background
            renderer.render();
            renderer.renderUI(timer);

            // Render FPS counter if enabled
            if (showFPS)
            {
                renderer.renderFPS(fpsCounter.getFPS(), true);
            }

            // Render the pause menu on top
            game.renderPauseButton(renderer);

            glWindow.windowSwapBuffers();
            break;

        case GameState::PLAYING:
        {
            if (loopsSinceLastFrame == FramesPerLogicLoop)
            {
                loopsSinceLastFrame = 0;

                // Resume sounds and check music when returning to playing state
                if (game.getPreviousState() == GameState::PAUSED ||
                    game.getPreviousState() == GameState::SETTINGS)
                {
                    WorldSystem::resumeSounds();
                    WorldSystem::updateAudioState(); // Make sure music is playing if enabled
                }

                handleUtilityInputs(renderer, showFPS,
                                    fKeyPressed, bKeyPressed, hKeyPressed,
                                    glWindow, fpsCounter, shouldExit,
                                    worldSystem);

                // worldSystem.emitSmokeParticles(0.1f, 0.1f, 0.0f);
                // Do all rendering here, only once
                renderer.render();
                worldSystem.renderParticles();
                renderer.handleNotifications(elapsed_ms);
                renderer.renderUI(timer);
                game.renderPauseButton(renderer);

                if (Settings::windowSettings.show_fps)
                {
                    renderer.renderFPS(fpsCounter.getFPS(), true);
                }

                glWindow.windowSwapBuffers();
            }

            loopsSinceLastFrame++;

            // Update center for playable area
            worldSystem.movementProcessing(); // PROCESS MOVEMENTS BASED ON THE DECISIONS MADE BY FRAME BUFFER
            worldSystem.playerCollisions(&renderer);
            worldSystem.hitBoxCollisions();
            worldSystem.updatePlayableArea();
            physicsSystem.step();

            // NEEDS TO BE ELAPSED_MS / 1000.0f FOR PARTICLES TO RENDER!!
            // OTHERWISE THEY DIE OUT TOO FAST
            worldSystem.step(elapsed_ms / 1000.0f);

            worldSystem.updateStateTimers(PLAYER_STATE_TIMER_STEP);

            checkIsRoundOver(renderer, botInstance, worldSystem, game, botEnabled, botStateMachine);

            // time the next logic check
            auto end = std::chrono::steady_clock::now();
            std::chrono::duration<double, std::milli> FastLoopIterTime = end - start;
            game.getCurrentLevel();
            // Calculate the remaining time to sleep
            int sleepDuration = static_cast<int>(targetLogicDuration) - static_cast<int>(FastLoopIterTime.count());
            // std::cout << "i wanna sleep for " << sleepDuration << std::endl;
            if (sleepDuration > 0)
            {
                auto sleepEnd = std::chrono::steady_clock::now() + std::chrono::milliseconds(sleepDuration);
                while (std::chrono::steady_clock::now() < sleepEnd)
                {
                    worldSystem.handleInput(game.getCurrentLevel(), false); // this sets player inputs #3
                } // Do input polling during wait time maybe and input conflict resoltion each logic step rather than each frame
            }
        }
        break;

        case GameState::LOADING:
            std::cout << "warning: enumeration value 'LOADING' not handled in switch [-Wswitch]" << std::endl;
            if (Settings::windowSettings.show_fps)
            {
                fpsCounter.update(renderer, false);
                renderer.renderFPS(fpsCounter.getFPS(), true);
            }
            glWindow.windowSwapBuffers();
            break;

        case GameState::ROUND_OVER:
            renderer.render();
            renderer.renderUI(timer);
            renderer.renderRoundOver(1);

            if (!renderer.isExitAnimationStarted())
            {
                if (registry.healths.get(renderer.m_player1).currentHealth > registry.healths.get(renderer.m_player2).currentHealth)
                {
                    game.updateArcadeLevel();
                }
                // Stop all sounds regardless of whether new ones will play
                WorldSystem::stopAllSounds();
            }

            // Only handle enter press if exit animation hasn't started
            if (!renderer.isExitAnimationStarted() &&
                isKeyPressed(GLFW_KEY_ENTER))
            {
                renderer.startExitAnimation();
            }
            // Only reset the game once the exit animation is complete
            if (renderer.isExitAnimationComplete())
            {
                roundEnded = false;
                game.resetGame(renderer, worldSystem);
                resetInterpVariables();                // Reset interpolation variables including countdown timer
                WorldSystem::playGameCountDownSound(); // Play the countdown sound
                // Only play music if enabled in settings AND sound effects are enabled
                if (Settings::audioSettings.enable_music && Settings::audioSettings.enable_sound_effects)
                {
                    WorldSystem::playBackgroundMusic();
                }
                game.setState(GameState::ROUND_START);
            }

            if (Settings::windowSettings.show_fps)
            {
                fpsCounter.update(renderer, false);
                renderer.renderFPS(fpsCounter.getFPS(), true);
            }
            glfwSwapInterval(1);
            glWindow.windowSwapBuffers();
            break;
        case GameState::MATCH_OVER:
            game.renderMatchOver(renderer);
            glWindow.windowSwapBuffers();
            break;

        case GameState::LEVEL_OVER:
            game.renderLevelOver(renderer);
            glWindow.windowSwapBuffers();
            break;
        default:
            std::cerr << "unhandled game state" << std::endl;
            if (Settings::windowSettings.show_fps)
            {
                fpsCounter.update(renderer, false);
                renderer.renderFPS(fpsCounter.getFPS(), true);
            }
            glWindow.windowSwapBuffers();
            if (shouldExit)
                break;
            break;
        }

        if (shouldExit)
            break;
    }

    game.saveCurrentState();
    // Cleanup in correct order
    SDL_Quit();
    return 0;
}
