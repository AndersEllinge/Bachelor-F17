
#include "dialog.hpp"
#include <rw/loaders/xml/XMLPropertyLoader.hpp>
#include <rw/loaders/xml/XMLPropertySaver.hpp>
#include <boost/filesystem.hpp>
#include <boost/bind.hpp>


dialog::dialog(QString dialog, QWidget *parent)
    : QDialog(parent)
{
    mainLayout = new QVBoxLayout();

    setLayout(mainLayout);

    setWindowTitle(dialog); //title of the dialog window

}


dialog::dialog(rw::models::WorkCell::Ptr wc, QString dialog, QWidget *parent)
    : QDialog(parent)
{
    _workCell = wc;

    mainLayout = new QVBoxLayout();

    setWindowTitle(dialog); //title of the dialog window

}

void dialog::addToDialog(QWidget *widget)
{
    mainLayout->addWidget(widget); //add the widget to the main layout
    setLayout(mainLayout);
}

QWidget* dialog::createButtonBox()
{
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel); //create ok and cancel buttons
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept())); //ok --> accept
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject())); //cancel --> reject

    return buttonBox;
}

QWidget* dialog::createNameBox()
{
    QGroupBox* nameBox = new QGroupBox(tr("Name")); //container widget, has name and a frame
    QHBoxLayout *layout = new QHBoxLayout; //layout for the container widget

    nameLine = new QLineEdit(); // a line edit to type in
    layout->addWidget(nameLine); //add to container wiget

    nameBox->setLayout(layout); //set container layout

    return nameBox;
}

QWidget* dialog::createCheckFramesBox()
{
    QGroupBox* checkBox = new QGroupBox(tr("Configurations:")); //container widget, has name and a frame
    QGridLayout *layout = new QGridLayout(); // layout for the container widget
    QButtonGroup *group = new QButtonGroup();  //check able buttons.


    checkFrames[0] = new QCheckBox("New Moveable Frame",this);//we need 2 check buttons
    checkFrames[1] = new QCheckBox("Existing Frame",this);//we ned 2 check buttons

    group->addButton(checkFrames[0]); // add them to the group
    group->addButton(checkFrames[1]); // add the to the group

    checkFrames[0]->setChecked(true); //we allways want one true, exclusive by default

    layout->addWidget(checkFrames[0],0,0);//add button
    layout->addWidget(checkFrames[1],0,1);//add button

    checkBox->setLayout(layout);

    return checkBox;
}

QWidget* dialog::createConfigurationBox()
{
    QGroupBox* configuration = new QGroupBox(tr("Configurations:"));
    QGridLayout *layout = new QGridLayout();

    QLabel *labels[6]; //create some labels so we can identify stuff

    labels[0] = new QLabel(tr("X"));
    labels[1] = new QLabel(tr("Y"));
    labels[2] = new QLabel(tr("Z"));
    labels[3] = new QLabel(tr("Roll"));
    labels[4] = new QLabel(tr("Pitch"));
    labels[5] = new QLabel(tr("Yaw"));

    doubleSpinBoxes[0] = makeDoubleSpinBox();   //make all the clickie things to increase the value
    doubleSpinBoxes[1] = makeDoubleSpinBox();
    doubleSpinBoxes[2] = makeDoubleSpinBox();
    doubleSpinBoxes[3] = makeDoubleSpinBox();
    doubleSpinBoxes[4] = makeDoubleSpinBox();
    doubleSpinBoxes[5] = makeDoubleSpinBox();

    for (size_t i = 0; i < 3; i++) {    //adding the x y z label and spinbox
        layout->addWidget(labels[i], i, 0);
        layout->addWidget(doubleSpinBoxes[i], i, 1);
    }
    for (size_t i = 3; i < 6; i++) { //adding the RPY label and spinbox
        layout->addWidget(labels[i], i-3, 2);
        layout->addWidget(doubleSpinBoxes[i], i-3, 3);
    }

    layout->setColumnStretch(1, 10); //fixed size
    layout->setColumnStretch(3, 10);
    configuration->setLayout(layout);

    return configuration;
}

QWidget* dialog::createConfigurationBoxCube()
{
    QGroupBox* configurationCube = new QGroupBox(tr("Dimension sizes:"));
    QGridLayout *layout = new QGridLayout();

    QLabel *labels[3];

    labels[0] = new QLabel(tr("X"));
    labels[1] = new QLabel(tr("Y"));
    labels[2] = new QLabel(tr("Z"));


    doubleSpinBoxesGeometires[0] = makeDoubleSpinBox();
    doubleSpinBoxesGeometires[1] = makeDoubleSpinBox();
    doubleSpinBoxesGeometires[2] = makeDoubleSpinBox();

    for (size_t i = 0; i < 3; i++) {
        layout->addWidget(labels[i], i, 0);
        layout->addWidget(doubleSpinBoxesGeometires[i], i, 1);
    }
    configurationCube->setLayout(layout);

    return configurationCube;
}

QWidget* dialog::createConfigurationBoxSphere()
{
    QGroupBox* configurationSphere = new QGroupBox(tr("Dimension sizes:"));
    QGridLayout *layout = new QGridLayout();

    QLabel *labels;

    labels = new QLabel(tr("Radius"));

    doubleSpinBoxesGeometires[0] = makeDoubleSpinBox();

    layout->addWidget(labels, 0, 0);
    layout->addWidget(doubleSpinBoxesGeometires[0], 0, 1);

    configurationSphere->setLayout(layout);

    return configurationSphere;
}

