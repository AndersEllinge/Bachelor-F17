/*
    Loader version 1.0
    Able to add content of a xml workcell file to an existing workcell.
    Cannot handle duplicates yet.
*/

#ifndef EI_LOADER
#define EI_LOADER

#include <rw/models/WorkCell.hpp>

namespace ei {
class loader {
public:
    loader(){}
    virtual ~loader(){}

    static void add(const std::string filename, rw::models::WorkCell::Ptr wc);
    static rw::models::WorkCell::Ptr load(std::string filename);

private:
    void addToWorkCell(std::string fname, rw::models::WorkCell::Ptr wc);
};
}

#endif
