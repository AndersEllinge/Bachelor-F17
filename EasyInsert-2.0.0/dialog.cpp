
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

    setWindowTitle(dialog);

}


dialog::dialog(rw::models::WorkCell::Ptr wc, QString dialog, QWidget *parent)
    : QDialog(parent)
{
    _workCell = wc;

    mainLayout = new QVBoxLayout();

    //setLayout(mainLayout);

    setWindowTitle(dialog);

}

void dialog::addToDialog(QWidget *widget)
{
    mainLayout->addWidget(widget);
    setLayout(mainLayout);
}

QWidget* dialog::createButtonBox()
{
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    return buttonBox;
}

QWidget* dialog::createNameBox()
{
    nameBox = new QGroupBox(tr("Name"));
    QHBoxLayout *layout = new QHBoxLayout;

    nameLine = new QLineEdit();
    layout->addWidget(nameLine);

    nameBox->setLayout(layout);

    return nameBox;
}

QWidget* dialog::createCheckFramesBox()
{
    checkBox = new QGroupBox(tr("Configurations:"));
    QGridLayout *layout = new QGridLayout();
    QButtonGroup *group = new QButtonGroup();


    checkFrames[0] = new QCheckBox("New Moveable Frame",this);
    checkFrames[1] = new QCheckBox("Existing Frame",this);

    group->addButton(checkFrames[0]);
    group->addButton(checkFrames[1]);

    checkFrames[0]->setChecked(true);

    layout->addWidget(checkFrames[0],0,0);
    layout->addWidget(checkFrames[1],0,1);

    checkBox->setLayout(layout);

    return checkBox;
}

QWidget* dialog::createConfigurationBox()
{
    configuration = new QGroupBox(tr("Configurations:"));
    QGridLayout *layout = new QGridLayout();

    QLabel *labels[6];

    labels[0] = new QLabel(tr("X"));
    labels[1] = new QLabel(tr("Y"));
    labels[2] = new QLabel(tr("Z"));
    labels[3] = new QLabel(tr("Roll"));
    labels[4] = new QLabel(tr("Pitch"));
    labels[5] = new QLabel(tr("Yaw"));

    doubleSpinBoxes[0] = makeDoubleSpinBox();
    doubleSpinBoxes[1] = makeDoubleSpinBox();
    doubleSpinBoxes[2] = makeDoubleSpinBox();
    doubleSpinBoxes[3] = makeDoubleSpinBox();
    doubleSpinBoxes[4] = makeDoubleSpinBox();
    doubleSpinBoxes[5] = makeDoubleSpinBox();

    for (size_t i = 0; i < 3; i++) {
        layout->addWidget(labels[i], i, 0);
        layout->addWidget(doubleSpinBoxes[i], i, 1);
        //layout->setAlignment(labels[i], Qt::AlignRight);
        //layout->setAlignment(doubleSpinBoxes[i], Qt::AlignLeft);
    }
    for (size_t i = 3; i < 6; i++) {
        layout->addWidget(labels[i], i-3, 2);
        layout->addWidget(doubleSpinBoxes[i], i-3, 3);
        //layout->setAlignment(labels[i], Qt::AlignRight);
        //layout->setAlignment(doubleSpinBoxes[i], Qt::AlignLeft);
    }

    layout->setColumnStretch(1, 10);
    layout->setColumnStretch(3, 10);
    configuration->setLayout(layout);

    return configuration;
}

QWidget* dialog::createConfigurationBoxCube()
{
    configurationCube = new QGroupBox(tr("Dimension sizes:"));
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

    //layout->setColumnStretch(1, 10);
    //layout->setColumnStretch(2, 20);
    configurationCube->setLayout(layout);

    return configurationCube;
}

QWidget* dialog::createConfigurationBoxSphere()
{
    configurationSphere = new QGroupBox(tr("Dimension sizes:"));
    QGridLayout *layout = new QGridLayout();

    QLabel *labels;

    labels = new QLabel(tr("Radius"));

    doubleSpinBoxesGeometires[0] = makeDoubleSpinBox();

    layout->addWidget(labels, 0, 0);
    layout->addWidget(doubleSpinBoxesGeometires[0], 0, 1);


    //layout->setColumnStretch(1, 10);
    //layout->setColumnStretch(2, 20);
    configurationSphere->setLayout(layout);

    return configurationSphere;
}

QWidget* dialog::createConfigurationBoxCone()
{
    configurationCone = new QGroupBox(tr("Dimension sizes:"));
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


    //layout->setColumnStretch(1, 10);
    //layout->setColumnStretch(2, 20);
    configurationCone->setLayout(layout);

    return configurationCone;
}

