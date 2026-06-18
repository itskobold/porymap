#ifndef MAPHEADER_H
#define MAPHEADER_H

#include <QObject>
#include <QString>
#include <array>

// Keep in sync with MAX_MAP_LOCATIONS in pokeemerald (include/global.fieldmap.h).
// The per-tile location attribute is 2 bits wide, so a map can define up to 4
// distinct location property sets.
#define MAX_MAP_LOCATIONS 4

class MapHeader : public QObject
{
    Q_OBJECT
public:
    MapHeader(QObject *parent = nullptr) : QObject(parent) {};
    ~MapHeader() {};
    MapHeader(const MapHeader& other);
    MapHeader& operator=(const MapHeader& other);

    // The per-location properties (mirrors struct MapHeaderLocationData in pokeemerald).
    // A map stores MAX_MAP_LOCATIONS of these; the per-tile location attribute selects
    // which one is active at runtime. Slots beyond numLocations are kept for editing but
    // are not compiled into the ROM.
    struct LocationData {
        QString secondaryTileset;
        QString song;            // music
        QString location;        // region_map_section
        QString type;            // map_type
        QString battleScene;     // battle_scene
        bool showsLocationName = false; // show_map_name

        bool operator==(const LocationData& other) const {
            return secondaryTileset == other.secondaryTileset
                && song == other.song
                && location == other.location
                && type == other.type
                && battleScene == other.battleScene
                && showsLocationName == other.showsLocationName;
        }
        bool operator!=(const LocationData& other) const { return !(operator==(other)); }
    };

    bool operator==(const MapHeader& other) const {
        return m_locations == other.m_locations
        && m_mapGridX == other.m_mapGridX
        && m_mapGridY == other.m_mapGridY
        && m_biomeGroup == other.m_biomeGroup
        && m_requiresFlash == other.m_requiresFlash
        && m_weather == other.m_weather
        && m_allowsRunning == other.m_allowsRunning
        && m_allowsBiking == other.m_allowsBiking
        && m_allowsEscaping == other.m_allowsEscaping
        && m_floorNumber == other.m_floorNumber
        && m_numLocations == other.m_numLocations;
    }
    bool operator!=(const MapHeader& other) const {
        return !(operator==(other));
    }

    static int clampLocationIndex(int i) {
        if (i < 0) return 0;
        if (i >= MAX_MAP_LOCATIONS) return MAX_MAP_LOCATIONS - 1;
        return i;
    }

    // Per-location accessors. The index selects one of the MAX_MAP_LOCATIONS slots.
    void setSecondaryTileset(int i, const QString &secondaryTileset);
    void setSong(int i, const QString &song);
    void setLocation(int i, const QString &location);
    void setType(int i, const QString &type);
    void setBattleScene(int i, const QString &battleScene);
    void setShowsLocationName(int i, bool showsLocationName);
    void setLocationData(int i, const LocationData &data);

    QString secondaryTileset(int i) const { return m_locations[clampLocationIndex(i)].secondaryTileset; }
    QString song(int i) const             { return m_locations[clampLocationIndex(i)].song; }
    QString location(int i) const         { return m_locations[clampLocationIndex(i)].location; }
    QString type(int i) const             { return m_locations[clampLocationIndex(i)].type; }
    QString battleScene(int i) const      { return m_locations[clampLocationIndex(i)].battleScene; }
    bool showsLocationName(int i) const   { return m_locations[clampLocationIndex(i)].showsLocationName; }
    LocationData locationData(int i) const { return m_locations[clampLocationIndex(i)]; }

    // Backward-compatible accessors that operate on the default location (slot 0).
    // The scripting API and the map list (which displays the default location name)
    // use these.
    void setSecondaryTileset(const QString &secondaryTileset) { setSecondaryTileset(0, secondaryTileset); }
    void setSong(const QString &song)                         { setSong(0, song); }
    void setLocation(const QString &location)                 { setLocation(0, location); }
    void setType(const QString &type)                         { setType(0, type); }
    void setBattleScene(const QString &battleScene)           { setBattleScene(0, battleScene); }
    void setShowsLocationName(bool showsLocationName)         { setShowsLocationName(0, showsLocationName); }

    QString secondaryTileset() const { return secondaryTileset(0); }
    QString song() const             { return song(0); }
    QString location() const         { return location(0); }
    QString type() const             { return type(0); }
    QString battleScene() const      { return battleScene(0); }
    bool showsLocationName() const   { return showsLocationName(0); }

    // Map-wide (non per-location) fields.
    // Position of this map within the world map grid (mirrors mapGridX/mapGridY in the
    // pokeemerald MapHeader). X can be 0-29 (5 bits), Y can be 0-15 (4 bits).
    void setMapGridX(int pos);
    void setMapGridY(int pos);
    // Biome group define name, e.g. "BIOME_GROUP_OVERWORLD" (see constants/biome.h).
    void setBiomeGroup(const QString &biomeGroup);
    void setRequiresFlash(bool requiresFlash);
    void setWeather(const QString &weather);
    void setAllowsRunning(bool allowsRunning);
    void setAllowsBiking(bool allowsBiking);
    void setAllowsEscaping(bool allowsEscaping);
    void setFloorNumber(int floorNumber);
    void setNumLocations(int numLocations);

    int mapGridX() const { return m_mapGridX; }
    int mapGridY() const { return m_mapGridY; }
    QString biomeGroup() const { return m_biomeGroup; }
    bool requiresFlash() const { return m_requiresFlash; }
    QString weather() const { return m_weather; }
    bool allowsRunning() const { return m_allowsRunning; }
    bool allowsBiking() const { return m_allowsBiking; }
    bool allowsEscaping() const { return m_allowsEscaping; }
    int floorNumber() const { return m_floorNumber; }
    int numLocations() const { return m_numLocations; }

signals:
    // Per-location changes. 'index' identifies which location slot changed.
    void locationDataChanged(int index);
    // Slot-0 convenience signals, kept for the scripting API and the map list.
    void songChanged(QString);
    void locationChanged(QString);
    void typeChanged(QString);
    void battleSceneChanged(QString);
    void showsLocationNameChanged(bool);
    void secondaryTilesetChanged(QString);

    void mapGridXChanged(int);
    void mapGridYChanged(int);
    void biomeGroupChanged(QString);
    void requiresFlashChanged(bool);
    void weatherChanged(QString);
    void allowsRunningChanged(bool);
    void allowsBikingChanged(bool);
    void allowsEscapingChanged(bool);
    void floorNumberChanged(int);
    void numLocationsChanged(int);
    void modified();

private:
    std::array<LocationData, MAX_MAP_LOCATIONS> m_locations = {};
    int m_mapGridX = 0;
    int m_mapGridY = 0;
    QString m_biomeGroup;
    bool m_requiresFlash = false;
    QString m_weather;
    bool m_allowsRunning = false;
    bool m_allowsBiking = false;
    bool m_allowsEscaping = false;
    int m_floorNumber = 0;
    // Number of distinct per-tile location values used by this map (1-4). Stored in the
    // map header as numLocations-1; the binary -1 conversion happens in the mapjson tool.
    int m_numLocations = 1;
};

#endif // MAPHEADER_H
