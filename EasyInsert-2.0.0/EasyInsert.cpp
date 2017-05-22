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
    tabWindow->addTab(createDevTab(), "Devices");
    tabWindow->addTab(createGeoTab(), "Geometries");
    tabWindow->addTab(createDeleteTab(), "Delete");
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
    //connect the stateChangedListener
    getRobWorkStudio()->stateChangedEvent().add(boost::bind(&EasyInsert::stateChangedListener, this, _1), this);
}



void EasyInsert::open(WorkCell* workcell)
{
    _workcell = workcell;
    _state = getRobWorkStudio()->getState();

    showFrameStructure(); //update the views of the deletetab
}

void EasyInsert::close()
{
    clearListContent(); //might as well clear stuff related to the delete tab, might not be needed
    try {
		rw::loaders::XMLPropertySaver::save(_propMap, "eisettings.xml"); //save the settings
	} catch(const rw::common::Exception& e) {
		RW_WARN("Error saving settings file: " << e);
	} catch(...) {
		RW_WARN("Error saving settings file due to unknown exception!");
	}
    _workcell = NULL;
}

void EasyInsert::setupSettings()
{
    boost::filesystem::path settingsPath("eisettings.xml");//find the settings file
    if( exists(settingsPath) ){ //if it exists
        try {
            _propMap = rw::loaders::XMLPropertyLoader::load("eisettings.xml"); //load the settings file
        } catch(rw::common::Exception &e){
            RW_WARN("Could not load settings from 'eisettings.xml': " << e.getMessage().getText() << "\n Using default settings!");
        } catch(std::exception &e){
            RW_WARN("Could not load settings from 'eisettings.xml': " << e.what() << "\n Using default settings!");
        }
    }
	_settingsMap = _propMap.getPtr<rw::common::PropertyMap>("EasyInsertSettings"); // save a pointer to the EI settings
    if(_settingsMap==NULL){ // check if it point to anything
        _propMap.add("EasyInsertSettings", "Settings for EasyInsert", rw::common::PropertyMap()); //add a EI property if it does not
        _settingsMap = _propMap.getPtr<rw::common::PropertyMap>("EasyInsertSettings"); //update the pointer
    }
}

QToolBar* EasyInsert::createToolBar()
{
    QToolBar *toolBar = new QToolBar(this);
	QToolButton *settingsButton = new QToolButton();
	settingsButton->setIcon(QIcon(":/settings.png"));
	settingsButton->setToolTip("Settings");
	QMenu *settingsMenu = new QMenu("Settings menu"); //the settings menu
	QAction *settingsAction = new QAction("Libraries",this); // the action when user clicks the settings menu
	settingsMenu->addAction(settingsAction); // add the action to the menu
	settingsButton->setMenu(settingsMenu); // add the menu to the settings button
	settingsButton->setPopupMode(QToolButton::InstantPopup);
	toolBar->addWidget(settingsButton);
    connect(settingsAction, SIGNAL(triggered()), this, SLOT(settings())); //connect the action of the menu to the settings slot
    return toolBar;
}

QWidget* EasyInsert::createDevTab()
{
    QScrollArea *widg = new QScrollArea();
    widg->setWidgetResizable(true);
    widg->setFrameShape(QFrame::NoFrame); // looks ugly
    QWidget *devTab = new QWidget();
    QVBoxLayout *verticalLayout = new QVBoxLayout(devTab);
    view = new QListView(devTab); //create the view
	dirmodel = new QFileSystemModel(view); // create the model
    view->setModel(dirmodel); // set the model

    //set the view root to the models root, also the models root is set to devices path from the settings
    view->setRootIndex(dirmodel->setRootPath(QString::fromStdString(_settingsMap->get<std::string>("Devices", "/"))));

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
    //widg->setWidgetResizable(true);
    widg->setFrameShape(QFrame::NoFrame);
    QWidget *geoTab = new QWidget();
    QGridLayout *layout = new QGridLayout(geoTab);

    QToolButton *btns[8];

    btns[0] = new QToolButton();
    btns[0]->setIcon(QIcon(":icons/cube.png"));
    btns[0]->setToolTip("Cube");
    btns[0]->setIconSize(QSize(50, 50));
    connect(btns[0], SIGNAL(clicked()), this, SLOT(cube()));

    btns[1] = new QToolButton();
    btns[1]->setIcon(QIcon(":icons/cylinder.png"));
    btns[1]->setToolTip("Cylinder");
    btns[1]->setIconSize(QSize(50, 50));
    connect(btns[1], SIGNAL(clicked()), this, SLOT(cylinder()));

    btns[2] = new QToolButton();
    btns[2]->setIcon(QIcon(":icons/cone.png"));
    btns[2]->setToolTip("Cone");
    btns[2]->setIconSize(QSize(50, 50));
    connect(btns[2], SIGNAL(clicked()), this, SLOT(cone()));

    btns[3] = new QToolButton();
    btns[3]->setIcon(QIcon(":icons/plane.png"));
    btns[3]->setToolTip("Plane");
    btns[3]->setIconSize(QSize(50, 50));
    connect(btns[3], SIGNAL(clicked()), this, SLOT(plane()));

    btns[4] = new QToolButton();
    btns[4]->setIcon(QIcon(":icons/sphere.png"));
    btns[4]->setToolTip("Sphere");
    btns[4]->setIconSize(QSize(50, 50));
    connect(btns[4], SIGNAL(clicked()),this, SLOT(sphere()));

    btns[5] = new QToolButton();
    btns[5]->setIcon(QIcon(":icons/tube.png"));
    btns[5]->setToolTip("Tube");
    btns[5]->setIconSize(QSize(50, 50));
    connect(btns[5], SIGNAL(clicked()),this, SLOT(tube()));

    btns[6] = new QToolButton();
    btns[6]->setIcon(QIcon(":icons/fixed.png"));
    btns[6]->setToolTip("Fixed Frame");
    btns[6]->setIconSize(QSize(50, 50));
    connect(btns[6], SIGNAL(clicked()),this, SLOT(fixedFrame()));

    btns[7] = new QToolButton();
    btns[7]->setIcon(QIcon(":icons/movable.png"));
    btns[7]->setToolTip("Movable Frame");
    btns[7]->setIconSize(QSize(50, 50));
    connect(btns[7], SIGNAL(clicked()),this, SLOT(movableFrame()));

    layout->addWidget(btns[0], 0, 0);
    layout->addWidget(btns[1], 0, 1);
    layout->addWidget(btns[2], 1, 0);
    layout->addWidget(btns[3], 1, 1);
    layout->addWidget(btns[4], 2, 0);
    layout->addWidget(btns[5], 2, 1);
    layout->addWidget(btns[6], 3, 0);
    layout->addWidget(btns[7], 3, 1);

    //layout->setColumnMinimumWidth(0,50);
    //layout->setColumnMinimumWidth(1,50);

    geoTab->setLayout(layout);
    widg->setWidget(geoTab);
    return widg;
}

