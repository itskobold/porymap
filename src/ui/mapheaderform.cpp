#include "mapheaderform.h"
#include "ui_mapheaderform.h"

#include <QFormLayout>
#include <QLabel>
#include <QTabWidget>

MapHeaderForm::MapHeaderForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MapHeaderForm)
{
    ui->setupUi(this);

    // This value is an s8 by default, but we don't need to unnecessarily limit users.
    ui->spinBox_FloorNumber->setMinimum(INT_MIN);
    ui->spinBox_FloorNumber->setMaximum(INT_MAX);

    createLocationTabs();

    connect(ui->comboBox_Weather, &QComboBox::currentTextChanged, this, &MapHeaderForm::onWeatherChanged);
    connect(ui->checkBox_RequiresFlash, &QCheckBox::toggled, this, &MapHeaderForm::onRequiresFlashChanged);
    connect(ui->checkBox_AllowRunning,  &QCheckBox::toggled, this, &MapHeaderForm::onAllowRunningChanged);
    connect(ui->checkBox_AllowBiking,   &QCheckBox::toggled, this, &MapHeaderForm::onAllowBikingChanged);
    connect(ui->checkBox_AllowEscaping, &QCheckBox::toggled, this, &MapHeaderForm::onAllowEscapingChanged);
    connect(ui->spinBox_FloorNumber, QOverload<int>::of(&QSpinBox::valueChanged), this, &MapHeaderForm::onFloorNumberChanged);
    connect(ui->spinBox_MapGridX, QOverload<int>::of(&QSpinBox::valueChanged), this, &MapHeaderForm::onMapGridXChanged);
    connect(ui->spinBox_MapGridY, QOverload<int>::of(&QSpinBox::valueChanged), this, &MapHeaderForm::onMapGridYChanged);
    connect(ui->comboBox_BiomeGroup, &QComboBox::currentTextChanged, this, &MapHeaderForm::onBiomeGroupChanged);

    connect(ui->tabWidget_Locations, &QTabWidget::currentChanged, this, &MapHeaderForm::onCurrentTabChanged);
}

MapHeaderForm::~MapHeaderForm()
{
    delete ui;
}

