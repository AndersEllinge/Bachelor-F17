#include "EasyInsert.hpp"

#include <rw/loaders/xml/XMLPropertyLoader.hpp>
#include <rw/loaders/xml/XMLPropertySaver.hpp>
#include <boost/filesystem.hpp>
#include <boost/bind.hpp>

using rw::kinematics::State;
using rw::models::WorkCell;
using rws::RobWorkStudioPlugin;

EasyInsert::EasyInsert():
    RobWorkStudioPlugin("EasyInsert", QIcon(":/pa_icon.png"))
{
    //load settings from eisettings.xml
    setupSettings();

    //Creating the base of the plugin, everything is put "onto" this.
    QScrollArea *widg = new QScrollArea(this);
	widg->setWidgetResizable(true);
	QWidget *dockWidgetContent = new QWidget(this);
	QVBoxLayout *verticalLayout = new QVBoxLayout(dockWidgetContent);

    //create and add widgets below
    _toolBar = createToolBar();
    verticalLayout->addWidget(_toolBar);
    verticalLayout->setAlignment(_toolBar,Qt::AlignRight);

    QTabWidget *tabWindow = new QTabWidget(dockWidgetContent);
    _devTab = createDevTab();
    _geoTab = createGeoTab();
    tabWindow->addTab(_devTab, "Devices");
    tabWindow->addTab(_geoTab, "Geometries");
    verticalLayout->addWidget(tabWindow);

    //done with creating all the stuff, now set the layout for the base

    dockWidgetContent->setLayout(verticalLayout);
    widg->setWidget(dockWidgetContent);
	this->setWidget(widg);

}

EasyInsert::~EasyInsert()
{
}

void EasyInsert::initialize()
{
    getRobWorkStudio()->stateChangedEvent().add(boost::bind(&EasyInsert::stateChangedListener, this, _1), this);
}

void EasyInsert::open(WorkCell* workcell)
{
}

void EasyInsert::close()
{
}

void EasyInsert::setupSettings()
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

QToolBar* EasyInsert::createToolBar()
{
    QToolBar *toolBar = new QToolBar(this);

	QToolButton *undoButton = new QToolButton();
	undoButton->setIcon(QIcon(":/undo.png"));
	undoButton->setToolTip("Undo");
	QToolButton *settingsButton = new QToolButton();
	settingsButton->setIcon(QIcon(":/settings.png"));
	settingsButton->setToolTip("Settings");
	QMenu *settingsMenu = new QMenu("Settings menu");
	QAction *settingsAction = new QAction("Libraries",this);
	settingsMenu->addAction(settingsAction);
	settingsButton->setMenu(settingsMenu);
	settingsButton->setPopupMode(QToolButton::InstantPopup);
	toolBar->addWidget(undoButton);
	toolBar->addWidget(settingsButton);

    connect(settingsAction, SIGNAL(triggered()), this, SLOT(settings()));
    return toolBar;
}

QWidget* EasyInsert::createDevTab()
{
    QScrollArea *widg = new QScrollArea();
    widg->setWidgetResizable(true);
    widg->setFrameShape(QFrame::NoFrame);
    QWidget *devTab = new QWidget();
    QVBoxLayout *verticalLayout = new QVBoxLayout(devTab);
    view = new listView(devTab);
	dirmodel = new QFileSystemModel(view);
    view->setModel(dirmodel);
	view->setRootIndex(dirmodel->setRootPath(_settingsMap->get<std::string>("Devices", "/").c_str()));

    QPushButton *loadBtn = new QPushButton("Load",devTab);
    connect(loadBtn, SIGNAL(clicked()), this, SLOT(loadDevice()));

    verticalLayout->addWidget(view);
    verticalLayout->addWidget(loadBtn);


    devTab->setLayout(verticalLayout);
    widg->setWidget(devTab);
    return widg;
}