QWidget* EasyInsert::createDeleteTab()
{
    QScrollArea *widg = new QScrollArea();
    widg->setWidgetResizable(true);
    widg->setFrameShape(QFrame::NoFrame);
    QWidget *devTab = new QWidget();
    QVBoxLayout *verticalLayout = new QVBoxLayout(devTab);

    QGroupBox *delDevBox = new QGroupBox(tr("Devices:"));
    QGroupBox *delFrameBox = new QGroupBox(tr("Frames:"));
    QGroupBox *delObjBox = new QGroupBox(tr("Objects:"));
    QGridLayout *layoutDev = new QGridLayout();
    QGridLayout *layoutObj = new QGridLayout();
    QGridLayout *layoutFrame = new QGridLayout();
    QVBoxLayout *delButtons = new QVBoxLayout(); // need a layout for the 2 buttons for delete frame

    _deviceWidget = new QListWidget(this); //container widgets
    _objectWidget = new QListWidget(this);
    _frameWidget = new QListWidget(this);
    _frameDelBtns = new QWidget(); // container widget for the 2 buttons

    QLabel *singleDel = new QLabel(this); // some labels for the 2 delete frame buttons
    singleDel->setText("Single");
    QLabel *recurvDel = new QLabel(this);
    recurvDel->setText("Recursive");


    //createing the buttons and connecting them all
    QPushButton *delDevBtn = new QPushButton("Delete",devTab);
    connect(delDevBtn, SIGNAL(clicked()), this, SLOT(deleteDev()));
    QPushButton *delObjBtn = new QPushButton("Delete",devTab);
    connect(delObjBtn, SIGNAL(clicked()), this, SLOT(deleteObj()));
    QPushButton *delSingleBtn = new QPushButton("Delete",devTab);
    connect(delSingleBtn, SIGNAL(clicked()), this, SLOT(deleteSingleFrame()));
    QPushButton *delRecursiveBtn = new QPushButton("Delete",devTab);
    connect(delRecursiveBtn, SIGNAL(clicked()), this, SLOT(deleteFrame()));

    layoutDev->addWidget(_deviceWidget,0,0);
    layoutDev->addWidget(delDevBtn,0,1);
    delDevBox->setLayout(layoutDev);

    layoutObj->addWidget(_objectWidget,0,0);
    layoutObj->addWidget(delObjBtn,0,1);
    delObjBox->setLayout(layoutObj);

    delButtons->addWidget(singleDel);
    delButtons->addWidget(delSingleBtn);
    delButtons->addWidget(recurvDel);
    delButtons->addWidget(delRecursiveBtn);
    _frameDelBtns->setLayout(delButtons);
    layoutFrame->addWidget(_frameWidget,0,0);
    layoutFrame->addWidget(_frameDelBtns,0,1);
    delFrameBox->setLayout(layoutFrame);

    verticalLayout->addWidget(delDevBox);
    verticalLayout->addWidget(delObjBox);
    verticalLayout->addWidget(delFrameBox);

    devTab->setLayout(verticalLayout);
    widg->setWidget(devTab);
    return widg;
}


