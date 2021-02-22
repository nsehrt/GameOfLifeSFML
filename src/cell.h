#pragma once

#include <SFML/Graphics.hpp>

//drawable cell
class Cell : public sf::RectangleShape
{
    public:

    explicit Cell() : sf::RectangleShape({ 20,20 })
    {
        setStatus(false);
    }

    void setStatus(bool alive = true)
    {
        mAlive = alive;

        if(mAlive)
        {
            setFillColor(aliveColor);
        }
        else
        {
            setFillColor(deadColor);
        }
    }

    bool getStatus()const
    {
        return mAlive;
    }

    void setGridPos(int x, int y)
    {
        gridPos = { x,y };
        setPosition(x * 20.f, y * 20.f);
    }

    int getGridX() const
    {
        return gridPos.x;
    }

    int getGridY() const
    {
        return gridPos.y;
    }

    sf::Color aliveColor = sf::Color::Green;
    sf::Color deadColor = sf::Color::Red;

    private:

    bool mAlive = false;
    sf::Vector2<int> gridPos{};
    
};