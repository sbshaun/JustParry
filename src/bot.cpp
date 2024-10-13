#include "bot.hpp"
#include "renderer.hpp"
#include <iostream>

Bot botInstance;

void Bot::pollBotRng(){
    int rng = rand() % 20;
    std::cout << rng << std::endl;
    if(rng >= 5){ // 1/4 time stand still
            botInstance.m_bot_input.up = false;
            botInstance.m_bot_input.down = false;
            botInstance.m_bot_input.left = false;
            botInstance.m_bot_input.right = false;
            botInstance.m_bot_input.time = rng;
    }
    if(rng <= 15){// for 1/4 the time jump
        if(rng <=16){
            if(rng % 2 == 1){//for about half of the jump time jump in a direction
                botInstance.m_bot_input.up = true;
                botInstance.m_bot_input.left = true;
                botInstance.m_bot_input.time = rng - 16;
            } else {
                botInstance.m_bot_input.up = true;
                botInstance.m_bot_input.right = true;
                botInstance.m_bot_input.time = rng - 16;                
            }
        } else {
            botInstance.m_bot_input.up = true;
            botInstance.m_bot_input.time = 1;
        }
    } else { //for other half time based on even or odd move left or right for that time
            if(rng % 2 == 1){
                botInstance.m_bot_input.left = true;
                botInstance.m_bot_input.time = rng;
            } else {
                botInstance.m_bot_input.right = true;
                botInstance.m_bot_input.time = rng;                
            }
    }
}