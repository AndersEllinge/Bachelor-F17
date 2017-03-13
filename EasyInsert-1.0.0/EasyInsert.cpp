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

    connect(treeView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(treeViewPressed(const QModelIndex &)));
    connect(loadBtn, SIGNAL(pressed()), this, SLOT(btnPressed()));
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
}

void EasyInsert::close()
{
}

void EasyInsert::stateChangedListener(const State& state)
{
}

void EasyInsert::treeViewPressed(const QModelIndex & index)
{
    fName = dirmodel->fileInfo(index).absoluteFilePath();		// keep the information about the file
}

void EasyInsert::btnPressed()
{
    QObject *obj = sender();
	if(obj==loadBtn) {
        //Get workcell from scene
        rw::models::WorkCell::Ptr wc = getRobWorkStudio()->getWorkCell();

        //create dummy for rws and swap it in
        rw::models::WorkCell::Ptr dummy = rw::common::ownedPtr(new rw::models::WorkCell("dummy"));
        getRobWorkStudio()->setWorkCell(dummy);

        //edit the workcell
        ei::loader::add(fName.toStdString(),wc);

        //swap workcell back in
        getRobWorkStudio()->setWorkCell(wc);
	}
}

#if !RWS_USE_QT5
#include <QtCore/qplugin.h>
Q_EXPORT_PLUGIN(EasyInsert);
#endif
