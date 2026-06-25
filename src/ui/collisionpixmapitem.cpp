#include "collisionpixmapitem.h"
#include "editcommands.h"
#include "metatile.h"

void CollisionPixmapItem::draw(bool ignoreCache) {
    if (this->layout) {
        this->layout->setCollisionItem(this);
        setPixmap(this->layout->renderCollision(ignoreCache));
        setOpacity(*this->opacity);
    }
}

void CollisionPixmapItem::paint(QGraphicsSceneMouseEvent *event) {
    if (event->type() == QEvent::GraphicsSceneMouseRelease) {
        actionId_++;
    } else if (this->layout) {
        Blockdata oldCollision = this->layout->blockdata;

        QPoint pos = Metatile::coordFromPixmapCoord(event->pos());

        // Set straight paths on/off and snap to the dominant axis when on
        if (event->modifiers() & Qt::ControlModifier) {
            this->lockNondominantAxis(event);
            pos = this->adjustCoords(pos);
        } else {
            this->prevStraightPathState = false;
            this->lockedAxis = LayoutPixmapItem::Axis::None;
        }

        Block block;
        if (this->layout->getBlock(pos.x(), pos.y(), &block)) {
            bool collision = this->selector->paintCollision();
            // Painting normal collision also sets cliff collision; cliff can be painted alone.
            bool cliffCollision = collision || this->selector->paintCliffCollision();
            block.setCollision(collision ? 1 : 0);
            block.setCliffCollision(cliffCollision ? 1 : 0);
            block.setElevation(this->selector->selectedValue());
            this->layout->setBlock(pos.x(), pos.y(), block, true);
        }

        if (this->layout->blockdata != oldCollision) {
            this->layout->editHistory.push(new PaintCollision(this->layout, oldCollision, this->layout->blockdata, actionId_));
        }
    }
}

void CollisionPixmapItem::floodFill(QGraphicsSceneMouseEvent *event) {
    if (event->type() == QEvent::GraphicsSceneMouseRelease) {
        this->actionId_++;
    } else if (this->layout) {
        Blockdata oldCollision = this->layout->blockdata;

        QPoint pos = Metatile::coordFromPixmapCoord(event->pos());
        bool collision = this->selector->paintCollision();
        bool cliffCollision = collision || this->selector->paintCliffCollision();
        this->layout->floodFillCollisionElevation(pos.x(), pos.y(), collision ? 1 : 0, cliffCollision ? 1 : 0, this->selector->selectedValue());

        if (this->layout->blockdata != oldCollision) {
            this->layout->editHistory.push(new BucketFillCollision(this->layout, oldCollision, this->layout->blockdata));
        }
    }
}

void CollisionPixmapItem::magicFill(QGraphicsSceneMouseEvent *event) {
    if (event->type() == QEvent::GraphicsSceneMouseRelease) {
        this->actionId_++;
    } else if (this->layout) {
        Blockdata oldCollision = this->layout->blockdata;
        QPoint pos = Metatile::coordFromPixmapCoord(event->pos());
        bool collision = this->selector->paintCollision();
        bool cliffCollision = collision || this->selector->paintCliffCollision();
        this->layout->magicFillCollisionElevation(pos.x(), pos.y(), collision ? 1 : 0, cliffCollision ? 1 : 0, this->selector->selectedValue());

        if (this->layout->blockdata != oldCollision) {
            this->layout->editHistory.push(new MagicFillCollision(this->layout, oldCollision, this->layout->blockdata));
        }
    }
}

void CollisionPixmapItem::pick(QGraphicsSceneMouseEvent *event) {
    QPoint pos = Metatile::coordFromPixmapCoord(event->pos());
    this->updateSelection(pos);
}

void CollisionPixmapItem::updateMovementPermissionSelection(QGraphicsSceneMouseEvent *event) {
    QPoint pos = Metatile::coordFromPixmapCoord(event->pos());

    // Snap point to within map bounds.
    if (pos.x() < 0) pos.setX(0);
    if (pos.x() >= this->layout->getWidth()) pos.setX(this->layout->getWidth() - 1);
    if (pos.y() < 0) pos.setY(0);
    if (pos.y() >= this->layout->getHeight()) pos.setY(this->layout->getHeight() - 1);
    this->updateSelection(pos);
}

void CollisionPixmapItem::updateSelection(QPoint pos) {
    Block block;
    if (this->layout->getBlock(pos.x(), pos.y(), &block)) {
        this->selector->setSelectedValue(block.elevation());
    }
}
