#ifndef EASYINSERT_HPP
#define EASYINSERT_HPP

#include <RobWorkStudio.hpp>
#include <RobWorkStudioConfig.hpp>
#include <rws/RobWorkStudioPlugin.hpp>
#include "ui_EasyInsert.h"
#include <rw/common/Ptr.hpp>

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
    QFileSystemModel *dirmodel;
    QString fName;
    QString sPath = "/home/osboxes/Desktop/Git/Bachelor-F17";
    rw::models::WorkCell* _workcell;

private slots:
    void stateChangedListener(const rw::kinematics::State& state);
    void loadBtnPressed(const QModelIndex & index);

};

#endif
