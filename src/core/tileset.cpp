#include "tileset.h"
#include "metatile.h"
#include "project.h"
#include "log.h"
#include "config.h"
#include "imageproviders.h"
#include "validator.h"

#include <QPainter>
#include <QImage>
#include <algorithm>


Tileset::Tileset(const Tileset &other)
    : name(other.name),
      is_secondary(other.is_secondary),
      tiles_label(other.tiles_label),
      palettes_label(other.palettes_label),
      metatiles_label(other.metatiles_label),
      metatiles_path(other.metatiles_path),
      metatile_attrs_label(other.metatile_attrs_label),
      metatile_attrs_path(other.metatile_attrs_path),
      metatile_compositing_label(other.metatile_compositing_label),
      metatile_compositing_path(other.metatile_compositing_path),
      tilesImagePath(other.tilesImagePath),
      palettePath(other.palettePath),
      numPalettes(other.numPalettes),
      metatileLabels(other.metatileLabels),
      palettes(other.palettes),
      palettePreviews(other.palettePreviews),
      m_tilesImage(other.m_tilesImage.copy()),
      m_hasUnsavedTilesImage(other.m_hasUnsavedTilesImage)
{
    for (auto tile : other.m_tiles) {
        m_tiles.append(tile.copy());
    }

    for (auto *metatile : other.m_metatiles) {
        m_metatiles.append(new Metatile(*metatile));
    }
}

Tileset &Tileset::operator=(const Tileset &other) {
    name = other.name;
    is_secondary = other.is_secondary;
    tiles_label = other.tiles_label;
    palettes_label = other.palettes_label;
    metatiles_label = other.metatiles_label;
    metatiles_path = other.metatiles_path;
    metatile_attrs_label = other.metatile_attrs_label;
    metatile_attrs_path = other.metatile_attrs_path;
    metatile_compositing_label = other.metatile_compositing_label;
    metatile_compositing_path = other.metatile_compositing_path;
    tilesImagePath = other.tilesImagePath;
    m_tilesImage = other.m_tilesImage.copy();
    palettePath = other.palettePath;
    numPalettes = other.numPalettes;
    metatileLabels = other.metatileLabels;
    palettes = other.palettes;
    palettePreviews = other.palettePreviews;

    m_tiles.clear();
    for (auto tile : other.m_tiles) {
        m_tiles.append(tile.copy());
    }

    clearMetatiles();
    for (auto *metatile : other.m_metatiles) {
        m_metatiles.append(new Metatile(*metatile));
    }

    return *this;
}

Tileset::~Tileset() {
    clearMetatiles();
}

void Tileset::clearMetatiles() {
    qDeleteAll(m_metatiles);
    m_metatiles.clear();
}

void Tileset::setMetatiles(const QList<Metatile*> &metatiles) {
    clearMetatiles();
    m_metatiles = metatiles;
}

void Tileset::addMetatile(Metatile* metatile) {
    m_metatiles.append(metatile);
}

void Tileset::resizeMetatiles(int newNumMetatiles) {
    if (newNumMetatiles < 0) newNumMetatiles = 0;
    while (m_metatiles.length() > newNumMetatiles) {
        delete m_metatiles.takeLast();
    }
    const int numTiles = projectConfig.getNumTilesInMetatile();
    while (m_metatiles.length() < newNumMetatiles) {
        m_metatiles.append(new Metatile(numTiles));
    }
}

uint16_t Tileset::firstMetatileId() const {
    return this->is_secondary ? Project::getNumMetatilesPrimary() : 0;
}

uint16_t Tileset::lastMetatileId() const {
    return qMax(1, firstMetatileId() + m_metatiles.length()) - 1;
}

int Tileset::maxMetatiles() const {
    return this->is_secondary ? Project::getNumMetatilesSecondary() : Project::getNumMetatilesPrimary();
}

uint16_t Tileset::firstTileId() const {
    return this->is_secondary ? Project::getNumTilesPrimary() : 0;
}

uint16_t Tileset::lastTileId() const {
    return qMax(1, firstMetatileId() + m_tiles.length()) - 1;
}

int Tileset::maxTiles() const {
    return this->is_secondary ? Project::getNumTilesSecondary() : Project::getNumTilesPrimary();
}

Tileset* Tileset::getPaletteTileset(int paletteId, Tileset *primaryTileset, Tileset *secondaryTileset) {
    return const_cast<Tileset*>(getPaletteTileset(paletteId, static_cast<const Tileset*>(primaryTileset), static_cast<const Tileset*>(secondaryTileset)));
}

