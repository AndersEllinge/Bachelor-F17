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

    rw::math::Vector3D<double> displacement(inputX->value()/100, inputY->value()/100, inputZ->value()/100);
    rw::math::Transform3D<double> testTransform(displacement);

    rw::models::WorkCell::Ptr wc = getRobWorkStudio()->getWorkCell(); // Get wc from rws

    rw::models::WorkCell::Ptr dummy = rw::common::ownedPtr(new rw::models::WorkCell("dummy")); // Create dummy wc for swap
    getRobWorkStudio()->setWorkCell(dummy); // Temporarily swap out wc from rws

    // Loader test: Device
    //ei::loader::add("/home/mathias/Desktop/Git/Bachelor-F17/testPluginV2/Fanuc-LRM200i/fanuc_lrm200i.xml", wc, name.toStdString(),
    //    inputX->value(), inputY->value(), inputZ->value(), inputR->value(), inputP->value(), inputRY->value()); // Run loader

    // Creator test: Frames
    ei::creator::addFixedFrame(wc, "testFixed", "WORLD");
    ei::creator::addMovableFrame(wc, "testMovable", "testFixed");

    // Creator test: Box
    //ei::creator::addBox("testBox", "testFixed", wc, 0.1f, 0.1f, 0.1f, testTransform);
    //ei::creator::addBox("testBox2", wc->findFrame("WORLD"), wc, 0.2f, 0.2f, 0.2f, testTransform);

    // Creator test: Plane
    //ei::creator::addPlane("testPlane", "testFixed", wc, testTransform);
    //ei::creator::addPlane("testPlane2", wc->findFrame("WORLD"), wc, testTransform);

    // Creator test: Sphere
    //ei::creator::addSphere("testSphere", "testFixed", wc, 0.1f, testTransform);
    //ei::creator::addSphere("testSphere2", wc->findFrame("WORLD"), wc, 0.2f, testTransform);

    // Creator test: Cone
    //ei::creator::addCone("testCone", "testFixed", wc, 0.1f, 0.2f, testTransform);
    //ei::creator::addCone("testCone2", wc->findFrame("WORLD"), wc, 0.2f, 0.4f, testTransform);

    // Creator test: Cylinder
    //ei::creator::addCylinder("testCylinder", "testFixed", wc, 0.1f, 0.2f, testTransform);
    //ei::creator::addCylinder("testCylinder2", wc->findFrame("WORLD"), wc, 0.2f, 0.4f, testTransform);

    // Creator test: Tube
    //ei::creator::addTube("testTube", "testFixed", wc, 0.2f, 0.1f, 0.4f, testTransform);
    //ei::creator::addTube("testTube2", wc->findFrame("WORLD"), wc, 0.4f, 0.2f, 0.8f, testTransform);



    getRobWorkStudio()->setWorkCell(wc); // Swap back wc into rws

    // Yaw x, Roll z, Pitch y
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
