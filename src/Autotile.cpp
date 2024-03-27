#include "Autotile.hpp"

#include <spdlog/spdlog.h>

enum Direction
{
    NONE                  = 0,
    NORTH                 = 1,
    WEST                  = 2,
    NORTH_WEST            = 3,
    EAST                  = 4,
    NORTH_EAST            = 5,
    WEST_EAST             = 6,
    NORTH_WEST_EAST       = 7,
    SOUTH                 = 8,
    NORTH_SOUTH           = 9,
    WEST_SOUTH            = 10,
    NORTH_WEST_SOUTH      = 11,
    EAST_SOUTH            = 12,
    NORTH_EAST_SOUTH      = 13,
    WEST_EAST_SOUTH       = 14,
    NORTH_WEST_EAST_SOUTH = 15
};

Autotile::Autotile(Maze *maze, Tilemap *tilemap)
{
    m_tilemap = tilemap;
    m_maze    = maze;
    m_walls.reserve(16);

    // wall tiles are calculated based on their neighor using a bitmask. The
    // bitmask is calculated by setting the bit to 1 if the neighbor is a wall
    // and 0 if the neighbor is a floor. The bits are set in the following order:
    //
    //   1        bit 0
    // 2   4      bit 1 and bit 2
    //   8        bit 3
    //
    // this is a 4-bit number, so there are 16 possible combinations. The
    // wall tiles are calculated by iterating over the bitmask and setting
    // the wall tile based on the bitmask. The wall tiles are stored in a vector
    // and the index of the vector is the bitmask.
    //
    // 0 represents a wall with no neighbors

    m_walls[NONE]                  = 20; // a wall with no neighbors
    m_walls[NORTH]                 = 50; // a wall with a neighbor to the north
    m_walls[WEST]                  = 17; // a wall with a neighbor to the west
    m_walls[NORTH_WEST]            = 51; // a wall with neighbors to the north and west
    m_walls[EAST]                  = 18; // a wall with a neighbor to the east
    m_walls[NORTH_EAST]            = 48; // a wall with neighbors to the north and east
    m_walls[WEST_EAST]             = 2;  // a wall with neighbors to the west and east
    m_walls[NORTH_WEST_EAST]       = 49; // a wall with neighbors to the north, west, and east
    m_walls[SOUTH]                 = 33; // a wall with a neighbor to the south
    m_walls[NORTH_SOUTH]           = 32; // a wall with neighbors to the north and south
    m_walls[WEST_SOUTH]            = 4;  // a wall with neighbors to the west and south
    m_walls[NORTH_WEST_SOUTH]      = 19; // a wall with neighbors to the north, west, and south
    m_walls[EAST_SOUTH]            = 0;  // a wall with neighbors to the east and south
    m_walls[NORTH_EAST_SOUTH]      = 16; // a wall with neighbors to the north, east, and south
    m_walls[WEST_EAST_SOUTH]       = 34; // a wall with neighbors to the west, east, and south
    m_walls[NORTH_WEST_EAST_SOUTH] = 1;  // a wall with neighbors to the north, west, east, and south

    m_floor = 7;  // a floor tile
    m_void  = 55; // if a tile is completely surrounded by walls, it is a void tile

    spdlog::info("Autotile::Autotile() initialized");
}

bool Autotile::isVoid(unsigned int x, unsigned int y)
{
    if (m_maze->isCell(sf::Vector2u(x - 1, y - 1), Cell::WALL) &&
        m_maze->isCell(sf::Vector2u(x + 1, y - 1), Cell::WALL) &&
        m_maze->isCell(sf::Vector2u(x - 1, y + 1), Cell::WALL) &&
        m_maze->isCell(sf::Vector2u(x + 1, y + 1), Cell::WALL))
    {
        return true;
    }

    return false;
}

bool Autotile::isNotWall(unsigned int x, unsigned int y)
{
    return m_maze->isCell(sf::Vector2u(x, y), Cell::ROOM) || m_maze->isCell(sf::Vector2u(x, y), Cell::CORRIDOR) ||
           m_maze->isCell(sf::Vector2u(x, y), Cell::DOOR);
}