const Tileset* Tileset::getPaletteTileset(int paletteId, const Tileset *primaryTileset, const Tileset *secondaryTileset) {
    if (paletteId < Project::getNumPalettesPrimary()) {
        return primaryTileset;
    } else if (paletteId < Project::getNumPalettesTotal()) {
        return secondaryTileset;
    } else {
        return nullptr;
    }
}

Tileset* Tileset::getTileTileset(int tileId, Tileset *primaryTileset, Tileset *secondaryTileset) {
    return const_cast<Tileset*>(getTileTileset(tileId, static_cast<const Tileset*>(primaryTileset), static_cast<const Tileset*>(secondaryTileset)));
}

// Get the tileset *expected* to contain the given 'tileId'. Note that this does not mean the tile actually exists in that tileset.
const Tileset* Tileset::getTileTileset(int tileId, const Tileset *primaryTileset, const Tileset *secondaryTileset) {
    if (tileId < Project::getNumTilesPrimary()) {
        return primaryTileset;
    } else if (tileId < Project::getNumTilesTotal()) {
        return secondaryTileset;
    } else {
        return nullptr;
    }
}

Tileset* Tileset::getMetatileTileset(int metatileId, Tileset *primaryTileset, Tileset *secondaryTileset) {
    return const_cast<Tileset*>(getMetatileTileset(metatileId, static_cast<const Tileset*>(primaryTileset), static_cast<const Tileset*>(secondaryTileset)));
}

// Get the tileset *expected* to contain the given 'metatileId'. Note that this does not mean the metatile actually exists in that tileset.
const Tileset* Tileset::getMetatileTileset(int metatileId, const Tileset *primaryTileset, const Tileset *secondaryTileset) {
    if (metatileId < Project::getNumMetatilesPrimary()) {
        return primaryTileset;
    } else if (metatileId < Project::getNumMetatilesTotal()) {
        return secondaryTileset;
    } else {
        return nullptr;
    }
}

Metatile* Tileset::getMetatile(int metatileId, Tileset *primaryTileset, Tileset *secondaryTileset) {
    return const_cast<Metatile*>(getMetatile(metatileId, static_cast<const Tileset*>(primaryTileset), static_cast<const Tileset*>(secondaryTileset)));
}

const Metatile* Tileset::getMetatile(int metatileId, const Tileset *primaryTileset, const Tileset *secondaryTileset) {
    const Tileset *tileset = Tileset::getMetatileTileset(metatileId, primaryTileset, secondaryTileset);
    if (!tileset) {
        return nullptr;
    }
    int index = Metatile::getIndexInTileset(metatileId);
    return tileset->m_metatiles.value(index, nullptr);
}

// Metatile labels are stored per-tileset. When looking for a metatile label, first search in the tileset
// that the metatile belongs to. If one isn't found, search in the other tileset. Labels coming from the
// tileset that the metatile does not belong to are shared and cannot be edited via Porymap.
Tileset* Tileset::getMetatileLabelTileset(int metatileId, Tileset *primaryTileset, Tileset *secondaryTileset) {
    Tileset *mainTileset = nullptr;
    Tileset *alternateTileset = nullptr;
    if (metatileId < Project::getNumMetatilesPrimary()) {
        mainTileset = primaryTileset;
        alternateTileset = secondaryTileset;
    } else if (metatileId < Project::getNumMetatilesTotal()) {
        mainTileset = secondaryTileset;
        alternateTileset = primaryTileset;
    }

    if (mainTileset && !mainTileset->metatileLabels.value(metatileId).isEmpty()) {
        return mainTileset;
    } else if (alternateTileset && !alternateTileset->metatileLabels.value(metatileId).isEmpty()) {
        return alternateTileset;
    }
    return nullptr;
}

// Return the pair of possible metatile labels for the specified metatile.
// "owned" is the label for the tileset to which the metatile belongs.
// "shared" is the label for the tileset to which the metatile does not belong.
MetatileLabelPair Tileset::getMetatileLabelPair(int metatileId, Tileset *primaryTileset, Tileset *secondaryTileset) {
    MetatileLabelPair labels;
    QString primaryMetatileLabel = primaryTileset ? primaryTileset->metatileLabels.value(metatileId) : "";
    QString secondaryMetatileLabel = secondaryTileset ? secondaryTileset->metatileLabels.value(metatileId) : "";

    if (metatileId < Project::getNumMetatilesPrimary()) {
        labels.owned = primaryMetatileLabel;
        labels.shared = secondaryMetatileLabel;
    } else if (metatileId < Project::getNumMetatilesTotal()) {
        labels.owned = secondaryMetatileLabel;
        labels.shared = primaryMetatileLabel;
    }
    return labels;
}

