#pragma once
#ifndef MAPLAYOUT_H
#define MAPLAYOUT_H

#include "blockdata.h"
#include "tileset.h"
#include <QImage>
#include <QList>
#include <QPixmap>
#include <QString>
#include <QUndoStack>

class Map;
class MapConnection;
class LayoutPixmapItem;
class CollisionPixmapItem;
class LocationPixmapItem;
class BiomePixmapItem;
class BorderMetatilesPixmapItem;

class Layout : public QObject {
    Q_OBJECT
public:
    Layout() {};
    Layout(const Layout &other);

    static QString layoutConstantFromName(const QString &name);

    bool hasUnsavedDataChanges = false;

    QString id;
    QString name;
    QString newFolderPath;

    int width;
    int height;
    int border_width;
    int border_height;
    int pixelWidth() const { return this->width * Metatile::pixelWidth(); }
    int pixelHeight() const { return this->height * Metatile::pixelHeight(); }
    QSize pixelSize() const { return QSize(pixelWidth(), pixelHeight()); }

    QString border_path;
    QString blockdata_path;
    QString attributes_path;

    QString tileset_primary_label;
    QString tileset_secondary_label;

    Tileset *tileset_primary = nullptr;
    Tileset *tileset_secondary = nullptr;

    // Secondary tileset for each map location slot (index = a tile's location attribute),
    // used to render each tile with the secondary tileset of its location. Populated by
    // the editor from the current map's header. Empty in layout-only mode, where rendering
    // falls back to the single tileset_secondary above. See secondaryTilesetForLocation().
    QList<Tileset*> location_tilesets;

    QJsonObject customData;

    Blockdata blockdata;

    QImage image;
    QPixmap pixmap;
    QImage border_image;
    QPixmap border_pixmap;
    QImage collision_image;
    QPixmap collision_pixmap;
    QImage location_image;
    QPixmap location_pixmap;
    QImage biome_image;
    QPixmap biome_pixmap;

    Blockdata border;
    Blockdata cached_blockdata;
    Blockdata cached_collision;
    Blockdata cached_location;
    Blockdata cached_biome;
    Blockdata cached_border;
    struct {
        Blockdata blocks;
        QSize layoutDimensions;
        Blockdata border;
        QSize borderDimensions;
    } lastCommitBlocks; // to track map changes

    void setMetatileLayerOrder(const QList<int> &layerOrder) { m_metatileLayerOrder = layerOrder; }
    const QList<int> &metatileLayerOrder() const {
        return !m_metatileLayerOrder.isEmpty() ? m_metatileLayerOrder : Layout::globalMetatileLayerOrder();
    }
    static void setGlobalMetatileLayerOrder(const QList<int> &layerOrder) { s_globalMetatileLayerOrder = layerOrder; }
    static const QList<int> &globalMetatileLayerOrder() {
        static const QList<int> defaultLayerOrder = {0, 1, 2};
        return !s_globalMetatileLayerOrder.isEmpty() ? s_globalMetatileLayerOrder : defaultLayerOrder;
    }

    void setMetatileLayerOpacity(const QList<float> &layerOpacity) { m_metatileLayerOpacity = layerOpacity; }
    const QList<float> &metatileLayerOpacity() const {
        return !m_metatileLayerOpacity.isEmpty() ? m_metatileLayerOpacity : Layout::globalMetatileLayerOpacity();
    }
    static void setGlobalMetatileLayerOpacity(const QList<float> &layerOpacity) { s_globalMetatileLayerOpacity = layerOpacity; }
    static const QList<float> &globalMetatileLayerOpacity() {
        static const QList<float> defaultLayerOpacity = {1.0, 1.0, 1.0};
        return !s_globalMetatileLayerOpacity.isEmpty() ? s_globalMetatileLayerOpacity : defaultLayerOpacity;
    }

    LayoutPixmapItem *layoutItem = nullptr;
    CollisionPixmapItem *collisionItem = nullptr;
    LocationPixmapItem *locationItem = nullptr;
    BiomePixmapItem *biomeItem = nullptr;
    BorderMetatilesPixmapItem *borderItem = nullptr;

    // Tiles whose location value is >= this limit are drawn as "out of bounds" and
    // block saving. Set from the current map's Num. Locations; defaults to allowing
    // everything (relevant in layout-only mode, where there's no map header).
    int locationLimit = 0xFFFF;

    QUndoStack editHistory;

    // to simplify new layout settings transfer between functions
    struct Settings {
        QString id;
        QString name;
        // The name of a new layout's folder in `data/layouts/` is not always the same as the layout's name
        // (e.g. the majority of the default layouts use the name of their associated map).
        QString folderName;
        int width;
        int height;
        int borderWidth;
        int borderHeight;
        QString primaryTilesetLabel;
        QString secondaryTilesetLabel;
    };
    Settings settings() const;

    Layout *copy() const;
    void copyFrom(const Layout *other);

    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getBorderWidth() const { return border_width; }
    int getBorderHeight() const { return border_height; }
    QMargins getBorderMargins() const;
    QRect getVisibleRect() const;

    bool isWithinBounds(int x, int y) const;
    bool isWithinBounds(const QPoint &pos) const;
    bool isWithinBounds(const QRect &rect) const;
    bool isWithinBorderBounds(int x, int y) const;