void EasyInsert::showFrameStructure()
{
    std::cout << "showFrameStructure running:" << std::endl;
    clearListContent(); //clear everything used in the delete tab
    if (_workcell != NULL) { // only if valid workcell
        const std::vector<rw::models::Device::Ptr>& devices = _workcell->getDevices(); //get devices
        typedef std::vector<rw::models::Device::Ptr>::const_iterator MI; // make iterator
        for (MI it = devices.begin(); it != devices.end(); ++it) { // go through the devices
            rw::models::SerialDevice::Ptr sdevice = (*it).cast<rw::models::SerialDevice>(); // try and cast the device to a serial device
            if (sdevice) { // if the cast worked
                QListWidgetItem* deviceItem = new QListWidgetItem(); //create item to add
                _deviceWidget->addItem(deviceItem); // add it to the device view
                deviceItem->setText(sdevice->getName().c_str()); // set the text to the device name
                const std::vector<rw::kinematics::Frame*> frames = sdevice->frames(); //get all the frames of the device
                for (size_t i = 0; i < frames.size(); i++) {   //save them for later
                    _devFrameList.push_back(frames[i]);
                }
                continue;
            }
            rw::models::TreeDevice::Ptr tdevice = (*it).cast<rw::models::TreeDevice>();
            if(tdevice){
                QListWidgetItem* deviceItem = new QListWidgetItem();
                _deviceWidget->addItem(deviceItem); // own deviceItem
                deviceItem->setText(tdevice->getName().c_str());
                const std::vector<rw::kinematics::Frame*> frames = sdevice->frames();
                for (size_t i = 0; i < frames.size(); i++) {
                    _devFrameList.push_back(frames[i]);
                }
                continue;
            }
        }
    }

    if (_workcell != NULL) {
        _frames = _workcell->getFrames(); // get all the frames of the workcell

        for (size_t i = 0; i < _devFrameList.size(); i++) { //compare them with the device frames
            for (size_t j = 0; j < _frames.size(); j++) {
                if (_frames[j]->getName() == _devFrameList[i]->getName()){  //remove those damn device frames
                    _frames.erase(_frames.begin()+j);
                    break;
                }
            }
        }
        for (size_t i = 0; i < _frames.size(); i++) {   //go through the remaining frames
            QListWidgetItem* deviceItem = new QListWidgetItem();    //create items for em
            _frameWidget->addItem(deviceItem); // add it to the frames view
            deviceItem->setText(_frames[i]->getName().c_str()); //set the text to the frame name
        }
    }

    if (_workcell != NULL) {
        std::vector<rw::models::Object::Ptr> object = _workcell->getObjects(); //get all objects of the workcell
        for (size_t i = 0; i < _devFrameList.size(); i++) { //compare with device frames
            bool removeObj = false;
            for (size_t j = 0; j < object.size() && !removeObj; j++) {  // compare with all the objects
                const std::vector <rw::kinematics::Frame*>& objectFrames = object[j].get()->getFrames(); // get all the frames associated with an object
                for (size_t k = 0; k < objectFrames.size() && !removeObj; k++) {   // go through those frames
                    if (objectFrames[k] == _devFrameList[i]) {  //remove those objects that are part of a device
                        object.erase(object.begin()+j);
                        removeObj = true;
                    }
                }
            }
        }
        for (size_t i = 0; i < object.size(); i++) {    //now add those objects to the object view of the delete tab
            QListWidgetItem* deviceItem = new QListWidgetItem();
            _objectWidget->addItem(deviceItem);
            deviceItem->setText(object[i].get()->getName().c_str());
        }
    }

}


void EasyInsert::settings()
{
    QString st = "Settings";

	dialog* settingsDialog = new dialog(st, this);

	settingsDialog->addToDialog(settingsDialog->createLibSettingsBox(_settingsMap));
	settingsDialog->addToDialog(settingsDialog->createButtonBox());
	settingsDialog->exec();

	if (settingsDialog->result() == QDialog::Accepted)
  	{
        try {
    		rw::loaders::XMLPropertySaver::save(_propMap, "eisettings.xml");
    	} catch(const rw::common::Exception& e) {
    		RW_WARN("Error saving settings file: " << e);
    	} catch(...) {
    		RW_WARN("Error saving settings file due to unknown exception!");
    	}
        //update the view with the new settings
        view->setRootIndex(dirmodel->setRootPath(QString::fromStdString(_settingsMap->get<std::string>("Devices", "/"))));
  	}
}

void EasyInsert::loadDevice()
{
    QString st = "Load";
    rw::models::WorkCell::Ptr wc = getRobWorkStudio()->getWorkCell();
	QModelIndex index = view->currentIndex();
	QString itemText = dirmodel->filePath(index);

	dialog* loadDialog = new dialog(wc, st, this);

	loadDialog->addToDialog(loadDialog->createNameBox());
    loadDialog->addToDialog(loadDialog->createFrameSelection());
	loadDialog->addToDialog(loadDialog->createConfigurationBox());
	loadDialog->addToDialog(loadDialog->createButtonBox());
	loadDialog->exec();


    if (loadDialog->result() == QDialog::Accepted)
  	{
    	rw::models::WorkCell::Ptr dummy = rw::common::ownedPtr(new rw::models::WorkCell("dummy")); // Create dummy wc for swap
        getRobWorkStudio()->setWorkCell(dummy); // Temporarily swap out wc from rws

        try
        {

            ei::creator creator;
            ei::loader::add(itemText.toStdString(), wc, loadDialog->getNameBox(), loadDialog->getFrameSelection(),
                creator.getTransform3D( loadDialog->getDisplacementX(),
                                        loadDialog->getDisplacementY(),
                                        loadDialog->getDisplacementZ(),
                                        loadDialog->getRotationR(),
                                        loadDialog->getRotationP(),
                                        loadDialog->getRotationY()
                                        )
            );

            getRobWorkStudio()->setWorkCell(wc); // Swap back wc into rws
        }
        catch(const std::exception& ex)
        {
            getRobWorkStudio()->setWorkCell(wc); // Swap back wc into rws
            RW_THROW("Name of device allready in use");
        }
  	}
    delete loadDialog;

}

