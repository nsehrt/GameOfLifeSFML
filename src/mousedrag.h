#pragma once

#include <SFML/Window/Mouse.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Vector2.hpp>

namespace sf 
{
    //use the mouse to drag a sf::View around
    class MouseDrag
    {
        public:

        explicit MouseDrag() = default;

        //call this when the button is pressed that starts the drag
        void startDrag()
        {
            if(mPressed) return;
            mStart = sf::Mouse::getPosition();
            mPressed = true;
        }

        //call this when that button is released
        void endDrag()
        {
            mPressed = false;
        }

        //call this in the main loop after the event polling
        void update(sf::View& view)
        {
            if(!mPressed) return;

            //calculate difference to start point
            const sf::Vector2i mousePos = sf::Mouse::getPosition();
            const sf::Vector2i delta = mousePos - mStart;
            mStart = mousePos;
            view.move(sf::Vector2f(delta)* -1.f * mSpeed);
        }

        //drag speed (for example set this to the total zoom value)
        void setSpeed(float speed)
        {
            mSpeed = speed;
        }

        //get the current drag speed
        float getSpeed() const
        {
            return mSpeed;
        }


        private:

        sf::Vector2i mStart{};
        bool mPressed = false;
        float mSpeed = 1.0f;
    };
}