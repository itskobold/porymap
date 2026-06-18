#ifndef BIOMEPIXMAPITEM_H
#define BIOMEPIXMAPITEM_H

#include <QSpinBox>

#include "metatileselector.h"
#include "layoutpixmapitem.h"
#include "map.h"
#include "settings.h"

class BiomePixmapItem : public LayoutPixmapItem {
    Q_OBJECT
public:
    BiomePixmapItem(Layout *layout, QSpinBox * selectedBiome, MetatileSelector *metatileSelector, Settings *settings, qreal *opacity)
        : LayoutPixmapItem(layout, metatileSelector, settings){
        this->selectedBiome = selectedBiome;
        this->opacity = opacity;
        layout->setBiomeItem(this);
    }
    QSpinBox * selectedBiome;
    qreal *opacity;
    void updateBiomeSelection(QGraphicsSceneMouseEvent *event);
    virtual void paint(QGraphicsSceneMouseEvent*) override;
    virtual void floodFill(QGraphicsSceneMouseEvent*) override;
    virtual void magicFill(QGraphicsSceneMouseEvent*) override;
    virtual void pick(QGraphicsSceneMouseEvent*) override;
    void draw(bool ignoreCache = false) override;

private:
    void updateSelection(QPoint pos);
};

#endif // BIOMEPIXMAPITEM_H
