#include "bot.hpp"
#include "renderer.hpp"
#include <iostream>

Bot botInstance;

void Bot::pollBotRng(){
    int rng = rand() % 20;
    std::cout << rng << std::endl;
    if(rng <= 10){
        botInstance.m_bot_input.up = true;
        if(rng <=14){
            //even odd check for jump forward and back
        }
    } else {
        //even go left
        //odd go right
    }
}