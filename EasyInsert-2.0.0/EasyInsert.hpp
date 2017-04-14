#ifndef EASYINSERT_HPP
#define EASYINSERT_HPP

#include <RobWorkStudio.hpp>
#include <RobWorkStudioConfig.hpp>
#include <rws/RobWorkStudioPlugin.hpp>
#include "ui_EasyInsert.h"
#include "listView.hpp"
#include "dialog.hpp"
#include "loader.hpp"
#include "creator.hpp"
#include <rw/common/Ptr.hpp>
#include <rw/math/Transform3D.hpp>

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QtCore>
#include <QScrollArea>
#include <QToolBar>
#include <QToolButton>
#include <QPushButton>
#include <QMenu>
#include <QAction>
#include <QFileSystemModel>
#include <QTabWidget>
#include <QListView>
#include <QFileSystemModel>


#include <rw/kinematics/FixedFrame.hpp>
#include <rw/kinematics/MovableFrame.hpp>
#include <rw/models/RevoluteJoint.hpp>
#include <rw/models/PrismaticJoint.hpp>

namespace rw { namespace models { class Device; } }

class EasyInsert: public rws::RobWorkStudioPlugin, private Ui::EasyInsert
{

Q_OBJECT
Q_INTERFACES( rws::RobWorkStudioPlugin )
#if RWS_USE_QT5
Q_PLUGIN_METADATA(IID "dk.sdu.mip.Robwork.RobWorkStudioPlugin/0.1" FILE "EasyInsert.json")
#endif

public:
    EasyInsert();
	virtual ~EasyInsert();

    virtual void open(rw::models::WorkCell* workcell);

    virtual void close();

    virtual void initialize();


private:
    void setupSettings();
    QToolBar* createToolBar();
    QWidget* createDevTab();
    QWidget* createGeoTab();
    void updateDeviceTab();

    QToolBar* _toolBar;
    QWidget* _devTab;
    QWidget* _geoTab;
    dialog* _settingsDialog;
    dialog* _loadDialog;
    dialog* _geometriDialog;

protected:

    listView *view;
    QFileSystemModel *dirmodel;
    rw::common::PropertyMap _propMap;
    rw::common::PropertyMap *_settingsMap;


private slots:
    void stateChangedListener(const rw::kinematics::State& state);
    void loadDevice();
    void settings();
    void cube();
    void plane();
    void sphere();
    void cone();
    void cylinder();
    void tube();
    void fixedFrame();
    void movableFrame();


};

#endif
