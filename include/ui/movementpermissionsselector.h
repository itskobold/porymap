#ifndef MOVEMENTPERMISSIONSSELECTOR_H
#define MOVEMENTPERMISSIONSSELECTOR_H

#include "selectablepixmapitem.h"
#include "block.h"

// Selector used on the Collision tab (and reused, generically, on the Locations tab).
//
// On the Collision tab the sheet is a horizontal strip of cells representing a single
// per-tile elevation value. The first Elevation::NumSpecial cells are the special
// movement types (elevation change, impassable, water, multi-level); the final cell is
// the "all levels" tile, whose value is FirstLevel plus the separately-set elevation
// level (driven by the slider/spin box beneath the picker).
class MovementPermissionsSelector: public SelectablePixmapItem {
    Q_OBJECT
public:
    MovementPermissionsSelector(QPixmap basePixmap)
    : SelectablePixmapItem(MovementPermissionsSelector::CellWidth, MovementPermissionsSelector::CellHeight, 1, 1) {
        this->basePixmap = basePixmap;
        setAcceptHoverEvents(true);
    }
    void draw();

    // Generic single-cell selection (column x, row y). Used by the Locations tab.
    void select(uint16_t x, uint16_t y);
    void setBasePixmap(QPixmap pixmap);

    // Collision tab: the selector as a single elevation value.
    uint16_t selectedValue() const;            // the value that will be painted
    int selectedLevel() const { return m_level; } // user-facing level (value - FirstLevel)
    bool isLevelCellSelected() const;          // is the "all levels" cell selected?
    void setSelectedValue(uint16_t value);     // select the cell/level matching a value
    void setElevationLevel(int level);         // set the "all levels" level, re-evaluate value
    static int maxElevationLevel();            // Block::getMaxElevation() - FirstLevel

    // Collision-vs-elevation paint mode. When painting collision, the tile is marked
    // impassable; otherwise the selected elevation level is painted. The elevation level
    // (selectedValue) is always applied to the tile in both modes.
    bool paintCollision() const { return m_paintCollision; }
    void setPaintCollision(bool on) { m_paintCollision = on; }

    // Cliff collision paint mode. Painted independently of normal collision; painting normal
    // collision also sets cliff collision (handled by the paint code, not here).
    bool paintCliffCollision() const { return m_paintCliffCollision; }
    void setPaintCliffCollision(bool on) { m_paintCliffCollision = on; }

    static const int CellWidth;
    static const int CellHeight;

protected:
    void hoverMoveEvent(QGraphicsSceneHoverEvent*);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent*);

private:
    uint16_t valueAt(int cell) const;
    QPixmap basePixmap;
    int m_level = 0; // user-facing elevation level for the "all levels" cell
    bool m_paintCollision = false; // paint mode: collision (true) vs elevation level (false)
    bool m_paintCliffCollision = false; // paint cliff collision (bit 6)

signals:
    void selectedValueChanged(uint16_t value);
    void hoveredMovementPermissionChanged(uint16_t value);
    void hoveredMovementPermissionCleared();
};

#endif // MOVEMENTPERMISSIONSSELECTOR_H
