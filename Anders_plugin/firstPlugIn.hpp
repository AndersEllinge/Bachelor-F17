#ifndef FIRSTPLUGIN_HPP
#define FIRSTPLUGIN_HPP

#include <RobWorkStudio.hpp>
#include <RobWorkStudioConfig.hpp>
#include <rws/RobWorkStudioPlugin.hpp>
#include "ui_firstPlugIn.h"
#include <rw/common/Ptr.hpp>
#include <QtCore>
#include <QtGui>
#include <QFileSystemModel>
#include <QListView>
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

QFileSystemModel *dirmodel;
//listView *fileList;
//QFileSystemModel *filemodel;

QString fName;

rw::models::WorkCell* _workcell;


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

private:
    rw::kinematics::Frame* createFrame(DummyFrame dFrame, rw::models::WorkCell::Ptr wc);



private slots:
    void treeViewPressed(const QModelIndex & index);
    void btnPressed();
    void printInfo(rw::models::WorkCell::Ptr wc);
    void stateChangedListener(const rw::kinematics::State& state);
};

#endif
