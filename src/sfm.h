#pragma once
#include <SFML/System/Vector2.hpp>

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