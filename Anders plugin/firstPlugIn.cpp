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

	connect(fileNameBtn, SIGNAL(pressed()), this, SLOT(btnPressed()));
	connect(workcellBtn, SIGNAL(pressed()), this, SLOT(btnPressed()));
	connect(treeView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(treeViewPressed(const QModelIndex &)));
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

void firstPlugIn::treeViewPressed(const QModelIndex & index)
{
	fName = dirmodel->fileInfo(index).absoluteFilePath();		// keep the information about the file
}

void firstPlugIn::btnPressed()
{
	QObject *obj = sender();
	if(obj==workcellBtn) {
	log().info() << _workcell->getFilename() << "\n";
	}
	else if(obj==fileNameBtn) {
	log().info() << fName.toStdString() << "\n";
	getRobWorkStudio()->openFile(fName.toStdString()); // open whatever robwork project file with rws
	}
}

void firstPlugIn::stateChangedListener(const State& state)
{
}

#if !RWS_USE_QT5
#include <QtCore/qplugin.h>
Q_EXPORT_PLUGIN(firstPlugIn);
#endif
