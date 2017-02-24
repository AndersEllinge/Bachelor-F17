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
    for (int i = 0; i < dwc->_framelist.size(); i++){
        createFrame(dwc->_framelist[i], workcell);
        log().info() << dwc->_framelist[i].getName() << " " << dwc->_framelist[i]._type << "\n";
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
void TestPlugin::createFrame(DummyFrame dFrame, rw::models::WorkCell::Ptr wc) {

    if (dFrame._isDepend) { // Create a dependent frame
        // Setup and check before creatinf dependent frames
        rw::kinematics::Frame* depFrame = wc->findFrame(dFrame.getDependsOn());
        if (depFrame == NULL)
            RW_WARN("The frame " << dFrame.getName() << " depends on the unknown frame " << dFrame.getDependsOn());

        rw::models::Joint* depJoint = dynamic_cast<rw::models::Joint*>(depFrame);
        if (depJoint == NULL)
            RW_THROW("The frame " << dFrame.getName() << " depends on the frame " << dFrame.getDependsOn() << " which is not a joint");

        // Create frames
        if (dFrame._type == "Revolute") {  // Create dependent Revolute frame/joint
            rw::models::DependentRevoluteJoint* joint = new rw::models::DependentRevoluteJoint(dFrame.getName(), dFrame._transform, depJoint, dFrame._gain, dFrame._offset);
            wc->addFrame(joint);
        }

        else if (dFrame._type == "Prismatic") { // Create dependent Prismatic frame/joint
            rw::models::DependentPrismaticJoint* joint = new rw::models::DependentPrismaticJoint(dFrame.getName(), dFrame._transform, depJoint, dFrame._gain, dFrame._offset);
            wc->addFrame(joint);
        }
    }

    else if (dFrame._type == "Fixed") { // Create a Fixed frame
        rw::kinematics::Frame* frame = new rw::kinematics::FixedFrame(dFrame.getName(), dFrame._transform);
        wc->addFrame(frame);
    }

    else if (dFrame._type == "Movable") { // Create a Movable frame
        rw::kinematics::MovableFrame* frame = new rw::kinematics::MovableFrame(dFrame.getName());
        wc->addFrame(frame);
        rw::kinematics::State state = wc->getDefaultState();
        frame->setTransform(dFrame._transform, state);
        state = wc->getStateStructure()->upgradeState(state); // Upgrade the state of the WorkCell
        wc->getStateStructure()->setDefaultState(state); // Set the state of the WorkCell
        getRobWorkStudio()->setState(state); // Update RWS to the new state
        //createMovableFrame(dFrame, wc);
    }

    else if (dFrame._type == "Prismatic") { // Create a Prismatic frame/joint
        rw::models::PrismaticJoint* j = new rw::models::PrismaticJoint(dFrame.getName(), dFrame._transform);
        wc->addFrame(j);
        addLimits(dFrame._limits, j);
        if (dFrame._state != ActiveState)
            j->setActive(false);
    }

    else if (dFrame._type == "Revolute") { // Create a Revolute frame/joint
        rw::models::RevoluteJoint* j = new rw::models::RevoluteJoint(dFrame.getName(), dFrame._transform);
        wc->addFrame(j);
        addLimits(dFrame._limits, j);
        if (dFrame._state != ActiveState)
            j->setActive(false);
    }

    else if (dFrame._type == "EndEffector") { // Create a EndEffector frame
        rw::kinematics::Frame* frame = new rw::kinematics::FixedFrame(dFrame.getName(), dFrame._transform);
        wc->addFrame(frame);
    }

    else{
        RW_THROW("FRAME TYPE ERROR");
    }
}

void TestPlugin::addLimits(std::vector<DummyLimit> &limits, rw::kinematics::Frame* frame) {
    rw::models::Joint* j = dynamic_cast<rw::models::Joint*>(frame);
    if (j == NULL)
        RW_THROW("FRAME NOT A JOINT");

    double convFactor;
    for (int i = 0; i < limits.size(); i++){
        convFactor = 1.0;
        if (dynamic_cast<rw::models::RevoluteJoint*>(j) != NULL) {
            convFactor = rw::math::Deg2Rad;
        }

        switch (limits[i]._type) {
            case (PosLimitType):
                j->setBounds(std::pair<rw::math::Q, rw::math::Q>(rw::math::Q(1, limits[i]._min * convFactor), rw::math::Q(1, limits[i]._max * convFactor)));
                log().info() << "Pos limit min -> " << limits[i]._min * convFactor << "\n";
                log().info() << "Pos limit max -> " << limits[i]._max * convFactor << "\n";
                break;
            case (VelLimitType):
                j->setMaxVelocity(rw::math::Q(1, limits[i]._max * convFactor));
                log().info() << "Vel limit -> " << limits[i]._max * convFactor << "\n";
                break;
            case (AccLimitType):
    		    j->setMaxAcceleration(rw::math::Q(1, limits[i]._max * convFactor));
                log().info() << "Acc limit -> " << limits[i]._max * convFactor << "\n";
    		    break;
            default:
                assert(0);
                break;
        }
    }

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
