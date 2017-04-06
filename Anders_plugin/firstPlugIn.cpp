#include "firstPlugIn.hpp"
//#include "listView.hpp"
#include <RobWorkStudio.hpp>
#include <rw/loaders/xml/XMLPropertyLoader.hpp>
#include <rw/loaders/xml/XMLPropertySaver.hpp>
#include <boost/filesystem.hpp>
#include <boost/bind.hpp>

using rw::kinematics::State;
using rw::models::WorkCell;
using rws::RobWorkStudioPlugin;

firstPlugIn::firstPlugIn():
	RobWorkStudioPlugin("firstPlugInUI", QIcon(":/pa_icon.png"))
{
	setupSettings();
	//create the widgets
	QScrollArea *widg = new QScrollArea(this);
	widg->setWidgetResizable(true);
	dockWidgetContent = new QWidget(this);
	verticalLayout = new QVBoxLayout(dockWidgetContent);
	_listView = new listView(dockWidgetContent);
	dirmodel = new QFileSystemModel(_listView);
    _listView->setModel(dirmodel);
	_listView->setRootIndex(dirmodel->setRootPath(_settingsMap->get<std::string>("Devices", "/").c_str()));
	//_selectionModel = _listView->selectionModel();


	//add menu bar, cant make a qmenubar apparently because linux sux with its global menubars
	_toolBar = new QToolBar(dockWidgetContent);

	_undoButton = new QToolButton();
	_undoButton->setIcon(QIcon(":/undo.png"));
	_undoButton->setToolTip("Undo");
	_settingsButton = new QToolButton();
	_settingsButton->setIcon(QIcon(":/settings.png"));
	_settingsButton->setToolTip("Settings");
	_settingsMenu = new QMenu("Settings menu");
	QAction *_settingsAction = new QAction("Libraries",this);
	_settingsMenu->addAction(_settingsAction);
	_settingsButton->setMenu(_settingsMenu);
	_settingsButton->setPopupMode(QToolButton::InstantPopup);
	_toolBar->addWidget(_undoButton);
	_toolBar->addWidget(_settingsButton);

	connect(_settingsAction, SIGNAL(triggered()), this, SLOT(settings()));

	//load buttons
	devBtn = new QPushButton("Devices",dockWidgetContent);
	geoBtn = new QPushButton("Geometries",dockWidgetContent);
	fraBtn = new QPushButton("Frames",dockWidgetContent);
	loadBtn = new QPushButton("Load",dockWidgetContent);

	//connect buttons
	connect(devBtn, SIGNAL(clicked()), this, SLOT(tabDevices()));
	connect(geoBtn, SIGNAL(clicked()), this, SLOT(tabGeometries()));
	connect(fraBtn, SIGNAL(clicked()), this, SLOT(tabFrames()));
	connect(loadBtn, SIGNAL(clicked()), this, SLOT(load()));

	//setup the layout
	verticalLayout->addWidget(_toolBar);
	verticalLayout->setAlignment(_toolBar,Qt::AlignRight);
	verticalLayout->addWidget(_listView);
	verticalLayout->addWidget(devBtn);
	verticalLayout->addWidget(geoBtn);
	verticalLayout->addWidget(fraBtn);
	verticalLayout->addSpacing(20);
	verticalLayout->addWidget(loadBtn);

	dockWidgetContent->setLayout(verticalLayout);

	widg->setWidget(dockWidgetContent);
	this->setWidget(widg);

}

firstPlugIn::~firstPlugIn()
{
}

void firstPlugIn::initialize() {
    	getRobWorkStudio()->stateChangedEvent().add(boost::bind(&firstPlugIn::stateChangedListener, this, _1), this);
}

void firstPlugIn::open(WorkCell* workcell)
{
	_workcell = workcell;
}

void firstPlugIn::close()
{
}

void firstPlugIn::setupSettings()
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

void firstPlugIn::mousePressEvent(QMouseEvent *event)
{
	//log().info() << "clikie clickie" << "\n";
}


void firstPlugIn::printLog(std::string text)
{
	log().info() << text << "\n";
}

void firstPlugIn::closeEvent( QCloseEvent * e)
{
	try {
		rw::loaders::XMLPropertySaver::save(_propMap, "eisettings.xml");
	} catch(const rw::common::Exception& e) {
		RW_WARN("Error saving settings file: " << e);
	} catch(...) {
		RW_WARN("Error saving settings file due to unknown exception!");
	}

	_propMap = rw::common::PropertyMap();
	e->accept();
}

void firstPlugIn::tabDevices()
{
	_listView->setRootIndex(dirmodel->setRootPath(_settingsMap->get<std::string>("Devices", "/").c_str()));
}

void firstPlugIn::tabGeometries()
{
	_listView->setRootIndex(dirmodel->setRootPath(_settingsMap->get<std::string>("Geometries", "/").c_str()));
}

void firstPlugIn::tabFrames()
{
	_listView->setRootIndex(dirmodel->setRootPath(_settingsMap->get<std::string>("Frames", "/").c_str()));
}

void firstPlugIn::load()
{
	QString st = "Load";
	QModelIndex index = _listView->currentIndex();
	QString itemText = dirmodel->filePath(index);

	_loadDialog = new dialog(itemText, st, dockWidgetContent);

	_loadDialog->addToDialog(_loadDialog->createNameBox());
	_loadDialog->addToDialog(_loadDialog->createConfigurationBox());
	_loadDialog->addToDialog(_loadDialog->createButtonBox());
	_loadDialog->open();
}

void firstPlugIn::settings()
{
	QString st = "Settings";

	_settingsDialog = new dialog(st, dockWidgetContent);

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
}

void firstPlugIn::printInfo(rw::models::WorkCell::Ptr wc) {
    log().info() << "Found " << wc->getFrames().size() << " frames:\n"; // List frames
    for (int i = 0; i < wc->getFrames().size(); i++) {
        log().info() << wc->getFrames()[i]->getName() << "\n";
    }
    log().info() << "\n";

    log().info() << "Found " << wc->getDevices().size() << " devices:\n"; // List devices
    for (int i = 0; i < wc->getDevices().size(); i++) {
        log().info() << wc->getDevices()[i]->getName() << "\n";
    }
    log().info() << "\n\n";
}

void firstPlugIn::stateChangedListener(const State& state)
{
}

#if !RWS_USE_QT5
#include <QtCore/qplugin.h>
Q_EXPORT_PLUGIN(firstPlugIn);
#endif
