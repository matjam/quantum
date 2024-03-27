#pragma once

/* PrefabRoomShape.hpp
 *
 * PrefabRoomShape defines the shape of a room by specifying the cells that make up the
 * room. Typically you would load a prefab from a file to define the shape of the
 * room, but you can also set the cells manually. The cells are stored in a vector
 * of Cells.
 */

#include "Cell.hpp"
#include <SFML/Graphics.hpp>
#include <vector>

class RoomShape
{
    private:
        sf::Vector2u      m_size;
        std::vector<Cell> m_cells;

    public:
        RoomShape();
        RoomShape(const sf::Vector2u &size);
        ~RoomShape() = default;

        void loadPrefab(const std::string &prefab);               // load a prefab from a file
        void setCell(const sf::Vector2u &offset, Cell cell);      // set a cell in the room
        Cell getCell(const sf::Vector2u &offset) const;           // get the cell at the specified offset
        bool isCell(const sf::Vector2u &offset, Cell cell) const; // check if the location contains the specified cell

        std::vector<Cell> getCells() const; // get the cells that make up the room
        sf::Vector2u      getSize() const;  // get the size of the room
};
