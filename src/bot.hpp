#ifndef BOT_HPP
#define BOT_HPP

#include "common.hpp"
#include <random>
#include "renderer.hpp"

class Bot {
    public:
        void pollBotRng(int timer, GlRender& renderer);
};


#endif  // BOT_HPP