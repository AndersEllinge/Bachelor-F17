#ifndef EASYINSERT_HPP
#define EASYINSERT_HPP

#include <RobWorkStudio.hpp>
#include <RobWorkStudioConfig.hpp>
#include <rws/RobWorkStudioPlugin.hpp>
//#include "ui_EasyInsert.h"
#include "dialog.hpp"
#include "loader.hpp"
#include "creator.hpp"
#include <rw/common/Ptr.hpp>
#include <rw/math/Transform3D.hpp>

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTreeWidget>
#include <QListWidget>
#include <QListWidgetItem>
#include <QTreeWidgetItem>
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
#include <rw/kinematics/Kinematics.hpp>
#include <rw/graphics/DrawableNode.hpp>
#include <rw/models/RevoluteJoint.hpp>
#include <rw/models/PrismaticJoint.hpp>

#include <rw/models/Device.hpp>
#include <rw/models/ParallelDevice.hpp>
#include <rw/models/SerialDevice.hpp>
#include <rw/models/MobileDevice.hpp>
#include <rw/models/TreeDevice.hpp>

namespace rw { namespace models { class Device; } }

class EasyInsert: public rws::RobWorkStudioPlugin
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
    QWidget* createDeleteTab();
    void showFrameStructure();
    void setupFrame(rw::kinematics::Frame& frame, QTreeWidgetItem* parentItem);
    void setupDrawables(rw::kinematics::Frame* frame, QTreeWidgetItem* parent);
    void clearListContent();
    void update();
    void removeAllObjects(rw::kinematics::Frame* frame, rw::models::WorkCell::Ptr wc);

    rw::kinematics::State _state;
    rw::models::WorkCell* _workcell;

    QListWidget* _deviceWidget;
    QListWidget* _frameWidget;
    QWidget* _frameDelBtns;
    QListWidget* _objectWidget;

    QToolBar* _toolBar;
    QWidget* _devTab;
    QWidget* _geoTab;
    QWidget* _deleteTab;

    std::vector<rw::kinematics::Frame*> _devFrameList;
    std::vector<rw::kinematics::Frame*> _frames;

    QListView *view;
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
    void deleteSingleFrame();
    void deleteFrame();
    void deleteChildren(rw::kinematics::Frame* frame, rw::models::WorkCell::Ptr wc);
    void deleteDev();
    void deleteObj();

};

#endif
