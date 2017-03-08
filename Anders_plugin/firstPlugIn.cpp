#include "firstPlugIn.hpp"
#include "listView.hpp"
#include <RobWorkStudio.hpp>
#include <boost/bind.hpp>



using rw::kinematics::State;
using rw::models::WorkCell;
using rws::RobWorkStudioPlugin;

firstPlugIn::firstPlugIn():
	RobWorkStudioPlugin("firstPlugInUI", QIcon(":/pa_icon.png"))
{
	//setupUi(this);
	//QString sPath = "/home/osboxes/Desktop/Git/Bachelor-F17/Anders_plugin";

    QDockWidget *window = new QDockWidget;

/*
	dirmodel = new QFileSystemModel(this);
	QListView *fileList = new QListView(this);
	fileList->setModel(dirmodel);
    fileList->setRootIndex(dirmodel->setRootPath(sPath));
	fileList->setMinimumSize(200,400);
	fileList->move(10,20);
*/

	QHBoxLayout *layout = new QHBoxLayout;
	fileList = new listView(this);
	layout->addWidget(fileList);
	window->setLayout(layout);



	/*
	dirmodel = new QFileSystemModel(this);
	treeView->setModel(dirmodel);
	treeView->setRootIndex(dirmodel->setRootPath(sPath));
	treeView->setDragEnabled(true);
	*/

	//connect(fileNameBtn, SIGNAL(pressed()), this, SLOT(btnPressed()));
	//connect(workcellBtn, SIGNAL(pressed()), this, SLOT(btnPressed()));
	//connect(loadObjectFrame, SIGNAL(pressed()), this, SLOT(btnPressed()));
	//connect(treeView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(treeViewPressed(const QModelIndex &)));
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
	rw::models::WorkCell::Ptr workcell = getRobWorkStudio()->getWorkCell();
	log().info() << "Info for scene WorkCell\n";
	printInfo(workcell);
	}

	else if(obj==fileNameBtn) {
	log().info() << fName.toStdString() << "\n";
	getRobWorkStudio()->openFile(fName.toStdString()); // open whatever robwork project file with rws
	}

	else if(obj==loadObjectFrame) {

	rw::models::WorkCell::Ptr workcell = getRobWorkStudio()->getWorkCell(); // Get WorkCell from scene

	boost::shared_ptr<DummyWorkcell> dwc = rw::loaders::XMLRWParser::parseWorkcell(fName.toStdString());

		for (int i = 0; i < dwc->_framelist.size(); i++){
			createFrame(dwc->_framelist[i], workcell);
		}
	}
}

void firstPlugIn::mousePressEvent(QMouseEvent *event)
{
	log().info() << "clikie clickie" << "\n";
}


rw::kinematics::Frame* firstPlugIn::createFrame(DummyFrame dFrame, rw::models::WorkCell::Ptr wc) {
    rw::kinematics::Frame* frame = NULL;
    rw::kinematics::State state;
    bool stateChanged = 0;

    if (dFrame._type == "Fixed"){
        frame = new rw::kinematics::FixedFrame(dFrame.getName(), dFrame._transform);
    }

    else if (dFrame._type == "Movable") {
        rw::kinematics::MovableFrame* movFrame = new rw::kinematics::MovableFrame(dFrame.getName());
        frame = movFrame;
        wc->addFrame(frame);
        state = getRobWorkStudio()->getWorkCell()->getDefaultState();
        movFrame->setTransform(dFrame._transform, state);
        stateChanged = 1;
    }

    else if (dFrame._type == "Prismatic") {
        frame = new rw::models::PrismaticJoint(dFrame.getName(), dFrame._transform);
    }

    else if (dFrame._type == "Revolute") {
        frame = new rw::models::RevoluteJoint(dFrame.getName(), dFrame._transform);
    }

    else if (dFrame._type == "EndEffector") {
        frame = new rw::kinematics::FixedFrame(dFrame.getName(), dFrame._transform);
    }

    else{
        RW_THROW("FRAME TYPE ERROR");
    }

    if (stateChanged){
        state = getRobWorkStudio()->getWorkCell()->getStateStructure()->upgradeState(state); // Update workcell to new state
        getRobWorkStudio()->setState(state); // Update RWS to the new state
    }

    return frame;
}

void firstPlugIn::printInfo(rw::models::WorkCell::Ptr wc) {
    log().info() << "Found " << wc->getFrames().size() << " frames:\n"; // List frames
    for (int i = 0; i < wc->getFrames().size(); i++) {
        log().info() << wc->getFrames()[i]->getName() << "\n";
    }
    log().info() << "\n";

    log().info() << "Found " << wc->getDevices().size() << " devices:\n"; // List devices
    for (int i = 0; i < wc->getDevices().size(); i++) {
        log().info() << wc->getDevices()[i]->getName() << "\n";
    }
    log().info() << "\n\n";
}

void firstPlugIn::stateChangedListener(const State& state)
{
}

#if !RWS_USE_QT5
#include <QtCore/qplugin.h>
Q_EXPORT_PLUGIN(firstPlugIn);
#endif
