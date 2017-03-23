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
    QString name = nameField->toPlainText();

    rw::models::WorkCell::Ptr wc = getRobWorkStudio()->getWorkCell(); // Get wc from rws

    rw::models::WorkCell::Ptr dummy = rw::common::ownedPtr(new rw::models::WorkCell("dummy")); // Create dummy wc for swap
    getRobWorkStudio()->setWorkCell(dummy); // Temporarily swap out wc from rws

    ei::loader::add("/home/mathias/Desktop/Git/Bachelor-F17/testPlugin/wc.xml", wc, name.toStdString()); // Run loader

    getRobWorkStudio()->setWorkCell(wc); // Swap back wc into rws

    rw::proximity::CollisionSetup colset = wc->getCollisionSetup();
    auto exList = colset.getExcludeList();

    //printInfo(wc);
}


#if !RWS_USE_QT5
#include <QtCore/qplugin.h>
Q_EXPORT_PLUGIN(TestPlugin);
#endif
