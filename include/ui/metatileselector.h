#ifndef METATILESELECTOR_H
#define METATILESELECTOR_H

#include <QPair>
#include "selectablepixmapitem.h"
#include "map.h"
#include "tileset.h"
#include "maplayout.h"

struct MetatileSelectionItem
{
    bool enabled;
    uint16_t metatileId;

    // Default values + compatibility with older compilers
    MetatileSelectionItem(bool enabled_ = false, uint16_t metatileId_ = 0)
        : enabled(enabled_), metatileId(metatileId_) {}
};

struct CollisionSelectionItem
{
    bool enabled;
    uint16_t collision;
    uint16_t elevation;

    // Default values + compatibility with older compilers
    CollisionSelectionItem(bool enabled_ = false, uint16_t collision_ = 0, uint16_t elevation_ = 0)
        : enabled(enabled_), collision(collision_), elevation(elevation_) {}
};

struct MetatileSelection
{
    QSize dimensions;
    bool hasCollision;
    QList<MetatileSelectionItem> metatileItems;
    QList<CollisionSelectionItem> collisionItems;
};

class MetatileSelector: public SelectablePixmapItem {
    Q_OBJECT
public:
    MetatileSelector(int numMetatilesWide, Layout *layout)
        : SelectablePixmapItem(Metatile::pixelSize()),
          numMetatilesWide(qMax(numMetatilesWide, 1))
    {
        this->externalSelection = false;
        this->prefabSelection = false;
        this->layout = layout;
        this->selection = MetatileSelection{};
        this->cellPos = QPoint(-1, -1);
        setAcceptHoverEvents(true);
    }

    // Which tileset(s) the selector renders. The picker presents one tab per tileset:
    // Primary shows the primary tileset's metatiles, Secondary shows a single secondary
    // tileset's metatiles. 'All' is the original combined primary+secondary view.
    enum class DisplaySection { All, Primary, Secondary };

    QSize getSelectionDimensions() const override;
    void draw() override;
    void refresh();

    // Sets which tileset section to display. For Secondary, an optional override lets the
    // picker render a secondary tileset other than the layout's active one (e.g. the
    // tileset of a different map location); locationIndex is the location slot that tileset
    // belongs to, stamped onto tiles painted from it (see paintLocation()).
    void setDisplaySection(DisplaySection section, Tileset *secondaryOverride = nullptr, int locationIndex = 0);
    DisplaySection displaySection() const { return this->m_section; }

    // The location attribute to apply to secondary-tileset tiles painted from the current
    // selection, or -1 when the selection's location should be left unchanged (i.e. when
    // painting primary-tileset tiles). This binds a secondary metatile to the location
    // whose tileset it was drawn from, which is what selects that tileset when rendering.
    int paintLocation() const { return this->m_section == DisplaySection::Secondary ? this->m_secondaryLocation : -1; }

    bool select(uint16_t metatile);
    void selectFromMap(uint16_t metatileId, uint16_t collision, uint16_t elevation);
    MetatileSelection getMetatileSelection() const { return this->selection; }
    void setPrefabSelection(MetatileSelection selection);
    void setExternalSelection(int, int, const QList<uint16_t>&, const QList<QPair<uint16_t, uint16_t>>&);
    QPoint getMetatileIdCoordsOnWidget(uint16_t metatileId) const;
    void setLayout(Layout *layout);
    bool isInternalSelection() const { return (!this->externalSelection && !this->prefabSelection); }

    Tileset *primaryTileset() const { return this->layout->tileset_primary; }
    Tileset *secondaryTileset() const { return this->layout->tileset_secondary; }
    // The secondary tileset used for the displayed sheet. This is the layout's active
    // secondary tileset unless a Secondary-section override has been set.
    Tileset *displaySecondaryTileset() const { return this->m_secondaryOverride ? this->m_secondaryOverride : secondaryTileset(); }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent*) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent*) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent*) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent*) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent*) override;
    void drawSelection() override;
private:
    const int numMetatilesWide;
    QPixmap basePixmap;
    bool externalSelection;
    bool prefabSelection;
    Layout *layout;
    DisplaySection m_section = DisplaySection::All;
    Tileset *m_secondaryOverride = nullptr;
    // The location slot the displayed secondary tileset belongs to (see paintLocation()).
    int m_secondaryLocation = 0;
    int externalSelectionWidth;
    int externalSelectionHeight;
    QList<uint16_t> externalSelectedMetatiles;
    MetatileSelection selection;
    QPoint cellPos;

    void updateBasePixmap();
    void updateSelectedMetatiles();
    void updateExternalSelectedMetatiles();
    uint16_t posToMetatileId(int x, int y, bool *ok = nullptr) const;
    uint16_t posToMetatileId(const QPoint &pos, bool *ok = nullptr) const;
    QPoint metatileIdToPos(uint16_t metatileId, bool *ok = nullptr) const;
    bool positionIsValid(const QPoint &pos) const;
    bool selectionIsValid();
    void hoverChanged();
    int numPrimaryMetatilesRounded() const;

signals:
    void hoveredMetatileSelectionChanged(uint16_t);
    void hoveredMetatileSelectionCleared();
    void selectedMetatilesChanged();
};

#endif // METATILESELECTOR_H
