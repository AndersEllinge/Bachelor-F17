#include "creator.hpp"
#include <RobWorkStudio.hpp>

// Transform
rw::math::Transform3D<double> ei::creator::getTransform3D(double x, double y, double z, double R, double P, double Y) {
    ei::creator creator;
    rw::math::RPY<double> rpy(R, P, Y);
    rw::math::Vector3D<double> displacement(x, y, z);
    rw::math::Transform3D<double> tmpTransform(displacement, rpy.toRotation3D());
    return tmpTransform;
}

// Frames
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

// Box
void ei::creator::addBox(std::string name, std::string parent, rw::models::WorkCell::Ptr wc, float x, float y, float z, rw::math::Transform3D<double> transform) {
    ei::creator creator;
    rw::kinematics::MovableFrame* mframe = creator.addMovableFrame(wc, name, parent);

    if (mframe == NULL) {
        rw::common::Log::log().info() << "parent frame not found\n";
        return;
    }

    creator.addBox(name, mframe, wc, x, y, z, transform);
}

void ei::creator::addBox(std::string name, rw::kinematics::Frame* frame, rw::models::WorkCell::Ptr wc, float x, float y, float z, rw::math::Transform3D<double> transform) {
    ei::creator creator;
    std::ostringstream val;
    val << "#Box " << x << " " << y << " " << z;
    creator.addObject(name, frame, wc, val.str(), transform);
}

// Plane
void ei::creator::addPlane(std::string name, std::string parent, rw::models::WorkCell::Ptr wc, rw::math::Transform3D<double> transform) {
    ei::creator creator;
    rw::kinematics::MovableFrame* mframe = creator.addMovableFrame(wc, name, parent);

    if (mframe == NULL) {
        rw::common::Log::log().info() << "parent frame not found\n";
        return;
    }

    creator.addPlane(name, mframe, wc, transform);
}

void ei::creator::addPlane(std::string name, rw::kinematics::Frame* frame, rw::models::WorkCell::Ptr wc, rw::math::Transform3D<double> transform) {
    ei::creator creator;
    std::ostringstream val;
    val << "#Plane";
    creator.addObject(name, frame, wc, val.str(), transform);
}

// Sphere
void ei::creator::addSphere(std::string name, std::string parent, rw::models::WorkCell::Ptr wc, float radius, rw::math::Transform3D<double> transform) {
    ei::creator creator;
    rw::kinematics::MovableFrame* mframe = creator.addMovableFrame(wc, name, parent);
    if (mframe == NULL)
        return;

    creator.addSphere(name, mframe, wc, radius, transform);
}

void ei::creator::addSphere(std::string name, rw::kinematics::Frame* frame, rw::models::WorkCell::Ptr wc, float radius, rw::math::Transform3D<double> transform) {
    ei::creator creator;
    std::ostringstream val;
    val << "#Sphere " << radius;
    creator.addObject(name, frame, wc, val.str(), transform);
}

// Cone
void ei::creator::addCone(std::string name, std::string parent, rw::models::WorkCell::Ptr wc, float radius, float height, rw::math::Transform3D<double> transform) {
    ei::creator creator;
    rw::kinematics::MovableFrame* mframe = creator.addMovableFrame(wc, name, parent);
    if (mframe == NULL)
        return;

    creator.addCone(name, mframe, wc, radius, height, transform);
}

void ei::creator::addCone(std::string name, rw::kinematics::Frame* frame, rw::models::WorkCell::Ptr wc, float radius, float height, rw::math::Transform3D<double> transform) {
    ei::creator creator;
    std::ostringstream val;
    val << "#Cone " << radius << " " << height;
    creator.addObject(name, frame, wc, val.str(), transform);
}

// Cylinder
void ei::creator::addCylinder(std::string name, std::string parent, rw::models::WorkCell::Ptr wc, float radius, float height, rw::math::Transform3D<double> transform) {
    ei::creator creator;
    rw::kinematics::MovableFrame* mframe = creator.addMovableFrame(wc, name, parent);
    if (mframe == NULL)
        return;

    creator.addCylinder(name, mframe, wc, radius, height, transform);
}

void ei::creator::addCylinder(std::string name, rw::kinematics::Frame* frame, rw::models::WorkCell::Ptr wc, float radius, float height, rw::math::Transform3D<double> transform) {
    ei::creator creator;
    std::ostringstream val;
    val << "#Cylinder " << radius << " " << height << " " << 20;
    creator.addObject(name, frame, wc, val.str(), transform);
}

// Tube
void ei::creator::addTube(std::string name, std::string parent, rw::models::WorkCell::Ptr wc, float radius, float thickness, float height, rw::math::Transform3D<double> transform) {
    ei::creator creator;
    rw::kinematics::MovableFrame* mframe = creator.addMovableFrame(wc, name, parent);
    if (mframe == NULL)
        return;

    creator.addTube(name, mframe, wc, radius, thickness, height, transform);
}

void ei::creator::addTube(std::string name, rw::kinematics::Frame* frame, rw::models::WorkCell::Ptr wc, float radius, float thickness, float height, rw::math::Transform3D<double> transform) {
    ei::creator creator;
    std::ostringstream val;
    val << "#Tube " << radius << " " << thickness << " " << height << " " << 20;
    creator.addObject(name, frame, wc, val.str(), transform);
}



// Util funcs
rw::geometry::Geometry::Ptr ei::creator::createGoem(std::string name, rw::kinematics::Frame* frame, std::string val, rw::math::Transform3D<double> transform) {
    rw::geometry::Geometry::Ptr geom = rw::loaders::GeometryFactory::load(val, true);
    geom->setName(name);
    geom->setTransform(transform);
    geom->setFrame(frame);

    return geom;
}

rw::graphics::Model3D::Ptr ei::creator::createModel(std::string name, std::string val, rw::math::Transform3D<double> transform) {
    rw::graphics::Model3D::Ptr model3d = rw::loaders::Model3DFactory::getModel(val, name);
    model3d->setTransform(transform);
    model3d->setName(name);

    return model3d;
}

void ei::creator::addObject(std::string name, rw::kinematics::Frame* frame, rw::models::WorkCell::Ptr wc, std::string val, rw::math::Transform3D<double> transform) {
    ei::creator creator;
    rw::models::RigidObject::Ptr object = new rw::models::RigidObject(frame);

    object->addModel(creator.createModel(name, val, transform));
    object->addGeometry(creator.createGoem(name, frame, val, transform));

    wc->add(object);
}
