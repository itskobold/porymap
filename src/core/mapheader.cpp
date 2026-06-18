#include "mapheader.h"

MapHeader::MapHeader(const MapHeader& other) : MapHeader() {
    m_locations = other.m_locations;
    m_mapGridX = other.m_mapGridX;
    m_mapGridY = other.m_mapGridY;
    m_biomeGroup = other.m_biomeGroup;
    m_requiresFlash = other.m_requiresFlash;
    m_weather = other.m_weather;
    m_allowsRunning = other.m_allowsRunning;
    m_allowsBiking = other.m_allowsBiking;
    m_allowsEscaping = other.m_allowsEscaping;
    m_floorNumber = other.m_floorNumber;
    m_numLocations = other.m_numLocations;
}

MapHeader &MapHeader::operator=(const MapHeader &other) {
    // We want to call each set function here to ensure any fieldChanged signals
    // are sent as necessary. This does also mean the modified signal can be sent
    // repeatedly (but for now at least that's not a big issue).
    for (int i = 0; i < MAX_MAP_LOCATIONS; i++)
        setLocationData(i, other.m_locations[i]);
    setMapGridX(other.m_mapGridX);
    setMapGridY(other.m_mapGridY);
    setBiomeGroup(other.m_biomeGroup);
    setRequiresFlash(other.m_requiresFlash);
    setWeather(other.m_weather);
    setAllowsRunning(other.m_allowsRunning);
    setAllowsBiking(other.m_allowsBiking);
    setAllowsEscaping(other.m_allowsEscaping);
    setFloorNumber(other.m_floorNumber);
    setNumLocations(other.m_numLocations);
    return *this;
}

void MapHeader::setSecondaryTileset(int i, const QString &secondaryTileset) {
    i = clampLocationIndex(i);
    if (m_locations[i].secondaryTileset == secondaryTileset)
        return;
    m_locations[i].secondaryTileset = secondaryTileset;
    if (i == 0)
        emit secondaryTilesetChanged(secondaryTileset);
    emit locationDataChanged(i);
    emit modified();
}

void MapHeader::setSong(int i, const QString &song) {
    i = clampLocationIndex(i);
    if (m_locations[i].song == song)
        return;
    m_locations[i].song = song;
    if (i == 0)
        emit songChanged(song);
    emit locationDataChanged(i);
    emit modified();
}

void MapHeader::setLocation(int i, const QString &location) {
    i = clampLocationIndex(i);
    if (m_locations[i].location == location)
        return;
    m_locations[i].location = location;
    if (i == 0)
        emit locationChanged(location);
    emit locationDataChanged(i);
    emit modified();
}

void MapHeader::setType(int i, const QString &type) {
    i = clampLocationIndex(i);
    if (m_locations[i].type == type)
        return;
    m_locations[i].type = type;
    if (i == 0)
        emit typeChanged(type);
    emit locationDataChanged(i);
    emit modified();
}

void MapHeader::setBattleScene(int i, const QString &battleScene) {
    i = clampLocationIndex(i);
    if (m_locations[i].battleScene == battleScene)
        return;
    m_locations[i].battleScene = battleScene;
    if (i == 0)
        emit battleSceneChanged(battleScene);
    emit locationDataChanged(i);
    emit modified();
}

void MapHeader::setShowsLocationName(int i, bool showsLocationName) {
    i = clampLocationIndex(i);
    if (m_locations[i].showsLocationName == showsLocationName)
        return;
    m_locations[i].showsLocationName = showsLocationName;
    if (i == 0)
        emit showsLocationNameChanged(showsLocationName);
    emit locationDataChanged(i);
    emit modified();
}

void MapHeader::setLocationData(int i, const LocationData &data) {
    // Route through the individual setters so the appropriate signals are emitted.
    setSecondaryTileset(i, data.secondaryTileset);
    setSong(i, data.song);
    setLocation(i, data.location);
    setType(i, data.type);
    setBattleScene(i, data.battleScene);
    setShowsLocationName(i, data.showsLocationName);
}

void MapHeader::setMapGridX(int pos) {
    if (m_mapGridX == pos)
        return;
    m_mapGridX = pos;
    emit mapGridXChanged(m_mapGridX);
    emit modified();
}

void MapHeader::setMapGridY(int pos) {
    if (m_mapGridY == pos)
        return;
    m_mapGridY = pos;
    emit mapGridYChanged(m_mapGridY);
    emit modified();
}

void MapHeader::setBiomeGroup(const QString &biomeGroup) {
    if (m_biomeGroup == biomeGroup)
        return;
    m_biomeGroup = biomeGroup;
    emit biomeGroupChanged(m_biomeGroup);
    emit modified();
}

void MapHeader::setRequiresFlash(bool requiresFlash) {
    if (m_requiresFlash == requiresFlash)
        return;
    m_requiresFlash = requiresFlash;
    emit requiresFlashChanged(m_requiresFlash);
    emit modified();
}

void MapHeader::setWeather(const QString &weather) {
    if (m_weather == weather)
        return;
    m_weather = weather;
    emit weatherChanged(m_weather);
    emit modified();
}

void MapHeader::setAllowsRunning(bool allowsRunning) {
    if (m_allowsRunning == allowsRunning)
        return;
    m_allowsRunning = allowsRunning;
    emit allowsRunningChanged(m_allowsRunning);
    emit modified();
}

void MapHeader::setAllowsBiking(bool allowsBiking) {
    if (m_allowsBiking == allowsBiking)
        return;
    m_allowsBiking = allowsBiking;
    emit allowsBikingChanged(m_allowsBiking);
    emit modified();
}

void MapHeader::setAllowsEscaping(bool allowsEscaping) {
    if (m_allowsEscaping == allowsEscaping)
        return;
    m_allowsEscaping = allowsEscaping;
    emit allowsEscapingChanged(m_allowsEscaping);
    emit modified();
}

void MapHeader::setFloorNumber(int floorNumber) {
    if (m_floorNumber == floorNumber)
        return;
    m_floorNumber = floorNumber;
    emit floorNumberChanged(m_floorNumber);
    emit modified();
}

void MapHeader::setNumLocations(int numLocations) {
    if (m_numLocations == numLocations)
        return;
    m_numLocations = numLocations;
    emit numLocationsChanged(m_numLocations);
    emit modified();
}
