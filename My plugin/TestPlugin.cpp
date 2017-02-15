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
	log().info() << "Started loading\n";

	// Load in XML file and and construct WorkCell
	rw::models::WorkCell::Ptr input = rw::loaders::XMLRWLoader::load("/home/mathias/Desktop/My plugin/wc.xml");
	log().info() << "Loaded file " << input->getFilename() << "\n";

	// Pull out information of the input WorkCell
	//std::vector<rw::models::Object::Ptr> objs;
	//objs = input->getObjects();

	// Add information to WorkCell under use
	//rw::models::WorkCell::Ptr workcell = getRobWorkStudio()->getView()->getWorkCellScene()->getWorkCell();
	
	//for (int i = 0; i < objs.size(); i++){
	//	workcell->add(objs[i]);
	//}
	
	
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



