#pragma once

#include <map>
#include <random>
#include <vector>

#include <SFML/Graphics.hpp>

#include "Cell.hpp"
#include "RoomShape.hpp"

// Room is a thin representation of a room in the maze. It simply holds the region id
// of the room. The region id is used to identify the room in the maze.
class Room
{
    private:
        sf::Uint32 m_region; // region id

    public:
        Room(sf::Uint32 region)
        {
            m_region = region;
        };

        ~Room() = default;

        // set the region id of the room
        void setRegion(sf::Uint32 region)
        {
            m_region = region;
        };

        // get the region id of the room
        sf::Uint32 getRegion() const
        {
            return m_region;
        };
};

class Maze
{
    private:
        sf::Vector2u              m_size;       // size of the maze
        std::vector<Cell>         m_cells;      // grid of cells for the maze
        std::vector<sf::Uint32>   m_regions;    // grid of regions for each cell in the maze
        std::vector<Room *>       m_rooms;      // list of rooms
        std::vector<sf::Vector2u> m_connectors; // list of connectors
        sf::Uint32                m_seed;       // seed used to generate the maze
        sf::Uint32                m_nextRegion; // next region id
        std::mt19937              m_gen;        // random number generator
        std::vector<RoomShape>    m_prefabs;    // list of prefab shapes

        // generate a room in the maze, up to the maximum number of attempts
        void generateRooms(sf::Uint32 max_attempts);

        // generates corridors by picking a random wall cell and carving corridors
        void generateCorridors();

        // find the connectors in the maze and add them to the list of connectors
        void findConnectors();

        // connect the rooms in the maze by carving corridors through connectors
        void connectRooms();

        // remove dead ends from the maze
        void removeDeadEnds(sf::Uint32 max_iterations);

        // test if a room will fit in the maze at the given location
        bool roomFits(const RoomShape &room, const sf::Vector2u &offset) const;

    public:
        Maze(const sf::Vector2u &size);
        ~Maze() = default;

        // generate a maze using the given seed
        void generate(sf::Uint32 seed);

        // get the size of the maze
        sf::Vector2u getSize() const;

        // get the cells that make up the maze
        std::vector<std::vector<Cell>> getCells() const;

        // get a specific cell in the maze
        Cell getCell(const sf::Vector2u &offset) const;

        // set a specific cell in the maze
        void setCell(const sf::Vector2u &offset, Cell cell);

        // check the type of a specific cell in the maze
        bool isCell(const sf::Vector2u &offset, Cell cell) const;
};