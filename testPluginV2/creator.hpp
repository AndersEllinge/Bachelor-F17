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

    static void addBox(std::string name, std::string parent, rw::models::WorkCell::Ptr wc, float x, float y, float z);

private:
    static rw::geometry::Geometry::Ptr createBoxGoem(std::string name, rw::kinematics::Frame* frame, float x, float y, float z);
    static rw::graphics::Model3D::Ptr createBoxModel(std::string name, float x, float y, float z);

};
}

#endif