// If the metatile has a label in the tileset it belongs to, return that label.
// If it doesn't, and the metatile has a label in the other tileset, return that label.
// Otherwise return an empty string.
QString Tileset::getMetatileLabel(int metatileId, Tileset *primaryTileset, Tileset *secondaryTileset) {
    MetatileLabelPair labels = Tileset::getMetatileLabelPair(metatileId, primaryTileset, secondaryTileset);
    return !labels.owned.isEmpty() ? labels.owned : labels.shared;
}

// Just get the "owned" metatile label, i.e. the one for the tileset that the metatile belongs to.
QString Tileset::getOwnedMetatileLabel(int metatileId, Tileset *primaryTileset, Tileset *secondaryTileset) {
    MetatileLabelPair labels = Tileset::getMetatileLabelPair(metatileId, primaryTileset, secondaryTileset);
    return labels.owned;
}

bool Tileset::setMetatileLabel(int metatileId, QString label, Tileset *primaryTileset, Tileset *secondaryTileset) {
    Tileset *tileset = Tileset::getMetatileTileset(metatileId, primaryTileset, secondaryTileset);
    if (!tileset)
        return false;

    if (!label.isEmpty()) {
        IdentifierValidator validator;
        if (!validator.isValid(label))
            return false;
    }

    tileset->metatileLabels[metatileId] = label;
    return true;
}

QString Tileset::getMetatileLabelPrefix()
{
    return Tileset::getMetatileLabelPrefix(this->name);
}

QString Tileset::getMetatileLabelPrefix(const QString &name)
{
    // Default is "gTileset_Name" --> "METATILE_Name_"
    const QString labelPrefix = projectConfig.getIdentifier(ProjectIdentifier::define_metatile_label_prefix);
    return QString("%1%2_").arg(labelPrefix).arg(Tileset::stripPrefix(name));
}

bool Tileset::metatileIsValid(uint16_t metatileId, const Tileset *primaryTileset, const Tileset *secondaryTileset) {
    return (primaryTileset && primaryTileset->containsMetatileId(metatileId))
        || (secondaryTileset && secondaryTileset->containsMetatileId(metatileId));
}

QList<QList<QRgb>> Tileset::getBlockPalettes(const Tileset *primaryTileset, const Tileset *secondaryTileset, bool useTruePalettes) {
    QList<QList<QRgb>> palettes;

    QList<QList<QRgb>> primaryPalettes;
    if (primaryTileset) {
        primaryPalettes = useTruePalettes ? primaryTileset->palettes : primaryTileset->palettePreviews;
    }
    for (int i = 0; i < Project::getNumPalettesPrimary(); i++) {
        palettes.append(primaryPalettes.value(i));
    }

    QList<QList<QRgb>> secondaryPalettes;
    if (secondaryTileset) {
        secondaryPalettes = useTruePalettes ? secondaryTileset->palettes : secondaryTileset->palettePreviews;
    }
    for (int i = Project::getNumPalettesPrimary(); i < Project::getNumPalettesTotal(); i++) {
        palettes.append(secondaryPalettes.value(i));
    }

    return palettes;
}

QList<QRgb> Tileset::getPalette(int paletteId, const Tileset *primaryTileset, const Tileset *secondaryTileset, bool useTruePalettes) {
    QList<QRgb> paletteTable;
    const Tileset *tileset = paletteId < Project::getNumPalettesPrimary()
            ? primaryTileset
            : secondaryTileset;
    if (!tileset) {
        return paletteTable;
    }

    auto palettes = useTruePalettes ? tileset->palettes : tileset->palettePreviews;
    if (paletteId < 0 || paletteId >= palettes.length()) {
        return paletteTable;
    }

    for (int i = 0; i < palettes.at(paletteId).length(); i++) {
        paletteTable.append(palettes.at(paletteId).at(i));
    }
    return paletteTable;
}

