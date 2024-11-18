#include "fps.hpp"

FPSCounter::FPSCounter(int target) : targetFPS(target), frameCount(0), currentFPS(0)
{
    lastTime = std::chrono::steady_clock::now();
    startTime = lastTime;
}

void FPSCounter::update(GlRender &renderer, bool showFPS)
{
    frameCount++;
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count();

    // Update FPS every second
    if (duration >= 1000)
    {
        currentFPS = frameCount;
        frameCount = 0;
        startTime = now;
    }
}

int FPSCounter::getFPS() const
{
    return currentFPS;
}

FPSCounter::~FPSCounter()
{
}