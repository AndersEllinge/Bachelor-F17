#ifndef TEXTFINDER_H
#define TEXTFINDER_H

#include <QWidget>
#include <QFile>
#include <QTextStream>
#include <QtCore>
#include <QtGui>

#include <RobWorkStudio.hpp>
#include <RobWorkStudioConfig.hpp>
#include <rws/RobWorkStudioPlugin.hpp>
#include <rw/common/Ptr.hpp>
#include "ui_textfinder.h"

#include <rw/loaders/rwxml/XMLParserUtil.hpp>
#include <rw/loaders/rwxml/XMLRWParser.hpp>
#include <rw/loaders/rwxml/XMLRWLoader.hpp>

#include <rw/kinematics/FixedFrame.hpp>
#include <rw/kinematics/MovableFrame.hpp>
#include <rw/models/RevoluteJoint.hpp>
#include <rw/models/PrismaticJoint.hpp>


namespace rw { namespace models { class Device; } }

namespace Ui {
class TextFinder;
}

class TextFinder : public rws::RobWorkStudioPlugin, private Ui::TextFinder
{
    Q_OBJECT

Q_INTERFACES( rws::RobWorkStudioPlugin )
#if RWS_USE_QT5
Q_PLUGIN_METADATA(IID "dk.sdu.mip.Robwork.RobWorkStudioPlugin/0.1" FILE "textfinder.json")
#endif
public:

    TextFinder(QWidget *parent = 0);
    virtual ~TextFinder();

private slots:
    void on_findButton_clicked();

private:
    Ui::TextFinder *ui;
    void loadTextFile();
};

#endif // TEXTFINDER_H
