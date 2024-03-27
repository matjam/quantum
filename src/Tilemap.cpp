#include "Tilemap.hpp"
#include <spdlog/spdlog.h>

Tilemap::Tilemap(const std::string  &filename,
                 const sf::Vector2u &tileSize,
                 const sf::Vector2u &mapSize,
                 const sf::Uint32    layers)
    : m_tilesheet(filename, tileSize)
{
    m_tileSize = tileSize;
    m_mapSize  = mapSize;
    m_layers.resize(layers);

    for (auto &layer : m_layers) {
        layer.resize(mapSize.x * mapSize.y);
    }
}

Tilemap::~Tilemap()
{
    m_layers.clear();
}

void Tilemap::setTile(sf::Uint32 layer, const sf::Vector2u &position, sf::Uint32 id)
{
    if (position.x >= m_mapSize.x || position.y >= m_mapSize.y) {
        return;
    }

    if (layer >= m_layers.size()) {
        spdlog::error("Tilemap::setTile: layer out of bounds");
        return;
    }

    m_layers[layer][position.y * m_mapSize.x + position.x] = id;
}

sf::Uint32 Tilemap::getTile(sf::Uint32 layer, const sf::Vector2u &position) const
{
    if (position.x >= m_mapSize.x || position.y >= m_mapSize.y) {
        return 0;
    }

    if (layer >= m_layers.size()) {
        return 0;
    }

    return m_layers[layer][position.y * m_mapSize.x + position.x];
}

void Tilemap::draw(sf::RenderTarget    &target,
                   const sf::FloatRect &viewPort,
                   const sf::Vector2f  &viewPosition,
                   const sf::Vector2u   scale)
{
    // The viewport represents the area inside the target that we will draw to. We will
    // make sure we do not draw outside of this area.
    //
    // The viewPosition represents where the center of the viewport is in tile coordinates.
    // We will draw the tiles around this position.
    //
    // The scale represents the scale of the tiles. We will use this to scale the tiles
    // when we draw them to the target.
    //
    // We need to make sure that when we draw the tiles, we only draw the tiles that are
    // visible in the viewPort, and we make sure that we only draw tiles inside the viewPort.
    // If a tile overlaps the edge of the viewPort, we will only draw the part of the tile
    // that is inside the viewPort and is visible.

    // Calculate the range of tiles that are visible in the view including the scale, and the
    // viewPosition of the camera in tile coordinates.
    sf::Vector2f viewSize(viewPort.width / m_tileSize.x, viewPort.height / m_tileSize.y);
    sf::Vector2f viewStart(viewPosition - viewSize / 2.f);
    sf::Vector2f viewEnd(viewPosition + viewSize / 2.f);

    // Clamp the viewStart and viewEnd to the bounds of the map so that we do not draw tiles
    // that are outside of the map.
    viewStart.x = std::max(0.f, std::min(static_cast<float>(m_mapSize.x), viewStart.x));
    viewStart.y = std::max(0.f, std::min(static_cast<float>(m_mapSize.y), viewStart.y));
    viewEnd.x   = std::max(0.f, std::min(static_cast<float>(m_mapSize.x), viewEnd.x));
    viewEnd.y   = std::max(0.f, std::min(static_cast<float>(m_mapSize.y), viewEnd.y));

    // Calculate the range of tiles that are visible in the view including the scale and
    // the viewPosition of the camera in tile coordinates.
    sf::Vector2u start(static_cast<sf::Uint32>(viewStart.x), static_cast<sf::Uint32>(viewStart.y));
    sf::Vector2u end(static_cast<sf::Uint32>(viewEnd.x), static_cast<sf::Uint32>(viewEnd.y));

    // Draw the visible tiles to the target, making sure that we draw the tiles inside the viewPort
    // centering the viewport on the viewPosition
    for (sf::Uint32 layer = 0; layer < m_layers.size(); layer++) {
        for (sf::Uint32 y = start.y; y < end.y + 1; y++) {
            for (sf::Uint32 x = start.x; x < end.x; x++) {
                auto id = getTile(layer, sf::Vector2u(x, y));
                if (id != 0) {
                    auto tilePosition =
                        sf::Vector2f((x - viewPosition.x) * m_tileSize.x * scale.x + viewPort.width / 2,
                                     (y - viewPosition.y) * m_tileSize.y * scale.y + viewPort.height / 2);

                    // check if the tile is either inside the viewPort or overlaps the edge of the viewPort
                    if (tilePosition.x + m_tileSize.x * scale.x > viewPort.left &&
                        tilePosition.x < viewPort.left + viewPort.width &&
                        tilePosition.y + m_tileSize.y * scale.y > viewPort.top &&
                        tilePosition.y < viewPort.top + viewPort.height)
                    {
                        m_tilesheet.drawTile(target, id, tilePosition, scale);
                    }
                }
            }
        }
    }

    // draw a box around the viewPort
    sf::RectangleShape box(sf::Vector2f(viewPort.width, viewPort.height));
    box.setPosition(viewPort.left, viewPort.top);
    box.setFillColor(sf::Color::Transparent);
    box.setOutlineColor(sf::Color::Red);
    box.setOutlineThickness(1);
    target.draw(box);
}
