#ifndef TESTPLUGIN_HPP
#define TESTPLUGIN_HPP

#include <RobWorkStudioConfig.hpp> // For RWS_USE_QT5 definition

#include <rws/RobWorkStudioPlugin.hpp>
#include <rw/loaders/rwxml/XMLParserUtil.hpp>
#include <rw/loaders/rwxml/XMLRWParser.hpp>
#include <rw/loaders/rwxml/XMLRWLoader.hpp>

#include <rw/math/Constants.hpp>
#include <rw/math/Q.hpp>

#include <rw/kinematics/FixedFrame.hpp>
#include <rw/kinematics/MovableFrame.hpp>
#include <rw/models/RevoluteJoint.hpp>
#include <rw/models/PrismaticJoint.hpp>
#include <rw/models/DependentRevoluteJoint.hpp>
#include <rw/models/DependentPrismaticJoint.hpp>
#include <rw/models/DHParameterSet.hpp>
#include <rw/models/RigidObject.hpp>

#include <rw/models/SerialDevice.hpp>

#include <rw/loaders/GeometryFactory.hpp>
#include <rw/loaders/Model3DFactory.hpp>

#include <rw/common/Property.hpp>
#include <rw/common/StringUtil.hpp>
#include <rw/common/AnyPtr.hpp>

#include <boost/lexical_cast.hpp>

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
    void createDevice(DummyDevice dDevice, rw::models::WorkCell::Ptr wc);

    // Help for createFrame
    void addPropertyToMap(const DummyProperty &dprop, rw::common::PropertyMap& map); // From XML RW LOADER (copy, namespace modified)
    void addModelToFrame(DummyModel& model, rw::kinematics::Frame* f, rw::models::WorkCell::Ptr wc); // Mainly from XML RW LOADER (changed input and output, modified function to fit)
    void addFrameToTree(DummyFrame dFrame, rw::models::WorkCell::Ptr wc, rw::kinematics::Frame* f);
    void addFrameProps(DummyFrame dFrame, rw::kinematics::Frame* f, rw::models::WorkCell::Ptr wc);
    void addLimits(std::vector<DummyLimit> &limits, rw::kinematics::Frame* frame);

    // Help for createDevice
    void addDevicePropsToFrame(DummyDevice dDevice, rw::kinematics::Frame* f, rw::models::WorkCell::Ptr wc);

    // Print functionality
    void printInfo(rw::models::WorkCell::Ptr wc);

    std::vector<rw::models::RigidObject::Ptr> obj;


private slots:
    void helloWorldBtnPressed();
	void spinBoxChanged();

    void stateChangedListener(const rw::kinematics::State& state);
};

#endif /*RINGONHOOKPLUGIN_HPP_*/
