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
	//rw::models::WorkCell::Ptr input = rw::loaders::XMLRWLoader::load("/home/mathias/Desktop/Git/Bachelor-F17/testPlugin/wc.xml");
	//log().info() << "Loaded file " << input->getName() << "\n\n";

	// Pull out and list information of the input WorkCell
    //log().info() << "Info for loaded WorkCell\n";
    //printInfo(input);

	// List information of current WorkCell
    rw::models::WorkCell::Ptr workcell = getRobWorkStudio()->getWorkCell(); // Get WorkCell from scene
    log().info() << "Info for scene WorkCell\n";
    printInfo(workcell);

    ///___________________________________________________________________________________________________________________________///

	// Add information to current WorkCell
    // Parse in the information to add
    boost::shared_ptr<DummyWorkcell> dwc = rw::loaders::XMLRWParser::parseWorkcell("/home/mathias/Desktop/Git/Bachelor-F17/testPlugin/wc.xml");

    // Create and add frames to scene WorkCell
    rw::kinematics::Frame* tmpFrame = NULL;
    for (int i = 0; i < dwc->_framelist.size(); i++){
        tmpFrame = createFrame(dwc->_framelist[i], workcell);
        //workcell->addFrame(tmpFrame);
        //log().info() << dwc->_framelist[i].getName() << " " << dwc->_framelist[i]._type << "\n";
    }

    log().info() << "Info for updated scene WorkCell\n";
    printInfo(workcell);







    /* Test of adding a frame WORKING!
    rw::kinematics::Frame *testFrame = NULL;
    rw::math::Transform3D<> testTransform;
    testFrame = new rw::kinematics::FixedFrame("testFrame", testTransform);
    log().info() << "Frame id: " << testFrame->getID() << "\n";

    workcell->addFrame(testFrame);

    log().info() << "Info for scene WorkCell\n";
    printInfo(workcell); */

}

void TestPlugin::spinBoxChanged() {
	QObject *obj = sender();
	if(obj == spinBox) {
		lcdNumber->display(spinBox->value());
	}
}

void TestPlugin::stateChangedListener(const State& state) {
}

// Extra functions here
rw::kinematics::Frame* TestPlugin::createFrame(DummyFrame dFrame, rw::models::WorkCell::Ptr wc) {
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


void TestPlugin::printInfo(rw::models::WorkCell::Ptr wc) {
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


#if !RWS_USE_QT5
#include <QtCore/qplugin.h>
Q_EXPORT_PLUGIN(TestPlugin);
#endif