// Builds one tab per location slot. The per-location field widgets are created here
// (rather than in the .ui file) so we don't have to duplicate the same set of widgets
// MAX_MAP_LOCATIONS times in Qt Designer.
void MapHeaderForm::createLocationTabs() {
    for (int i = 0; i < MAX_MAP_LOCATIONS; i++) {
        LocationTab tab;
        tab.widget = new QWidget(ui->tabWidget_Locations);
        auto *layout = new QFormLayout(tab.widget);
        layout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);

        tab.comboBox_SecondaryTileset = new NoScrollComboBox(tab.widget);
        tab.comboBox_SecondaryTileset->setEditable(true);
        tab.comboBox_SecondaryTileset->setInsertPolicy(QComboBox::NoInsert);
        tab.comboBox_SecondaryTileset->setToolTip(tr("The secondary tileset used to render this location. The map canvas renders using the secondary tileset of the location tab shown here."));

        tab.comboBox_Song = new NoScrollComboBox(tab.widget);
        tab.comboBox_Song->setEditable(true);
        tab.comboBox_Song->setInsertPolicy(QComboBox::NoInsert);
        tab.comboBox_Song->setToolTip(tr("The default background music for this location."));

        tab.comboBox_Location = new NoScrollComboBox(tab.widget);
        tab.comboBox_Location->setEditable(true);
        tab.comboBox_Location->setInsertPolicy(QComboBox::NoInsert);
        tab.comboBox_Location->setToolTip(tr("The section of the region map which this location is grouped under. This also determines the name of the map that is displayed when the player enters it."));

        tab.lineEdit_LocationName = new QLineEdit(tab.widget);
        tab.lineEdit_LocationName->setToolTip(tr("The name that will be displayed in-game for this Location. This name will be shared with any other map that has the same Location."));

        tab.comboBox_Type = new NoScrollComboBox(tab.widget);
        tab.comboBox_Type->setEditable(true);
        tab.comboBox_Type->setInsertPolicy(QComboBox::NoInsert);
        tab.comboBox_Type->setToolTip(tr("The map type is a general attribute used for many different things, e.g. underground type maps have a special transition effect when the player enters/exits the map."));

        tab.comboBox_BattleScene = new NoScrollComboBox(tab.widget);
        tab.comboBox_BattleScene->setEditable(true);
        tab.comboBox_BattleScene->setInsertPolicy(QComboBox::NoInsert);
        tab.comboBox_BattleScene->setToolTip(tr("This field is used to help determine what graphics to use in the background of battles in this location."));

        tab.checkBox_ShowLocationName = new QCheckBox(tab.widget);
        tab.checkBox_ShowLocationName->setToolTip(tr("If checked, a map name popup will appear when the player enters this location. The name that appears on this popup depends on the Location field."));

        layout->addRow(tr("Secondary Tileset"), tab.comboBox_SecondaryTileset);
        layout->addRow(tr("Song"), tab.comboBox_Song);
        layout->addRow(tr("Location"), tab.comboBox_Location);
        layout->addRow(tr("Location Name"), tab.lineEdit_LocationName);
        layout->addRow(tr("Type"), tab.comboBox_Type);
        layout->addRow(tr("Battle Scene"), tab.comboBox_BattleScene);
        layout->addRow(tr("Show Location Name"), tab.checkBox_ShowLocationName);

        ui->tabWidget_Locations->addTab(tab.widget, tr("Location %1").arg(i + 1));
        m_tabs[i] = tab;

        // Connect field changes to the header, capturing the slot index.
        connect(tab.comboBox_SecondaryTileset, &QComboBox::currentTextChanged, this, [this, i](const QString &v){ onSecondaryTilesetChanged(i, v); });
        connect(tab.comboBox_Song,        &QComboBox::currentTextChanged, this, [this, i](const QString &v){ onSongChanged(i, v); });
        connect(tab.comboBox_Location,    &QComboBox::currentTextChanged, this, [this, i](const QString &v){ onLocationChanged(i, v); });
        connect(tab.comboBox_Type,        &QComboBox::currentTextChanged, this, [this, i](const QString &v){ onTypeChanged(i, v); });
        connect(tab.comboBox_BattleScene, &QComboBox::currentTextChanged, this, [this, i](const QString &v){ onBattleSceneChanged(i, v); });
        connect(tab.checkBox_ShowLocationName, &QCheckBox::toggled, this, [this, i](bool v){ onShowLocationNameChanged(i, v); });
        connect(tab.lineEdit_LocationName, &QLineEdit::textChanged, this, [this, i](const QString &v){ onLocationNameChanged(i, v); });
    }
}

void MapHeaderForm::setProject(Project * project, bool allowChanges) {
    clear();

    if (m_project) {
        m_project->disconnect(this);
    }
    m_project = project;
    m_allowProjectChanges = allowChanges;

    if (!m_project)
        return;

    populateComboBoxes();

    // Hide config-specific settings
    bool hasFlags = projectConfig.mapAllowFlagsEnabled;
    ui->checkBox_AllowRunning->setVisible(hasFlags);
    ui->checkBox_AllowBiking->setVisible(hasFlags);
    ui->checkBox_AllowEscaping->setVisible(hasFlags);
    ui->label_AllowRunning->setVisible(hasFlags);
    ui->label_AllowBiking->setVisible(hasFlags);
    ui->label_AllowEscaping->setVisible(hasFlags);

    bool floorNumEnabled = projectConfig.floorNumberEnabled;
    ui->spinBox_FloorNumber->setVisible(floorNumEnabled);
    ui->label_FloorNumber->setVisible(floorNumEnabled);

    // If the project changes any of the displayed data, update it accordingly.
    connect(m_project, &Project::mapSectionIdNamesChanged, this, &MapHeaderForm::setLocationItems);
    connect(m_project, &Project::mapSectionDisplayNameChanged, this, [this]() {
        for (int i = 0; i < MAX_MAP_LOCATIONS; i++)
            updateLocationName(i);
    });
}

