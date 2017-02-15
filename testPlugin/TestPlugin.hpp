#ifndef TESTPLUGIN_HPP
#define TESTPLUGIN_HPP

#include <RobWorkStudioConfig.hpp> // For RWS_USE_QT5 definition

#include <rws/RobWorkStudioPlugin.hpp>
#include <rw/loaders/rwxml/XMLRWLoader.hpp>
#include <rw/models/Object.hpp>

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


private slots:
    void helloWorldBtnPressed();
	void spinBoxChanged();

    void stateChangedListener(const rw::kinematics::State& state);
};

#endif /*RINGONHOOKPLUGIN_HPP_*/
