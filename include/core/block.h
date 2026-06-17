#pragma once
#ifndef BLOCK_H
#define BLOCK_H

#include <QObject>

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
    void setElevation(uint16_t elevation);
    void setLocation(uint16_t location);
    void setBiome(uint16_t biome);
    uint16_t metatileId() const { return m_metatileId; }
    uint16_t collision() const { return m_collision; }
    uint16_t elevation() const { return m_elevation; }
    uint16_t location() const { return m_location; }
    uint16_t biome() const { return m_biome; }
    uint16_t rawValue() const;
    uint8_t attributesValue() const;
    void setAttributes(uint8_t data);
    static void setLayout();
    static uint16_t getMaxMetatileId();
    static uint16_t getMaxCollision();
    static uint16_t getMaxElevation();
    static uint16_t getMaxLocation();
    static uint16_t getMaxBiome();

    static const uint16_t maxValue;

private:
    uint16_t m_metatileId;
    uint16_t m_collision;
    uint16_t m_elevation;
    // Per-tile location field, packed into the map.bin word alongside the metatile id and
    // biome (default mask 0x0C00, i.e. 4 possible values). Editable via the Locations tab.
    uint16_t m_location;
    // Per-tile biome field, packed into the map.bin word (default mask 0xF000). Currently
    // unused by the engine.
    uint16_t m_biome;
};

#endif // BLOCK_H
