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
    QWidget* createLibSettingsBox();
    QWidget* createFrameSelection();

    rw::common::PropertyMap _propMap;
    rw::common::PropertyMap *_settingsMap;

    QDoubleSpinBox *doubleSpinBoxes[6];
    QDoubleSpinBox *doubleSpinBoxesGeometires[3];
    QLineEdit *nameLine;
    QComboBox *comboFrames;
    QCheckBox *checkFrames[2];

private:
    void reject();
    void setupSettings();
    QDoubleSpinBox* makeDoubleSpinBox();

    QGroupBox *nameBox;
    QGroupBox *configuration;
    QGroupBox *configurationCube;
    QGroupBox *configurationSphere;
    QGroupBox *configurationCone;
    QGroupBox *selectFrame;
    QGroupBox *checkBox;


    QVBoxLayout *mainLayout;

    QLineEdit *pathLine;

    QDialogButtonBox *buttonBox;

    rw::models::WorkCell::Ptr _workCell;

private slots:
    void setDirectoryDialog();


signals:
    void clicked(int);


};

#endif
