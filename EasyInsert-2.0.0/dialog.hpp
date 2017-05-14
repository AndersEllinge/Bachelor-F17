#ifndef dialog_H
#define dialog_H

#include <rw/models/WorkCell.hpp>
#include <RobWorkStudioConfig.hpp>
#include <rws/RobWorkStudioPlugin.hpp>

#include <QDialog>
#include <QtWidgets>
#include <QScrollArea>

class QCheckBox;
class QSpacerItem;
class QVariant;
class QButtonGroup;
class QCheckBow;
class QComboBox;
class QLabel;
class QDoubleSpinBox;
class QPushButton;
class QLineEdit;
class QMenuBar;
class QGroupBox;
class QDialogButtonBox;
class QMenu;

class dialog : public QDialog
{
    Q_OBJECT

public:
    dialog(QString dialog,QWidget *parent = 0);
    dialog(rw::models::WorkCell::Ptr wc, QString dialog, QWidget *parent = 0);
    ~dialog();

    void addToDialog(QWidget *widget);
    QWidget* createButtonBox();
    QWidget* createNameBox();
    QWidget* createCheckFramesBox();
    QWidget* createConfigurationBox();
    QWidget* createConfigurationBoxCube();
    QWidget* createConfigurationBoxSphere();
    QWidget* createConfigurationBoxCone();
    QWidget* createConfigurationBoxTube();
    QWidget* createLibSettingsBox(rw::common::PropertyMap *map);
    QWidget* createFrameSelection();

    rw::common::PropertyMap _propMap;
    rw::common::PropertyMap *_settingsMap;

    std::string getNameBox();
    std::string getFrameSelection();
    double getDisplacementX();
    double getDisplacementY();
    double getDisplacementZ();
    double getRotationR();
    double getRotationP();
    double getRotationY();
    double getGeometriX();
    double getGeometriY();
    double getGeometriZ();
    bool getCheckFrameMoveable();



private:
    void reject();
    void setupSettings(); //Not required anymore
    QDoubleSpinBox* makeDoubleSpinBox();

    QVBoxLayout *mainLayout;

    QLineEdit *nameLine;
    QComboBox *comboFrames;
    QDoubleSpinBox *doubleSpinBoxes[6];
    QDoubleSpinBox *doubleSpinBoxesGeometires[3];
    QCheckBox *checkFrames[2];

    QLineEdit *pathLine;

    QDialogButtonBox *buttonBox;

    rw::models::WorkCell::Ptr _workCell;

private slots:
    void setDirectoryDialog();


signals:
    void clicked(int);


};

#endif
