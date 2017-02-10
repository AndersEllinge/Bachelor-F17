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
    connect(helloWorldBtn, SIGNAL(pressed()), this, SLOT(btnPressed()) );
    //connect(_btn1    ,SIGNAL(pressed()), this, SLOT(btnPressed()) );
    //connect(_spinBox  ,SIGNAL(valueChanged(int)), this, SLOT(btnPressed()) );

}

TestPlugin::~TestPlugin() {
}

void TestPlugin::initialize() {
    getRobWorkStudio()->stateChangedEvent().add(boost::bind(&TestPlugin::stateChangedListener, this, _1), this);
}

void TestPlugin::open(WorkCell* workcell) {
}

void TestPlugin::close() {
}

void TestPlugin::btnPressed() {
	QObject *obj = sender();
	if(obj == helloWorldBtn) {
		log().info() << "Hello World!\n";
	}
}

void TestPlugin::stateChangedListener(const State& state) {
}

#if !RWS_USE_QT5
#include <QtCore/qplugin.h>
Q_EXPORT_PLUGIN(TestPlugin);
#endif



