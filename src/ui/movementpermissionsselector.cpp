#include "movementpermissionsselector.h"
#include <QPainter>

const int MovementPermissionsSelector::CellWidth = 32;
const int MovementPermissionsSelector::CellHeight = 32;

void MovementPermissionsSelector::draw() {
    this->setPixmap(this->basePixmap);
    this->drawSelection();
}

void MovementPermissionsSelector::setBasePixmap(QPixmap pixmap) {
    this->basePixmap = pixmap;
    this->draw();
}

void MovementPermissionsSelector::select(uint16_t x, uint16_t y) {
    SelectablePixmapItem::select(x, y, 1, 1);
}

int MovementPermissionsSelector::maxElevationLevel() {
    return static_cast<int>(Block::getMaxElevation()) - Elevation::FirstLevel;
}

// The elevation value represented by a given selector cell, using the current level for
// the "all levels" cell.
uint16_t MovementPermissionsSelector::valueAt(int cell) const {
    if (cell < Elevation::NumSpecial)
        return static_cast<uint16_t>(cell);
    return static_cast<uint16_t>(Elevation::FirstLevel + this->m_level);
}

uint16_t MovementPermissionsSelector::selectedValue() const {
    return valueAt(this->selectionInitialX);
}

bool MovementPermissionsSelector::isLevelCellSelected() const {
    return this->selectionInitialX >= Elevation::NumSpecial;
}

void MovementPermissionsSelector::setSelectedValue(uint16_t value) {
    int cell;
    if (value < Elevation::NumSpecial) {
        cell = value;
    } else {
        // The single "all levels" cell sits just past the special cells.
        cell = Elevation::NumSpecial;
        this->m_level = qBound(0, static_cast<int>(value) - Elevation::FirstLevel, maxElevationLevel());
    }
    SelectablePixmapItem::select(cell, 0, 1, 1);
    this->draw();
    emit selectedValueChanged(selectedValue());
}

void MovementPermissionsSelector::setElevationLevel(int level) {
    level = qBound(0, level, maxElevationLevel());
    bool changed = (level != this->m_level) || !isLevelCellSelected();
    this->m_level = level;
    // Setting a level implies painting an ordinary elevation, so move the selection to
    // the "all levels" cell.
    if (!isLevelCellSelected())
        SelectablePixmapItem::select(Elevation::NumSpecial, 0, 1, 1);
    this->draw();
    if (changed)
        emit selectedValueChanged(selectedValue());
}

void MovementPermissionsSelector::hoverMoveEvent(QGraphicsSceneHoverEvent *event) {
    QPoint pos = this->getCellPos(event->pos());
    emit this->hoveredMovementPermissionChanged(valueAt(pos.x()));
}

void MovementPermissionsSelector::hoverLeaveEvent(QGraphicsSceneHoverEvent *) {
    emit this->hoveredMovementPermissionCleared();
}
