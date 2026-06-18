#include "biomepixmapitem.h"
#include "editcommands.h"
#include "metatile.h"

void BiomePixmapItem::draw(bool ignoreCache) {
    if (this->layout) {
        this->layout->setBiomeItem(this);
        setPixmap(this->layout->renderBiome(ignoreCache));
        setOpacity(*this->opacity);
    }
}

void BiomePixmapItem::paint(QGraphicsSceneMouseEvent *event) {
    if (event->type() == QEvent::GraphicsSceneMouseRelease) {
        actionId_++;
    } else if (this->layout) {
        Blockdata oldBiome = this->layout->blockdata;

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
            block.setBiome(this->selectedBiome->value());
            this->layout->setBlock(pos.x(), pos.y(), block, true);
        }

        if (this->layout->blockdata != oldBiome) {
            this->layout->editHistory.push(new PaintBiome(this->layout, oldBiome, this->layout->blockdata, actionId_));
        }
    }
}

void BiomePixmapItem::floodFill(QGraphicsSceneMouseEvent *event) {
    if (event->type() == QEvent::GraphicsSceneMouseRelease) {
        this->actionId_++;
    } else if (this->layout) {
        Blockdata oldBiome = this->layout->blockdata;

        QPoint pos = Metatile::coordFromPixmapCoord(event->pos());
        uint16_t biome = this->selectedBiome->value();
        this->layout->floodFillBiome(pos.x(), pos.y(), biome);

        if (this->layout->blockdata != oldBiome) {
            this->layout->editHistory.push(new BucketFillBiome(this->layout, oldBiome, this->layout->blockdata));
        }
    }
}

void BiomePixmapItem::magicFill(QGraphicsSceneMouseEvent *event) {
    if (event->type() == QEvent::GraphicsSceneMouseRelease) {
        this->actionId_++;
    } else if (this->layout) {
        Blockdata oldBiome = this->layout->blockdata;
        QPoint pos = Metatile::coordFromPixmapCoord(event->pos());
        uint16_t biome = this->selectedBiome->value();
        this->layout->magicFillBiome(pos.x(), pos.y(), biome);

        if (this->layout->blockdata != oldBiome) {
            this->layout->editHistory.push(new MagicFillBiome(this->layout, oldBiome, this->layout->blockdata));
        }
    }
}

void BiomePixmapItem::pick(QGraphicsSceneMouseEvent *event) {
    QPoint pos = Metatile::coordFromPixmapCoord(event->pos());
    this->updateSelection(pos);
}

void BiomePixmapItem::updateBiomeSelection(QGraphicsSceneMouseEvent *event) {
    QPoint pos = Metatile::coordFromPixmapCoord(event->pos());

    // Snap point to within map bounds.
    if (pos.x() < 0) pos.setX(0);
    if (pos.x() >= this->layout->getWidth()) pos.setX(this->layout->getWidth() - 1);
    if (pos.y() < 0) pos.setY(0);
    if (pos.y() >= this->layout->getHeight()) pos.setY(this->layout->getHeight() - 1);
    this->updateSelection(pos);
}

void BiomePixmapItem::updateSelection(QPoint pos) {
    Block block;
    if (this->layout->getBlock(pos.x(), pos.y(), &block)) {
        this->selectedBiome->setValue(block.biome());
    }
}
