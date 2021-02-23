#pragma once

#include <tuple>
#include <SFML/Graphics.hpp>

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