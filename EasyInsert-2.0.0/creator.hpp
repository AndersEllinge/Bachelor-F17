/*
    Creator version 1.0
    Able to create and add frames and objects to an existing workcell.
*/

#ifndef EI_CREATOR
#define EI_CREATOR

#include <rw/models/WorkCell.hpp>
#include <rw/kinematics/FixedFrame.hpp>
#include <rw/kinematics/MovableFrame.hpp>
#include <rw/math/Transform3D.hpp>

#include <rw/loaders/Model3DFactory.hpp>
#include <rw/loaders/GeometryFactory.hpp>
#include <rw/models/RigidObject.hpp>

namespace ei {
class creator {
public:
    creator(){}
    virtual ~creator(){}

    static rw::kinematics::FixedFrame* createFixedFrame(std::string name, rw::math::Transform3D<double> transform = rw::math::Transform3D<double>());
    static rw::kinematics::MovableFrame* createMovableFrame(std::string name);

    static void addFixedFrame(rw::models::WorkCell::Ptr wc, std::string name, std::string parent, rw::math::Transform3D<double> transform = rw::math::Transform3D<double>());
    static rw::kinematics::MovableFrame* addMovableFrame(rw::models::WorkCell::Ptr wc, std::string name, std::string parent, rw::math::Transform3D<double> transform = rw::math::Transform3D<double>());

    static void addBox(std::string name, std::string parent, rw::models::WorkCell::Ptr wc, float x, float y, float z, rw::math::Transform3D<double> transform = rw::math::Transform3D<double>()); // Creates a new movable frame as frame for model and geo
    static void addBox(std::string name, rw::kinematics::Frame* frame, rw::models::WorkCell::Ptr wc, float x, float y, float z, rw::math::Transform3D<double> transform = rw::math::Transform3D<double>()); // Adds model and geo to specified frame

    static void addPlane(std::string name, std::string parent, rw::models::WorkCell::Ptr wc, rw::math::Transform3D<double> transform = rw::math::Transform3D<double>());
    static void addPlane(std::string name, rw::kinematics::Frame* frame, rw::models::WorkCell::Ptr wc, rw::math::Transform3D<double> transform = rw::math::Transform3D<double>());

    static void addSphere(std::string name, std::string parent, rw::models::WorkCell::Ptr wc, float radius, rw::math::Transform3D<double> transform = rw::math::Transform3D<double>());
    static void addSphere(std::string name, rw::kinematics::Frame* frame, rw::models::WorkCell::Ptr wc, float radius, rw::math::Transform3D<double> transform = rw::math::Transform3D<double>());

    static void addCone(std::string name, std::string parent, rw::models::WorkCell::Ptr wc, float radius, float height, rw::math::Transform3D<double> transform = rw::math::Transform3D<double>());
    static void addCone(std::string name, rw::kinematics::Frame* frame, rw::models::WorkCell::Ptr wc, float radius, float height, rw::math::Transform3D<double> transform = rw::math::Transform3D<double>());

    static void addCylinder(std::string name, std::string parent, rw::models::WorkCell::Ptr wc, float radius, float height, rw::math::Transform3D<double> transform = rw::math::Transform3D<double>());
    static void addCylinder(std::string name, rw::kinematics::Frame* frame, rw::models::WorkCell::Ptr wc, float radius, float height, rw::math::Transform3D<double> transform = rw::math::Transform3D<double>());

    static void addTube(std::string name, std::string parent, rw::models::WorkCell::Ptr wc, float radius, float thickness, float height, rw::math::Transform3D<double> transform = rw::math::Transform3D<double>());
    static void addTube(std::string name, rw::kinematics::Frame* frame, rw::models::WorkCell::Ptr wc, float radius, float thickness, float height, rw::math::Transform3D<double> transform = rw::math::Transform3D<double>());

private:
    static rw::geometry::Geometry::Ptr createGoem(std::string name, rw::kinematics::Frame* frame, std::string val, rw::math::Transform3D<double> transform = rw::math::Transform3D<double>());
    static rw::graphics::Model3D::Ptr createModel(std::string name, std::string val, rw::math::Transform3D<double> transform = rw::math::Transform3D<double>());
    static void addObject(std::string name, rw::kinematics::Frame* frame, rw::models::WorkCell::Ptr wc, std::string val, rw::math::Transform3D<double> transform = rw::math::Transform3D<double>());

};
}

#endif