QWidget* EasyInsert::createGeoTab()
{
    QScrollArea *widg = new QScrollArea();
    widg->setWidgetResizable(true);
    widg->setFrameShape(QFrame::NoFrame);
    QWidget *geoTab = new QWidget();
    QGridLayout *layout = new QGridLayout(geoTab);

    QToolButton *btns[8];

    btns[0] = new QToolButton();
    btns[0]->setIcon(QIcon(":/cube.png"));
    btns[0]->setToolTip("Cube");
    btns[0]->setIconSize(QSize(50, 50));

    btns[1] = new QToolButton();
    btns[1]->setIcon(QIcon(":/cylinder.png"));
    btns[1]->setToolTip("Cylinder");
    btns[1]->setIconSize(QSize(50, 50));

    btns[2] = new QToolButton();
    btns[2]->setIcon(QIcon(":/cone.png"));
    btns[2]->setToolTip("Cone");
    btns[2]->setIconSize(QSize(50, 50));

    btns[3] = new QToolButton();
    btns[3]->setIcon(QIcon(":/plane.png"));
    btns[3]->setToolTip("Plane");
    btns[3]->setIconSize(QSize(50, 50));

    btns[4] = new QToolButton();
    btns[4]->setIcon(QIcon(":/sphere.png"));
    btns[4]->setToolTip("Sphere");
    btns[4]->setIconSize(QSize(50, 50));

    btns[5] = new QToolButton();
    btns[5]->setIcon(QIcon(":/tube.png"));
    btns[5]->setToolTip("Tube");
    btns[5]->setIconSize(QSize(50, 50));

    btns[6] = new QToolButton();
    btns[6]->setIcon(QIcon(":/undo.png"));
    btns[6]->setToolTip("Undo");
    btns[6]->setIconSize(QSize(50, 50));

    btns[7] = new QToolButton();
    btns[7]->setIcon(QIcon(":/undo.png"));
    btns[7]->setToolTip("Undo");
    btns[7]->setIconSize(QSize(50, 50));

    layout->addWidget(btns[0], 0, 0);
    layout->addWidget(btns[1], 1, 0);
    layout->addWidget(btns[2], 2, 0);
    layout->addWidget(btns[3], 3, 0);
    layout->addWidget(btns[4], 0, 1);
    layout->addWidget(btns[5], 1, 1);
    layout->addWidget(btns[6], 2, 1);
    layout->addWidget(btns[7], 3, 1);

    geoTab->setLayout(layout);
    widg->setWidget(geoTab);
    return widg;
}


void EasyInsert::settings()
{
    QString st = "Settings";

	_settingsDialog = new dialog(st, this);

	_settingsDialog->addToDialog(_settingsDialog->createLibSettingsBox());
	_settingsDialog->addToDialog(_settingsDialog->createButtonBox());
	_settingsDialog->exec();

	if (_settingsDialog->result() == QDialog::Accepted)
  	{
    	_settingsMap->set<std::string>("Devices", _settingsDialog->_settingsMap->get<std::string>("Devices", "/"));
		_settingsMap->set<std::string>("Geometries", _settingsDialog->_settingsMap->get<std::string>("Geometries", "/"));
		_settingsMap->set<std::string>("Frames", _settingsDialog->_settingsMap->get<std::string>("Frames", "/"));
  	}

	try {
		rw::loaders::XMLPropertySaver::save(_propMap, "eisettings.xml");
	} catch(const rw::common::Exception& e) {
		RW_WARN("Error saving settings file: " << e);
	} catch(...) {
		RW_WARN("Error saving settings file due to unknown exception!");
	}
    view->setRootIndex(dirmodel->setRootPath(_settingsMap->get<std::string>("Devices", "/").c_str()));
}

void EasyInsert::loadDevice()
{
    QString st = "Load";
	QModelIndex index = view->currentIndex();
	QString itemText = dirmodel->filePath(index);

	_loadDialog = new dialog(itemText, st, this);

	_loadDialog->addToDialog(_loadDialog->createNameBox());
	_loadDialog->addToDialog(_loadDialog->createConfigurationBox());
	_loadDialog->addToDialog(_loadDialog->createButtonBox());
	_loadDialog->open();
}

void EasyInsert::stateChangedListener(const State& state)
{
}


#if !RWS_USE_QT5
#include <QtCore/qplugin.h>
Q_EXPORT_PLUGIN(EasyInsert);
#endif