QWidget* dialog::createConfigurationBoxCone()
{
    QGroupBox* configurationCone = new QGroupBox(tr("Dimension sizes:"));
    QGridLayout *layout = new QGridLayout();

    QLabel *labels[2];

    labels[0] = new QLabel(tr("Radius"));
    labels[1] = new QLabel(tr("Height"));

    doubleSpinBoxesGeometires[0] = makeDoubleSpinBox();
    doubleSpinBoxesGeometires[1] = makeDoubleSpinBox();

    layout->addWidget(labels[0], 0, 0);
    layout->addWidget(doubleSpinBoxesGeometires[0], 0, 1);

    layout->addWidget(labels[1], 1, 0);
    layout->addWidget(doubleSpinBoxesGeometires[1], 1, 1);

    configurationCone->setLayout(layout);

    return configurationCone;
}

QWidget* dialog::createConfigurationBoxTube()
{
    QGroupBox* configurationCone = new QGroupBox(tr("Dimension sizes:"));
    QGridLayout *layout = new QGridLayout();

    QLabel *labels[2];

    labels[0] = new QLabel(tr("Radius"));
    labels[1] = new QLabel(tr("Thickness"));
    labels[2] = new QLabel(tr("Height"));

    doubleSpinBoxesGeometires[0] = makeDoubleSpinBox();
    doubleSpinBoxesGeometires[1] = makeDoubleSpinBox();
    doubleSpinBoxesGeometires[2] = makeDoubleSpinBox();

    layout->addWidget(labels[0], 0, 0);
    layout->addWidget(doubleSpinBoxesGeometires[0], 0, 1);

    layout->addWidget(labels[1], 1, 0);
    layout->addWidget(doubleSpinBoxesGeometires[1], 1, 1);

    layout->addWidget(labels[2], 2, 0);
    layout->addWidget(doubleSpinBoxesGeometires[2], 2, 1);

    configurationCone->setLayout(layout);

    return configurationCone;
}

QWidget* dialog::createLibSettingsBox(rw::common::PropertyMap *map)
{
    _settingsMap = map; //the propertymap

    QPushButton* btns;  // the [...] button
    QLabel *labels; //a label for line edit
    QGroupBox *libSettingsbox = new QGroupBox(tr("Choose path for libraries: "));
    QGridLayout *layout = new QGridLayout();

    labels = new QLabel(tr("Devices"));

    pathLine = new QLineEdit(); //line edit with the library path
    pathLine->setReadOnly(true); //must chose the path with the [...] btn
    layout->addWidget(labels, 0, 0);
    layout->addWidget(pathLine, 0, 1);
    btns = new QPushButton("...",this);
    btns->setFixedSize(25,25);

    layout->addWidget(btns, 0, 2);
    connect(btns, SIGNAL(clicked()),this, SLOT(setDirectoryDialog()));

    QString str1 = _settingsMap->get<std::string>("Devices", "/").c_str(); //read the settings of the map

    pathLine->setText(str1);    // set the path

    libSettingsbox->setLayout(layout);

    return libSettingsbox;
}

QWidget* dialog::createFrameSelection()
{
    QGroupBox* selectFrame = new QGroupBox(tr("Select Frame:"));
    QGridLayout *layout = new QGridLayout();

    comboFrames = new QComboBox(); // drop down select thingie

    std::vector<rw::kinematics::Frame*> frames = _workCell->getFrames(); // we want to be able to select all frames

    for (size_t i = 0; i < frames.size(); i++) {
        comboFrames->addItem(QString::fromStdString(frames[i]->getName())); //add them all to the combobox
    }

    layout->addWidget(comboFrames);
    selectFrame->setLayout(layout);

    return selectFrame;

}

std::string dialog::getNameBox()
{
    return nameLine->text().toStdString();
}

std::string dialog::getFrameSelection()
{
    return comboFrames->currentText().toStdString();
}

double dialog::getDisplacementX()
{
    return doubleSpinBoxes[0]->value();
}

double dialog::getDisplacementY()
{
    return doubleSpinBoxes[1]->value();
}

double dialog::getDisplacementZ()
{
    return doubleSpinBoxes[2]->value();
}

double dialog::getRotationR()
{
    return doubleSpinBoxes[3]->value();
}

double dialog::getRotationP()
{
    return doubleSpinBoxes[4]->value();
}

double dialog::getRotationY()
{
    return doubleSpinBoxes[5]->value();
}

double dialog::getGeometriX()
{
    return doubleSpinBoxesGeometires[0]->value();
}

double dialog::getGeometriY()
{
    return doubleSpinBoxesGeometires[1]->value();
}

double dialog::getGeometriZ()
{
    return doubleSpinBoxesGeometires[2]->value();
}

bool dialog::getCheckFrameMoveable()
{
    return checkFrames[0]->isChecked();
}

std::string dialog::getSettingsMap()
{
    return _settingsMap->get<std::string>("Devices", "/");
}

void dialog::reject()
{
    if (1) {
        QDialog::reject();
    }
}

void dialog::setDirectoryDialog()
{
    QString dir = QFileDialog::getExistingDirectory( //this just returns a path the user selectes
		this,
		tr("Open Directory"),
       	pathLine->text(),
	    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (dir != "") { //if the user cancels, then nothing is chosen
        _settingsMap->set("Devices", dir.toStdString()); //update the settings
        pathLine->setText(dir); //update the path
    }

}

QDoubleSpinBox* dialog::makeDoubleSpinBox() //small utility function to help make spinboxes
{
    QDoubleSpinBox* box = new QDoubleSpinBox();
    box->setDecimals(3);
    box->setRange(-100, 100);

    box->setSingleStep(0.001);

    return box;
}

dialog::~dialog()
{

}
