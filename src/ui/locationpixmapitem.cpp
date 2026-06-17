#include "locationpixmapitem.h"
#include "editcommands.h"
#include "metatile.h"

void LocationPixmapItem::draw(bool ignoreCache) {
    if (this->layout) {
        this->layout->setLocationItem(this);
        setPixmap(this->layout->renderLocation(ignoreCache));
        setOpacity(*this->opacity);
    }
}

void LocationPixmapItem::paint(QGraphicsSceneMouseEvent *event) {
    if (event->type() == QEvent::GraphicsSceneMouseRelease) {
        actionId_++;
    } else if (this->layout) {
        Blockdata oldLocation = this->layout->blockdata;

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
        // A secondary-tileset tile's location is fixed by the tileset it was painted from,
        // so the Locations editor can't overwrite it.
        if (this->layout->getBlock(pos.x(), pos.y(), &block) && !Layout::metatileIsSecondary(block.metatileId())) {
            block.setLocation(this->selectedLocation->value());
            this->layout->setBlock(pos.x(), pos.y(), block, true);
        }

        if (this->layout->blockdata != oldLocation) {
            this->layout->editHistory.push(new PaintLocation(this->layout, oldLocation, this->layout->blockdata, actionId_));
        }
    }
}

void LocationPixmapItem::floodFill(QGraphicsSceneMouseEvent *event) {
    if (event->type() == QEvent::GraphicsSceneMouseRelease) {
        this->actionId_++;
    } else if (this->layout) {
        Blockdata oldLocation = this->layout->blockdata;

        QPoint pos = Metatile::coordFromPixmapCoord(event->pos());
        uint16_t location = this->selectedLocation->value();
        this->layout->floodFillLocation(pos.x(), pos.y(), location);

        if (this->layout->blockdata != oldLocation) {
            this->layout->editHistory.push(new BucketFillLocation(this->layout, oldLocation, this->layout->blockdata));
        }
    }
}

void LocationPixmapItem::magicFill(QGraphicsSceneMouseEvent *event) {
    if (event->type() == QEvent::GraphicsSceneMouseRelease) {
        this->actionId_++;
    } else if (this->layout) {
        Blockdata oldLocation = this->layout->blockdata;
        QPoint pos = Metatile::coordFromPixmapCoord(event->pos());
        uint16_t location = this->selectedLocation->value();
        this->layout->magicFillLocation(pos.x(), pos.y(), location);

        if (this->layout->blockdata != oldLocation) {
            this->layout->editHistory.push(new MagicFillLocation(this->layout, oldLocation, this->layout->blockdata));
        }
    }
}

void LocationPixmapItem::pick(QGraphicsSceneMouseEvent *event) {
    QPoint pos = Metatile::coordFromPixmapCoord(event->pos());
    this->updateSelection(pos);
}

void LocationPixmapItem::updateLocationSelection(QGraphicsSceneMouseEvent *event) {
    QPoint pos = Metatile::coordFromPixmapCoord(event->pos());

    // Snap point to within map bounds.
    if (pos.x() < 0) pos.setX(0);
    if (pos.x() >= this->layout->getWidth()) pos.setX(this->layout->getWidth() - 1);
    if (pos.y() < 0) pos.setY(0);
    if (pos.y() >= this->layout->getHeight()) pos.setY(this->layout->getHeight() - 1);
    this->updateSelection(pos);
}

void LocationPixmapItem::updateSelection(QPoint pos) {
    Block block;
    if (this->layout->getBlock(pos.x(), pos.y(), &block)) {
        this->selectedLocation->setValue(block.location());
    }
}