bool Tileset::appendToHeaders(const QString &filepath, const QString &friendlyName, bool usingAsm) {
    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append)) {
        logError(QString("Could not write to file \"%1\"").arg(filepath));
        return false;
    }
    QString isSecondaryStr = this->is_secondary ? "TRUE" : "FALSE";
    QString dataString = "\n";
    if (usingAsm) {
        // Append to asm file
        dataString.append("\t.align 2\n");
        dataString.append(QString("%1::\n").arg(this->name));
        dataString.append("\t.byte TRUE @ is compressed\n");
        dataString.append(QString("\t.byte %1 @ is secondary\n").arg(isSecondaryStr));
        dataString.append("\t.2byte 0 @ padding\n");
        dataString.append(QString("\t.4byte gTilesetTiles_%1\n").arg(friendlyName));
        dataString.append(QString("\t.4byte gTilesetPalettes_%1\n").arg(friendlyName));
        dataString.append(QString("\t.4byte gMetatiles_%1\n").arg(friendlyName));
        if (projectConfig.baseGameVersion == BaseGameVersion::pokefirered) {
            dataString.append("\t.4byte NULL @ animation callback\n");
            dataString.append(QString("\t.4byte gMetatileAttributes_%1\n").arg(friendlyName));
        } else {
            dataString.append(QString("\t.4byte gMetatileAttributes_%1\n").arg(friendlyName));
            dataString.append("\t.4byte NULL @ animation callback\n");
        }
    } else {
        // Append to C file
        dataString.append(QString("const struct Tileset %1 =\n{\n").arg(this->name));
        if (projectConfig.tilesetsHaveIsCompressed) dataString.append("    .isCompressed = TRUE,\n");
        dataString.append(QString("    .isSecondary = %1,\n").arg(isSecondaryStr));
        dataString.append(QString("    .tiles = gTilesetTiles_%1,\n").arg(friendlyName));
        dataString.append(QString("    .palettes = gTilesetPalettes_%1,\n").arg(friendlyName));
        dataString.append(QString("    .metatiles = gMetatiles_%1,\n").arg(friendlyName));
        dataString.append(QString("    .metatileAttributes = gMetatileAttributes_%1,\n").arg(friendlyName));
        dataString.append(QString("    .metatileCompositing = gMetatileCompositing_%1,\n").arg(friendlyName));
        if (projectConfig.tilesetsHaveCallback) dataString.append("    .callback = NULL,\n");
        dataString.append("};\n");
    }
    file.write(dataString.toUtf8());
    file.flush();
    file.close();
    return true;
}

bool Tileset::appendToGraphics(const QString &filepath, const QString &friendlyName, bool usingAsm) {
    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append)) {
        logError(QString("Could not write to file \"%1\"").arg(filepath));
        return false;
    }

    const QString tilesetDir = this->getExpectedDir();
    // Single 8bpp tiles.png + single multi-bank palette.pal. tiles.png stores absolute palette
    // indices; "-palette_mod 16" reduces them to per-bank relative indices at build time.
    const QString tilesPath = tilesetDir + "/tiles.png";
    const QString palettePath = tilesetDir + "/palette.pal";

    QString dataString = "\n";
    if (usingAsm) {
        // Append to asm file
        dataString.append("\t.align 2\n");
        dataString.append(QString("gTilesetPalettes_%1::\n").arg(friendlyName));
        dataString.append(QString("\t.incbin \"%1\"\n").arg(palettePath));
        dataString.append("\n\t.align 2\n");
        dataString.append(QString("gTilesetTiles_%1::\n").arg(friendlyName));
        dataString.append(QString("\t.incbin \"%1.8bpp\"\n").arg(tilesPath));
    } else {
        // Append to C file
        dataString.append(QString("const u16 gTilesetPalettes_%1[] = INCGFX_U16(\"%2\", \".gbapal\");\n").arg(friendlyName, palettePath));
        dataString.append(QString("const u32 gTilesetTiles_%1[] = INCGFX_U32(\"%2\", \".8bpp\", \"-palette_mod 16\");\n").arg(friendlyName, tilesPath));
    }
    file.write(dataString.toUtf8());
    file.flush();
    file.close();
    return true;
}

bool Tileset::appendToMetatiles(const QString &filepath, const QString &friendlyName, bool usingAsm) {
    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append)) {
        logError(QString("Could not write to file \"%1\"").arg(filepath));
        return false;
    }

    const QString tilesetDir = this->getExpectedDir();
    const QString metatilesPath = tilesetDir + "/metatiles.bin";
    const QString metatileAttrsPath = tilesetDir + "/metatile_attributes.bin";
    const QString metatileCompositingPath = tilesetDir + "/metatile_compositing.bin";

    QString dataString = "\n";
    if (usingAsm) {
        // Append to asm file
        dataString.append("\t.align 1\n");
        dataString.append(QString("gMetatiles_%1::\n").arg(friendlyName));
        dataString.append(QString("\t.incbin \"%1\"\n").arg(metatilesPath));
        dataString.append(QString("\n\t.align 1\n"));
        dataString.append(QString("gMetatileAttributes_%1::\n").arg(friendlyName));
        dataString.append(QString("\t.incbin \"%1\"\n").arg(metatileAttrsPath));
        dataString.append(QString("\n\t.align 1\n"));
        dataString.append(QString("gMetatileCompositing_%1::\n").arg(friendlyName));
        dataString.append(QString("\t.incbin \"%1\"\n").arg(metatileCompositingPath));
    } else {
        // Append to C file
        dataString.append(QString("const u16 gMetatiles_%1[] = INCBIN_U16(\"%2\");\n").arg(friendlyName, metatilesPath));
        QString numBits = QString::number(projectConfig.metatileAttributesSize * 8);
        dataString.append(QString("const u%1 gMetatileAttributes_%2[] = INCBIN_U%1(\"%3\");\n").arg(numBits, friendlyName, metatileAttrsPath));
        dataString.append(QString("const u8 gMetatileCompositing_%1[] = INCBIN_U8(\"%2\");\n").arg(friendlyName, metatileCompositingPath));
    }
    file.write(dataString.toUtf8());
    file.flush();
    file.close();
    return true;
}