bool Autotile::isWall(unsigned int x, unsigned int y)
{
    return m_maze->isCell(sf::Vector2u(x, y), Cell::WALL);
}

sf::Uint32 Autotile::voidMask(unsigned int x, unsigned int y)
{
    sf::Uint32 bitmask = 0;

    if (isVoid(x, y - 1)) {
        bitmask |= NORTH;
    }

    if (isVoid(x - 1, y)) {
        bitmask |= WEST;
    }

    if (isVoid(x + 1, y)) {
        bitmask |= EAST;
    }

    if (isVoid(x, y + 1)) {
        bitmask |= SOUTH;
    }

    return bitmask;
}

void Autotile::render()
{
    for (sf::Uint32 y = 0; y < m_maze->getSize().y; y++) {
        for (sf::Uint32 x = 0; x < m_maze->getSize().x; x++) {
            if (isVoid(x, y)) {
                m_tilemap->setTile(0, sf::Vector2u(x, y), m_void);
                continue;
            }

            Cell cell = m_maze->getCell(sf::Vector2u(x, y));

            if (cell == Cell::WALL) {
                sf::Uint32 bitmask = 0;

                if (isWall(x, y - 1)) {
                    bitmask |= NORTH;
                }

                if (isWall(x - 1, y)) {
                    bitmask |= WEST;
                }

                if (isWall(x + 1, y)) {
                    bitmask |= EAST;
                }

                if (isWall(x, y + 1)) {
                    bitmask |= SOUTH;
                }

                // when we have a wall with a wall on either side, and a floor tile on one side and a void tile on the
                // other we need to not choose the bitmask that will render a wall into the void tile

                if (bitmask == WEST_EAST_SOUTH) {
                    if (isVoid(x - 1, y)) {
                        bitmask = EAST_SOUTH;
                    } else if (isVoid(x + 1, y)) {
                        bitmask = WEST_SOUTH;
                    } else if (isVoid(x, y + 1)) {
                        bitmask = WEST_EAST;
                    }
                    m_tilemap->setTile(0, sf::Vector2u(x, y), m_walls[bitmask]);
                    continue;
                }

                if (bitmask == NORTH_WEST_EAST) {
                    if (isVoid(x, y - 1)) {
                        bitmask = WEST_EAST;
                    } else if (isVoid(x - 1, y)) {
                        bitmask = NORTH_EAST;
                    } else if (isVoid(x + 1, y)) {
                        bitmask = NORTH_WEST;
                    }
                    m_tilemap->setTile(0, sf::Vector2u(x, y), m_walls[bitmask]);
                    continue;
                }

                if (bitmask == NORTH_WEST_SOUTH) {
                    if (isVoid(x, y - 1)) {
                        bitmask = WEST_SOUTH;
                    } else if (isVoid(x - 1, y)) {
                        bitmask = NORTH_SOUTH;
                    } else if (isVoid(x, y + 1)) {
                        bitmask = NORTH_WEST;
                    }
                    m_tilemap->setTile(0, sf::Vector2u(x, y), m_walls[bitmask]);
                    continue;
                }

                if (bitmask == NORTH_EAST_SOUTH) {
                    if (isVoid(x, y + 1)) {
                        bitmask = NORTH_EAST;
                    } else if (isVoid(x + 1, y)) {
                        bitmask = NORTH_SOUTH;
                    } else if (isVoid(x, y - 1)) {
                        bitmask = EAST_SOUTH;
                    }
                    m_tilemap->setTile(0, sf::Vector2u(x, y), m_walls[bitmask]);
                    continue;
                }

                m_tilemap->setTile(0, sf::Vector2u(x, y), m_walls[bitmask]);
            } else {
                m_tilemap->setTile(0, sf::Vector2u(x, y), m_floor);
            }
        }
    }
}
