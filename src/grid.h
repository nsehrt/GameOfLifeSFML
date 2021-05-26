#pragma once

#include <SFML/Graphics.hpp>

namespace sf 
{
    //a sfml drawable grid
    class Grid : public sf::Drawable
    {
        public:

        //contruct the grid with x columns, y rows and a tile size. default color is white
        explicit Grid(int tileSize = 32, int columns = 1000, int rows = 1000)
            : mColumns(columns), mRows(rows), mTileSize(tileSize), mArray(sf::Lines, (columns + rows)*2), mColor(sf::Color::White)
        {
            
            int index = 0;
            float tSizeF = static_cast<float>(tileSize);

            //vertical lines
            for(int x = 0; x < columns; x++)
            {
                mArray[index++].position = { x * tSizeF, 0 };
                mArray[index++].position = { x * tSizeF, columns* tSizeF };
            }

            //horizontal lines
            for(int y = 0; y < rows; y++)
            {
                mArray[index++].position = { 0, y* tSizeF };
                mArray[index++].position = { columns * tSizeF, y * tSizeF };
            }

            //default color
            setColor(mColor);
        }

        //set the grid's color
        void setColor(const sf::Color color)
        {
            for(int i = 0; i < mArray.getVertexCount(); i++)
            {
                mArray[i].color = color;
            }
        }

        void setOpacity(float opacity = 1.0f)
        {
            opacity = opacity < 0.f ? 0.f : opacity;
            sf::Color newColor = sf::Color(mColor.r, mColor.g, mColor.b, static_cast<std::uint8_t>(255.0f * opacity));
            for(int i = 0; i < mArray.getVertexCount(); i++)
            {
                mArray[i].color = newColor;
            }
        }

        private:

        //required by sf::drawable
        virtual void draw(sf::RenderTarget& renderTarget, sf::RenderStates states) const
        {
            sf::RenderStates rs{};

            const sf::View view = renderTarget.getView();
            const sf::Vector2 viewSize = view.getSize();
            const sf::Vector2 topLeft = { view.getCenter().x - viewSize.x,
                                         view.getCenter().y - viewSize.y };
            
            //do not draw if too far zoomed out
            if(viewSize.x < 10000.f && viewSize.y < 10000.f)
            {
                //translate grid, so that it appears to be infinite
                const float tileSizeF = static_cast<float>(mTileSize);
                const sf::Vector2 translation = { std::floor(topLeft.x / tileSizeF) * tileSizeF,
                                                  std::floor(topLeft.y / tileSizeF)* tileSizeF };
                rs.transform.translate(translation);
                renderTarget.draw(mArray, rs);
            }
            
        }

        sf::Color mColor;
        sf::VertexArray mArray;
        int mColumns;
        int mRows;
        int mTileSize;
    };

}