// The path where Porymap expects a Tileset's graphics assets to be stored (but not necessarily where they actually are)
// Example: for gTileset_DepartmentStore, returns "data/tilesets/secondary/department_store"
QString Tileset::getExpectedDir()
{
    return Tileset::getExpectedDir(this->name, this->is_secondary);
}

QString Tileset::getExpectedDir(QString tilesetName, bool isSecondary)
{
    const QString basePath = isSecondary ? projectConfig.getFilePath(ProjectFilePath::data_secondary_tilesets_folders)
                                         : projectConfig.getFilePath(ProjectFilePath::data_primary_tilesets_folders);

    static const QRegularExpression re("([a-z])([A-Z0-9])");
    return basePath + Tileset::stripPrefix(tilesetName).replace(re, "\\1_\\2").toLower();
}

// Get the expected positions of the members in struct Tileset.
// Used when parsing asm tileset data, or C tileset data that's missing initializers.
QHash<int, QString> Tileset::getHeaderMemberMap(bool usingAsm)
{
     // The asm header has a padding field that needs to be skipped
    int paddingOffset = usingAsm ? 1 : 0;

    // The position of metatileAttributes changes between games
    bool isPokefirered = (projectConfig.baseGameVersion == BaseGameVersion::pokefirered);
    int metatileAttrPosition = (isPokefirered ? 6 : 5) + paddingOffset;

    auto map = QHash<int, QString>();
    map.insert(1, "isSecondary");
    map.insert(2 + paddingOffset, "tiles");
    map.insert(3 + paddingOffset, "palettes");
    map.insert(4 + paddingOffset, "metatiles");
    map.insert(metatileAttrPosition, "metatileAttributes");
    return map;
}

bool Tileset::loadMetatiles() {
    clearMetatiles();

    QFile file(this->metatiles_path);
    if (!file.open(QIODevice::ReadOnly)) {
        logError(QString("Could not open '%1' for reading: %2").arg(this->metatiles_path).arg(file.errorString()));
        return false;
    }

    QByteArray data = file.readAll();
    int tilesPerMetatile = projectConfig.getNumTilesInMetatile();
    int bytesPerMetatile = Tile::sizeInBytes() * tilesPerMetatile;
    int numMetatiles = data.length() / bytesPerMetatile;
    if (numMetatiles > maxMetatiles()) {
        logWarn(QString("%1 metatile count %2 exceeds limit of %3. Additional metatiles will be ignored.")
                        .arg(this->name)
                        .arg(numMetatiles)
                        .arg(maxMetatiles()));
        numMetatiles = maxMetatiles();
    }

    for (int i = 0; i < numMetatiles; i++) {
        auto metatile = new Metatile;
        int index = i * bytesPerMetatile;
        for (int j = 0; j < tilesPerMetatile; j++) {
            uint16_t tileRaw = static_cast<unsigned char>(data[index++]);
            tileRaw |= static_cast<unsigned char>(data[index++]) << 8;
            metatile->tiles.append(Tile(tileRaw));
        }
        m_metatiles.append(metatile);
    }
    return true;
}

bool Tileset::saveMetatiles() {
    QFile file(this->metatiles_path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        logError(QString("Could not open '%1' for writing: %2").arg(this->metatiles_path).arg(file.errorString()));
        return false;
    }

    QByteArray data;
    int numTiles = projectConfig.getNumTilesInMetatile();
    for (const auto &metatile : m_metatiles) {
        for (int i = 0; i < numTiles; i++) {
            uint16_t tile = metatile->tiles.value(i).rawValue();
            data.append(static_cast<char>(tile));
            data.append(static_cast<char>(tile >> 8));
        }
    }
    file.write(data);
    return true;
}

