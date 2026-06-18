#ifndef MAPHEADERFORM_H
#define MAPHEADERFORM_H

/*
    This is the UI class used to edit the fields in a map's header.
    It's intended to be used anywhere the UI needs to present an editor for a map's header,
    e.g. for the current map in the main editor or in the new map dialog.

    The per-location properties (secondary tileset, music, region map section, map type,
    battle scene, show map name) are presented in a tab for each of the map's location
    slots (see MAX_MAP_LOCATIONS / struct MapHeaderLocationData).
*/

#include <QWidget>
#include <QPointer>
#include <QLineEdit>
#include <QCheckBox>
#include <array>

#include "mapheader.h"
#include "project.h"
#include "noscrollcombobox.h"

namespace Ui {
class MapHeaderForm;
}

class MapHeaderForm : public QWidget
{
    Q_OBJECT

public:
    explicit MapHeaderForm(QWidget *parent = nullptr);
    ~MapHeaderForm();

    void setProject(Project * project, bool allowProjectChanges = true);
    void clear();

    // Adds a newly-created secondary tileset to each location's tileset selector, keeping
    // the current selections intact. Called when a tileset is created mid-session.
    void insertSecondaryTileset(int index, const QString &label);

    void setHeader(MapHeader *header);
    void setHeaderData(const MapHeader &header);
    MapHeader headerData() const;

    // Sets the region map section of the default location (slot 0).
    // Used by the new map dialog to default a map to the folder it's created under.
    void setLocation(const QString &location);

    // The region map section and its display name for the default location (slot 0).
    QString location() const;
    QString locationName() const;

    // The location slot whose tab is currently shown. Its secondary tileset is what
    // the map canvas renders with.
    int activeLocation() const;

signals:
    // Emitted when the secondary tileset that should be rendered changes, i.e. when the
    // active location tab changes or the active tab's secondary tileset is edited.
    void activeLocationTilesetChanged(const QString &tileset);

private:
    // The set of widgets that make up a single location tab.
    struct LocationTab {
        QWidget *widget = nullptr;
        NoScrollComboBox *comboBox_SecondaryTileset = nullptr;
        NoScrollComboBox *comboBox_Song = nullptr;
        NoScrollComboBox *comboBox_Location = nullptr;
        QLineEdit *lineEdit_LocationName = nullptr;
        NoScrollComboBox *comboBox_Type = nullptr;
        NoScrollComboBox *comboBox_BattleScene = nullptr;
        QCheckBox *checkBox_ShowLocationName = nullptr;
    };

    Ui::MapHeaderForm *ui;
    QPointer<MapHeader> m_header = nullptr;
    QPointer<Project> m_project = nullptr;
    bool m_allowProjectChanges = true;
    // True while we're pushing data into the widgets, so the widgets' change handlers
    // can ignore the resulting signals instead of writing back to the header.
    bool m_updating = false;
    std::array<LocationTab, MAX_MAP_LOCATIONS> m_tabs;

    void createLocationTabs();
    void populateComboBoxes();
    void refreshTab(int index);
    void refreshMapWideFields(const MapHeader &header);
    void updateTabStates();
    void updateLocationName(int index);
    void emitActiveTileset();

    void setText(NoScrollComboBox *combo, const QString &text) const;
    void setText(QLineEdit *lineEdit, const QString &text) const;
    void setLocationItems(const QStringList &locations);

    // Per-location field handlers (index identifies which location slot).
    void onSecondaryTilesetChanged(int index, const QString &tileset);
    void onSongChanged(int index, const QString &song);
    void onLocationChanged(int index, const QString &location);
    void onLocationNameChanged(int index, const QString &locationName);
    void onTypeChanged(int index, const QString &type);
    void onBattleSceneChanged(int index, const QString &battleScene);
    void onShowLocationNameChanged(int index, bool enabled);
    void onCurrentTabChanged(int index);

    // Map-wide field handlers.
    void onWeatherChanged(const QString &weather);
    void onRequiresFlashChanged(bool enabled);
    void onAllowRunningChanged(bool enabled);
    void onAllowBikingChanged(bool enabled);
    void onAllowEscapingChanged(bool enabled);
    void onFloorNumberChanged(int offset);
    void onMapGridXChanged(int pos);
    void onMapGridYChanged(int pos);
    void onBiomeGroupChanged(const QString &biomeGroup);
};

#endif // MAPHEADERFORM_H