    bool getBlock(int x, int y, Block *out) const;
    bool getBlock(const QPoint& pos, Block *out) const { return getBlock(pos.x(), pos.y(), out); }
    void setBlock(int x, int y, Block block, bool enableScriptCallback = false);
    void setBlock(const QPoint& pos, Block block, bool enableScriptCallback = false) { setBlock(pos.x(), pos.y(), block, enableScriptCallback); }
    void setBlockdata(Blockdata blockdata, bool enableScriptCallback = false);

    uint16_t getMetatileId(int x, int y) const;
    bool setMetatileId(int x, int y, uint16_t metatileId, bool enableScriptCallback = false);

    void adjustDimensions(const QMargins &margins, bool setNewBlockdata = true);
    void setDimensions(int newWidth, int newHeight, bool setNewBlockdata = true);
    void setBorderDimensions(int newWidth, int newHeight, bool setNewBlockdata = true, bool enableScriptCallback = false);

    void cacheBlockdata();
    void cacheCollision();
    void cacheLocation();
    void cacheBiome();
    void clearBorderCache();
    void cacheBorder();

    bool hasUnsavedChanges() const;

    bool save(const QString &root);
    bool saveBorder(const QString &root);
    bool saveBlockdata(const QString &root);

    bool loadBorder(const QString &root);
    bool loadBlockdata(const QString &root);

    bool layoutBlockChanged(int i, const Blockdata &curData, const Blockdata &cache);

    uint16_t getBorderMetatileId(int x, int y);
    void setBorderMetatileId(int x, int y, uint16_t metatileId, bool enableScriptCallback = false);
    void setBorderBlockData(Blockdata blockdata, bool enableScriptCallback = false);

    void floodFillCollisionElevation(int x, int y, uint16_t collision, uint16_t elevation);
    void _floodFillCollisionElevation(int x, int y, uint16_t collision, uint16_t elevation);
    void magicFillCollisionElevation(int x, int y, uint16_t collision, uint16_t elevation);

    void floodFillLocation(int x, int y, uint16_t location);
    void _floodFillLocation(int x, int y, uint16_t location);
    void magicFillLocation(int x, int y, uint16_t location);

    void floodFillBiome(int x, int y, uint16_t biome);
    void _floodFillBiome(int x, int y, uint16_t biome);
    void magicFillBiome(int x, int y, uint16_t biome);

    QPixmap render(bool ignoreCache = false, Layout *fromLayout = nullptr, const QRect &bounds = QRect(0, 0, -1, -1));
    QPixmap renderCollision(bool ignoreCache);
    QPixmap renderLocation(bool ignoreCache);
    QPixmap renderBiome(bool ignoreCache);
    QPixmap renderBorder(bool ignoreCache = false);

    QPixmap getLayoutItemPixmap();

    void setLayoutItem(LayoutPixmapItem *item) { layoutItem = item; }
    void setCollisionItem(CollisionPixmapItem *item) { collisionItem = item; }
    void setLocationItem(LocationPixmapItem *item) { locationItem = item; }
    void setBiomeItem(BiomePixmapItem *item) { biomeItem = item; }
    void setBorderItem(BorderMetatilesPixmapItem *item) { borderItem = item; }

    bool metatileIsValid(uint16_t metatileId) { return Tileset::metatileIsValid(metatileId, this->tileset_primary, this->tileset_secondary); }

    // The secondary tileset to render a tile with, given its location attribute. Falls back
    // to the layout's active secondary tileset for unknown locations (and in layout-only mode).
    Tileset *secondaryTilesetForLocation(int location) const;

    // True if a metatile belongs to a secondary tileset. Such tiles get their location from
    // the tileset they're painted from, so their location is locked in the Locations editor.
    static bool metatileIsSecondary(uint16_t metatileId);

    // A secondary-tileset tile this close to a tile of a different location renders with
    // the wrong tileset near a region border in-game. The in-game view is wider than it is
    // tall, so the conflict range is larger horizontally than vertically.
    static constexpr int LocationConflictRangeH = 8;
    static constexpr int LocationConflictRangeV = 6;
    // True if the tile at (x, y) is such a conflict. Used by the editor's error overlay.
    // The scan also reaches across the given map connections (diving connections are
    // ignored), comparing the secondary tileset the neighbouring map's location resolves to.
    bool isLocationConflictTile(int x, int y, const QList<MapConnection*> &connections = {}) const;
    // True if the layout contains any conflict tile. Used to block saving until resolved.
    bool hasLocationConflicts(const QList<MapConnection*> &connections = {}) const;

private:
    void setNewDimensionsBlockdata(int newWidth, int newHeight);
    void setNewBorderDimensionsBlockdata(int newWidth, int newHeight);
    bool writeBlockdata(const QString &path, const Blockdata &blockdata) const;
    bool writeAttributes(const QString &path, const Blockdata &blockdata) const;
    static Blockdata readBlockdata(const QString &path, QString *error);
    static void readAttributes(const QString &path, Blockdata *blockdata, QString *error);

    static int getBorderDrawDistance(int dimension, qreal minimum);

    void drawElevationBorders(QPainter &painter, int index, int px, int py);

    QList<int> m_metatileLayerOrder;
    QList<float> m_metatileLayerOpacity;
    static QList<int> s_globalMetatileLayerOrder;
    static QList<float> s_globalMetatileLayerOpacity;

signals:
    void dimensionsChanged(const QSize &size);
    void needsRedrawing();
};

#endif // MAPLAYOUT_H
