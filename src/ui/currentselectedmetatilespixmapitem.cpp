#include "currentselectedmetatilespixmapitem.h"
#include "imageproviders.h"
#include <QPainter>

QPixmap drawMetatileSelection(MetatileSelection selection, Layout *layout, int selectedBgMaterial) {
    int width = selection.dimensions.width() * Metatile::pixelWidth();
    int height = selection.dimensions.height() * Metatile::pixelHeight();
    QImage image(width, height, QImage::Format_RGBA8888);
    image.fill(QColor(0, 0, 0, 0));
    QPainter painter(&image);

    for (int i = 0; i < selection.dimensions.width(); i++) {
        for (int j = 0; j < selection.dimensions.height(); j++) {
            int x = i * Metatile::pixelWidth();
            int y = j * Metatile::pixelHeight();
            QPoint metatile_origin = QPoint(x, y);
            int index = j * selection.dimensions.width() + i;
            MetatileSelectionItem item = selection.metatileItems.value(index);
            if (item.enabled) {
                // Tiles picked off the map carry their source location, so render secondary
                // metatiles with that location's secondary tileset (matching the canvas)
                // rather than the active location's.
                const Tileset *secondary = (item.location >= 0 && Layout::metatileIsSecondary(item.metatileId))
                        ? layout->secondaryTilesetForLocation(item.location) : layout->tileset_secondary;
                // Flagged metatiles preview with the selected bg material (matches the picker).
                // selectedBgMaterial < 0 means no material context.
                const Metatile *metatile = Tileset::getMetatile(item.metatileId, layout->tileset_primary, secondary);
                const Metatile *materialMetatile = (selectedBgMaterial >= 0 && metatile && metatile->usesBgMaterial())
                        ? Tileset::getMetatile(selectedBgMaterial, layout->tileset_primary, secondary) : nullptr;
                QImage metatile_image = getMetatileImage(item.metatileId, layout->tileset_primary, secondary,
                                                         layout->metatileLayerOrder(), layout->metatileLayerOpacity(),
                                                         false, materialMetatile);
                painter.drawImage(metatile_origin, metatile_image);
            }
        }
    }

    painter.end();
    return QPixmap::fromImage(image);
}

void CurrentSelectedMetatilesPixmapItem::draw() {
    MetatileSelection selection = metatileSelector->getMetatileSelection();
    setPixmap(drawMetatileSelection(selection, this->layout, metatileSelector->selectedBgMaterial()));
}
