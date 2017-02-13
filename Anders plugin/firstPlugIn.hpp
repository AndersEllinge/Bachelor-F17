#ifndef FIRSTPLUGIN_HPP
#define FIRSTPLUGIN_HPP

#include <RobWorkStudioConfig.hpp>

#include <rws/RobWorkStudioPlugin.hpp>

#include "ui_firstPlugIn.h"

#include <rw/common/Ptr.hpp>


namespace rw { namespace models { class Device; } }

class firstPlugIn: public rws::RobWorkStudioPlugin, private Ui::firstPlugIn
{

rw::models::WorkCell* _workcell;

Q_OBJECT
Q_INTERFACES( rws::RobWorkStudioPlugin )
#if RWS_USE_QT5
Q_PLUGIN_METADATA(IID "dk.sdu.mip.Robwork.RobWorkStudioPlugin/0.1" FILE "plugin.json")
#endif
public:
    firstPlugIn();
	virtual ~firstPlugIn();

    virtual void open(rw::models::WorkCell* workcell);

    virtual void close();

    virtual void initialize();

private slots:
    void btnPressed();

    void stateChangedListener(const rw::kinematics::State& state);

    
};

#endif 
