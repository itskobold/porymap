#include "bgmaterialselector.h"
#include "imageproviders.h"
#include <QPainter>

void BgMaterialSelector::setLayout(Layout *layout) {
    this->m_layout = layout;
    this->m_basePixmap = QPixmap();
}

void BgMaterialSelector::refresh() {
    this->m_basePixmap = QPixmap();
    draw();
}

void BgMaterialSelector::updateBasePixmap() {
    if (!this->m_layout || !this->m_layout->tileset_primary) {
        this->m_basePixmap = QPixmap();
        return;
    }
    // The first 16 primary metatiles, 8 per row.
    QImage sheet = getMetatileSheetImage(
        this->m_layout->tileset_primary,
        this->m_layout->tileset_secondary,
        0, Count - 1, Wide,
        this->m_layout->metatileLayerOrder(),
        this->m_layout->metatileLayerOpacity(),
        Metatile::pixelSize(), false);
    // Cell 0 (bgMaterial 0 = "none") is shown as a plain black square.
    QPainter painter(&sheet);
    painter.fillRect(QRect(0, 0, Metatile::pixelWidth(), Metatile::pixelHeight()), Qt::black);
    painter.end();
    this->m_basePixmap = QPixmap::fromImage(sheet);
}

void BgMaterialSelector::draw() {
    if (this->m_basePixmap.isNull())
        updateBasePixmap();
    setPixmap(this->m_basePixmap);
    drawSelection();
}

void BgMaterialSelector::setSelectedBgMaterial(int value) {
    value = qBound(0, value, Count - 1);
    bool changed = (value != this->m_selected);
    this->m_selected = value;
    this->select(value % Wide, value / Wide); // base class updates selection + redraws
    if (changed)
        emit selectedBgMaterialChanged(value);
}

void BgMaterialSelector::updateSelected() {
    QPoint origin = this->getSelectionStart();
    int idx = qBound(0, origin.y() * Wide + origin.x(), Count - 1);
    if (idx != this->m_selected) {
        this->m_selected = idx;
        emit selectedBgMaterialChanged(idx);
    }
}

void BgMaterialSelector::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    SelectablePixmapItem::mousePressEvent(event);
    updateSelected();
}

void BgMaterialSelector::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    SelectablePixmapItem::mouseMoveEvent(event);
    updateSelected();
}

void BgMaterialSelector::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    SelectablePixmapItem::mouseReleaseEvent(event);
    updateSelected();
}