bool Tileset::loadMetatileAttributes() {
    QFile file(this->metatile_attrs_path);
    if (!file.open(QIODevice::ReadOnly)) {
        logError(QString("Could not open '%1' for reading: %2").arg(this->metatile_attrs_path).arg(file.errorString()));
        return false;
    }

    QByteArray data = file.readAll();
    int attrSize = projectConfig.metatileAttributesSize;
    int numMetatiles = m_metatiles.length();
    int numMetatileAttrs = data.length() / attrSize;
    if (numMetatileAttrs > numMetatiles) {
        logWarn(QString("%1 metatile attributes count %2 exceeds metatile count of %3. Additional attributes will be ignored.")
                            .arg(this->name)
                            .arg(numMetatileAttrs)
                            .arg(numMetatiles));
        numMetatileAttrs = numMetatiles;
    } else if (numMetatileAttrs < numMetatiles) {
        logWarn(QString("%1 metatile attributes count %2 is fewer than the metatile count of %3. Missing attributes will default to 0.")
                            .arg(this->name)
                            .arg(numMetatileAttrs)
                            .arg(numMetatiles));
    }

    for (int i = 0; i < numMetatileAttrs; i++) {
        uint32_t attributes = 0;
        for (int j = 0; j < attrSize; j++)
            attributes |= static_cast<unsigned char>(data.at(i * attrSize + j)) << (8 * j);
        m_metatiles.at(i)->setAttributes(attributes);
    }
    return true;
}

bool Tileset::saveMetatileAttributes() {
    QFile file(this->metatile_attrs_path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        logError(QString("Could not open '%1' for writing: %2").arg(this->metatile_attrs_path).arg(file.errorString()));
        return false;
    }

    QByteArray data;
    for (const auto &metatile : m_metatiles) {
        uint32_t attributes = metatile->getAttributes();
        for (int i = 0; i < projectConfig.metatileAttributesSize; i++)
            data.append(static_cast<char>(attributes >> (8 * i)));
    }
    file.write(data);
    return true;
}

// Per-metatile fg/bg compositing flags, one u8 each, stored parallel to the metatiles/attributes.
bool Tileset::loadMetatileCompositing() {
    QFile file(this->metatile_compositing_path);
    if (!file.open(QIODevice::ReadOnly)) {
        // Compositing data is optional; absent file just means every metatile defaults to 0.
        return true;
    }

    QByteArray data = file.readAll();
    int numMetatiles = m_metatiles.length();
    int numEntries = data.length();
    if (numEntries > numMetatiles) {
        logWarn(QString("%1 metatile compositing count %2 exceeds metatile count of %3. Additional entries will be ignored.")
                            .arg(this->name)
                            .arg(numEntries)
                            .arg(numMetatiles));
        numEntries = numMetatiles;
    } else if (numEntries < numMetatiles) {
        logWarn(QString("%1 metatile compositing count %2 is fewer than the metatile count of %3. Missing entries will default to 0.")
                            .arg(this->name)
                            .arg(numEntries)
                            .arg(numMetatiles));
    }

    for (int i = 0; i < numEntries; i++)
        m_metatiles.at(i)->setCompositing(static_cast<unsigned char>(data.at(i)));
    return true;
}

bool Tileset::saveMetatileCompositing() {
    QFile file(this->metatile_compositing_path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        logError(QString("Could not open '%1' for writing: %2").arg(this->metatile_compositing_path).arg(file.errorString()));
        return false;
    }

    QByteArray data;
    for (const auto &metatile : m_metatiles)
        data.append(static_cast<char>(metatile->compositing()));
    file.write(data);
    return true;
}

