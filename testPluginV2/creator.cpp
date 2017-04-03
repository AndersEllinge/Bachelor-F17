#include "creator.hpp"
#include <RobWorkStudio.hpp>

rw::kinematics::FixedFrame* ei::creator::createFixedFrame(std::string name, rw::math::Transform3D<double> transform) {
    rw::kinematics::FixedFrame* tmpPtr = new rw::kinematics::FixedFrame(name, transform);
    return tmpPtr;
}

rw::kinematics::MovableFrame* ei::creator::createMovableFrame(std::string name) {
    rw::kinematics::MovableFrame* tmpPtr = new rw::kinematics::MovableFrame(name);
    return tmpPtr;
}

void ei::creator::addFixedFrame(rw::models::WorkCell::Ptr wc, std::string name, std::string parent, rw::math::Transform3D<double> transform) {
    ei::creator creator;
    rw::kinematics::Frame* frame = dynamic_cast<rw::kinematics::Frame*>(creator.createFixedFrame(name, transform));
    rw::kinematics::Frame* parentFrame = wc->findFrame(parent);
    if(parentFrame == NULL) {
        rw::common::Log::log().info() << "parent frame not found\n";
        return;
    }
    wc->addFrame(frame, parentFrame);
    rw::common::Log::log().info() << "Added frame " << name << " to frame " << parent << std::endl;
}

rw::kinematics::MovableFrame* ei::creator::addMovableFrame(rw::models::WorkCell::Ptr wc, std::string name, std::string parent, rw::math::Transform3D<double> transform) {
    ei::creator creator;
    rw::kinematics::MovableFrame* mframe = creator.createMovableFrame(name);
    rw::kinematics::Frame* frame = dynamic_cast<rw::kinematics::Frame*>(mframe);
    rw::kinematics::Frame* parentFrame = wc->findFrame(parent);
    if(parentFrame == NULL) {
        rw::common::Log::log().info() << "parent frame not found\n";
        return NULL;
    }
    wc->addFrame(frame, parentFrame);

    rw::kinematics::State state = wc->getDefaultState(); // Get state of wc

    mframe->setTransform(transform, state);

    state = wc->getStateStructure()->upgradeState(state); // Upgrade state
    wc->getStateStructure()->setDefaultState(state); // Update state
    rw::common::Log::log().info() << "Added frame " << name << " to frame " << parent << std::endl;
    return mframe;
}


void ei::creator::addBox(std::string name, std::string parent, rw::models::WorkCell::Ptr wc, float x, float y, float z) {
    ei::creator creator;
    rw::kinematics::MovableFrame* mframe = creator.addMovableFrame(wc, name, parent);

    if (mframe == NULL) {
        rw::common::Log::log().info() << "parent frame not found\n";
        return;
    }

    rw::models::RigidObject::Ptr object = new rw::models::RigidObject(mframe);

    object->addModel(creator.createBoxModel(name, x, y, z));
    object->addGeometry(creator.createBoxGoem(name, mframe, x, y, z));

    wc->add(object);
}




rw::geometry::Geometry::Ptr ei::creator::createBoxGoem(std::string name, rw::kinematics::Frame* frame, float x, float y, float z) {
    std::ostringstream val;
    val << "#Box " << x << " " << y << " " << z;

    rw::geometry::Geometry::Ptr geom = rw::loaders::GeometryFactory::load(val.str(), true);
    geom->setName(name);
    geom->setTransform(rw::math::Transform3D<double>()); // Set transform to default
    geom->setFrame(frame);

    return geom;
}

rw::graphics::Model3D::Ptr ei::creator::createBoxModel(std::string name, float x, float y, float z) {
    std::ostringstream val;
    val << "#Box " << x << " " << y << " " << z;

    rw::graphics::Model3D::Ptr model3d = rw::loaders::Model3DFactory::getModel(val.str(), name);
    model3d->setTransform(rw::math::Transform3D<double>()); // Set transform to default
    model3d->setName(name);

    return model3d;
}
