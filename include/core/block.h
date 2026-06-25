#pragma once
#ifndef BLOCK_H
#define BLOCK_H

#include <QObject>

// The per-tile attributes are a 16-bit value (attributes.bin, 2 bytes/tile): an 8-bit
// elevation level (bits 0-7, 0-255), cliff collision (bit 8), collision (bit 9) and a 4-bit
// bgMaterial field (bits 10-13). Cliff collision only blocks objects climbing behind a
// cliff; it is painted independently, and painting normal collision also sets it. There are
// no special elevation values: every value is an ordinary level, shown to the user directly.
// bgMaterial selects a primary metatile (1-15) whose middle layer replaces this tile's when
// non-zero (see the game's DrawMetatile); 0 means "use this tile's own layers".
// NumSpecial/FirstLevel are retained as 0 so level == value arithmetic and the (now-inert)
// special-value guards elsewhere keep compiling.
namespace Elevation {
    constexpr int NumSpecial = 0;
    constexpr int FirstLevel = 0;
}

class Block
{
public:
    Block();
    Block(uint16_t);
    Block(uint16_t metatileId, uint16_t collision, uint16_t elevation);
    Block(const Block &);
    Block &operator=(const Block &);
    bool operator ==(Block) const;
    bool operator !=(Block) const;
    void setMetatileId(uint16_t metatileId);
    void setCollision(uint16_t collision);
    void setCliffCollision(uint16_t cliffCollision);
    void setElevation(uint16_t elevation);
    void setLocation(uint16_t location);
    void setBiome(uint16_t biome);
    void setBgMaterial(uint16_t bgMaterial);
    uint16_t metatileId() const { return m_metatileId; }
    uint16_t collision() const { return m_collision; }
    uint16_t cliffCollision() const { return m_cliffCollision; }
    uint16_t elevation() const { return m_elevation; }
    uint16_t location() const { return m_location; }
    uint16_t biome() const { return m_biome; }
    uint16_t bgMaterial() const { return m_bgMaterial; }
    uint16_t rawValue() const;
    uint16_t attributesValue() const;
    void setAttributes(uint16_t data);
    static void setLayout();
    static uint16_t getMaxMetatileId();
    static uint16_t getMaxCollision();
    static uint16_t getMaxCliffCollision();
    static uint16_t getMaxElevation();
    static uint16_t getMaxLocation();
    static uint16_t getMaxBiome();
    static uint16_t getMaxBgMaterial();

    static const uint16_t maxValue;

private:
    uint16_t m_metatileId;
    uint16_t m_collision;
    // Cliff-collision bit (attribute bit 6): walls off the behind-cliff region only.
    uint16_t m_cliffCollision;
    uint16_t m_elevation;
    // Per-tile location field, packed into the map.bin word alongside the metatile id and
    // biome (default mask 0x0C00, i.e. 4 possible values). Editable via the Locations tab.
    uint16_t m_location;
    // Per-tile biome field, packed into the map.bin word (default mask 0xF000). Currently
    // unused by the engine.
    uint16_t m_biome;
    // Per-tile bgMaterial field, packed into the attribute value (default mask 0x3C00).
    uint16_t m_bgMaterial;
};

#endif // BLOCK_H