void MapHeaderForm::populateComboBoxes() {
    const QSignalBlocker b_Weather(ui->comboBox_Weather);
    ui->comboBox_Weather->clear();
    ui->comboBox_Weather->addItems(m_project->weatherNames);

    const QSignalBlocker b_BiomeGroup(ui->comboBox_BiomeGroup);
    ui->comboBox_BiomeGroup->clear();
    ui->comboBox_BiomeGroup->addItems(m_project->biomeGroupNames);

    const QStringList locations = m_project->locationNames();
    for (auto &tab : m_tabs) {
        const QSignalBlocker b_Tileset(tab.comboBox_SecondaryTileset);
        tab.comboBox_SecondaryTileset->clear();
        tab.comboBox_SecondaryTileset->addItems(m_project->secondaryTilesetLabels);

        const QSignalBlocker b_Song(tab.comboBox_Song);
        tab.comboBox_Song->clear();
        tab.comboBox_Song->addItems(m_project->songNames);

        const QSignalBlocker b_Type(tab.comboBox_Type);
        tab.comboBox_Type->clear();
        tab.comboBox_Type->addItems(m_project->mapTypes);

        const QSignalBlocker b_BattleScene(tab.comboBox_BattleScene);
        tab.comboBox_BattleScene->clear();
        tab.comboBox_BattleScene->addItems(m_project->mapBattleScenes);

        const QSignalBlocker b_Location(tab.comboBox_Location);
        tab.comboBox_Location->clear();
        tab.comboBox_Location->addItems(locations);
    }
}

void MapHeaderForm::insertSecondaryTileset(int index, const QString &label) {
    for (auto &tab : m_tabs) {
        const QSignalBlocker b(tab.comboBox_SecondaryTileset);
        tab.comboBox_SecondaryTileset->insertItem(index, label);
    }
}

void MapHeaderForm::setLocationItems(const QStringList &locations) {
    for (auto &tab : m_tabs) {
        const QSignalBlocker b(tab.comboBox_Location);
        const QString before = tab.comboBox_Location->currentText();
        tab.comboBox_Location->clear();
        tab.comboBox_Location->addItems(locations);
        tab.comboBox_Location->setTextItem(before);
    }
}

// Assign a MapHeader that the form will keep in sync with the UI.
void MapHeaderForm::setHeader(MapHeader *header) {
    if (m_header == header)
        return;

    if (m_header) {
        m_header->disconnect(this);
    }
    m_header = header;

    if (!m_header) {
        clear();
        return;
    }

    // If the MapHeader is changed externally (e.g. with the scripting API) update the UI.
    connect(m_header, &MapHeader::locationDataChanged, this, &MapHeaderForm::refreshTab);
    connect(m_header, &MapHeader::numLocationsChanged, this, [this]() { updateTabStates(); });
    connect(m_header, &MapHeader::requiresFlashChanged, this, [this]() { if (m_header) refreshMapWideFields(*m_header); });
    connect(m_header, &MapHeader::weatherChanged,       this, [this]() { if (m_header) refreshMapWideFields(*m_header); });
    connect(m_header, &MapHeader::allowsRunningChanged, this, [this]() { if (m_header) refreshMapWideFields(*m_header); });
    connect(m_header, &MapHeader::allowsBikingChanged,  this, [this]() { if (m_header) refreshMapWideFields(*m_header); });
    connect(m_header, &MapHeader::allowsEscapingChanged,this, [this]() { if (m_header) refreshMapWideFields(*m_header); });
    connect(m_header, &MapHeader::floorNumberChanged,   this, [this]() { if (m_header) refreshMapWideFields(*m_header); });
    connect(m_header, &MapHeader::mapGridXChanged,      this, [this]() { if (m_header) refreshMapWideFields(*m_header); });
    connect(m_header, &MapHeader::mapGridYChanged,      this, [this]() { if (m_header) refreshMapWideFields(*m_header); });
    connect(m_header, &MapHeader::biomeGroupChanged,    this, [this]() { if (m_header) refreshMapWideFields(*m_header); });

    // Immediately update the UI to reflect the assigned MapHeader
    setHeaderData(*m_header);
}

void MapHeaderForm::clear() {
    m_header = nullptr;
    setHeaderData(MapHeader());
}

void MapHeaderForm::setHeaderData(const MapHeader &header) {
    m_updating = true;
    for (int i = 0; i < MAX_MAP_LOCATIONS; i++) {
        const MapHeader::LocationData data = header.locationData(i);
        setText(m_tabs[i].comboBox_SecondaryTileset, data.secondaryTileset);
        setText(m_tabs[i].comboBox_Song, data.song);
        setText(m_tabs[i].comboBox_Location, data.location);
        setText(m_tabs[i].comboBox_Type, data.type);
        setText(m_tabs[i].comboBox_BattleScene, data.battleScene);
        m_tabs[i].checkBox_ShowLocationName->setChecked(data.showsLocationName);
    }
    refreshMapWideFields(header);
    m_updating = false;

    // Tab availability follows the map's location count.
    {
        const QSignalBlocker b(ui->tabWidget_Locations);
        for (int i = 0; i < MAX_MAP_LOCATIONS; i++)
            ui->tabWidget_Locations->setTabEnabled(i, i < header.numLocations());
    }
    if (ui->tabWidget_Locations->currentIndex() >= header.numLocations())
        ui->tabWidget_Locations->setCurrentIndex(0);

    for (int i = 0; i < MAX_MAP_LOCATIONS; i++)
        updateLocationName(i);

    emitActiveTileset();
}

