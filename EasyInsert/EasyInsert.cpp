#include "EasyInsert.hpp"
#include <boost/bind.hpp>

using rw::kinematics::State;
using rw::models::WorkCell;
using rws::RobWorkStudioPlugin;


EasyInsert::EasyInsert():
    RobWorkStudioPlugin("EasyInsert", QIcon(":/pa_icon.png"))
{
    setupUi(this);

    dirmodel = new QFileSystemModel(this);
	treeView->setModel(dirmodel);
	treeView->setRootIndex(dirmodel->setRootPath(sPath));

}

EasyInsert::~EasyInsert()
{
}

void EasyInsert::initialize()
{
    getRobWorkStudio()->stateChangedEvent().add(boost::bind(&EasyInsert::stateChangedListener, this, _1), this);
}

void EasyInsert::open(WorkCell* workcell)
{
	_workcell = workcell;
}

void EasyInsert::close()
{
}

void EasyInsert::stateChangedListener(const State& state)
{
}

void EasyInsert::loadBtnPressed(const QModelIndex & index)
{
    fName = dirmodel->fileInfo(index).absoluteFilePath();		// keep the information about the file
}

#if !RWS_USE_QT5
#include <QtCore/qplugin.h>
Q_EXPORT_PLUGIN(EasyInsert);
#endif