void EasyInsert::cube()
{
    QString st = "Cube";
    rw::models::WorkCell::Ptr wc = getRobWorkStudio()->getWorkCell();
    dialog* geometriDialog = new dialog(wc,st,this);

    geometriDialog->addToDialog(geometriDialog->createNameBox());
    geometriDialog->addToDialog(geometriDialog->createCheckFramesBox());
    geometriDialog->addToDialog(geometriDialog->createFrameSelection());
    geometriDialog->addToDialog(geometriDialog->createConfigurationBoxCube());
    geometriDialog->addToDialog(geometriDialog->createConfigurationBox());

    geometriDialog->addToDialog(geometriDialog->createButtonBox());

    geometriDialog->exec();

    if (geometriDialog->result() == QDialog::Accepted )
  	{
        if (geometriDialog->getNameBox() == "") {
            RW_THROW("Give geometry a name");
        }

        rw::models::WorkCell::Ptr wc = getRobWorkStudio()->getWorkCell();

        rw::models::WorkCell::Ptr dummy = rw::common::ownedPtr(new rw::models::WorkCell("dummy")); // Create dummy wc for swap
        getRobWorkStudio()->setWorkCell(dummy); // Temporarily swap out wc from rws

        try
        {

            ei::creator creator;
            rw::math::Transform3D<double> transform = creator.getTransform3D(
                                    geometriDialog->getDisplacementX(),
                                    geometriDialog->getDisplacementY(),
                                    geometriDialog->getDisplacementZ(),
                                    geometriDialog->getRotationR(),
                                    geometriDialog->getRotationP(),
                                    geometriDialog->getRotationY());

            if (geometriDialog->getCheckFrameMoveable()) // moveable frame
                ei::creator::addBox(geometriDialog->getNameBox(),
                                    geometriDialog->getFrameSelection(),
                                    wc,
                                    (float)geometriDialog->getGeometriX(),
                                    (float)geometriDialog->getGeometriY(),
                                    (float)geometriDialog->getGeometriZ(),
                                    transform);
            else // fixed frame
                ei::creator::addBox(geometriDialog->getNameBox(),
                                    wc->findFrame(geometriDialog->getFrameSelection()),
                                    wc,
                                    (float)geometriDialog->getGeometriX(),
                                    (float)geometriDialog->getGeometriY(),
                                    (float)geometriDialog->getGeometriZ(),
                                    transform);

            getRobWorkStudio()->setWorkCell(wc); // Swap back wc into rws
        }
        catch(const std::exception& ex)
        {
            getRobWorkStudio()->setWorkCell(wc); // Swap back wc into rws
            RW_THROW("Name of geometri allready in use");
        }
  	}
    delete geometriDialog;
}

void EasyInsert::plane()
{
    QString st = "Plane";
    rw::models::WorkCell::Ptr wc = getRobWorkStudio()->getWorkCell();
    dialog* geometriDialog = new dialog(wc,st,this);

    geometriDialog->addToDialog(geometriDialog->createNameBox());
    geometriDialog->addToDialog(geometriDialog->createCheckFramesBox());
    geometriDialog->addToDialog(geometriDialog->createFrameSelection());
    geometriDialog->addToDialog(geometriDialog->createConfigurationBox());

    geometriDialog->addToDialog(geometriDialog->createButtonBox());

    geometriDialog->exec();

    if (geometriDialog->result() == QDialog::Accepted)
    {
        if (geometriDialog->getNameBox() == "") {
            RW_THROW("Give geometry a name");
        }
        rw::models::WorkCell::Ptr wc = getRobWorkStudio()->getWorkCell();

        rw::models::WorkCell::Ptr dummy = rw::common::ownedPtr(new rw::models::WorkCell("dummy")); // Create dummy wc for swap
        getRobWorkStudio()->setWorkCell(dummy); // Temporarily swap out wc from rws

        try
        {
            ei::creator creator;
            rw::math::Transform3D<double> transform = creator.getTransform3D(
                                    geometriDialog->getDisplacementX(),
                                    geometriDialog->getDisplacementY(),
                                    geometriDialog->getDisplacementZ(),
                                    geometriDialog->getRotationR(),
                                    geometriDialog->getRotationP(),
                                    geometriDialog->getRotationY());

            if (geometriDialog->getCheckFrameMoveable()) // check for new moveable
                ei::creator::addPlane(geometriDialog->getNameBox(),
                                    geometriDialog->getFrameSelection(),
                                    wc,
                                    transform);
            else // else fixed frame
                ei::creator::addPlane(geometriDialog->getNameBox(),
                                    wc->findFrame(geometriDialog->getFrameSelection()),
                                    wc,
                                    transform);

            getRobWorkStudio()->setWorkCell(wc); // Swap back wc into rws
        }
        catch(const std::exception& ex)
        {
            getRobWorkStudio()->setWorkCell(wc); // Swap back wc into rws
            RW_THROW("Name of device allready in use");
        }
    }
    delete geometriDialog;
}

