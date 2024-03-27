#include "Maze.hpp"
#include "RoomShape.hpp"

#include <spdlog/spdlog.h>

// A maze generator that uses the methodology described in
// https://journal.stuffwithstuff.com/2014/12/21/rooms-and-mazes/
// to generate a maze with rooms and corridors.
//
// The maze is represented as a 2D grid of cells. Each cell can be one of
// four types: room, corridor, wall, or door. The maze is generated by
// starting with a grid of walls, and then carving out rooms and corridors
// using a random walk algorithm.
//
// The maze is generated in three phases; first, the rooms are generated,
// then the corridors are generated, and finally the doors are placed.
//
// As we generate a room or corridor, we keep track of the cells that are
// part of the room or corridor, and we use this information to place doors
// between the rooms and corridors.
//
// We also keep track of the cells that are part of the rooms, and assign
// each room a unique region ID. This allows us to determine which cells
// are part of which room, and to determine the connectivity of the rooms.
//
// Finally, we randomly place doors between the rooms and corridors until
// we have joined all the rooms and corridors into a single connected maze.

Maze::Maze(const sf::Vector2u &size)
{
    m_size = size;
    m_cells.resize(size.x * size.y, Cell::WALL);

    // generate a set of prefab rooms
    m_prefabs.push_back(RoomShape(sf::Vector2u(3, 3)));
    m_prefabs.push_back(RoomShape(sf::Vector2u(3, 5)));
    m_prefabs.push_back(RoomShape(sf::Vector2u(5, 3)));
    m_prefabs.push_back(RoomShape(sf::Vector2u(5, 5)));
    m_prefabs.push_back(RoomShape(sf::Vector2u(7, 7)));
    m_prefabs.push_back(RoomShape(sf::Vector2u(9, 9)));
    m_prefabs.push_back(RoomShape(sf::Vector2u(11, 11)));
    m_prefabs.push_back(RoomShape(sf::Vector2u(7, 11)));
    m_prefabs.push_back(RoomShape(sf::Vector2u(11, 7)));
    m_prefabs.push_back(RoomShape(sf::Vector2u(9, 5)));
    m_prefabs.push_back(RoomShape(sf::Vector2u(5, 9)));
}

void Maze::generate(sf::Uint32 seed)
{
    spdlog::info("Maze::generate: generating maze with seed {}", seed);

    m_seed = seed;
    m_gen.seed(seed);

    // generate the rooms
    generateRooms(5000);

    // generate the corridors
    generateCorridors();

    // find the connectors
    findConnectors();

    // connect the rooms
    connectRooms();

    // remove dead ends
    removeDeadEnds(100);
}

void Maze::generateRooms(sf::Uint32 max_attempts)
{
    spdlog::info("Maze::generateRooms: generating rooms with {} attempts", max_attempts);

    // generate a room in the maze, up to the maximum number of attempts
    for (sf::Uint32 i = 0; i < max_attempts; i++) {
        // pick a random room shape
        sf::Vector2u size(0, 0);

        // pick a random prefab room
        RoomShape room = m_prefabs[m_gen() % m_prefabs.size()];

        // pick a random location for the room. It needs to fit in the maze, and not
        // overlap with any other rooms, and it must be an odd number of cells wide
        // and high and located on an odd number of cells.

        sf::Vector2u offset(0, 0);

        // quantize the offset to an odd number
        offset.x = m_gen() % (m_size.x / 2) * 2 + 1;
        offset.y = m_gen() % (m_size.y / 2) * 2 + 1;

        if (!roomFits(room, offset)) {
            spdlog::info("Maze::generateRooms: room does not fit at ({}, {})", offset.x, offset.y);
            continue;
        }

        // carve out the room
        for (sf::Uint32 y = 0; y < room.getSize().y; y++) {
            for (sf::Uint32 x = 0; x < room.getSize().x; x++) {
                setCell(sf::Vector2u(offset.x + x, offset.y + y), room.getCell(sf::Vector2u(x, y)));
            }
        }

        spdlog::info("Maze::generateRooms: generated room at ({}, {}) size ({}, {})",
                     offset.x,
                     offset.y,
                     room.getSize().x,
                     room.getSize().y);
    }
}

void Maze::generateCorridors()
{
}

void Maze::findConnectors()
{
}

void Maze::connectRooms()
{
}

void Maze::removeDeadEnds(sf::Uint32 max_iterations)
{
}

bool Maze::roomFits(const RoomShape &room, const sf::Vector2u &offset) const
{
    // test if the room bounds are within the maze bounds
    if (offset.x + room.getSize().x >= m_size.x || offset.y + room.getSize().y >= m_size.y) {
        return false;
    }

    // Check if the room overlaps with any other rooms
    for (sf::Uint32 y = 0; y < room.getSize().y; y++) {
        for (sf::Uint32 x = 0; x < room.getSize().x; x++) {
            if (room.getCell(sf::Vector2u(x, y)) == Cell::ROOM &&
                getCell(sf::Vector2u(offset.x + x, offset.y + y)) != Cell::WALL)
            {
                return false;
            }
        }
    }

    return true;
}

sf::Vector2u Maze::getSize() const
{
    return m_size;
}

std::vector<std::vector<Cell>> Maze::getCells() const
{
    return std::vector<std::vector<Cell>>();
}

Cell Maze::getCell(const sf::Vector2u &offset) const
{
    if (offset.x >= m_size.x || offset.y >= m_size.y) {
        return Cell::WALL;
    }

    return m_cells[m_size.x * offset.y + offset.x];
}

void Maze::setCell(const sf::Vector2u &offset, Cell cell)
{
    if (offset.x >= m_size.x || offset.y >= m_size.y) {
        spdlog::error("Maze::setCell: offset out of range");
        return;
    }

    if (cell == Cell::ROOM || cell == Cell::CORRIDOR || cell == Cell::WALL || cell == Cell::DOOR) {
        m_cells[m_size.x * offset.y + offset.x] = cell;
    } else {
        spdlog::error("Maze::setCell: invalid cell type");
    }
}

bool Maze::isCell(const sf::Vector2u &offset, Cell cell) const
{
    return getCell(offset) == cell;
}
