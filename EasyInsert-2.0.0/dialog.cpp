
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


dialog::dialog(QString filePath, QString dialog, QWidget *parent)
    : QDialog(parent)
{
    mainLayout = new QVBoxLayout();

    setLayout(mainLayout);

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

QWidget* dialog::createConfigurationBox()
{
    configuration = new QGroupBox(tr("Configurations:"));
    QGridLayout *layout = new QGridLayout();
    for (int i = 0; i < 3; ++i)
    {
        if ( i == 1 )
            labels[i] = new QLabel(tr("X"));
        else if ( i == 2 )
            labels[i] = new QLabel(tr("Y"));
        else
            labels[i] = new QLabel(tr("Z"));

        doubleSpinBoxes[i] = new QDoubleSpinBox();
        layout->addWidget(labels[i], i + 1, 0);
        layout->addWidget(doubleSpinBoxes[i], i + 1, 1);
    }

    layout->setColumnStretch(1, 10);
    layout->setColumnStretch(2, 20);
    configuration->setLayout(layout);

    return configuration;
}

QWidget* dialog::createLibSettingsBox()
{
    setupSettings();
    QSignalMapper *signalMapper = new QSignalMapper(this);

    //QLineEdit *pathLine[3];
    QPushButton* btns[3];
    QGroupBox *libSettingsbox = new QGroupBox(tr("Choose path for libraries:"));
    QGridLayout *layout = new QGridLayout();
    for (int i = 0; i < 3; ++i)
    {
        if ( i == 0 )
            labels[i] = new QLabel(tr("Devices"));
        else if ( i == 1 )
            labels[i] = new QLabel(tr("Geometries"));
        else
            labels[i] = new QLabel(tr("Frames"));

        pathLine[i] = new QLineEdit();
        pathLine[i]->setEchoMode(QLineEdit::Normal);
        pathLine[i]->setReadOnly(true);
        layout->addWidget(labels[i], i + 1, 0);
        layout->addWidget(pathLine[i], i + 1, 1);
        btns[i] = new QPushButton("...",this);
        btns[i]->setFixedSize(25,25);

        connect(btns[i], SIGNAL(clicked()), signalMapper, SLOT(map()));
        signalMapper->setMapping(btns[i], i);
        layout->addWidget(btns[i], i + 1, 2);
    }
    connect(signalMapper, SIGNAL(mapped(int)),this, SIGNAL(clicked(int)));
    connect(this, SIGNAL(clicked(int)),this, SLOT(setDirectoryDialog(int)));

    QString str1 = _settingsMap->get<std::string>("Devices", "/").c_str();
    QString str2 = _settingsMap->get<std::string>("Geometries", "/").c_str();
    QString str3 = _settingsMap->get<std::string>("Frames", "/").c_str();

    pathLine[0]->setText(str1);
    pathLine[1]->setText(str2);
    pathLine[2]->setText(str3);

    //layout->setColumnStretch(1, 10);
    //layout->setColumnStretch(2, 20);
    //layout->setColumnStretch(3, 10);
    libSettingsbox->setLayout(layout);

    return libSettingsbox;
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

void dialog::setDirectoryDialog(int i)
{
    QString dir = QFileDialog::getExistingDirectory(
		this,
		tr("Open Directory"),
       	"/",
	    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (i == 0) {
        _settingsMap->set<std::string>("Devices", dir.toStdString());
    } else if (i == 1) {
        _settingsMap->set<std::string>("Geometries", dir.toStdString());
    } else {
        _settingsMap->set<std::string>("Frames", dir.toStdString());
    }
    
    QString str1 = _settingsMap->get<std::string>("Devices", "/").c_str();
    QString str2 = _settingsMap->get<std::string>("Geometries", "/").c_str();
    QString str3 = _settingsMap->get<std::string>("Frames", "/").c_str();

    pathLine[0]->setText(str1);
    pathLine[1]->setText(str2);
    pathLine[2]->setText(str3);

}

dialog::~dialog()
{

}
