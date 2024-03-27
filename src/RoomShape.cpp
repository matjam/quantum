#include "RoomShape.hpp"
#include <filesystem>
#include <fstream>
#include <spdlog/spdlog.h>

RoomShape::RoomShape()
{
}

RoomShape::RoomShape(const sf::Vector2u &size)
{
    spdlog::info("RoomShape::PrefabRoomShape(const sf::Vector2u &size)");

    // A room must be odd in size, so if the size is even then we add 1 to the
    // width and height to make it odd. We also reserve the cells vector to
    // avoid reallocations when adding cells to the room.

    m_size.x = size.x % 2 == 0 ? size.x + 1 : size.x;
    m_size.y = size.y % 2 == 0 ? size.y + 1 : size.y;
    m_cells.reserve(size.x * size.y);

    // by default, if you provide a size then a room will be created of that size.
    // The room will be a solid block of room cells, with no walls around the edge.

    for (unsigned int y = 0; y < size.y; y++) {
        for (unsigned int x = 0; x < size.x; x++) {
            m_cells.push_back(Cell::ROOM);
        }
    }

    spdlog::info("PrefabRoomShape::PrefabRoomShape: created room of size ({}, {})", m_size.x, m_size.y);
}

void RoomShape::loadPrefab(const std::string &prefab)
{
    // a prefab is a text file that contains the cells that make up the room;
    // the file is a plain ascii file with each line being a row of the room
    // and each character in the line being a cell in the row; the character
    // is a period if the cell is a room cell and an asterisk if the cell is
    // not a room cell. The prefab file is read twice: first to get the
    // dimensions of the room and to validate the file, and then to get the
    // cells that make up the room.

    spdlog::info("PrefabRoom::loadPrefab: loading prefab from {}", prefab);

    if (!std::filesystem::exists(prefab)) {
        spdlog::error("PrefabRoom::loadPrefab: prefab file does not exist");
        return;
    }

    // open the file for reading
    std::ifstream file(prefab);
    if (!file.is_open()) {
        spdlog::error("PrefabRoom::loadPrefab: failed to open prefab file");
        return;
    }

    // read the file to get the dimensions of the room. We also check that the
    // file is not empty, and that the lines are consistent in length. The room
    // must be an odd number of cells wide and high, but does not need to be
    // square or enclosed.

    std::string line;
    m_size = sf::Vector2u(0, 0);

    while (std::getline(file, line)) {
        if (line.empty()) {
            spdlog::error("PrefabRoom::loadPrefab: empty line in prefab file");
            continue;
        }

        if (m_size.x == 0) {
            m_size.x = line.size();
        } else if (m_size.x != line.size()) {
            spdlog::error("PrefabRoom::loadPrefab: inconsistent line length in prefab file");
            return;
        }

        // all lines should contain only spaces and asterisks
        for (auto c : line) {
            if (c != '*' && c != '.') {
                spdlog::error("PrefabRoom::loadPrefab: invalid character in prefab file");
                return;
            }
        }

        m_size.y++;
    }

    if (m_size.x == 0 || m_size.y == 0) {
        spdlog::error("PrefabRoom::loadPrefab: empty prefab file");
        return;
    }

    if (m_size.x % 2 == 0 || m_size.y % 2 == 0) {
        spdlog::error("PrefabRoom::loadPrefab: room dimensions are not odd");
        return;
    }

    // read the file again to get the cells that make up the room
    file.clear();
    file.seekg(0, std::ios::beg);

    m_cells.clear();
    m_cells.reserve(m_size.x * m_size.y);

    for (unsigned int y = 0; y < m_size.y; y++) {
        std::getline(file, line);

        for (unsigned int x = 0; x < m_size.x; x++) {
            if (line[x] == ' ') {
                m_cells[y * m_size.x + x] = Cell::ROOM;
            } else {
                m_cells[y * m_size.x + x] = Cell::WALL;
            }
        }
    }
}

void RoomShape::setCell(const sf::Vector2u &offset, Cell cell)
{
    m_cells[offset.y * m_size.x + offset.x] = cell;
}

Cell RoomShape::getCell(const sf::Vector2u &offset) const
{
    return m_cells[offset.y * m_size.x + offset.x];
}

bool RoomShape::isCell(const sf::Vector2u &offset, Cell cell) const
{
    return m_cells[offset.y * m_size.x + offset.x] == cell;
}

std::vector<Cell> RoomShape::getCells() const
{
    return m_cells;
}

sf::Vector2u RoomShape::getSize() const
{
    return m_size;
}
