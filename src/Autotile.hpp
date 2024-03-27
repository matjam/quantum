#pragma once

#include "Maze.hpp"
#include "Tilemap.hpp"

// Autotile is a class that renders a maze to a tilemap. The maze is rendered
// by iterating over the cells in the maze and rendering the cells to the tilemap.
//
// Walls are automatically rendered based on the types of the adjacent cells.

class Autotile
{
    private:
        Tilemap                *m_tilemap;
        Maze                   *m_maze;
        std::vector<sf::Uint32> m_walls; // vector of wall tiles based on the type of the adjacent cells
        sf::Uint32              m_floor; // id of the floor tile
        sf::Uint32              m_void;  // id of the void tile

    public:
        Autotile(Maze *maze, Tilemap *tilemap);
        ~Autotile() = default;

        // render the maze to the tilemap
        void render();

        bool       isVoid(unsigned int x, unsigned int y);
        bool       isNotWall(unsigned int x, unsigned int y);
        bool       isWall(unsigned int x, unsigned int y);
        sf::Uint32 voidMask(unsigned int x, unsigned int y);
};
