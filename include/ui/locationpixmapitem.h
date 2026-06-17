#ifndef LOCATIONPIXMAPITEM_H
#define LOCATIONPIXMAPITEM_H

#include <QSpinBox>

#include "metatileselector.h"
#include "layoutpixmapitem.h"
#include "map.h"
#include "settings.h"

class LocationPixmapItem : public LayoutPixmapItem {
    Q_OBJECT
public:
    LocationPixmapItem(Layout *layout, QSpinBox * selectedLocation, MetatileSelector *metatileSelector, Settings *settings, qreal *opacity)
        : LayoutPixmapItem(layout, metatileSelector, settings){
        this->selectedLocation = selectedLocation;
        this->opacity = opacity;
        layout->setLocationItem(this);
    }
    QSpinBox * selectedLocation;
    qreal *opacity;
    void updateLocationSelection(QGraphicsSceneMouseEvent *event);
    virtual void paint(QGraphicsSceneMouseEvent*) override;
    virtual void floodFill(QGraphicsSceneMouseEvent*) override;
    virtual void magicFill(QGraphicsSceneMouseEvent*) override;
    virtual void pick(QGraphicsSceneMouseEvent*) override;
    void draw(bool ignoreCache = false) override;

private:
    void updateSelection(QPoint pos);
};

#endif // LOCATIONPIXMAPITEM_H