void MapHeaderForm::refreshMapWideFields(const MapHeader &header) {
    const bool prevUpdating = m_updating;
    m_updating = true;
    ui->checkBox_RequiresFlash->setChecked(header.requiresFlash());
    setText(ui->comboBox_Weather, header.weather());
    ui->checkBox_AllowRunning->setChecked(header.allowsRunning());
    ui->checkBox_AllowBiking->setChecked(header.allowsBiking());
    ui->checkBox_AllowEscaping->setChecked(header.allowsEscaping());
    ui->spinBox_FloorNumber->setValue(header.floorNumber());
    ui->spinBox_MapGridX->setValue(header.mapGridX());
    ui->spinBox_MapGridY->setValue(header.mapGridY());
    setText(ui->comboBox_BiomeGroup, header.biomeGroup());
    m_updating = prevUpdating;
}

// Refresh a single location tab from the tracked header (used for external changes).
void MapHeaderForm::refreshTab(int index) {
    if (!m_header || index < 0 || index >= MAX_MAP_LOCATIONS)
        return;
    const MapHeader::LocationData data = m_header->locationData(index);
    const bool prevUpdating = m_updating;
    m_updating = true;
    setText(m_tabs[index].comboBox_SecondaryTileset, data.secondaryTileset);
    setText(m_tabs[index].comboBox_Song, data.song);
    setText(m_tabs[index].comboBox_Location, data.location);
    setText(m_tabs[index].comboBox_Type, data.type);
    setText(m_tabs[index].comboBox_BattleScene, data.battleScene);
    m_tabs[index].checkBox_ShowLocationName->setChecked(data.showsLocationName);
    m_updating = prevUpdating;
    updateLocationName(index);
    if (index == activeLocation())
        emitActiveTileset();
}

void MapHeaderForm::updateTabStates() {
    if (!m_header)
        return;
    const QSignalBlocker b(ui->tabWidget_Locations);
    for (int i = 0; i < MAX_MAP_LOCATIONS; i++)
        ui->tabWidget_Locations->setTabEnabled(i, i < m_header->numLocations());
    if (ui->tabWidget_Locations->currentIndex() >= m_header->numLocations())
        ui->tabWidget_Locations->setCurrentIndex(0);
}

MapHeader MapHeaderForm::headerData() const {
    if (m_header)
        return *m_header;

    // Build header from UI
    MapHeader header;
    for (int i = 0; i < MAX_MAP_LOCATIONS; i++) {
        MapHeader::LocationData data;
        data.secondaryTileset = m_tabs[i].comboBox_SecondaryTileset->currentText();
        data.song = m_tabs[i].comboBox_Song->currentText();
        data.location = m_tabs[i].comboBox_Location->currentText();
        data.type = m_tabs[i].comboBox_Type->currentText();
        data.battleScene = m_tabs[i].comboBox_BattleScene->currentText();
        data.showsLocationName = m_tabs[i].checkBox_ShowLocationName->isChecked();
        header.setLocationData(i, data);
    }
    header.setRequiresFlash(ui->checkBox_RequiresFlash->isChecked());
    header.setWeather(ui->comboBox_Weather->currentText());
    header.setAllowsRunning(ui->checkBox_AllowRunning->isChecked());
    header.setAllowsBiking(ui->checkBox_AllowBiking->isChecked());
    header.setAllowsEscaping(ui->checkBox_AllowEscaping->isChecked());
    header.setFloorNumber(ui->spinBox_FloorNumber->value());
    header.setMapGridX(ui->spinBox_MapGridX->value());
    header.setMapGridY(ui->spinBox_MapGridY->value());
    header.setBiomeGroup(ui->comboBox_BiomeGroup->currentText());
    return header;
}

int MapHeaderForm::activeLocation() const {
    const int index = ui->tabWidget_Locations->currentIndex();
    return index < 0 ? 0 : index;
}