void EasyInsert::sphere()
{
    QString st = "Sphere";
    rw::models::WorkCell::Ptr wc = getRobWorkStudio()->getWorkCell();
    dialog* geometriDialog = new dialog(wc,st,this);

    geometriDialog->addToDialog(geometriDialog->createNameBox());
    geometriDialog->addToDialog(geometriDialog->createCheckFramesBox());
    geometriDialog->addToDialog(geometriDialog->createFrameSelection());
    geometriDialog->addToDialog(geometriDialog->createConfigurationBoxSphere());
    geometriDialog->addToDialog(geometriDialog->createConfigurationBox());

    geometriDialog->addToDialog(geometriDialog->createButtonBox());

    geometriDialog->exec();

    if (geometriDialog->result() == QDialog::Accepted)
    {
        if (geometriDialog->getNameBox() == "") {
            RW_THROW("Give geometry a name");
        }

        rw::models::WorkCell::Ptr wc = getRobWorkStudio()->getWorkCell();

        rw::models::WorkCell::Ptr dummy = rw::common::ownedPtr(new rw::models::WorkCell("dummy")); // Create dummy wc for swap
        getRobWorkStudio()->setWorkCell(dummy); // Temporarily swap out wc from rws

        try
        {
            ei::creator creator;
            rw::math::Transform3D<double> transform = creator.getTransform3D(
                                    geometriDialog->getDisplacementX(),
                                    geometriDialog->getDisplacementY(),
                                    geometriDialog->getDisplacementZ(),
                                    geometriDialog->getRotationR(),
                                    geometriDialog->getRotationP(),
                                    geometriDialog->getRotationY());

            if (geometriDialog->getCheckFrameMoveable()) // check for new moveable
                ei::creator::addSphere(geometriDialog->getNameBox(),
                                    geometriDialog->getFrameSelection(),
                                    wc,
                                    geometriDialog->getGeometriX(), //Radius
                                    transform);
            else
                ei::creator::addSphere(geometriDialog->getNameBox(),
                                    wc->findFrame(geometriDialog->getFrameSelection()),
                                    wc,
                                    geometriDialog->getGeometriX(), //Radius
                                    transform);

            getRobWorkStudio()->setWorkCell(wc); // Swap back wc into rws
        }
        catch(const std::exception& ex)
        {
            getRobWorkStudio()->setWorkCell(wc); // Swap back wc into rws
            RW_THROW("Name of device allready in use");
        }
    }
    delete geometriDialog;
}

void EasyInsert::cone()
{
    QString st = "Cone";
    rw::models::WorkCell::Ptr wc = getRobWorkStudio()->getWorkCell();
    dialog* geometriDialog = new dialog(wc,st,this);

    geometriDialog->addToDialog(geometriDialog->createNameBox());
    geometriDialog->addToDialog(geometriDialog->createCheckFramesBox());
    geometriDialog->addToDialog(geometriDialog->createFrameSelection());
    geometriDialog->addToDialog(geometriDialog->createConfigurationBoxCone());
    geometriDialog->addToDialog(geometriDialog->createConfigurationBox());

    geometriDialog->addToDialog(geometriDialog->createButtonBox());

    geometriDialog->exec();

    if (geometriDialog->result() == QDialog::Accepted)
    {
        if (geometriDialog->getNameBox() == "") {
            RW_THROW("Give geometry a name");
        }

        rw::models::WorkCell::Ptr wc = getRobWorkStudio()->getWorkCell();

        rw::models::WorkCell::Ptr dummy = rw::common::ownedPtr(new rw::models::WorkCell("dummy")); // Create dummy wc for swap
        getRobWorkStudio()->setWorkCell(dummy); // Temporarily swap out wc from rws

        try
        {
            ei::creator creator;
            rw::math::Transform3D<double> transform = creator.getTransform3D(
                                    geometriDialog->getDisplacementX(),
                                    geometriDialog->getDisplacementY(),
                                    geometriDialog->getDisplacementZ(),
                                    geometriDialog->getRotationR(),
                                    geometriDialog->getRotationP(),
                                    geometriDialog->getRotationY());

            if (geometriDialog->getCheckFrameMoveable()) // check for new moveable
                ei::creator::addCone(geometriDialog->getNameBox(),
                                    geometriDialog->getFrameSelection(),
                                    wc,
                                    geometriDialog->getGeometriX(), //Radius
                                    geometriDialog->getGeometriY(), //Height
                                    transform);
            else
                ei::creator::addCone(geometriDialog->getNameBox(),
                                    wc->findFrame(geometriDialog->getFrameSelection()),
                                    wc,
                                    geometriDialog->getGeometriX(), //Radius
                                    geometriDialog->getGeometriY(), //Height
                                    transform);

            getRobWorkStudio()->setWorkCell(wc); // Swap back wc into rws
        }
        catch(const std::exception& ex)
        {
            getRobWorkStudio()->setWorkCell(wc); // Swap back wc into rws
            RW_THROW("Name of device allready in use");
        }
    }
    delete geometriDialog;
}

