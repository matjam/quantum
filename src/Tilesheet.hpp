#pragma once
#include <SFML/Graphics.hpp>

// The Tilesheet class is a simple wrapper around a texture that allows us to
// draw individual tiles from the texture. It also provides a method to draw a
// specific tile to a render target.
//
// Every tile is assumed to be the same size, and the tiles are assumed to be
// arranged in a grid within the texture. The constructor takes a filename and
// a tile size, and the filename is used to load the texture, and the tile size
// is used to calculate the sub-rectangles for each tile in the texture.
class Tilesheet
{
    private:
        sf::Texture               m_texture;
        sf::Vector2u              m_tileSize;
        std::vector<sf::Sprite *> m_sprites;
        sf::Uint32                m_tilesPerRow;

    public:
        // The constructor takes a filename and a tile size. The filename is used to
        // load the texture, and the tile size is used to calculate the sub-rectangles
        // for each tile in the texture.
        Tilesheet(const std::string &filename, const sf::Vector2u &tileSize);
        ~Tilesheet();

        // The drawTile method takes a render target, an ID, and a position, and draws
        // the tile with the given ID to the render target at the given position. The
        // position is in pixel coordinates, not tile coordinates.
        void drawTile(sf::RenderTarget &target, sf::Uint32 id, const sf::Vector2f &position, const sf::Vector2u scale);

        // The getTile method returns a pointer to a sprite that represents the tile
        // with the given ID. The scale parameter is used to scale the sprite. The
        // returned sprite is owned by the tilesheet, and should not be deleted by the
        // caller.
        sf::Sprite *getTile(sf::Uint32 id, sf::Vector2u scale) const;

        // getTileCount returns the number of tiles in the tilesheet
        sf::Uint32 getTileCount() const
        {
            return m_sprites.size();
        }

        // getTileSize returns the size of the tiles in the tilesheet
        sf::Vector2u getTileSize() const
        {
            return m_tileSize;
        }

        // getTilesPerRow returns the number of tiles per row in the tilesheet
        sf::Uint32 getTilesPerRow() const
        {
            return m_tilesPerRow;
        }

        // getNumberOfTiles returns the number of tiles in the tilesheet
        sf::Uint32 getNumberOfTiles() const
        {
            return m_sprites.size();
        }

        // getTexture returns a reference to the texture used by the tilesheet
        const sf::Texture &getTexture() const
        {
            return m_texture;
        }
};
