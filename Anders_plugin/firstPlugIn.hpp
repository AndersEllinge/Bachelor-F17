#ifndef FIRSTPLUGIN_HPP
#define FIRSTPLUGIN_HPP

#include <RobWorkStudioConfig.hpp>
#include <rws/RobWorkStudioPlugin.hpp>

#include "ui_firstPlugIn.h"
#include "listView.hpp"
#include "dialog.hpp"
#include <rw/common/Ptr.hpp>
#include <QtCore>
#include <QtGui>
#include <QToolBar>
#include <QToolButton>
#include <QAction>
#include <QMenu>
#include <QFileSystemModel>
#include <QListView>
#include <QScrollArea>
#include <QPushButton>
#include <rw/loaders/rwxml/XMLParserUtil.hpp>
#include <rw/loaders/rwxml/XMLRWParser.hpp>
#include <rw/loaders/rwxml/XMLRWLoader.hpp>

#include <rw/kinematics/FixedFrame.hpp>
#include <rw/kinematics/MovableFrame.hpp>
#include <rw/models/RevoluteJoint.hpp>
#include <rw/models/PrismaticJoint.hpp>

namespace rw { namespace models { class Device; } }

//class listView;

class firstPlugIn: public rws::RobWorkStudioPlugin, private Ui::firstPlugIn
{

Q_OBJECT
Q_INTERFACES( rws::RobWorkStudioPlugin )
#if RWS_USE_QT5
Q_PLUGIN_METADATA(IID "dk.sdu.mip.Robwork.RobWorkStudioPlugin/0.1" FILE "firstPlugIn.json")
#endif
public:
    firstPlugIn();
	virtual ~firstPlugIn();

    virtual void open(rw::models::WorkCell* workcell);

    virtual void close();

    virtual void initialize();

    void mousePressEvent(QMouseEvent *event);

    void printLog(std::string text);

protected:
    //! Close Event inherited from QT
	void closeEvent( QCloseEvent * e );

    rw::common::PropertyMap _propMap;
    rw::common::PropertyMap *_settingsMap;

private:
    void setupSettings();


    rw::models::WorkCell* _workcell;

    QString sPath = "/";
    listView* _listView;
    QFileSystemModel *dirmodel;
    //QItemSelectionModel* _selectionModel;
    QWidget* dockWidgetContent;
    QVBoxLayout* verticalLayout;
    dialog* _loadDialog = nullptr;
    dialog* _settingsDialog = nullptr;

    QPushButton* devBtn;    //device sort button
    QPushButton* geoBtn;    //geometry sort button
    QPushButton* fraBtn;    //frame sort button
    QPushButton* loadBtn;   //load button for things to be loaded into the workcell

    QToolBar* _toolBar;
    QToolButton* _settingsButton;
    QToolButton* _undoButton;
    QMenu* _settingsMenu;


private slots:
    void tabDevices();
    void tabGeometries();
    void tabFrames();
    void load();
    void settings();
    void printInfo(rw::models::WorkCell::Ptr wc);
    void stateChangedListener(const rw::kinematics::State& state);
};

#endif
