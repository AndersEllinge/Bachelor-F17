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
    rw::models::WorkCell::Ptr wc = getRobWorkStudio()->getWorkCell(); // Get WorkCell from scene
    log().info() << "Info for scene WorkCell\n";
    printInfo(wc);
    /*
    ///___________________________________________________________________________________________________________________________///

	// Add information to current WorkCell
    // Parse in the information to add
    boost::shared_ptr<DummyWorkcell> dwc = rw::loaders::XMLRWParser::parseWorkcell("/home/mathias/Desktop/Git/Bachelor-F17/testPlugin/wc.xml");

    // Create and add frames to scene WorkCell
    for (size_t i = 0; i < dwc->_framelist.size(); i++){
        createFrame(dwc->_framelist[i], wc);
        //log().info() << dwc->_framelist[i].getName() << " " << dwc->_framelist[i]._type << "\n";
    }

    // Create and add devices to current WorkCell
    for (size_t i = 0; i < dwc->_devlist.size(); i++) {
        createDevice(dwc->_devlist[i], wc);
    }
    */

    rw::models::WorkCell::Ptr dummy = rw::common::ownedPtr(new rw::models::WorkCell("dummy"));
    getRobWorkStudio()->setWorkCell(dummy);

    /*
    testLoader loader;
    std::string path = "/home/mathias/Desktop/Git/Bachelor-F17/testPlugin/wc.xml";
    loader.addToWorkCell(path, wc);
    */

    ei::loader::add("/home/mathias/Desktop/Git/Bachelor-F17/testPlugin/wc.xml", wc);

    //getRobWorkStudio()->updateAndRepaint();

    log().info() << "Info for updated scene WorkCell\n";
    printInfo(wc);

    getRobWorkStudio()->setWorkCell(wc);







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

// Extra functions from here
rw::kinematics::Frame* TestPlugin::createFrame(DummyFrame dFrame, rw::models::WorkCell::Ptr wc) {
    rw::kinematics::Frame* frame = NULL;

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
            frame = joint;
            addFrameToTree(dFrame, wc, frame);
        }

        else if (dFrame._type == "Prismatic") { // Create dependent Prismatic frame/joint
            rw::models::DependentPrismaticJoint* joint = new rw::models::DependentPrismaticJoint(dFrame.getName(), dFrame._transform, depJoint, dFrame._gain, dFrame._offset);
            frame = joint;
            addFrameToTree(dFrame, wc, frame);
        }
    }

    else if (dFrame._type == "Fixed") { // Create a Fixed frame
        frame = new rw::kinematics::FixedFrame(dFrame.getName(), dFrame._transform);
        addFrameToTree(dFrame, wc, frame);
    }

    else if (dFrame._type == "Movable") { // Create a Movable frame
        rw::kinematics::MovableFrame* mFrame = new rw::kinematics::MovableFrame(dFrame.getName());
        frame = mFrame;
        addFrameToTree(dFrame, wc, frame);
        rw::kinematics::State state = wc->getDefaultState();
        mFrame->setTransform(dFrame._transform, state);
        state = wc->getStateStructure()->upgradeState(state); // Upgrade the state of the WorkCell
        wc->getStateStructure()->setDefaultState(state); // Set the state of the WorkCell
        getRobWorkStudio()->setState(state); // Update RWS to the new state
    }

    else if (dFrame._type == "Prismatic") { // Create a Prismatic frame/joint
        rw::models::PrismaticJoint* joint = new rw::models::PrismaticJoint(dFrame.getName(), dFrame._transform);
        frame = joint;
        addFrameToTree(dFrame, wc, frame);
        addLimits(dFrame._limits, joint);
        if (dFrame._state != ActiveState)
            joint->setActive(false);
    }

    else if (dFrame._type == "Revolute") { // Create a Revolute frame/joint
        rw::models::RevoluteJoint* joint = new rw::models::RevoluteJoint(dFrame.getName(), dFrame._transform);
        frame = joint;
        addFrameToTree(dFrame, wc, frame);
        addLimits(dFrame._limits, joint);
        if (dFrame._state != ActiveState)
            joint->setActive(false);
    }

    else if (dFrame._type == "EndEffector") { // Create a EndEffector frame
        frame = new rw::kinematics::FixedFrame(dFrame.getName(), dFrame._transform);
        addFrameToTree(dFrame, wc, frame);
    }

    else{
        RW_THROW("FRAME TYPE ERROR");
    }

    // Add DHparams as property if any
    if (dFrame._hasDHparam) {
        DHParam &param = dFrame._dhparam;

        if (param._dhtype == Revolute) {
            if (param._type == "HGP" && param._hgptype == "parallel") {
                rw::models::DHParameterSet dhset(param._alpha, param._a, param._offset, param._b, true);
                rw::models::DHParameterSet::set(dhset, frame);
            } else {
                rw::models::DHParameterSet dhset(param._alpha, param._a, param._d, param._offset, param._type);
                rw::models::DHParameterSet::set(dhset, frame);
            }
        } else if (param._dhtype == Prismatic) {
            if (param._type == "HGP" && param._hgptype == "parallel") {
                rw::models::DHParameterSet dhset(param._alpha, param._a, param._beta, param._offset, true);
                rw::models::DHParameterSet::set(dhset, frame);
            } else {
                rw::models::DHParameterSet dhset(param._alpha, param._a, param._offset, param._theta, param._type);
                rw::models::DHParameterSet::set(dhset, frame);
            }
        }
    }

    addFrameProps(dFrame, frame, wc);

    return frame;
}

