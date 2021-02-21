#pragma once

#include "imgui.h"
#include "imgui-SFML.h"

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics.hpp>
#include <array>
#include <iostream>

#include "gametime.h"
#include "grid.h"
#include "cell.h"

enum class GState
{
    Drawing,
    Playing
};

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
    public:

    explicit Gol() : grid(20) {};

    void init();
    bool run();

    private:

    GState gs = GState::Drawing;
    sf::RenderWindow window;
    sf::RenderTexture renderTexture;
    sf::Grid grid;
    std::vector<Cell> cells{};
    GameTime gt{};
    const float mSpeed = 750.0f;
};