QWidget* dialog::createConfigurationBoxTube()
{
    configurationCone = new QGroupBox(tr("Dimension sizes:"));
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


    //layout->setColumnStretch(1, 10);
    //layout->setColumnStretch(2, 20);
    configurationCone->setLayout(layout);

    return configurationCone;
}

QWidget* dialog::createLibSettingsBox()
{
    setupSettings();
    //QSignalMapper *signalMapper = new QSignalMapper(this);

    //QLineEdit *pathLine[3];
    QPushButton* btns;
    QLabel *labels;
    QGroupBox *libSettingsbox = new QGroupBox(tr("Choose path for libraries: "));
    QGridLayout *layout = new QGridLayout();

    labels = new QLabel(tr("Devices"));

    pathLine = new QLineEdit();
    pathLine->setEchoMode(QLineEdit::Normal);
    pathLine->setReadOnly(true);
    layout->addWidget(labels, 0, 0);
    layout->addWidget(pathLine, 0, 1);
    btns = new QPushButton("...",this);
    btns->setFixedSize(25,25);

    /*  connect(btns[i], SIGNAL(clicked()), signalMapper, SLOT(map())); // this mapping is used when more than 1 lirbiary is set
        signalMapper->setMapping(btns[i], i);                           // because you cant pass integers with the slots
        layout->addWidget(btns[i], i + 1, 2);                           // so to enable this again, remember to add an parameter int to setdirdialog

    connect(signalMapper, SIGNAL(mapped(int)),this, SIGNAL(clicked(int)));
    connect(this, SIGNAL(clicked(int)),this, SLOT(setDirectoryDialog(int)));
    */
    layout->addWidget(btns, 0, 2);
    connect(btns, SIGNAL(clicked()),this, SLOT(setDirectoryDialog()));

    QString str1 = _settingsMap->get<std::string>("Devices", "/").c_str();

    pathLine->setText(str1);

    libSettingsbox->setLayout(layout);

    return libSettingsbox;
}

QWidget* dialog::createFrameSelection()
{
    selectFrame = new QGroupBox(tr("Select Frame:"));
    QGridLayout *layout = new QGridLayout();

    comboFrames = new QComboBox();

    std::vector<rw::kinematics::Frame*> frames = _workCell->getFrames();

    for (size_t i = 0; i < frames.size(); i++) {
        comboFrames->addItem(QString::fromStdString(frames[i]->getName()));
    }

    layout->addWidget(comboFrames);
    selectFrame->setLayout(layout);

    return selectFrame;

}

void dialog::reject()
{
    if (1) {
        QDialog::reject();
    }
}

void dialog::setupSettings()
{
    boost::filesystem::path settingsPath("eisettings.xml");
	rw::common::PropertyMap settings;
    if( exists(settingsPath) ){
        try {
            //settings = XMLPropertyLoader::load("rwsettings.xml");
            //_propMap.set<std::string>("SettingsFileName", "rwsettings.xml");
            _propMap = rw::loaders::XMLPropertyLoader::load("eisettings.xml");
        } catch(rw::common::Exception &e){
            RW_WARN("Could not load settings from 'rwsettings.xml': " << e.getMessage().getText() << "\n Using default settings!");
        } catch(std::exception &e){
            RW_WARN("Could not load settings from 'rwsettings.xml': " << e.what() << "\n Using default settings!");
            // loading failed so we just go on with an empty map
        }
    }
	rw::common::PropertyMap *currentSettings = _propMap.getPtr<rw::common::PropertyMap>("EasyInsertSettings");
	if(currentSettings==NULL){
        _propMap.add("EasyInsertSettings", "Settings for EasyInsert", settings);
        currentSettings = _propMap.getPtr<rw::common::PropertyMap>("EasyInsertSettings");
    }
	_settingsMap = _propMap.getPtr<rw::common::PropertyMap>("EasyInsertSettings");
}

void dialog::setDirectoryDialog()
{
    QString dir = QFileDialog::getExistingDirectory(
		this,
		tr("Open Directory"),
       	"/",
	    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    //if (i == 0) {
        _settingsMap->set<std::string>("Devices", dir.toStdString());
    //} else if (i == 1) {
    //    _settingsMap->set<std::string>("Geometries", dir.toStdString());
    //} else {
    //    _settingsMap->set<std::string>("Frames", dir.toStdString());
    //}

    QString str1 = _settingsMap->get<std::string>("Devices", "/").c_str();
    //QString str2 = _settingsMap->get<std::string>("Geometries", "/").c_str();
    //QString str3 = _settingsMap->get<std::string>("Frames", "/").c_str();

    pathLine->setText(str1);
    //pathLine[1]->setText(str2);
    //pathLine[2]->setText(str3);

}

QDoubleSpinBox* dialog::makeDoubleSpinBox()
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
