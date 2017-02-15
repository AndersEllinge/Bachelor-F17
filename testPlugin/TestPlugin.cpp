#include "TestPlugin.hpp"

#include <RobWorkStudio.hpp>

#include <boost/bind.hpp>

using rw::kinematics::State;
using rw::models::WorkCell;
using rws::RobWorkStudioPlugin;

TestPlugin::TestPlugin():
    RobWorkStudioPlugin("TestPlugin", QIcon(":/pa_icon.png")) 
{
    setupUi(this);

    // now connect stuff from the ui component
    connect(helloWorldBtn, SIGNAL(pressed()), this, SLOT(helloWorldBtnPressed()) );
    //connect(_btn1    ,SIGNAL(pressed()), this, SLOT(btnPressed()) );
    connect(spinBox, SIGNAL(valueChanged(int)), this, SLOT(spinBoxChanged()) );

}

TestPlugin::~TestPlugin() {
}

void TestPlugin::initialize() {
    getRobWorkStudio()->stateChangedEvent().add(boost::bind(&TestPlugin::stateChangedListener, this, _1), this);
}

void TestPlugin::open(WorkCell* workcell) {
	_workcell = workcell;
}

void TestPlugin::close() {
}

void TestPlugin::helloWorldBtnPressed() {
	log().info() << "Started loading\n\n";

	// Load in XML file and and construct WorkCell
	rw::models::WorkCell::Ptr input = rw::loaders::XMLRWLoader::load("/home/mathias/Desktop/My plugin/wc.xml");
	log().info() << "Loaded file " << input->getFilename() << "\n\n";

	// Pull out information of the input WorkCell
	std::vector<rw::kinematics::Frame*> frms = input->getFrames();
	log().info() << "Found " << frms.size() << " frames\n"; // List number of frames
	log().info() << "Frames:\n"; // List frames
	for (int i = 0; i < frms.size(); i++){
		log().info() << frms[i]->getName() << "\n";
	}
	log().info() << "\n";

	std::vector<rw::models::Device::Ptr> dvs = input->getDevices();
	log().info() << "Found " << dvs.size() << " devices\n"; // List number of devices
	log().info() << "Devices:\n"; // List devices
	for (int i = 0; i < dvs.size(); i++){
		log().info() << dvs[i]->getName() << "\n";
	}
	log().info() << "\n";
	

	// List information of current WorkCell
	rw::models::WorkCell::Ptr workcell = getRobWorkStudio()->getView()->getWorkCellScene()->getWorkCell(); // Get WorkCell from scene
	std::vector<rw::models::Device::Ptr> wcDvs = workcell->getDevices();
	log().info() << "Found " << wcDvs.size() << " devices in the scene WorkCell\n"; // List number of devices
	log().info() << "Devices in the scene WorkCell:\n";
	for (int i = 0; i < wcDvs.size(); i++){
		log().info() << wcDvs[i]->getName() << "\n";
	}
	log().info() << "\n";

	// Add information to current WorkCell
	rw::models::Device::Ptr tmp = dvs[0];
	tmp->unregister();
	//tmp->registerIn(workcell->getStateStructure());
	workcell->add(tmp);

	// List information of curretn WorkCell again
	wcDvs.clear();
	wcDvs = workcell->getDevices();
	log().info() << "Found " << wcDvs.size() << " devices in the scene WorkCell\n"; // List number of devices
	log().info() << "Devices in the scene WorkCell:\n";
	for (int i = 0; i < wcDvs.size(); i++){
		log().info() << wcDvs[i]->getName() << "\n";
	}
	log().info() << "\n";
}

void TestPlugin::spinBoxChanged() {
	QObject *obj = sender();
	if(obj == spinBox) {
		lcdNumber->display(spinBox->value());
	}
}

void TestPlugin::stateChangedListener(const State& state) {
}

#if !RWS_USE_QT5
#include <QtCore/qplugin.h>
Q_EXPORT_PLUGIN(TestPlugin);
#endif