void MapHeaderForm::setLocation(const QString &location) {
    // Used externally to default a new map's region map section (slot 0).
    setText(m_tabs[0].comboBox_Location, location);
    if (m_header)
        m_header->setLocation(0, location);
    updateLocationName(0);
}

QString MapHeaderForm::location() const {
    return m_tabs[0].comboBox_Location->currentText();
}

QString MapHeaderForm::locationName() const {
    return m_tabs[0].lineEdit_LocationName->text();
}

void MapHeaderForm::updateLocationName(int index) {
    if (index < 0 || index >= MAX_MAP_LOCATIONS)
        return;
    const QString name = m_project ? m_project->getMapsecDisplayName(m_tabs[index].comboBox_Location->currentText()) : QString();
    setText(m_tabs[index].lineEdit_LocationName, name);
}

void MapHeaderForm::emitActiveTileset() {
    const int i = activeLocation();
    emit activeLocationTilesetChanged(m_tabs[i].comboBox_SecondaryTileset->currentText());
}

// If we always call setText / setTextItem the user's cursor may move to the end of the text while they're typing.
void MapHeaderForm::setText(NoScrollComboBox *combo, const QString &text) const {
    if (combo->currentText() != text)
        combo->setTextItem(text);
}
void MapHeaderForm::setText(QLineEdit *lineEdit, const QString &text) const {
    if (lineEdit->text() != text)
        lineEdit->setText(text);
}

// --- Per-location field handlers ---
void MapHeaderForm::onSecondaryTilesetChanged(int index, const QString &tileset) {
    if (m_updating) return;
    if (m_header) m_header->setSecondaryTileset(index, tileset);
    if (index == activeLocation())
        emit activeLocationTilesetChanged(tileset);
}
void MapHeaderForm::onSongChanged(int index, const QString &song) {
    if (m_updating) return;
    if (m_header) m_header->setSong(index, song);
}
void MapHeaderForm::onLocationChanged(int index, const QString &location) {
    if (m_updating) return;
    if (m_header) m_header->setLocation(index, location);
    updateLocationName(index);
}
void MapHeaderForm::onTypeChanged(int index, const QString &type) {
    if (m_updating) return;
    if (m_header) m_header->setType(index, type);
}
void MapHeaderForm::onBattleSceneChanged(int index, const QString &battleScene) {
    if (m_updating) return;
    if (m_header) m_header->setBattleScene(index, battleScene);
}
void MapHeaderForm::onShowLocationNameChanged(int index, bool enabled) {
    if (m_updating) return;
    if (m_header) m_header->setShowsLocationName(index, enabled);
}
void MapHeaderForm::onLocationNameChanged(int index, const QString &locationName) {
    if (m_updating) return;
    if (m_project && m_allowProjectChanges) {
        // The location name is part of the project, not the map header.
        m_project->setMapsecDisplayName(m_tabs[index].comboBox_Location->currentText(), locationName);
    }
}
void MapHeaderForm::onCurrentTabChanged(int index) {
    Q_UNUSED(index)
    // The map should now render using the newly-selected location's secondary tileset.
    emitActiveTileset();
}

// --- Map-wide field handlers ---
void MapHeaderForm::onWeatherChanged(const QString &weather) {       if (!m_updating && m_header) m_header->setWeather(weather); }
void MapHeaderForm::onRequiresFlashChanged(bool enabled) {           if (!m_updating && m_header) m_header->setRequiresFlash(enabled); }
void MapHeaderForm::onAllowRunningChanged(bool enabled) {            if (!m_updating && m_header) m_header->setAllowsRunning(enabled); }
void MapHeaderForm::onAllowBikingChanged(bool enabled) {             if (!m_updating && m_header) m_header->setAllowsBiking(enabled); }
void MapHeaderForm::onAllowEscapingChanged(bool enabled) {           if (!m_updating && m_header) m_header->setAllowsEscaping(enabled); }
void MapHeaderForm::onFloorNumberChanged(int offset) {               if (!m_updating && m_header) m_header->setFloorNumber(offset); }
void MapHeaderForm::onMapGridXChanged(int pos) {                     if (!m_updating && m_header) m_header->setMapGridX(pos); }
void MapHeaderForm::onMapGridYChanged(int pos) {                     if (!m_updating && m_header) m_header->setMapGridY(pos); }
void MapHeaderForm::onBiomeGroupChanged(const QString &biomeGroup) { if (!m_updating && m_header) m_header->setBiomeGroup(biomeGroup); }
