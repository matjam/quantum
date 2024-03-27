#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <cstdio>
#include <spdlog/spdlog.h>

#include "Autotile.hpp"
#include "Maze.hpp"
#include "Tilemap.hpp"
#include "Tilesheet.hpp"
#include "TilesheetExplorer.hpp"

int main()
{
    spdlog::info("starting quantum ...");

    // configure spdlog debug mode
    spdlog::set_level(spdlog::level::debug);

    Tilemap tilemap("assets/RogueEnvironment16x16.png", sf::Vector2u(16, 16), sf::Vector2u(200, 200), 1);

    Maze maze(sf::Vector2u(200, 200));
    maze.generate(1000);

    spdlog::info("maze generated");

    // draw the maze to the tilemap
    Autotile autotile(&maze, &tilemap);
    autotile.render();

    // print current working directory using C++ standard library
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != nullptr) {
        spdlog::info("Current working directory: {}", cwd);
    } else {
        spdlog::error("getcwd() failed");
    }

    sf::RenderWindow window(sf::VideoMode(1920, 1028), "Quantum");
    window.setFramerateLimit(120);

    sf::Vector2f      viewPosition(0, 0);
    sf::Vector2f      viewDesiredPosition(0, 0);
    float             viewSpeed = 0.1;
    TilesheetExplorer explorer(tilemap.getTilesheet());

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::KeyPressed:
                switch (event.key.code) {
                case sf::Keyboard::Escape:
                    window.close();
                    break;
                case sf::Keyboard::F1:
                    explorer.run(&window);
                    break;
                default:
                    break;
                }
                break;
            default:
                break;
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            viewDesiredPosition.y -= 1;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            viewDesiredPosition.y += 1;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            viewDesiredPosition.x -= 1;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            viewDesiredPosition.x += 1;
        }

        window.clear();

        // draw the Tilemap to the window
        tilemap.draw(window, sf::FloatRect(0, 0, 1920, 1080), viewPosition, sf::Vector2u(2, 2));
        window.display();

        // move the view towards the desired position
        viewPosition += (viewDesiredPosition - viewPosition) * viewSpeed;
    }

    return EXIT_SUCCESS;
}
