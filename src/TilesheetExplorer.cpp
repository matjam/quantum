#include <algorithm>
#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include "TilesheetExplorer.hpp"

TilesheetExplorer::TilesheetExplorer(Tilesheet *tilesheet)
{
    m_tilesheet = tilesheet;
    m_viewPos   = sf::Vector2f(-50, -50);
    m_tileSize  = m_tilesheet->getTileSize();
    m_scale     = sf::Vector2u(2, 2);

    m_font.loadFromFile("assets/fonts/TerminessNerdFontMono-Regular.ttf");
    m_font.setSmooth(true);
    m_text.setFont(m_font);
    m_text.setCharacterSize(24);
    m_text.setFillColor(sf::Color::White);
    m_text.setPosition(10, 10);

    spdlog::info("TilesheetExplorer::TilesheetExplorer() initialized");
}

void TilesheetExplorer::run(sf::RenderWindow *window)
{
    spdlog::info("TilesheetExplorer::run() entered");
    bool running = true;

    m_lastMousePos = sf::Vector2f(0, 0);
    m_mousePos     = sf::Vector2f(0, 0);
    bool mouseDown = false;

    // a vector of selected tiles that we will shade red when the user presses space
    std::vector<bool> selected_tiles;
    selected_tiles.resize(m_tilesheet->getTileCount(), false);

    while (running) {
        sf::Event event;

        while (window->pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                running = false;
            }

            if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                case sf::Keyboard::Escape:
                    running = false;
                    break;
                case sf::Keyboard::PageUp:
                    m_scale.x = std::min(m_scale.x + 1, 8u);
                    m_scale.y = std::min(m_scale.y + 1, 8u);
                    break;
                case sf::Keyboard::PageDown:
                    m_scale.x = std::max(m_scale.x - 1, 1u);
                    m_scale.y = std::max(m_scale.y - 1, 1u);
                    break;
                case sf::Keyboard::Space:
                    // check if this tile is already selected, we toggle the selection
                    if (selected_tiles[m_selectedTile.y * m_tilesheet->getTilesPerRow() + m_selectedTile.x]) {
                        selected_tiles[m_selectedTile.y * m_tilesheet->getTilesPerRow() + m_selectedTile.x] = false;
                    } else {
                        selected_tiles[m_selectedTile.y * m_tilesheet->getTilesPerRow() + m_selectedTile.x] = true;
                    }

                    break;
                default:
                    break;
                }
            }

            if (event.type == sf::Event::MouseWheelScrolled) {
                if (event.mouseWheelScroll.delta > 0) {
                    m_scale.x = std::min(m_scale.x + 1, 8u);
                    m_scale.y = std::min(m_scale.y + 1, 8u);
                } else {
                    m_scale.x = std::max(m_scale.x - 1, 1u);
                    m_scale.y = std::max(m_scale.y - 1, 1u);
                }
            }

            if (event.type == sf::Event::MouseMoved) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(*window);
                m_selectedTile        = sf::Vector2u((mousePos.x + m_viewPos.x) / (m_tileSize.x * m_scale.x),
                                              (mousePos.y + m_viewPos.y) / (m_tileSize.y * m_scale.y));
                m_selected            = true;
            }
        }

        // if the mouse is down, check if it was down last frame and if it was, move the view
        // by dragging the mouse like you are grabbing the tilesheet and moving it around.
        // If it wasn't down last frame, just set the last mouse position to the current mouse position.
        // We need to make sure when we drag down the spritesheet, the view moves up, and when we drag
        // up the spritesheet, the view moves down. The same goes for left and right.
        //
        // We also don't want the sheet to snap to the mouse position, so we need to keep track of the
        // last mouse position and the current mouse position and move the view by the difference between
        // the two.

        if (window->hasFocus() && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            m_mousePos = sf::Vector2f(sf::Mouse::getPosition(*window));
            if (mouseDown) {
                m_viewPos.x += (m_lastMousePos.x - m_mousePos.x);
                m_viewPos.y += (m_lastMousePos.y - m_mousePos.y);
            }
            m_lastMousePos = m_mousePos;
            mouseDown      = true;
        } else {
            mouseDown = false;
        }

        if (window->hasFocus() && sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
            // reset the view position
            m_viewPos = sf::Vector2f(-50, -50);
        }

        if (window->hasFocus() && sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            m_viewPos.x -= m_tileSize.x * m_scale.x;
        }

        if (window->hasFocus() && sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            m_viewPos.x += m_tileSize.x * m_scale.x;
        }

        if (window->hasFocus() && sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            m_viewPos.y -= m_tileSize.y * m_scale.y;
        }

        if (window->hasFocus() && sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            m_viewPos.y += m_tileSize.y * m_scale.y;
        }

        window->clear(sf::Color::Black);

        for (sf::Uint32 i = 0; i < m_tilesheet->getTileCount(); i++) {
            sf::Sprite *tile = m_tilesheet->getTile(i, m_scale);
            tile->setPosition((i % m_tilesheet->getTilesPerRow()) * m_tileSize.x * m_scale.x - m_viewPos.x,
                              (i / m_tilesheet->getTilesPerRow()) * m_tileSize.y * m_scale.y - m_viewPos.y);
            window->draw(*tile);
        }

        if (m_selected) {
            // if the mouse is not over a tile, display nothing
            if (m_selectedTile.x >= m_tilesheet->getTilesPerRow() ||
                m_selectedTile.y >= m_tilesheet->getTileCount() / m_tilesheet->getTilesPerRow())
            {
                m_text.setString("select a tile to view its ID");
            } else {

                // tiles are numbered from left to right, top to bottom.
                m_text.setString(fmt::format("Selected tile: ({}, {}) ID: {}",
                                             m_selectedTile.x,
                                             m_selectedTile.y,
                                             m_selectedTile.y * m_tilesheet->getTilesPerRow() + m_selectedTile.x));
            }
        }

        window->clear(sf::Color::Black);

        for (sf::Uint32 i = 0; i < m_tilesheet->getTileCount(); i++) {
            sf::Sprite *tile = m_tilesheet->getTile(i, m_scale);
            tile->setPosition((i % m_tilesheet->getTilesPerRow()) * m_tileSize.x * m_scale.x - m_viewPos.x,
                              (i / m_tilesheet->getTilesPerRow()) * m_tileSize.y * m_scale.y - m_viewPos.y);
            window->draw(*tile);
        }

        // shade the selected tiles red
        for (sf::Uint32 i = 0; i < m_tilesheet->getTileCount(); i++) {
            sf::Sprite *tile = m_tilesheet->getTile(i, m_scale);
            tile->setPosition((i % m_tilesheet->getTilesPerRow()) * m_tileSize.x * m_scale.x - m_viewPos.x,
                              (i / m_tilesheet->getTilesPerRow()) * m_tileSize.y * m_scale.y - m_viewPos.y);
            if (selected_tiles[i]) {
                // tile needs to be 50% opaque
                tile->setColor(sf::Color(255, 0, 0, 128));
            } else {
                // tile needs to be completely transparent
                tile->setColor(sf::Color::White);
            }

            window->draw(*tile);
        }

        // draw a black rectangle to hold the text
        sf::RectangleShape rect(sf::Vector2f(400, 50));
        rect.setFillColor(sf::Color::Black);
        rect.setPosition(0, 0);
        window->draw(rect);
        window->draw(m_text);
        window->display();
    }

    spdlog::info("TilesheetExplorer::run() exited");
}