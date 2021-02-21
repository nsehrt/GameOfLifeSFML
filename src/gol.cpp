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

    cells.reserve(60 * 45);

    for(int i = 0; i < 60 * 45; i++)
    {
        Cell cell{};

        cell.setGridPos(i % 60, i / 60);

        cells.push_back(cell);
    }
}

bool Gol::run()
{
    gt.reset();
    float stepTimer = 0.0f;
    float stepTime = 1.0f;
    int amountAlive = 0;
    bool mousePrev = false;
    bool mouseCurr = false;
    float moveSpeed = 0.0f;
    sf::Time calcTime{};

    float zoom = 1.0f;
    float zoomAmount = 1.1f;

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
                    zoomViewAt({ event.mouseWheelScroll.x, event.mouseWheelScroll.y }, renderTexture, (1.f / zoomAmount));
                else if(event.mouseWheelScroll.delta < 0)
                    zoomViewAt({ event.mouseWheelScroll.x, event.mouseWheelScroll.y }, renderTexture, zoomAmount);


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

        //view move
        sf::Vector2f translation{ 0.f,0.f };

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            translation.y -= 500.f;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            translation.y += 500.f;
            
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            translation.x -= 500.f;
 
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            translation.x += 500.f;
        }

        const auto normTranslation = sfm::vec2Normalize(translation);

        moveSpeed = sfm::vec2Magnitude(normTranslation);

        mainView.move(normTranslation * mSpeed * fTime);


        //process mouse
        const sf::Vector2<int> mPos{ (int)mainView.getCenter().x - (int)renderTexture.getSize().x / 2, (int)mainView.getCenter().y - (int)renderTexture.getSize().y / 2 };
        const auto mousePos = sf::Mouse::getPosition(window) + mPos;
        mouseCurr = sf::Mouse::isButtonPressed(sf::Mouse::Left);


        if(gs == GState::Playing)
        {

            stepTimer += gt.getDelta().asSeconds();

            if(stepTimer >= stepTime)
            {
                sf::Clock cClock{};
                stepTimer -= stepTime;
                amountAlive = 0;

                //get all alive cells
                std::vector<Cell*> toAlive{}, toDead{};

                for(auto& cell : cells)
                {
                    int neighbours = Cell::getAliveNeighbours(cells, cell.getGridX(), cell.getGridY());

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


        if(!mouseCurr && mousePrev && gs == GState::Drawing
           && !ImGui::GetIO().WantCaptureMouse)
        {
            sf::Vector2i PixelPos = sf::Mouse::getPosition(window);
            int x = static_cast<sf::Vector2i>(window.mapPixelToCoords(PixelPos, renderTexture.getView())).x / 20;
            int y = static_cast<sf::Vector2i>(window.mapPixelToCoords(PixelPos, renderTexture.getView())).y / 20;

            int pos = x + (y * 60);
            if(pos < 60 * 45 && pos >= 0)
                cells[pos].setStatus(!cells[pos].getStatus());
        }



        //render
        ImGui::SFML::Update(window, deltaTime);

        ImGui::Begin("settings window"); // begin window


        if(ImGui::Button(gs == GState::Drawing ? "Play" : "Pause"))
        {
            gs = gs == GState::Drawing ? GState::Playing : GState::Drawing;
        }

        // Window title text edit
        //ImGui::Text("Move speed: %.2f", moveSpeed);
        ImGui::Text("Alive cells: %d", amountAlive);
        ImGui::SliderFloat("Step time", &stepTime, 0.05f, 2.0f);

        ImGui::End();

        renderTexture.clear();


        renderTexture.setView(mainView);



        for(const auto& c : cells)
        {
            renderTexture.draw(c);
        }

        renderTexture.draw(grid);

        renderTexture.display();

        const sf::Texture& texture = renderTexture.getTexture();
        sf::Sprite renderSprite(texture);

        window.clear();


        window.draw(renderSprite);

        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();

    return EXIT_SUCCESS;
}