void EasyInsert::cylinder()
{
    QString st = "Cylinder";
    rw::models::WorkCell::Ptr wc = getRobWorkStudio()->getWorkCell();
    dialog* geometriDialog = new dialog(wc,st,this);

    geometriDialog->addToDialog(geometriDialog->createNameBox());
    geometriDialog->addToDialog(geometriDialog->createCheckFramesBox());
    geometriDialog->addToDialog(geometriDialog->createFrameSelection());
    geometriDialog->addToDialog(geometriDialog->createConfigurationBoxCone()); //cylinder and cone uses the same parameters
    geometriDialog->addToDialog(geometriDialog->createConfigurationBox());

    geometriDialog->addToDialog(geometriDialog->createButtonBox());

    geometriDialog->exec();

    if (geometriDialog->result() == QDialog::Accepted)
    {
        if (geometriDialog->getNameBox() == "") {
            RW_THROW("Give geometry a name");
        }

        rw::models::WorkCell::Ptr wc = getRobWorkStudio()->getWorkCell();

        rw::models::WorkCell::Ptr dummy = rw::common::ownedPtr(new rw::models::WorkCell("dummy")); // Create dummy wc for swap
        getRobWorkStudio()->setWorkCell(dummy); // Temporarily swap out wc from rws

        try
        {
            ei::creator creator;
            rw::math::Transform3D<double> transform = creator.getTransform3D(
                                    geometriDialog->getDisplacementX(),
                                    geometriDialog->getDisplacementY(),
                                    geometriDialog->getDisplacementZ(),
                                    geometriDialog->getRotationR(),
                                    geometriDialog->getRotationP(),
                                    geometriDialog->getRotationY());

            if (geometriDialog->getCheckFrameMoveable()) // check for new moveable
                ei::creator::addCylinder(geometriDialog->getNameBox(),
                                    geometriDialog->getFrameSelection(),
                                    wc,
                                    geometriDialog->getGeometriX(), //Radius
                                    geometriDialog->getGeometriY(), //Height
                                    transform);
            else
                ei::creator::addCylinder(geometriDialog->getNameBox(),
                                    wc->findFrame(geometriDialog->getFrameSelection()),
                                    wc,
                                    geometriDialog->getGeometriX(), //Radius
                                    geometriDialog->getGeometriY(), //Height
                                    transform);

            getRobWorkStudio()->setWorkCell(wc); // Swap back wc into rws
        }
        catch(const std::exception& ex)
        {
            getRobWorkStudio()->setWorkCell(wc); // Swap back wc into rws
            RW_THROW("Name of device allready in use");
        }
    }
    delete geometriDialog;
}

void EasyInsert::tube()
{
    QString st = "Tube";
    rw::models::WorkCell::Ptr wc = getRobWorkStudio()->getWorkCell();
    dialog* geometriDialog = new dialog(wc,st,this);

    geometriDialog->addToDialog(geometriDialog->createNameBox());
    geometriDialog->addToDialog(geometriDialog->createCheckFramesBox());
    geometriDialog->addToDialog(geometriDialog->createFrameSelection());
    geometriDialog->addToDialog(geometriDialog->createConfigurationBoxTube());
    geometriDialog->addToDialog(geometriDialog->createConfigurationBox());

    geometriDialog->addToDialog(geometriDialog->createButtonBox());

    geometriDialog->exec();

    if (geometriDialog->result() == QDialog::Accepted)
    {
        if (geometriDialog->getNameBox() == "") {
            RW_THROW("Give geometry a name");
        }

        rw::models::WorkCell::Ptr wc = getRobWorkStudio()->getWorkCell();

        rw::models::WorkCell::Ptr dummy = rw::common::ownedPtr(new rw::models::WorkCell("dummy")); // Create dummy wc for swap
        getRobWorkStudio()->setWorkCell(dummy); // Temporarily swap out wc from rws

        try
        {
            ei::creator creator;
            rw::math::Transform3D<double> transform = creator.getTransform3D(
                                    geometriDialog->getDisplacementX(),
                                    geometriDialog->getDisplacementY(),
                                    geometriDialog->getDisplacementZ(),
                                    geometriDialog->getRotationR(),
                                    geometriDialog->getRotationP(),
                                    geometriDialog->getRotationY());

            if (geometriDialog->getCheckFrameMoveable()) // check for new moveable
                ei::creator::addTube(geometriDialog->getNameBox(),
                                    geometriDialog->getFrameSelection(),
                                    wc,
                                    geometriDialog->getGeometriX(), //Radius
                                    geometriDialog->getGeometriY(), //Thickness
                                    geometriDialog->getGeometriZ(), //Height
                                    transform);
            else
                ei::creator::addTube(geometriDialog->getNameBox(),
                                    wc->findFrame(geometriDialog->getFrameSelection()),
                                    wc,
                                    geometriDialog->getGeometriX(), //Raidus
                                    geometriDialog->getGeometriY(), //Thickness
                                    geometriDialog->getGeometriZ(), //Height
                                    transform);


            getRobWorkStudio()->setWorkCell(wc); // Swap back wc into rws
        }
        catch(const std::exception& ex)
        {
            getRobWorkStudio()->setWorkCell(wc); // Swap back wc into rws
            RW_THROW("Name of device allready in use");
        }
    }
    delete geometriDialog;
}

