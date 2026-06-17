#ifndef COLLISIONPIXMAPITEM_H
#define COLLISIONPIXMAPITEM_H

#include "metatileselector.h"
#include "movementpermissionsselector.h"
#include "layoutpixmapitem.h"
#include "map.h"
#include "settings.h"

class CollisionPixmapItem : public LayoutPixmapItem {
    Q_OBJECT
public:
    CollisionPixmapItem(Layout *layout, MovementPermissionsSelector *selector, MetatileSelector *metatileSelector, Settings *settings, qreal *opacity)
        : LayoutPixmapItem(layout, metatileSelector, settings){
        this->selector = selector;
        this->opacity = opacity;
        layout->setCollisionItem(this);
    }
    MovementPermissionsSelector *selector;
    qreal *opacity;
    void updateMovementPermissionSelection(QGraphicsSceneMouseEvent *event);
    virtual void paint(QGraphicsSceneMouseEvent*) override;
    virtual void floodFill(QGraphicsSceneMouseEvent*) override;
    virtual void magicFill(QGraphicsSceneMouseEvent*) override;
    virtual void pick(QGraphicsSceneMouseEvent*) override;
    void draw(bool ignoreCache = false) override;

private:
    void updateSelection(QPoint pos);
};

#endif // COLLISIONPIXMAPITEM_H
