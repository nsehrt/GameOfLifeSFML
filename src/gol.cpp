#include "gol.h"

void Gol::init()
{
    sf::ContextSettings settings{};
    settings.antialiasingLevel = 16;
    window.create(sf::VideoMode(1200, 900), "", sf::Style::Default, settings);
    window.setVerticalSyncEnabled(true);
    window.setTitle("Game of life SFML");

    renderTexture.create(1200, 900, settings);

    ImGui::SFML::Init(window);

    cells.reserve(5000);
    toDelete.reserve(1000);
    newCells.reserve(1000);

}

bool Gol::run()
{
    gt.reset();
    float stepTimer = 0.0f;
    float stepTime = 0.25f;
    int amountAlive = 0;

    bool mousePrev = false;
    bool mouseCurr = false;

    float moveSpeed = 0.0f;
    sf::Time calcTime{};

    float zoomCurrent = 1.0f;
    float zoomAmount = 1.05f;
    std::array<float, 3> bgColor{0.f};
    sf::Color sfColor{32, 84, 91};
    bgColor[0] = sfColor.r / 255.f;
    bgColor[1] = sfColor.g / 255.f;
    bgColor[2] = sfColor.b / 255.f;

    while(window.isOpen())
    {
        gt.update();

        sf::Event event;
        while(window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);

            if(event.type == sf::Event::Closed)
            {
                window.close();
            }

            if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
            {
                window.close();
            }

            if(event.type == sf::Event::MouseWheelScrolled)
            {
                if(event.mouseWheelScroll.delta > 0)
                {
                    zoomViewAt({ event.mouseWheelScroll.x, event.mouseWheelScroll.y }, renderTexture, (1.f / zoomAmount));
                    zoomCurrent *= (1.f / zoomAmount);
                }    
                else if(event.mouseWheelScroll.delta < 0)
                {
                    zoomViewAt({ event.mouseWheelScroll.x, event.mouseWheelScroll.y }, renderTexture, zoomAmount);
                    zoomCurrent *= zoomAmount;
                }

            }

            if(event.type == sf::Event::MouseButtonPressed && sf::Mouse::isButtonPressed(sf::Mouse::Middle))
            {
                mouseDrag.startDrag();
            }
            else if(event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Middle)
            {
                mouseDrag.endDrag();
            }


            if(event.type == sf::Event::Resized)
            {
                // update the view to the new size of the window
                sf::FloatRect visibleArea(0.f, 0.f, static_cast<float>(event.size.width), static_cast<float>(event.size.height));
                renderTexture.setView(sf::View(visibleArea));
            }

        }

        const sf::Time deltaTime = gt.getDelta();
        const float fTime = deltaTime.asSeconds();

        mousePrev = mouseCurr;

        sf::View mainView = renderTexture.getView();

        //process mouse
        const sf::Vector2<int> mPos{ (int)mainView.getCenter().x - (int)renderTexture.getSize().x / 2, (int)mainView.getCenter().y - (int)renderTexture.getSize().y / 2 };
        const auto mousePos = sf::Mouse::getPosition(window) + mPos;
        mouseCurr = sf::Mouse::isButtonPressed(sf::Mouse::Left);

        //update mouse drag
        mouseDrag.setSpeed(zoomCurrent);
        mouseDrag.update(mainView);

        

        if(gs == GState::Playing)
        {

            stepTimer += gt.getDelta().asSeconds();

            if(stepTimer >= stepTime)
            {
                sf::Clock cClock{};
                stepTimer -= stepTime;
                amountAlive = 0;
                amountSteps++;

                static const std::array<PosPair, 8> possible = {
                    std::make_pair(-1,-1), {0,-1},{1,-1},
                    {-1,0},{1,0},
                    {-1,1}, {0,1}, {1,1}
                                };

                //delete all dead cells
                toDelete.clear();

                for(const auto& [pos, cell] : cells)
                {
                    if(cell.getStatus() == false)
                        toDelete.push_back(pos);
                }
                
                for(const auto& pos : toDelete)
                {
                    cells.erase(pos);
                }

                //fill in dead cells around live ones
                newCells.clear();

                for(const auto& [pos, cell] : cells)
                {
                    for(const auto& [xp,yp] : possible)
                    {
                        const int newX = pos.first + xp;
                        const int newY = pos.second + yp;

                        if(!existsCellAtPosition({ newX,newY }))
                        {
                            Cell newCell{};
                            newCell.setGridPos(newX, newY);
                            newCell.setStatus(false);
                            newCells.push_back(newCell);
                        }

                    }
                }

                for(auto& cell : newCells)
                {
                    cells[{cell.getGridX(), cell.getGridY()}] = cell;
                }

                //get all alive cells
                std::vector<Cell*> toAlive{}, toDead{};

                for(auto& [pos, cell] : cells)
                {
                    int neighbours = getAliveNeighbours(cells, { cell.getGridX(), cell.getGridY() });

                    if(cell.getStatus())
                    {
                        amountAlive++;
                        if(neighbours <= 1 || neighbours >= 4)
                            toDead.push_back(&cell);
                    }
                    else
                    {
                        if(neighbours == 3)
                            toAlive.push_back(&cell);
                    }
                }

                for(auto& cell : toAlive)
                {
                    cell->setStatus(true);
                }

                for(auto& cell : toDead)
                {
                    cell->setStatus(false);
                }

                amountAlive -= static_cast<int>(toDead.size());
                calcTime = cClock.getElapsedTime();
            }

        }


        //toggle the status of a cell
        if(!mouseCurr && mousePrev && gs == GState::Drawing
           && !ImGui::GetIO().WantCaptureMouse)
        {

            //create new cell or delete it

            sf::Vector2i PixelPos = sf::Mouse::getPosition(window);
            int x = static_cast<sf::Vector2i>(window.mapPixelToCoords(PixelPos, renderTexture.getView())).x / 20;
            int y = static_cast<sf::Vector2i>(window.mapPixelToCoords(PixelPos, renderTexture.getView())).y / 20;

            bool exists = existsCellAtPosition({ x,y });

            if(!exists)
            {
                Cell newCell{};
                newCell.setGridPos(x, y);
                newCell.setStatus();
                cells[{x, y}] = newCell;
            }
            else
            {
                cells.erase({ x,y });
            }
        }



        //IMGui setup
        ImGui::SFML::Update(window, deltaTime);

        ImGui::Begin("settings window");


        if(ImGui::Button(gs == GState::Drawing ? "Play" : "Pause"))
        {
            gs = gs == GState::Drawing ? GState::Playing : GState::Drawing;
            if(gs == GState::Playing) amountSteps = 0;
        }

        ImGui::Text("Alive cells: %d", amountAlive);
        ImGui::Text("Step number: %d", amountSteps);
        ImGui::Text("Step calc time: %d ms", calcTime.asMilliseconds());
        ImGui::SliderFloat("Step time", &stepTime, 0.010f, 1.0f);

        ImGui::Separator();

        if(ImGui::ColorEdit3("Background", bgColor.data()))
        {
            sfColor.r = static_cast<sf::Uint8>(bgColor[0] * 255.f);
            sfColor.g = static_cast<sf::Uint8>(bgColor[1] * 255.f);
            sfColor.b = static_cast<sf::Uint8>(bgColor[2] * 255.f);
        }

        ImGui::Checkbox("Draw to be checked cells", &drawCheckCells);
        ImGui::Checkbox("Draw grid", &drawGrid);

        ImGui::End();


        //render to texture
        renderTexture.clear(sfColor);
        renderTexture.setView(mainView);

        if(drawCheckCells)
        {
            for(const auto& [k, v] : cells)
            {
                renderTexture.draw(v);
            }
        }
        else
        {
            for(const auto& [k, v] : cells)
            {
                if(v.getStatus())
                renderTexture.draw(v);
            }
        }

        if(drawGrid)
        {
            renderTexture.draw(grid);
        }

        renderTexture.display();

        const sf::Texture& texture = renderTexture.getTexture();
        sf::Sprite renderSprite(texture);


        //final render to window
        window.clear();

        window.draw(renderSprite);

        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();

    return EXIT_SUCCESS;
}
