#ifndef TESTPLUGIN_HPP
#define TESTPLUGIN_HPP

#include <RobWorkStudioConfig.hpp> // For RWS_USE_QT5 definition

#include <rws/RobWorkStudioPlugin.hpp>
#include <rw/loaders/rwxml/XMLParserUtil.hpp>
#include <rw/loaders/rwxml/XMLRWParser.hpp>
#include <rw/loaders/rwxml/XMLRWLoader.hpp>

#include <rw/kinematics/FixedFrame.hpp>
#include <rw/kinematics/MovableFrame.hpp>
#include <rw/models/RevoluteJoint.hpp>
#include <rw/models/PrismaticJoint.hpp>

#include "ui_TestPlugin.h"

class TestPlugin: public rws::RobWorkStudioPlugin, private Ui::TestPlugin
{
rw::models::WorkCell* _workcell;
Q_OBJECT
Q_INTERFACES( rws::RobWorkStudioPlugin )
#if RWS_USE_QT5
Q_PLUGIN_METADATA(IID "dk.sdu.mip.Robwork.RobWorkStudioPlugin/0.1" FILE "plugin.json")
#endif
public:
    TestPlugin();
	virtual ~TestPlugin();

    virtual void open(rw::models::WorkCell* workcell);

    virtual void close();

    virtual void initialize();

private:
    rw::kinematics::Frame* createFrame(DummyFrame dFrame, rw::models::WorkCell::Ptr wc);

    // Print functionality
    void printInfo(rw::models::WorkCell::Ptr wc);


private slots:
    void helloWorldBtnPressed();
	void spinBoxChanged();

    void stateChangedListener(const rw::kinematics::State& state);
};

#endif /*RINGONHOOKPLUGIN_HPP_*/