void TestPlugin::addFrameToTree(DummyFrame dFrame, rw::models::WorkCell::Ptr wc, rw::kinematics::Frame* f) {
    rw::kinematics::Frame* parent = wc->findFrame(dFrame._refframe);
    wc->addFrame(f, parent);

}

void TestPlugin::addFrameProps(DummyFrame dFrame, rw::kinematics::Frame* f, rw::models::WorkCell::Ptr wc) {
    for (size_t i = 0; i < dFrame._properties.size(); i++) {
        const DummyProperty& dprop = dFrame._properties[i];
        addPropertyToMap(dprop, f->getPropertyMap());
    }
    for (size_t i = 0; i < dFrame._models.size(); i++) {
        addModelToFrame(dFrame._models[i], f, wc);
    }
}

void TestPlugin::addLimits(std::vector<DummyLimit> &limits, rw::kinematics::Frame* frame) {
    rw::models::Joint* j = dynamic_cast<rw::models::Joint*>(frame);
    if (j == NULL)
        return;
        //RW_THROW("FRAME NOT A JOINT");

    double convFactor;
    for (size_t i = 0; i < limits.size(); i++){
        convFactor = 1.0;
        if (dynamic_cast<rw::models::RevoluteJoint*>(j) != NULL) {
            convFactor = rw::math::Deg2Rad;
        }

        switch (limits[i]._type) {
            case (PosLimitType):
                j->setBounds(std::pair<rw::math::Q, rw::math::Q>(rw::math::Q(1, limits[i]._min * convFactor), rw::math::Q(1, limits[i]._max * convFactor)));
                //log().info() << "Pos limit min -> " << limits[i]._min * convFactor << "\n";
                //log().info() << "Pos limit max -> " << limits[i]._max * convFactor << "\n";
                break;
            case (VelLimitType):
                j->setMaxVelocity(rw::math::Q(1, limits[i]._max * convFactor));
                //log().info() << "Vel limit -> " << limits[i]._max * convFactor << "\n";
                break;
            case (AccLimitType):
    		    j->setMaxAcceleration(rw::math::Q(1, limits[i]._max * convFactor));
                //log().info() << "Acc limit -> " << limits[i]._max * convFactor << "\n";
    		    break;
            default:
                assert(0);
                break;
        }
    }

}

void TestPlugin::addPropertyToMap(const DummyProperty &dprop, rw::common::PropertyMap& map){
    if(dprop._type=="string"){
        map.add(dprop._name, dprop._desc, dprop._val);
    } else if(dprop._type=="double"){

    	try {
    		double val = boost::lexical_cast<double>(dprop._val);
        	map.add(dprop._name, dprop._desc, val);
    	} catch(const std::exception& e) {

    		RW_WARN("Could not parse double property value: " << dprop._name << ". An error occoured:\n " << std::string(e.what()));

    	}
    } else if(dprop._type=="Q"){
        std::stringstream istr(dprop._val);
        std::vector<double> res;
        while (!istr.eof()) {
            double d;
            istr >> d;
            res.push_back(d);
        }
        rw::math::Q val(res);
        map.add(dprop._name, dprop._desc, val);
    }

}

