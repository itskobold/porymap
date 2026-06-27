#pragma once
#ifndef METATILE_H
#define METATILE_H

#include "tile.h"
#include "config.h"
#include "bitpacker.h"
#include <QImage>
#include <QPoint>
#include <QString>

class Project;


class Metatile
{
public:
    Metatile() = default;
    Metatile(const Metatile &other) = default;
    Metatile &operator=(const Metatile &other) = default;
    Metatile(const int numTiles);

    enum LayerType {
        Normal,
        Covered,
        Split,
        Count
    };

    enum Attr {
        Behavior,
        TerrainType,
        EncounterType,
        LayerType,
        BgMaterial, // 1-bit flag: gate the bgMaterial render path for this metatile
        Unused, // Preserve bits not used by the other attributes
    };

    // Per-metatile fg/bg compositing flags. Stored separately from the attributes, in a parallel
    // metatile_compositing.bin (one u8 per metatile). Each cliff state gets one bit per layer:
    // a set bit composites that layer into the foreground plane, a clear bit into the background.
    // Mirrors METATILE_COMPOSITE_* in pokeemerald.
    enum CompositeState { CompositeFront, CompositeBehind };
    static constexpr int kCompositeFrontShift = 0;
    static constexpr int kCompositeBehindShift = 3;
    static constexpr int kCompositeLayerCount = 3;

public:
    QList<Tile> tiles;

    uint32_t getAttributes() const;
    uint32_t getAttribute(Metatile::Attr attr) const { return this->attributes.value(attr, 0); }
    void setAttributes(uint32_t data);
    void setAttributes(uint32_t data, BaseGameVersion version);
    void setAttribute(Metatile::Attr attr, uint32_t value);

    // For convenience
    uint32_t behavior()      const { return this->getAttribute(Attr::Behavior); }
    uint32_t terrainType()   const { return this->getAttribute(Attr::TerrainType); }
    uint32_t encounterType() const { return this->getAttribute(Attr::EncounterType); }
    uint32_t layerType()     const { return this->getAttribute(Attr::LayerType); }
    bool usesBgMaterial()    const { return this->getAttribute(Attr::BgMaterial) != 0; }
    void setBehavior(int value)      { this->setAttribute(Attr::Behavior, static_cast<uint32_t>(value)); }
    void setTerrainType(int value)   { this->setAttribute(Attr::TerrainType, static_cast<uint32_t>(value)); }
    void setEncounterType(int value) { this->setAttribute(Attr::EncounterType, static_cast<uint32_t>(value)); }
    void setLayerType(int value)     { this->setAttribute(Attr::LayerType, static_cast<uint32_t>(value)); }
    void setUsesBgMaterial(bool on)  { this->setAttribute(Attr::BgMaterial, on ? 1 : 0); }

    uint8_t compositing() const { return this->m_compositing; }
    void setCompositing(uint8_t value) { this->m_compositing = value; }
    bool compositeForeground(CompositeState state, int layer) const;
    void setCompositeForeground(CompositeState state, int layer, bool on);

    static int getIndexInTileset(int);
    static QPoint coordFromPixmapCoord(const QPointF &pixelCoord);
    static uint32_t getDefaultAttributesMask(BaseGameVersion version, Metatile::Attr attr);
    static uint32_t getMaxAttributesMask();
    static int getDefaultAttributesSize(BaseGameVersion version);
    static void setLayout(Project*);
    static QString getMetatileIdString(uint16_t metatileId);
    static QString getMetatileIdStrings(const QList<uint16_t> &metatileIds);
    static QString getLayerName(int layerNum);

    static constexpr int tileWidth() { return 2; }
    static constexpr int tileHeight() { return 2; }
    static constexpr int tilesPerLayer() { return Metatile::tileWidth() * Metatile::tileHeight(); }
    static constexpr int pixelWidth() { return Metatile::tileWidth() * Tile::pixelWidth(); }
    static constexpr int pixelHeight() { return Metatile::tileHeight() * Tile::pixelHeight(); }
    static constexpr QSize pixelSize() { return QSize(pixelWidth(), pixelHeight()); }

    inline bool operator==(const Metatile &other) {
        return this->tiles == other.tiles && this->attributes == other.attributes
            && this->m_compositing == other.m_compositing;
    }

    inline bool operator!=(const Metatile &other) {
        return !(operator==(other));
    }

private:
    QMap<Metatile::Attr, uint32_t> attributes;
    uint8_t m_compositing = 0;

    static int compositeBit(CompositeState state, int layer);
};

#endif // METATILE_H
