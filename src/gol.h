#pragma once

#include "imgui.h"
#include "imgui-SFML.h"

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics.hpp>
#include <array>
#include <unordered_map>
#include <iostream>

#include "gametime.h"
#include "grid.h"
#include "cell.h"
#include "mousedrag.h"
#include "sfm.h"
#include "misc.h"

enum class GState
{
    Drawing,
    Playing
};

class Gol
{

    using PosPair = std::pair<int, int>;

    public:

    explicit Gol() : grid(20) {};

    void init();
    bool run();

    bool existsCellAtPosition(const PosPair& p) const
    {
        if(cells.find(p) == cells.end())
            return false;
        return true;
    }

    bool existsCellAtPosition(const PosPair&& p) const
    {
        return existsCellAtPosition(p);
    }

    int getAliveNeighbours(std::unordered_map<PosPair, Cell, pair_hash>& grid, const PosPair& pos)
    {
        int numNeighbours = 0;

        static const std::array<std::pair<int, int>, 8> possible = {
            std::make_pair(-1,-1), {0,-1},{1,-1},
            {-1,0},{1,0},
            {-1,1}, {0,1}, {1,1}
        };

        for(auto& [xp, yp] : possible)
        {
            int newX = pos.first + xp;
            int newY = pos.second + yp;

            if(existsCellAtPosition({ newX, newY }) && grid[{newX, newY}].getStatus())
                numNeighbours++;

            if(numNeighbours == 4) break;
        }



        return numNeighbours;
    }

    private:

    GState gs = GState::Drawing;
    sf::RenderWindow window;
    sf::RenderTexture renderTexture;
    sf::Grid grid;
    std::unordered_map<PosPair, Cell, pair_hash> cells{};
    std::vector<PosPair> toDelete{};
    std::vector<Cell> newCells;
    
    GameTime gt{};
    sf::MouseDrag mouseDrag{};
    const float mSpeed = 750.0f;
    bool drawCheckCells = false;
    bool drawGrid = true;
    int amountSteps = 0;
    bool controlWindowOpen = false;
};