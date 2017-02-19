#include "firstPlugIn.hpp"
#include <RobWorkStudio.hpp>
#include <boost/bind.hpp>
#include </usr/local/RobWork/src/rw/models/WorkCell.hpp>
#include <rw/loaders/WorkCellLoader.hpp>


using rw::kinematics::State;
using rw::models::WorkCell;
using rws::RobWorkStudioPlugin;

firstPlugIn::firstPlugIn():
	RobWorkStudioPlugin("firstPlugInUI", QIcon(":/pa_icon.png"))
{
	setupUi(this);

	//Connect buttons and stuff

	QString sPath = "/usr/local/RobWork/example/ModelData/XMLDevices";

	dirmodel = new QFileSystemModel(this);
	treeView->setModel(dirmodel);
	treeView->setRootIndex(dirmodel->setRootPath(sPath));

	connect(pushBtn1, SIGNAL(pressed()), this, SLOT(btnPressed()));
	connect(workcellBtn, SIGNAL(pressed()), this, SLOT(btnPressed()));
}

firstPlugIn::~firstPlugIn()
{
}

void firstPlugIn::initialize() {
    	getRobWorkStudio()->stateChangedEvent().add(boost::bind(&firstPlugIn::stateChangedListener, this, _1), this);
}

void firstPlugIn::open(WorkCell* workcell)
{
	_workcell = workcell;
}

void firstPlugIn::close()
{
}

void firstPlugIn::btnPressed()
{
	QObject *obj = sender();
	if(obj==pushBtn1) {
        log().info() << "Button 1 pressed\n";
	}
	else if(obj==workcellBtn) {
	log().info() << _workcell->getFilename() << "\n";
	}
}

void firstPlugIn::stateChangedListener(const State& state)
{
}

#if !RWS_USE_QT5
#include <QtCore/qplugin.h>
Q_EXPORT_PLUGIN(firstPlugIn);
#endif
