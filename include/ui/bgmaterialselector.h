#ifndef BGMATERIALSELECTOR_H
#define BGMATERIALSELECTOR_H

#include "selectablepixmapitem.h"
#include "maplayout.h"

// Picker for the per-tile bgMaterial attribute. Shows the first 16 primary metatiles in an
// 8x2 grid (cell 0 is drawn as a black square = "none"); the selected cell index is the
// bgMaterial value (0-15) stamped onto painted tiles. Also holds the "paint bgMaterial only"
// toggle, which makes painting/filling change only bgMaterial and leave the metatile alone.
class BgMaterialSelector : public SelectablePixmapItem {
    Q_OBJECT
public:
    BgMaterialSelector(Layout *layout)
        : SelectablePixmapItem(Metatile::pixelSize(), QSize(1, 1)), m_layout(layout) {}

    static constexpr int Count = 16; // bgMaterial is a 4-bit field (0-15)
    static constexpr int Wide = 8;   // 8x2 grid

    void draw() override;
    void refresh();
    void setLayout(Layout *layout);

    int selectedBgMaterial() const { return this->m_selected; }
    void setSelectedBgMaterial(int value);

    bool paintOnly() const { return this->m_paintOnly; }
    void setPaintOnly(bool on) { this->m_paintOnly = on; }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent*) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent*) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent*) override;

private:
    Layout *m_layout = nullptr;
    QPixmap m_basePixmap;
    int m_selected = 0;
    bool m_paintOnly = false;

    void updateBasePixmap();
    void updateSelected();

signals:
    void selectedBgMaterialChanged(int);
};

#endif // BGMATERIALSELECTOR_H
