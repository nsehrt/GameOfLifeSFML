#pragma once

#include <SFML/System/Clock.hpp>

class GameTime
{
    public:

    explicit GameTime() = default;

    //resets all internal clocks
    void reset()
    {
        mInternalDeltaClock.restart();
        mInternalTotalClock.restart();
    }

    //updates the delta time
    void update()
    {
        mInternalDeltaTime = mInternalDeltaClock.restart();
    }

    //gets time scaled delta or true delta
    sf::Time getDelta(bool scaled = true)
    {
        return scaled ? mInternalDeltaTime * mTimeScale : mInternalDeltaTime;
    }

    //gets time scaled total or true total
    sf::Time getTotal(bool scaled = true)
    {
        return scaled ? mInternalTotalClock.getElapsedTime() * mTimeScale : mInternalTotalClock.getElapsedTime();
    }

    //set the time scale, 1.0f is default, 2.0f is twice as fast etc.
    void setTimeScale(float timeScale = 1.0f)
    {
        mTimeScale = std::abs(timeScale);
    }

    private:

    sf::Clock mInternalDeltaClock{};
    sf::Clock mInternalTotalClock{};
    sf::Time mInternalDeltaTime{};

    float mTimeScale = 1.0f;
};