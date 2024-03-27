#pragma once

#include "Tilesheet.hpp"
#include <SFML/Graphics.hpp>

// TilesheetExplorer takes a tilesheet and renders it to a window and provides
// a way to explore the tilesheet by moving the view around the tilesheet
// giving the offset of the tile under the mouse cursor.

class TilesheetExplorer
{
    private:
        Tilesheet   *m_tilesheet;    // the tilesheet
        sf::Vector2f m_viewPos;      // position of the view
        sf::Vector2u m_tileSize;     // size of the tiles
        sf::Vector2u m_selectedTile; // selected tile
        bool         m_selected;     // true if a tile is selected
        sf::Vector2u m_scale;        // scale of the tiles
        sf::Font     m_font;         // font for the text
        sf::Text     m_text;         // text for the selected tile
        sf::Vector2f m_mousePos;     // position of the mouse
        sf::Vector2f m_lastMousePos; // last position of the mouse

    public:
        TilesheetExplorer(Tilesheet *tilesheet);
        ~TilesheetExplorer() = default;

        void run(sf::RenderWindow *window); // run the tilesheet explorer
};