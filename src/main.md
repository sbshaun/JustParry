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
    inits world and sets menu state
<h4>MENU</h4>
    set both player to unready (for character select) <br>
    enable vsync (caps frame rate) <br>
    WorldSystem calls to stop sound <br>
    call game.renderMenu() <br>
    check for a mouse action and act accordingly game.handleMenuInput(glWindow.window, renderer) <br>
    check for fps counter <br>
    check for escape to close <br>
    draw next frame to window <br>   
<h4>ARCADE_MENU</h4>
    call game.randerArcadeMenu<br>
    check for arcade menu inputs and change state accordingly<br>
    fps counter <br>
    draw frames<br>
<h4>ARCADE_PREFIGHT</h4>
    enable the bots <br>
    call handlers and renderers for the character select screen <br>
    fps counter <br>
    draw frames<br>
<h4>ARCADE_WIN</h4>
    to be implemented
<h4>ARCADE_LOSE</h4>
    to be implemented
<h4>HELP</h4>
    draw the help screen and handle inputs to switch back to menu<br>
    fps counter <br>
    draw frames<br>
<h4>SETTINGS</h4>
    WorldSystem calls to turn off sounds <br>
    handleMenuInput
    fps counter <br>
    Check if it was in game, if it was go back to the PAUSED state <br>
    draw frames<br>
<h4>SETTINGS_EXIT</h4>
    Save settings and reload<br>
    return to previous state<br>
<h4>CHARACTER_SELECT</h4>
    call handlers and renderers for the character select screen <br>
    fps counter <br>
    draw frames<br>
<h4>ROUND_START</h4>
    Checks timing for logic loop <br>
      -  worldSystem call to update playable stage<br>
      -  renderer call to render and renderUI<br>
      -  game call to renderPauseButton<br>
      -  interp call for character jump in animation <br>
      -  check if game is finished loading <br>
      --  play music if enabled and set state to playing <br>
      - fps counter <br>
      - draw frames<br>
    increment logic loop counter <br>
    start timer for the next frame <br>
    run busy wait loop for the next frame with calls to worldSystem handleInput 
<h4>PAUSED</h4>
    stop sounds <br>
    renderer calls <br>
    fps<br>
    game call to render the menu (why is the function called renderPauseButton) <br>
    draw frame <br>
<h4>PLAYING</h4>
    checks timing for logic loop<br>
    -reset logic loop counter<br>
    -if state changed start sounds<br>
    -handleUtilityiInputs (debug binds basically)<br>
    -call renderer for render, hitNotifications and UI rendering<br>
    -woldSystem renderParticles<br>
    -game renderPauseButton<br>
    -fps<br>
    -draw frame<br>
    *OUTSIDE OF THE TIMED LOOP*<br>
    increment loopsSinceLastFrame<br>
    update movements, collisions and physics<br>
    worldSystem step timers and state timers<br>
    checkIsRoundOver<br>
    start timer for the next frame <br>
    run busy wait loop for the next frame with calls to worldSystem handleInput 
<h4>LOADING</h4>
    fps and draw frame <br>
<h4>ROUND_OVER</h4>
    render calls for render, UI, and roundEnd<br>
    attempt start the exit animation and stop sounds <br>
    if the exit animation has not started handle input to start it <br>
    when the exit animation is over<br>
    -reset all the ingame cars and start the next round <br>
    fps<br>
    enable vsync and draw frames <br>

    





