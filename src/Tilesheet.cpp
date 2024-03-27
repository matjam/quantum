#include "Tilesheet.hpp"
#include <SFML/System/Time.hpp>
#include <spdlog/spdlog.h>

Tilesheet::Tilesheet(const std::string &filename, const sf::Vector2u &tileSize)
{
    sf::Clock clock;

    m_texture.loadFromFile(filename);
    m_tileSize = tileSize;
    m_sprites.clear();
    m_tilesPerRow = m_texture.getSize().x / tileSize.x;

    // this generates a default set of sub-rectangles for each tile in the texture
    auto textureSize = m_texture.getSize();
    for (sf::Uint32 y = 0; y < textureSize.y; y += tileSize.y) {
        for (sf::Uint32 x = 0; x < textureSize.x; x += tileSize.x) {
            sf::Sprite *sprite = new sf::Sprite();
            sprite->setTexture(m_texture);
            sprite->setTextureRect(sf::IntRect(x, y, tileSize.x, tileSize.y));
            m_sprites.push_back(sprite);
        }
    }

    spdlog::debug("Loaded Tilesheet from {} with {} tiles in {}ms",
                  filename,
                  m_sprites.size(),
                  clock.getElapsedTime().asMilliseconds());
}

Tilesheet::~Tilesheet()
{
    for (auto sprite : m_sprites) {
        delete sprite;
    }
    m_sprites.clear();
}

sf::Sprite *Tilesheet::getTile(sf::Uint32 id, sf::Vector2u scale) const
{
    if (id >= m_sprites.size()) {
        spdlog::error("Tilesheet::getTile: id out of bounds");
        return nullptr;
    }
    auto sprite = m_sprites[id];
    sprite->setScale(sf::Vector2f(scale.x, scale.y));
    return sprite;
}

void Tilesheet::drawTile(sf::RenderTarget   &target,
                         sf::Uint32          id,
                         const sf::Vector2f &position,
                         const sf::Vector2u  scale)
{
    auto sprite = getTile(id, scale);
    sprite->setPosition(position);
    target.draw(*sprite);
}
