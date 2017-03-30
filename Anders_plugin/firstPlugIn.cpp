#include "firstPlugIn.hpp"
//#include "listView.hpp"
#include <RobWorkStudio.hpp>
#include <boost/bind.hpp>




using rw::kinematics::State;
using rw::models::WorkCell;
using rws::RobWorkStudioPlugin;

firstPlugIn::firstPlugIn():
	RobWorkStudioPlugin("firstPlugInUI", QIcon(":/pa_icon.png"))
{
	//create the widgets
	QScrollArea *widg = new QScrollArea(this);
	widg->setWidgetResizable(true);
	dockWidgetContent = new QWidget(this);
	verticalLayout = new QVBoxLayout(dockWidgetContent);
	_listView = new listView(dockWidgetContent);
	dirmodel = new QFileSystemModel(_listView);
    _listView->setModel(dirmodel);
	_listView->setRootIndex(dirmodel->setRootPath(sPath));
	//_selectionModel = _listView->selectionModel();

	//add menu bar, cant make a qmenubar apparently because linux sux with its global menubars
	_toolBar = new QToolBar(dockWidgetContent);

	_settingsButton = new QToolButton();
	_settingsButton->setIcon(QIcon(":/settings.png"));
	_settingsMenu = new QMenu("Settings menu");
	QAction *_settingsAction = new QAction("test menu item",this);
	_settingsMenu->addAction(_settingsAction);
	_settingsButton->setMenu(_settingsMenu);
	_settingsButton->setPopupMode(QToolButton::InstantPopup);
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

void firstPlugIn::mousePressEvent(QMouseEvent *event)
{
	log().info() << "clikie clickie" << "\n";
}

void firstPlugIn::printLog(std::string text)
{
	log().info() << text << "\n";
}

void firstPlugIn::tabDevices()
{
	sPath = "/usr/local/RobWork/example/ModelData";
	_listView->setRootIndex(dirmodel->setRootPath(sPath));
}

void firstPlugIn::tabGeometries()
{
	sPath = "/usr/local/RobWork";
	_listView->setRootIndex(dirmodel->setRootPath(sPath));
}

void firstPlugIn::tabFrames()
{
	sPath = "/usr/local/RobWork/example/ModelData/XMLDevices/Fanuc-LRM200i";
	_listView->setRootIndex(dirmodel->setRootPath(sPath));
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

	_settingsDialog->addToDialog(_settingsDialog->createButtonBox());
	_settingsDialog->open();
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