void EasyInsert::fixedFrame()
{
    QString st = "Fixed Frame";
    rw::models::WorkCell::Ptr wc = getRobWorkStudio()->getWorkCell();
    dialog* geometriDialog = new dialog(wc,st,this);

    geometriDialog->addToDialog(geometriDialog->createNameBox());
    //geometriDialog->addToDialog(geometriDialog->createCheckFramesBox());
    geometriDialog->addToDialog(geometriDialog->createFrameSelection());
    geometriDialog->addToDialog(geometriDialog->createConfigurationBox());

    geometriDialog->addToDialog(geometriDialog->createButtonBox());

    geometriDialog->exec();

    if (geometriDialog->result() == QDialog::Accepted)
    {
        if (geometriDialog->getNameBox() == "") {
            RW_THROW("Give frame a name");
        }

        rw::models::WorkCell::Ptr wc = getRobWorkStudio()->getWorkCell();

        rw::models::WorkCell::Ptr dummy = rw::common::ownedPtr(new rw::models::WorkCell("dummy")); // Create dummy wc for swap
        getRobWorkStudio()->setWorkCell(dummy); // Temporarily swap out wc from rws

        try
        {
            ei::creator creator;
            rw::math::Transform3D<double> transform = creator.getTransform3D(
                                    geometriDialog->getDisplacementX(),
                                    geometriDialog->getDisplacementY(),
                                    geometriDialog->getDisplacementZ(),
                                    geometriDialog->getRotationR(),
                                    geometriDialog->getRotationP(),
                                    geometriDialog->getRotationY());

            ei::creator::addFixedFrame(wc,
                                geometriDialog->getNameBox(),
                                geometriDialog->getFrameSelection(),
                                transform);

            getRobWorkStudio()->setWorkCell(wc); // Swap back wc into rws
        }
        catch(const std::exception& ex)
        {
            getRobWorkStudio()->setWorkCell(wc); // Swap back wc into rws
            RW_THROW("Name of frame allready in use");
        }
    }
    delete geometriDialog;
}

void EasyInsert::movableFrame()
{
    QString st = "Movable Frame";
    rw::models::WorkCell::Ptr wc = getRobWorkStudio()->getWorkCell();
    dialog* geometriDialog = new dialog(wc,st,this);

    geometriDialog->addToDialog(geometriDialog->createNameBox());
    //geometriDialog->addToDialog(geometriDialog->createCheckFramesBox());
    geometriDialog->addToDialog(geometriDialog->createFrameSelection());
    geometriDialog->addToDialog(geometriDialog->createConfigurationBox());

    geometriDialog->addToDialog(geometriDialog->createButtonBox());

    geometriDialog->exec();

    if (geometriDialog->result() == QDialog::Accepted)
    {
        if (geometriDialog->getNameBox() == "") {
            RW_THROW("Give frame a name");
        }

        rw::models::WorkCell::Ptr wc = getRobWorkStudio()->getWorkCell();

        rw::models::WorkCell::Ptr dummy = rw::common::ownedPtr(new rw::models::WorkCell("dummy")); // Create dummy wc for swap
        getRobWorkStudio()->setWorkCell(dummy); // Temporarily swap out wc from rws

        try
        {
            ei::creator creator;
            rw::math::Transform3D<double> transform = creator.getTransform3D(
                                    geometriDialog->getDisplacementX(),
                                    geometriDialog->getDisplacementY(),
                                    geometriDialog->getDisplacementZ(),
                                    geometriDialog->getRotationR(),
                                    geometriDialog->getRotationP(),
                                    geometriDialog->getRotationY());

            ei::creator::addMovableFrame(wc,
                                geometriDialog->getNameBox(),
                                geometriDialog->getFrameSelection(),
                                transform);

            getRobWorkStudio()->setWorkCell(wc); // Swap back wc into rws
        }
        catch(const std::exception& ex)
        {
            getRobWorkStudio()->setWorkCell(wc); // Swap back wc into rws
            RW_THROW("Name of frame allready in use");
        }
    }
    delete geometriDialog;
}

void EasyInsert::deleteSingleFrame()
{
    if (_frameWidget->currentItem() != NULL) { //select something

        QListWidgetItem* item = _frameWidget->currentItem();    //get the item
        rw::models::WorkCell::Ptr wc = getRobWorkStudio()->getWorkCell();
        rw::kinematics::Frame* frame = wc->findFrame(item->text().toStdString()); //get the frame

        rw::models::WorkCell::Ptr dummy = rw::common::ownedPtr(new rw::models::WorkCell("dummy")); // Create dummy wc for swap
        getRobWorkStudio()->setWorkCell(dummy); // Temporarily swap out wc from rws

        std::vector<rw::kinematics::Frame*> frames = wc->getFrames();//get all frames
        int id = frame->getID();
        if( frames[id]!=NULL ){
            // check if frame has staticly connected children
            rw::kinematics::Frame::iterator_pair iter = frames[id]->getChildren();
            if( iter.first!=iter.second ){ //when the iterator pair are equal, there are no childrens
                getRobWorkStudio()->setWorkCell(wc); // Swap back wc into rws
                RW_THROW("Frame has staticly connected children and therefore cannot be removed from tree!");
                return;
            }
        }

        removeAllObjects(frame,wc); //remove all the objects from the frame
        if(frame->getName() != "WORLD") //done delete world.. come on
            wc->remove(frame); //remove frame

        getRobWorkStudio()->setWorkCell(wc); // Swap back wc into rws

    }
    else
        rw::common::Log::log().info() << "select something " << std::endl;

}

