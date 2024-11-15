#pragma once
#include <chrono>
#include "../renderer.hpp"

class FPSCounter
{
private:
    int targetFPS;
    int frameCount;
    int currentFPS;
    std::chrono::time_point<std::chrono::steady_clock> lastTime;
    std::chrono::time_point<std::chrono::steady_clock> startTime;

public:
    FPSCounter(int target = 60);
    ~FPSCounter();
    void update(GlRender &renderer, bool showFPS);
    int getFPS() const;
};