bool Tileset::loadTilesImage(QImage *importedImage) {
    QImage image;
    bool imported = false;
    if (importedImage) {
        image = *importedImage;
        imported = true;
    } else if (QFile::exists(this->tilesImagePath)) {
        // No image provided, load from file path. tiles.png is an 8bpp (256-colour) indexed image
        // storing absolute palette indices; keep its indices intact rather than requantizing. The
        // colorCount > numColorsPerPalette() branch below reduces them to per-bank relative indices,
        // mirroring the game's "-palette_mod 16".
        image = QImage(this->tilesImagePath);
        if (image.format() != QImage::Format_Indexed8)
            image = image.convertToFormat(QImage::Format_Indexed8, Qt::ThresholdDither);
    }

    if (image.isNull()) {
        logWarn(QString("Failed to load tiles image for %1. Using default tiles image.").arg(this->name));
        image = QImage(Tile::pixelWidth(), Tile::pixelHeight(), QImage::Format_Indexed8);
        image.fill(0);
    }

    // Validate image dimensions
    if (image.width() % Tile::pixelWidth() || image.height() % Tile::pixelHeight()) {
        logError(QString("%1 tiles image has invalid dimensions %2x%3. Dimensions must be a multiple of %4x%5.")
                            .arg(this->name)
                            .arg(image.width())
                            .arg(image.height())
                            .arg(Tile::pixelWidth())
                            .arg(Tile::pixelHeight()));
        return false;
    }

    // Validate the number of colors in the image.
    int colorCount = image.colorCount();
    if (colorCount > Tileset::numColorsPerPalette()) {
        flattenTo4bppImage(&image);
    } else if (colorCount < Tileset::numColorsPerPalette()) {
        QVector<QRgb> colorTable = image.colorTable();
        for (int i = colorTable.length(); i < Tileset::numColorsPerPalette(); i++) {
            colorTable.append(0);
        }
        image.setColorTable(colorTable);
    }
    m_tilesImage = image;

    // Cut up the full tiles image into individual tile images.
    m_tiles.clear();
    for (int y = 0; y < image.height(); y += Tile::pixelHeight())
    for (int x = 0; x < image.width(); x += Tile::pixelWidth()) {
        m_tiles.append(image.copy(x, y, Tile::pixelWidth(), Tile::pixelHeight()));
    }

    if (m_tiles.length() > maxTiles()) {
        logWarn(QString("%1 tile count of %2 exceeds limit of %3. Additional tiles will not be displayed.")
                            .arg(this->name)
                            .arg(m_tiles.length())
                            .arg(maxTiles()));

        // Just resize m_tiles so that numTiles() reports the correct tile count.
        // We'll leave m_tilesImage alone (it doesn't get displayed, and we don't want to delete the user's image data).
        m_tiles = m_tiles.mid(0, maxTiles());
    }

    if (imported) {
        // Only set this flag once we've successfully loaded the tiles image.
        m_hasUnsavedTilesImage = true;
    }

    return true;
}

bool Tileset::saveTilesImage() {
    // Only write the tiles image if it was changed.
    // Porymap will only ever change an existing tiles image by importing a new one.
    if (!m_hasUnsavedTilesImage)
        return true;

    if (!m_tilesImage.save(this->tilesImagePath, "PNG")) {
        logError(QString("Failed to save tiles image '%1'").arg(this->tilesImagePath));
        return false;
    }

    m_hasUnsavedTilesImage = false;
    return true;
}

// First palette bank index belonging to this tileset. Primary banks occupy 0..NUM_PALS_PRIMARY-1,
// secondary banks NUM_PALS_PRIMARY..NUM_PALS_TOTAL-1, matching the in-game palette layout.
int Tileset::paletteOffset() const {
    return this->is_secondary ? Project::getNumPalettesPrimary() : 0;
}

// Palettes are stored as one multi-bank JASC palette.pal (16 colours per bank). We keep the
// editor's per-bank model: the 16 absolute banks 0-15 are kept, and this tileset's banks are
// loaded into the range starting at paletteOffset(). Banks belonging to the paired tileset stay
// as dummy colours here (they're supplied by that tileset).
bool Tileset::loadPalettes() {
    this->palettes.clear();
    this->palettePreviews.clear();

    // Start every bank as dummy greyscale.
    for (int i = 0; i < Tileset::maxPalettes(); i++) {
        QList<QRgb> palette;
        for (int j = 0; j < Tileset::numColorsPerPalette(); j++) {
            int colorComponent = j * (256 / Tileset::numColorsPerPalette());
            palette.append(qRgb(colorComponent, colorComponent, colorComponent));
        }
        this->palettes.append(palette);
    }

    this->numPalettes = 0;
    if (!this->palettePath.isEmpty()) {
        bool error = false;
        QList<QRgb> colors = PaletteUtil::parse(this->palettePath, &error);
        if (error) colors.clear();

        const int offset = paletteOffset();
        const int colorsPerPalette = Tileset::numColorsPerPalette();
        int numBanks = colors.length() / colorsPerPalette;
        for (int b = 0; b < numBanks; b++) {
            int bank = offset + b;
            if (bank >= Tileset::maxPalettes())
                break;
            QList<QRgb> palette;
            for (int j = 0; j < colorsPerPalette; j++)
                palette.append(colors.at(b * colorsPerPalette + j));
            this->palettes[bank] = palette;
        }
        this->numPalettes = numBanks;
    }

    this->palettePreviews = this->palettes;
    return true;
}

bool Tileset::savePalettes() {
    const int offset = paletteOffset();
    int count = this->numPalettes;
    if (count <= 0) {
        // Never loaded a count (e.g. brand new tileset); fall back to the standard bank count.
        count = this->is_secondary ? (Project::getNumPalettesTotal() - Project::getNumPalettesPrimary())
                                   : Project::getNumPalettesPrimary();
    }

    QVector<QRgb> combined;
    for (int b = 0; b < count; b++) {
        const QList<QRgb> &palette = this->palettes.value(offset + b);
        for (int j = 0; j < Tileset::numColorsPerPalette(); j++)
            combined.append(palette.value(j, qRgb(0, 0, 0)));
    }
    if (combined.isEmpty())
        return true;
    return PaletteUtil::writeJASC(this->palettePath, combined, 0, combined.size());
}

