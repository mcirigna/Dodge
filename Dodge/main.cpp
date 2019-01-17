#include <iostream>
#include "Ecs/Ecs.hpp"
#include "Header/Components.hpp"
#include "Header/Systems.hpp"

void start()
{
    sf::Vector2f windowSize(1200, 625);
    sf::RenderWindow window(sf::VideoMode(windowSize.x, windowSize.y), "Window");
    unsigned int frameLimit = 60;
    window.setFramerateLimit(frameLimit);
    
    ecs::EntityManager em;
    
    Spawner spawner;
    spawner.init(em);
    
    Input input;
    EnemySystem enemies(windowSize.y);
    Motion motion(sf::FloatRect(sf::Vector2f(0,0), windowSize));
    Render render(&window);
    
    sf::Clock clock;
    sf::Time timeSinceLastUpdate;
    sf::Time TimePerFrame = sf::seconds(1.f/60.f);
    
    while(window.isOpen())
    {
        bool repaint = false;
        
        sf::Time deltaTime = clock.restart();
        timeSinceLastUpdate += deltaTime;
        if (timeSinceLastUpdate > TimePerFrame)
        {
            /* process events */
            sf::Event event;
            
            while (window.pollEvent(event))
            {
                switch (event.type)
                {
                    case sf::Event::Closed: // Exit
                        window.close();
                        break;
                }
            }
            
            
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                input.send(sf::Keyboard::Right);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                input.send(sf::Keyboard::Left);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                input.send(sf::Keyboard::Up);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                input.send(sf::Keyboard::Down);
            
            /* done */
            
            timeSinceLastUpdate -= TimePerFrame;
            repaint = true;
            
            input.update(em, TimePerFrame.asSeconds());
            enemies.update(em, TimePerFrame.asSeconds());
            motion.update(em, TimePerFrame.asSeconds());
            spawner.update(em, TimePerFrame.asSeconds());
        }
        
        if (repaint)
        {
            window.clear();
            render.update(em);
            window.display();
        }
    }
}

int main(int argc, const char * argv[])
{
    start();
}