void TestPlugin::addModelToFrame(DummyModel& model, rw::kinematics::Frame* f, rw::models::WorkCell::Ptr wc) {

	for (size_t i = 0; i < model._geo.size(); i++) {
		std::ostringstream val;
        bool needsToBeAdded = 0;

		switch (model._geo[i]._type) {
		case PolyType:
			if (!rw::common::StringUtil::isAbsoluteFileName(model._geo[i]._filename + ".tmp")) {
				val << rw::common::StringUtil::getDirectoryName(model._geo[i]._pos.file);
			}
			val << model._geo[i]._filename;
			break;
		case PlaneType:
			val << "#Plane";
			break;
		case CubeType:
			val << "#Box " << model._geo[i]._x << " " << model._geo[i]._y << " " << model._geo[i]._z;
			break;
		case SphereType:
			val << "#Sphere " << model._geo[i]._radius;
			break;
		case ConeType:
			val << "#Cone " << model._geo[i]._radius << " " << model._geo[i]._z;
			break;
		case CylinderType:
			val << "#Cylinder " << model._geo[i]._radius << " " << model._geo[i]._z << " " << 20;
			break;
		case TubeType:
			// #Tube radius thickness height divisions
			val << "#Tube " << model._geo[i]._radius << " " << model._geo[i]._x << " " << model._geo[i]._z << " " << 20;
			break;
		case CustomType:
			val << "#Custom " << model._geo[i]._filename << " " << model._geo[i]._parameters;
			break;
		default:
			val << "";
			break;
		}

		rw::models::RigidObject::Ptr object;
        if (model._isDrawable || model._colmodel) {
            if (wc->findObject(f->getName()) != NULL) {
                object = wc->findObject(f->getName()).cast<rw::models::RigidObject>();
            }
            else{
                object = new rw::models::RigidObject(f);
                needsToBeAdded = 1;
            }
        }

		if (model._colmodel && model._isDrawable) {

			// The geom is to be used as both collision geometry and visualization model
			rw::graphics::Model3D::Ptr model3d = rw::loaders::Model3DFactory::getModel(val.str(), model._name);
			model3d->setTransform(model._transform);
			model3d->setName(model._name);

			rw::geometry::Geometry::Ptr geom = rw::loaders::GeometryFactory::load(val.str(), true);
			geom->setName(model._name);
			geom->setTransform(model._transform);
			geom->setFrame(f);

			if (object != NULL) {
				object->addModel(model3d);
				object->addGeometry(geom);
			}

		} else if (model._colmodel) {
			// its only a collision geometry

			rw::geometry::Geometry::Ptr geom = rw::loaders::GeometryFactory::load(val.str(), true);
			geom->setName(model._name);

			geom->setTransform(model._transform);
			geom->setFrame(f);

			if (object != NULL) {
				object->addGeometry(geom);
			}

		} else if (model._isDrawable) {
			// its only a drawable

			rw::graphics::Model3D::Ptr model3d = rw::loaders::Model3DFactory::getModel(val.str(), val.str());

			model3d->setName(model._name);

			model3d->setTransform(model._transform);

			if (object != NULL) {
				object->addModel(model3d);
			}

		}
        if (needsToBeAdded)
            wc->add(object);
	}
}

void TestPlugin::createDevice(DummyDevice dDevice, rw::models::WorkCell::Ptr wc) {
    // Create the device and add to wc
    rw::models::Device::Ptr device = NULL;

    if (dDevice._type == SerialType) {
        std::vector<rw::kinematics::Frame*> chain;
        for (size_t i = 0; i < dDevice._frames.size(); i++) {
            chain.push_back(createFrame(dDevice._frames[i], wc));
        }

        BOOST_FOREACH(DummyFrame& dframe, dDevice._frames) {
            //addFrameProps(dframe, wc->findFrame(dframe.getName()), wc);
            addDevicePropsToFrame(dDevice, wc->findFrame(dframe.getName()), wc);
        }

        device = new rw::models::SerialDevice(chain, dDevice.getName(), wc->getDefaultState());
        wc->addDevice(device);
    }


    // Add all device properties
	typedef std::pair<std::string, std::vector<DummyProperty> > DPropValType;
	BOOST_FOREACH( DPropValType val, dDevice._propertyMap) {
		BOOST_FOREACH( DummyProperty dprop, val.second ) {
		    addPropertyToMap(dprop,  device->getPropertyMap());
		}
	}

    BOOST_FOREACH(QConfig& config, dDevice._qconfig) {
        device->getBase()->getPropertyMap().add<rw::math::Q>(config.name, "", rw::math::Q(config.q.size(), &config.q[0]));
        device->getPropertyMap().add<rw::math::Q>(config.name, "", rw::math::Q(config.q.size(), &config.q[0]));
    }

    //getRobWorkStudio()->updateAndRepaint();
}

void TestPlugin::addDevicePropsToFrame(DummyDevice dDevice, rw::kinematics::Frame* f, rw::models::WorkCell::Ptr wc) {
    std::vector<boost::shared_ptr<rw::common::Property<std::string> > > proplist = dDevice._propMap[f->getName()];

    for (size_t j = 0; j < proplist.size(); j++) {
		f->getPropertyMap().add(proplist[j]->getIdentifier(), proplist[j]->getDescription(), proplist[j]->getValue());
	}

    std::vector<DummyModel> modellist = dDevice._modelMap[f->getName()];
    for (size_t j = 0; j < modellist.size(); j++) {
    	addModelToFrame(modellist[j], f, wc);
	}

    std::vector<DummyLimit> limits = dDevice._limitMap[f->getName()];
    addLimits(limits, f);

}

void TestPlugin::printInfo(rw::models::WorkCell::Ptr wc) {
    log().info() << "Found " << wc->getFrames().size() << " frames:\n"; // List frames
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


#if !RWS_USE_QT5
#include <QtCore/qplugin.h>
Q_EXPORT_PLUGIN(TestPlugin);
#endif
