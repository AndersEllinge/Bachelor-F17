/*
    Loader version 1.0
    Able to add content of a xml workcell file to an existing workcell or new workcell.
*/

#ifndef EI_LOADER
#define EI_LOADER

#include <rw/models/WorkCell.hpp>

namespace ei {
class loader {
public:
    loader(){}
    virtual ~loader(){}

    // Functions related to adding to a WorkCell
    static void add(const std::string filename, rw::models::WorkCell::Ptr wc, std::string name, std::string startFrame);
    static void add(const std::string filename, rw::models::WorkCell::Ptr wc, std::string name, std::string startFrame, rw::math::Transform3D<double> transform);
    static void add(const std::string filename, rw::models::WorkCell::Ptr wc, std::string name, std::string startFrame, double x, double y, double z, double R, double P, double Y);

    // Function for loading a WorkCell
    static rw::models::WorkCell::Ptr load(std::string filename, std::string name = "");

private:
    std::string addToWorkCell(std::string fname, rw::models::WorkCell::Ptr wc, std::string name, std::string startFrame);
};
}

#endif