bool Tileset::load() {
    bool success = true;
    if (!loadPalettes()) success = false;
    if (!loadTilesImage()) success = false;
    if (!loadMetatiles()) success = false;
    if (!loadMetatileAttributes()) success = false;
    if (!loadMetatileCompositing()) success = false;
    return success;
}

// Because metatile labels are global (and handled by the project) we don't save them here.
bool Tileset::save() {
    bool success = true;
    if (!savePalettes()) success = false;
    if (!saveTilesImage()) success = false;
    if (!saveMetatiles()) success = false;
    if (!saveMetatileAttributes()) success = false;
    if (!saveMetatileCompositing()) success = false;
    return success;
}

QString Tileset::stripPrefix(const QString &fullName) {
    return QString(fullName).replace(projectConfig.getIdentifier(ProjectIdentifier::symbol_tilesets_prefix), "");
}

// Find which of the specified color IDs in 'searchColors' are not used by any of this tileset's metatiles.
// The 'pairedTileset' may be used to get the tile images for any tiles that don't belong to this tileset.
// If 'searchColors' is empty, it will for search for all unused colors.
QSet<int> Tileset::getUnusedColorIds(int paletteId, const Tileset *pairedTileset, const QSet<int> &searchColors) const {
    QSet<int> unusedColors = searchColors;
    if (unusedColors.isEmpty()) {
        // Search for all colors
        for (int i = 0; i < Tileset::numColorsPerPalette(); i++) {
            unusedColors.insert(i);
        }
    }
    const Tileset *primaryTileset = this->is_secondary ? pairedTileset : this;
    const Tileset *secondaryTileset = this->is_secondary ? this : pairedTileset;
    QSet<uint16_t> seenTileIds;
    for (const auto &metatile : m_metatiles)
    for (const auto &tile : metatile->tiles) {
        if (tile.palette != paletteId)
            continue;

        // Save time by ignoring tiles we've already inspected.
        if (seenTileIds.contains(tile.tileId))
            continue;
        seenTileIds.insert(tile.tileId);

        QImage image = getTileImage(tile.tileId, primaryTileset, secondaryTileset);
        if (image.isNull() || image.sizeInBytes() < Tile::numPixels())
            continue;

        const uchar * pixels = image.constBits();
        for (int i = 0; i < Tile::numPixels(); i++) {
            auto it = unusedColors.constFind(pixels[i]);
            if (it != unusedColors.constEnd()) {
                unusedColors.erase(it);
                if (unusedColors.isEmpty()) {
                    return {};
                }
            }
        }
    }
    return unusedColors;
}

// Returns the list of metatile IDs representing all the metatiles in this tileset that use the specified color ID.
QList<uint16_t> Tileset::findMetatilesUsingColor(int paletteId, int colorId, const Tileset *pairedTileset) const {
    const Tileset *primaryTileset = this->is_secondary ? pairedTileset : this;
    const Tileset *secondaryTileset = this->is_secondary ? this : pairedTileset;
    QSet<uint16_t> metatileIdSet;
    QHash<uint16_t, bool> tileContainsColor;
    uint16_t metatileIdBase = firstMetatileId();
    for (int i = 0; i < m_metatiles.length(); i++) {
        uint16_t metatileId = i + metatileIdBase;
        for (const auto &tile : m_metatiles.at(i)->tiles) {
            if (tile.palette != paletteId)
                continue;

            // Save time on tiles we've already inspected by getting the cached result.
            auto tileIt = tileContainsColor.constFind(tile.tileId);
            if (tileIt != tileContainsColor.constEnd()) {
                if (tileIt.value()) metatileIdSet.insert(metatileId);
                continue;
            }
            tileContainsColor[tile.tileId] = false;

            QImage image = getTileImage(tile.tileId, primaryTileset, secondaryTileset);
            if (image.isNull() || image.sizeInBytes() < Tile::numPixels())
                continue;

            const uchar * pixels = image.constBits();
            for (int j = 0; j < Tile::numPixels(); j++) {
                if (pixels[j] == colorId) {
                    metatileIdSet.insert(metatileId);
                    tileContainsColor[tile.tileId] = true;
                    break;
                }
            }
        }
    }
    QList<uint16_t> metatileIds(metatileIdSet.constBegin(), metatileIdSet.constEnd());
    std::sort(metatileIds.begin(), metatileIds.end());
    return metatileIds;
}
