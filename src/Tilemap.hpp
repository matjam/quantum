#pragma once
#include <SFML/Graphics.hpp>

#include "Tilesheet.hpp"

// A tilemap is a layered grid of tiles. Each layer is a 2D array of tile IDs
// that correspond to tiles in a tilesheet. The tilemap also has a position and
// a size, and it can be drawn to a render target.
class Tilemap
{
    private:
        sf::Vector2u                         m_tileSize;
        sf::Vector2u                         m_mapSize;
        std::vector<std::vector<sf::Uint32>> m_layers;
        Tilesheet                            m_tilesheet;

    public:
        Tilemap() = delete;
        // The constructor takes a filename for the tilesheet, a tile size and a map
        // size.
        Tilemap(const std::string  &filename,
                const sf::Vector2u &tileSize,
                const sf::Vector2u &mapSize,
                const sf::Uint32    layers = 1);
        ~Tilemap();

        // setTile sets the tile ID at the given position in the given layer.
        void setTile(sf::Uint32 layer, const sf::Vector2u &position, sf::Uint32 id);

        // getTile returns the tile ID at the given position in the given layer.
        sf::Uint32 getTile(sf::Uint32 layer, const sf::Vector2u &position) const;

        // draw draws the tilemap to the given render target. viewPort is the
        // rectangle in the target that the tilemap should be drawn to, and
        // viewPosition is the center of the view in tile coordinates. Fractional tile
        // coordinates are allowed, and the tilemap will be drawn with sub-tile
        // precision.
        void draw(sf::RenderTarget    &target,
                  const sf::FloatRect &viewPort,
                  const sf::Vector2f  &viewPosition,
                  const sf::Vector2u   scale);

        // getTilesheet returns a pointer to the tilesheet used by the tilemap.
        Tilesheet *getTilesheet()
        {
            return &m_tilesheet;
        }
};
