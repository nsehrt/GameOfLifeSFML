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


enum class GState
{
    Drawing,
    Playing
};

//hash function to enable std::pair as key
struct pair_hash
{
    template <class T1, class T2>
    std::size_t operator () (const std::pair<T1, T2>& p) const
    {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);

        return h1 ^ h2;
    }
};

//zoom to mouse position
template<typename T>
void zoomViewAt(sf::Vector2i pixel, T& window, float zoom)
{
    const sf::Vector2f beforeCoord{ window.mapPixelToCoords(pixel) };
    sf::View view{ window.getView() };
    view.zoom(zoom);
    window.setView(view);
    const sf::Vector2f afterCoord{ window.mapPixelToCoords(pixel) };
    const sf::Vector2f offsetCoords{ beforeCoord - afterCoord };
    view.move(offsetCoords);
    window.setView(view);
}

//vector helper functions
namespace sfm
{
    template<typename T>
    float vec2Magnitude(const sf::Vector2<T>& vector)
    {
        return (std::sqrt(vector.x * vector.x + vector.y * vector.y));
    }

    template<typename T>
    auto vec2Normalize(const sf::Vector2<T>& vector)
    {
        const float mag = vec2Magnitude(vector);
        return mag == 0.f ? sf::Vector2f{ 0.f,0.f } : vector / sfm::vec2Magnitude(vector);
    }
}



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
};