#include "common.hpp"
#include <random>

class Bot {
    public:

    struct botInput {
        bool up;
        bool down;
        bool left;
        bool right;
        int time;
        int scale;
    };
    
    botInput m_bot_input;

    static const int bot_time_scale = 0.5;

    Bot(){
        m_bot_input={false, false, false, false, 0, bot_time_scale};
    };

    void pollBotRng();

    botInput getbotInput() const {
        return m_bot_input;
    };

    void setBotInput(bool up, bool down, bool left, bool right, int time) {
        m_bot_input = {up, down, left, right, time, bot_time_scale};
    };

};
