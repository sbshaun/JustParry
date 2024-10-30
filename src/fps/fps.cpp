#include <iostream>
#include <chrono>
#include <thread>
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
    FPSCounter(int target = 60) : targetFPS(target), frameCount(0), currentFPS(0)
    {
        lastTime = std::chrono::steady_clock::now();
        startTime = lastTime;
    }

    //  calculate FPS
    void update(GlRender &renderer, bool showFPS)
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
        renderer.renderFPS(currentFPS, showFPS);
    }

    // Getter for FPS
    int getFPS() const
    {
        return (currentFPS > targetFPS ? targetFPS : currentFPS);
    }
};
