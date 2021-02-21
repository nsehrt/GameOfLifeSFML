#pragma once

#include <SFML/Graphics.hpp>
#include <array>

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
            setFillColor(sf::Color::Green);
        }
        else
        {
            setFillColor(sf::Color::Red);
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

    static int getAliveNeighbours(const std::vector<Cell>& grid, int x, int y)
    {
        int numNeighbours = 0;

        static const std::array<std::pair<int, int>, 8> possible = {
            std::make_pair(-1,-1), {0,-1},{1,-1},
            {-1,0},{1,0},
            {-1,1}, {0,1}, {1,1}
        };

        for(auto& [xp, yp] : possible)
        {
            int newX = x + xp;
            int newY = y + yp;

            if(newX < 0 || newX >= 60)
                continue;

            if(newY < 0 || newY >= 45)
                continue;

            int index = newX + newY * 60;

            if(grid[index].getStatus())
                numNeighbours++;

            if(numNeighbours == 4) break;
        }



        return numNeighbours;
    }

    private:

    bool mAlive = false;
    sf::Vector2<int> gridPos{};

};