void EasyInsert::deleteFrame()
{
    if (_frameWidget->currentItem() != NULL) {

        QListWidgetItem* item = _frameWidget->currentItem();
        rw::models::WorkCell::Ptr wc = getRobWorkStudio()->getWorkCell();
        rw::kinematics::Frame* frame = wc->findFrame(item->text().toStdString());

        rw::models::WorkCell::Ptr dummy = rw::common::ownedPtr(new rw::models::WorkCell("dummy")); // Create dummy wc for swap
        getRobWorkStudio()->setWorkCell(dummy); // Temporarily swap out wc from rws

        deleteChildren(frame, wc); // find and delete children

        getRobWorkStudio()->setWorkCell(wc); // Swap back wc into rws
    }
    else
        rw::common::Log::log().info() << "select something " << std::endl;

}

void EasyInsert::deleteChildren(rw::kinematics::Frame* frame, rw::models::WorkCell::Ptr wc)
{
    if (frame->getName() != "WORLD") {
        std::vector<rw::kinematics::Frame*> frames = wc->getFrames();
        int id = frame->getID();
        if( frames[id]!=NULL ){
            rw::kinematics::Frame::iterator_pair iter = frames[id]->getChildren(wc->getDefaultState()); //get iterator pair of the children
            std::vector<rw::kinematics::Frame*> children; //a vector for found children
            if ( iter.first!=iter.second){  //stop when no children
                for (; iter.first!=iter.second; iter.first++) { //go through the children
                    rw::kinematics::Frame* child = &(*(iter.first));
                    children.push_back(child);  //save them
                }
                for (size_t i = 0; i < children.size(); i++) { //delete the childrens children!
                    deleteChildren(children[i],wc);
                }
            }
        }
        removeAllObjects(frame,wc); //remove objects
        wc->remove(frame);  //remove frame
    }
}

void EasyInsert::removeAllObjects(rw::kinematics::Frame* frame, rw::models::WorkCell::Ptr wc)
{
    std::vector<rw::models::Object::Ptr> object = wc->getObjects();

    for (size_t i = 0; i < object.size(); i++) {
        const std::vector <rw::kinematics::Frame*>& objectFrames = object[i].get()->getFrames();
        for (size_t j = 0; j < objectFrames.size(); j++) {
            if (objectFrames[j] == frame) {
                wc->removeObject(object[i].get());
                break;
            }
        }
    }
}

void EasyInsert::deleteDev()
{
    if (_deviceWidget->currentItem() != NULL) {
        QListWidgetItem* item = _deviceWidget->currentItem();
        rw::models::WorkCell::Ptr wc = getRobWorkStudio()->getWorkCell(); // please corret sometime
        rw::common::Ptr <rw::models::Device> device = wc->findDevice(item->text().toStdString());
        //std::vector<rw::models::Object::Ptr> object = wc->getObjects();

        rw::models::WorkCell::Ptr dummy = rw::common::ownedPtr(new rw::models::WorkCell("dummy")); // Create dummy wc for swap
        getRobWorkStudio()->setWorkCell(dummy); // Temporarily swap out wc from rws

        wc->remove(device.get());


        getRobWorkStudio()->setWorkCell(wc); // Swap back wc into rws
    }
    else
        rw::common::Log::log().info() << "select something " << std::endl;
}

void EasyInsert::deleteObj()
{
    if (_objectWidget->currentItem() != NULL) {
        QListWidgetItem* item = _objectWidget->currentItem();

        rw::models::WorkCell::Ptr wc = getRobWorkStudio()->getWorkCell(); // please corret sometime
        rw::common::Ptr <rw::models::Object> object = wc->findObject(item->text().toStdString());
        //std::vector<rw::models::Object::Ptr> object = wc->getObjects();

        rw::models::WorkCell::Ptr dummy = rw::common::ownedPtr(new rw::models::WorkCell("dummy")); // Create dummy wc for swap
        getRobWorkStudio()->setWorkCell(dummy); // Temporarily swap out wc from rws

        wc->removeObject(object.get());


        getRobWorkStudio()->setWorkCell(wc); // Swap back wc into rws
    }
    else
        rw::common::Log::log().info() << "select something " << std::endl;
}


void EasyInsert::update()
{
    showFrameStructure();
}

void EasyInsert::clearListContent()
{
    //clear everything used in the delete tab
    _deviceWidget->clear();
    _devFrameList.clear();
    _frameWidget->clear();
    _frames.clear();
    _objectWidget->clear();
}

void EasyInsert::stateChangedListener(const State& state)
{
    //update stuff when the state changes
    _state = state;
    update();
}


#if !RWS_USE_QT5
#include <QtCore/qplugin.h>
Q_EXPORT_PLUGIN(EasyInsert);
#endif
