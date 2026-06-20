#ifndef CONNECTIONPIXMAPITEM_H
#define CONNECTIONPIXMAPITEM_H

#include "mapconnection.h"
#include "map.h"
#include "metatile.h"
#include <QGraphicsPixmapItem>
#include <QPainter>
#include <QPointer>
#include <QKeyEvent>

class ConnectionPixmapItem : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
public:
    ConnectionPixmapItem(MapConnection* connection);

    const QPointer<MapConnection> connection;

    void setEditable(bool editable);
    bool getEditable();

    void setSelected(bool selected);

    // Sets which data layer (metatiles/collision/location/biome) is drawn over this
    // connection's visible edge, matching the editor's current edit mode.
    void setOverlay(Map::Layer layer, qreal opacity);

    void render(bool ignoreCache = false);

signals:
    void positionChanged(qreal x, qreal y);

private:
    QPixmap basePixmap;
    QPixmap overlayPixmap;
    Map::Layer overlayLayer = Map::Layer::Metatiles;
    qreal overlayOpacity = 1.0;
    qreal originX = 0;
    qreal originY = 0;
    bool selected = false;
    unsigned actionId = 0;

    static const int mWidth = Metatile::pixelWidth();
    static const int mHeight = Metatile::pixelHeight();

    void updatePos();
    void updateOrigin();
    void updateOverlay();
    void refresh();

protected:
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    virtual void mousePressEvent(QGraphicsSceneMouseEvent*) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent*) override;
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent*) override;

signals:
    void connectionItemDoubleClicked(MapConnection*);
    void selectionChanged(bool selected);
    void deleteRequested(MapConnection*);
};

#endif // CONNECTIONPIXMAPITEM_H
