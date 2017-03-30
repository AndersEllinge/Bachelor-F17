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
    connect(insertBtn, SIGNAL(pressed()), this, SLOT(insertBtnPressed()) );

}

TestPlugin::~TestPlugin() {
}

void TestPlugin::initialize() {
    getRobWorkStudio()->stateChangedEvent().add(boost::bind(&TestPlugin::stateChangedListener, this, _1), this);
}

void TestPlugin::open(WorkCell* workcell) {
	//_workcell = workcell;
}

void TestPlugin::close() {
}

void TestPlugin::stateChangedListener(const State& state) {
}

void TestPlugin::printInfo(rw::models::WorkCell::Ptr wc) {
    log().info() << "\nFound " << wc->getFrames().size() << " frames:\n"; // List frames
    for (size_t i = 0; i < wc->getFrames().size(); i++) {
        log().info() << wc->getFrames()[i]->getName() << "\n";
    }
    log().info() << "\n";

    log().info() << "Found " << wc->getDevices().size() << " devices:\n"; // List devices
    for (size_t i = 0; i < wc->getDevices().size(); i++) {
        log().info() << wc->getDevices()[i]->getName() << "\n";
    }
    log().info() << "\n\n";

    log().info() << "Found " << wc->getObjects().size() << " objects:\n"; // List objects
    for (size_t i = 0; i < wc->getObjects().size(); i++) {
        log().info() << wc->getObjects()[i]->getName() << " with geoms: ";
        for(size_t j = 0; j < wc->getObjects()[i]->getGeometry().size(); j++){
            log().info() << wc->getObjects()[i]->getGeometry()[j]->getName() << " ";
        }
        log().info() << "\n";
    }
    log().info() << "\n\n";
}

void TestPlugin::insertBtnPressed() {
    QString name = inputName->text();

    rw::models::WorkCell::Ptr wc = getRobWorkStudio()->getWorkCell(); // Get wc from rws

    rw::models::WorkCell::Ptr dummy = rw::common::ownedPtr(new rw::models::WorkCell("dummy")); // Create dummy wc for swap
    getRobWorkStudio()->setWorkCell(dummy); // Temporarily swap out wc from rws

    ei::loader::add("/home/mathias/Desktop/Git/Bachelor-F17/testPluginV2/Fanuc-LRM200i/fanuc_lrm200i.xml", wc, name.toStdString()); // Run loader

    rw::math::Vector3D<double> displacement(inputX->value()/100, inputY->value()/100, inputZ->value()/100);

    // Yaw x, Roll z, Pitch y
    rw::math::Rotation3D<double> rotationR(cos(inputR->value()), sin(inputR->value()), 0, -sin(inputR->value()), cos(inputR->value()), 0, 0, 0, 1);
    rw::math::Rotation3D<double> rotationY(1, 0, 0, 0, cos(inputRY->value()), sin(inputRY->value()), 0, -sin(inputRY->value()), cos(inputRY->value()));
    rw::math::Rotation3D<double> rotationP(cos(inputP->value()), 0, -sin(inputP->value()), 0, 1, 0, sin(inputP->value()), 0, cos(inputP->value()));

    rw::math::Rotation3D<double> combinedRotation = rw::math::Rotation3D<double>::multiply(rw::math::Rotation3D<double>::multiply(rotationP, rotationR), rotationY);

    rw::math::Transform3D<double> transform(displacement, combinedRotation);

    rw::kinematics::State state = wc->getDefaultState();

    // TEST FOR INITIAL PLACEMENT
    rw::kinematics::Frame* base = wc->findDevice(name.toStdString())->getBase();

    rw::kinematics::MovableFrame* mbase = dynamic_cast<rw::kinematics::MovableFrame*>(base); // Test for base is movable frame
    if (mbase != NULL)
        mbase->setTransform(transform, state);

    rw::kinematics::FixedFrame* fbase = dynamic_cast<rw::kinematics::FixedFrame*>(base); // Test for base is fixed frame
    if (fbase != NULL)
        fbase->setTransform(transform);

    state = wc->getStateStructure()->upgradeState(state);
    wc->getStateStructure()->setDefaultState(state);

    getRobWorkStudio()->setWorkCell(wc); // Swap back wc into rws

    //log().info() << "\n";
    //log().info() << "VALUE FOR X-INPUT = " << inputX->value() << "\n";
    //log().info() << "VALUE FOR Y-INPUT = " << inputY->value() << "\n";
    //log().info() << "VALUE FOR Z-INPUT = " << inputZ->value() << "\n";

    //printInfo(wc);
}


#if !RWS_USE_QT5
#include <QtCore/qplugin.h>
Q_EXPORT_PLUGIN(TestPlugin);
#endif
