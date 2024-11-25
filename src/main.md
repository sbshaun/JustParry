<h2>Description</h2>
Main program loop

<h2>Imports</h2>


<h2>Definitions</h2>
*Local Vars*

    int timer
    static bool roundEnded
    auto last_time

*Functions*

    int generateUI(GlRender &renderer) 
Counts down the timer <br>
Calls renderer.renderUI(timer)

    void checkIsRoundOver(GlRender &renderer, Bot &botInstance, WorldSystem &worldSystem, Game &game, bool &botEnabled)
Checks for the healths of the 2 players and the timer<br>
If its over change the state to ROUND_OVER, otherwise keep running <br>
Calls renderer.renderRoundeOver(1), renderer.renderUI(timer)

    int main()
*Initialize*<br>
glWindow<br>
FighterManager<br>
renderer<br>
fpsCounter<br>

*Construct*<br>
renderer<br>
worldSystem<br
physicsSystem<br>
botInstance<br>
game<br>

*Local Function Vars*

    bool fKeyPressed
    bool bKeyPressed
    bool hKeyPressed
    bool shouldExit   
    const float targetLogicDuration 
    const float FramesPerLogicLoop
    int loopsSinceLastFrame

    bool p1Ready
    bool p2Ready
    static bool p1KeyPressed 
    static bool p2KeyPressed 
    bool goDown1 
    bool goUp1
    bool goDown2
    bool goUp2

    float offsetY1
    float offsetY2
    auto t = std::esolution_clock::now();

Initializes the font<br>
*Main Loop*

Start a timer for this iteration
Checks the current *GameState* from the construct *game*

<h4>INIT</h4>
<h4>MENU</h4>
<h4>ARCADE_MENU</h4>
<h4>ARCADE_PREFIGHT</h4>
<h4>ARCADE_WIN</h4>
<h4>ARCADE_LOSE</h4>
<h4>HELP</h4>
<h4>SETTINGS</h4>
<h4>SETTINGS_EXIT</h4>
<h4>CHARACTER_SELECT</h4>
<h4>ROUND_START</h4>
<h4>PAUSED</h4>
<h4>PLAYING</h4>
<h4>LOADING</h4>
<h4>ROUND_OVER</h4>




