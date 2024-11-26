<h2>Description</h2>

An instance of the game engine. Handles collisions directly. makes calls to input handling, rendering and audio

<h2>Definitions</h2>

*LOCAL VARS*

bool botEnabled = false; <br>
static bool isPlayerWalking;<br>
static const int WALK_SOUND_CHANNEL = 5;<br>
static constexpr float WALK_SOUND_TIMEOUT = 50.f; <br>
static float walkStopTimer;	<br>

BloodParticleSystem bloodSystem;<br>
SmokeParticleSystem smokeSystem;<br>

GlRender *renderer;<br>

std::unique_ptr<InputHandler> player1InputHandler;<br>
std::unique_ptr<InputHandler> player2InputHandler;<br>
std::unique_ptr<StateMachine> player1StateMachine;<br>
std::unique_ptr<StateMachine> player2StateMachine;<br>

Motion *player1Motion;<br>
Motion *player2Motion;<br>

CollisionBox *player1CollisionBox;<br>
CollisionBox *player2CollisionBox;<br>

static Mix_Music *background_music;<br>
static Mix_Chunk *punch_sound;<br>
static Mix_Chunk *walk_sound;<br>

static Mix_Chunk *parry_sound;<br>
static Mix_Chunk *kick_sound;<br>
static Mix_Chunk *menu_select_sound;<br>
static Mix_Chunk *menu_confirm_sound;<br>
static Mix_Chunk *game_count_down_sound;<br>

---

    canMove()
checks the state of PlayerState to see if its allowed to move

---
    applyDamage()
apply damage to an Entity player

---
    ~WorldSystem()
destructor<br>
cleans up
- shaders
- all components
- player input handlers
- clears registry
- music

---
    init()

sets the local renderer to be the one passed in <br>
init SDL and audio <br>
sets all the audio file paths and loads them and volume settings <br>
HARD CODEDED IN TO LOAD BIRDMAN FOR BOTH PLAYERS <br>
bind renderer players to the newly crreated Player1 and Player2 entities <br>
create the wall and floor entities and playable area (ALSO HARD CODED) <br>
initInputHandlers and stateMachines for the players <br>
set pointers for the motion and colission boxes of the players <br>
bind the bloodSystem and SmokeSystem to this instance<br>

---
    step()
helper to call updates for the blood and smoke system

---
    emitBloodParticles()
helper that calls the blood

---
    emitSmokeParticles()
helper thjat calls the smoke

---
    renderParticles()
helper that calls render functions for blood and smoke

---
    initInputHandlers()
takes the SettingsPxControls and them to the appropriate actions for both players as an InputMapping<br>
then initalize the input handlers with the InputMapping 

---
    initStateMachines()

---
    handleInput()
calls the InputHandler for each player<br>
unless p2 is a bot

---
