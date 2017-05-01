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
    _deleteTab = createDeleteTab();
    tabWindow->addTab(_devTab, "Devices");
    tabWindow->addTab(_geoTab, "Geometries");
    tabWindow->addTab(_deleteTab, "Delete");
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

void EasyInsert::workcellChangedListener(int notUsed)
{
    QMetaObject::invokeMethod(this, "update", Qt::QueuedConnection);
}

void EasyInsert::open(WorkCell* workcell)
{
    _workcell = workcell;
    _state = getRobWorkStudio()->getState();
    //_treeWidget->setHeaderLabels(QStringList("WorkCell"));

    showFrameStructure();

    // connect the workcell changed handler
    _workcell->workCellChangedEvent().add(boost::bind(&EasyInsert::workcellChangedListener, this, _1), this);
}

void EasyInsert::close()
{
    clearListContent();

    _workcell = NULL;
    //_treeWidget->setHeaderLabels(QStringList("WorkCell"));
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
            RW_WARN("Could not load settings from 'eisettings.xml': " << e.getMessage().getText() << "\n Using default settings!");
        } catch(std::exception &e){
            RW_WARN("Could not load settings from 'eisettings.xml': " << e.what() << "\n Using default settings!");
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

	//QToolButton *undoButton = new QToolButton(); // undo button removed until it the functionallity is added
	//undoButton->setIcon(QIcon(":/undo.png"));
	//undoButton->setToolTip("Undo");
	QToolButton *settingsButton = new QToolButton();
	settingsButton->setIcon(QIcon(":/settings.png"));
	settingsButton->setToolTip("Settings");
	QMenu *settingsMenu = new QMenu("Settings menu");
	QAction *settingsAction = new QAction("Libraries",this);
	settingsMenu->addAction(settingsAction);
	settingsButton->setMenu(settingsMenu);
	settingsButton->setPopupMode(QToolButton::InstantPopup);
	//toolBar->addWidget(undoButton);
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

    layout->setColumnMinimumWidth(0,50);
    layout->setColumnMinimumWidth(1,50);

    geoTab->setLayout(layout);
    widg->setWidget(geoTab);
    return widg;
}

QWidget* EasyInsert::createDeleteTab()
{
    //QGridLayout *layout = new QGridLayout();

    QScrollArea *widg = new QScrollArea();
    widg->setWidgetResizable(true);
    widg->setFrameShape(QFrame::NoFrame);
    QWidget *devTab = new QWidget();
    QVBoxLayout *verticalLayout = new QVBoxLayout(devTab);

    QGroupBox *delDevBox = new QGroupBox(tr("Devices:"));
    QGroupBox *delFrameBox = new QGroupBox(tr("Frames:"));
    QGroupBox *delObjBox = new QGroupBox(tr("Objects:"));
    QGridLayout *layoutDev = new QGridLayout();
    QGridLayout *layoutFrame = new QGridLayout();
    QGridLayout *layoutObj = new QGridLayout();

    _deviceWidget = new QListWidget(this);
    _frameWidget = new QListWidget(this);
    _objectWidget = new QListWidget(this);


    QPushButton *delDevBtn = new QPushButton("Delete",devTab);
    connect(delDevBtn, SIGNAL(clicked()), this, SLOT(deleteDev()));
    QPushButton *delObjBtn = new QPushButton("Delete",devTab);
    connect(delObjBtn, SIGNAL(clicked()), this, SLOT(deleteObj()));
    QPushButton *delBtn = new QPushButton("Delete",devTab);
    connect(delBtn, SIGNAL(clicked()), this, SLOT(deleteFrame()));

    layoutDev->addWidget(_deviceWidget,0,0);
    layoutDev->addWidget(delDevBtn,0,1);
    delDevBox->setLayout(layoutDev);

    layoutObj->addWidget(_objectWidget,0,0);
    layoutObj->addWidget(delObjBtn,0,1);
    delObjBox->setLayout(layoutObj);

    layoutFrame->addWidget(_frameWidget,0,0);
    layoutFrame->addWidget(delBtn,0,1);
    delFrameBox->setLayout(layoutFrame);

    verticalLayout->addWidget(delDevBox);
    verticalLayout->addWidget(delObjBox);
    verticalLayout->addWidget(delFrameBox);

    devTab->setLayout(verticalLayout);
    widg->setWidget(devTab);
    return widg;
}

/*void EasyInsert::showFrameStructure()
{
    clearTreeContent();
    _treeWidget->setHeaderLabels(QStringList("Frame Structure"));

    if (_workcell != NULL) {
		const std::vector<rw::models::Device::Ptr>& devices = _workcell->getDevices();
		typedef std::vector<rw::models::Device::Ptr>::const_iterator MI;
        for (MI it = devices.begin(); it != devices.end(); ++it) {
			rw::models::SerialDevice::Ptr sdevice = (*it).cast<rw::models::SerialDevice>();
            if (sdevice) {
                QTreeWidgetItem* deviceItem = new QTreeWidgetItem();
                _treeWidget->addTopLevelItem(deviceItem); // own deviceItem

                _deviceMap.insert(std::make_pair(deviceItem, sdevice)); // delete this sometime maybe
                deviceItem->setText(0, sdevice->getName().c_str());
                deviceItem->setIcon(0, QIcon(":/images/device.png"));

                continue;
            }
			rw::models::TreeDevice::Ptr tdevice = (*it).cast<rw::models::TreeDevice>();
            if(tdevice){
                QTreeWidgetItem* deviceItem = new QTreeWidgetItem();
                _treeWidget->addTopLevelItem(deviceItem); // own deviceItem

                _deviceMap.insert(std::make_pair(deviceItem, tdevice));
                deviceItem->setText(0, tdevice->getName().c_str());
                deviceItem->setIcon(0, QIcon(":/images/device.png"));

                continue;
            }
			rw::models::ParallelDevice::Ptr pdevice = (*it).cast<rw::models::ParallelDevice>();
            if(pdevice){
                QTreeWidgetItem* deviceItem = new QTreeWidgetItem();
                _treeWidget->addTopLevelItem(deviceItem); // own deviceItem

                _deviceMap.insert(std::make_pair(deviceItem, pdevice));
                deviceItem->setText(0, pdevice->getName().c_str());
                deviceItem->setIcon(0, QIcon(":/images/device.png"));

                continue;
            }
			rw::models::MobileDevice::Ptr mdevice = (*it).cast<rw::models::MobileDevice>();
            if(mdevice){
                QTreeWidgetItem* deviceItem = new QTreeWidgetItem();
                _treeWidget->addTopLevelItem(deviceItem); // own deviceItem

                _deviceMap.insert(std::make_pair(deviceItem, mdevice));
                deviceItem->setText(0, mdevice->getName().c_str());
                deviceItem->setIcon(0, QIcon(":/images/device.png"));

                continue;
            }
        }
    }

    if (_workcell != NULL) {
        setupFrame(*_workcell->getWorldFrame(), NULL);
    }
}*/

void EasyInsert::showFrameStructure()
{
    clearListContent();
    if (_workcell != NULL) {
        const std::vector<rw::models::Device::Ptr>& devices = _workcell->getDevices();
        typedef std::vector<rw::models::Device::Ptr>::const_iterator MI;
        for (MI it = devices.begin(); it != devices.end(); ++it) {
            rw::models::SerialDevice::Ptr sdevice = (*it).cast<rw::models::SerialDevice>();
            if (sdevice) {
                QListWidgetItem* deviceItem = new QListWidgetItem();
                _deviceWidget->addItem(deviceItem); // own deviceItem
                deviceItem->setText(sdevice->getName().c_str());
                const std::vector<rw::kinematics::Frame*> frames = sdevice->frames();
                for (size_t i = 0; i < frames.size(); i++) {
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
        _frames = _workcell->getFrames();

        for (size_t i = 0; i < _devFrameList.size(); i++) {
            for (size_t j = 0; j < _frames.size(); j++) {
                if (_frames[j]->getName() == _devFrameList[i]->getName()){
                    _frames.erase(_frames.begin()+j);
                    break;
                }
            }
        }
        for (size_t i = 0; i < _frames.size(); i++) {
            QListWidgetItem* deviceItem = new QListWidgetItem();
            _frameWidget->addItem(deviceItem); // own deviceItem
            deviceItem->setText(_frames[i]->getName().c_str());
        }
    }

    if (_workcell != NULL) {
        std::vector<rw::models::Object::Ptr> object = _workcell->getObjects();
        for (size_t i = 0; i < _devFrameList.size(); i++) {
            bool removeObj = false;
            for (size_t j = 0; j < object.size() && !removeObj; j++) {
                const std::vector <rw::kinematics::Frame*>& objectFrames = object[j].get()->getFrames();
                for (size_t k = 0; k < objectFrames.size() && !removeObj; k++) {
                    if (objectFrames[k] == _devFrameList[i]) {
                        object.erase(object.begin()+j);
                        removeObj = true;
                    }
                }
            }
        }
        for (size_t i = 0; i < object.size(); i++) {
            QListWidgetItem* deviceItem = new QListWidgetItem();
            _objectWidget->addItem(deviceItem); // own deviceItem
            deviceItem->setText(object[i].get()->getName().c_str());
        }
    }

}

/*void EasyInsert::setupFrame(rw::kinematics::Frame& frame, QTreeWidgetItem* parentItem)
{
    QTreeWidgetItem* item = new QTreeWidgetItem();

    const std::vector<rw::models::Device::Ptr>& devices = _workcell->getDevices();
    typedef std::vector<rw::models::Device::Ptr>::const_iterator MI;
    for (MI it = devices.begin(); it != devices.end(); ++it) {
        rw::models::SerialDevice::Ptr sdevice = (*it).cast<rw::models::SerialDevice>();
        const std::vector<rw::kinematics::Frame*> frames = sdevice->frames();
        for (size_t i = 0; i < frames.size(); i++) {
            //rw::common::Log::log().info() << "device lort " << frames[i]->getName() << std::endl;

            if (frame.getName() == frames[i]->getName()) {

                rw::kinematics::Frame::iterator_pair children = frame.getChildren(_state);
                for (rw::kinematics::Frame::iterator it = children.first; it != children.second; ++it) {
                    setupFrame(*it, item);
                }
                setupDrawables(&frame, item);
                return;
            }
        }
    }
    if (parentItem != NULL)
        parentItem->addChild(item); // own item
    else
        _treeWidget->addTopLevelItem(item); // own item

    std::string name = frame.getName();
    item->setText(0, name.c_str());

    _frameMap.insert(std::make_pair(item, &frame)); //register frame item

    if ( dynamic_cast<rw::models::Joint*>(&frame) )
        item->setIcon(0,QIcon(":/images/joint.png"));
    else
        item->setIcon(0,QIcon(":/images/frame.png"));

    rw::kinematics::Frame::iterator_pair children = frame.getChildren(_state);
    for (rw::kinematics::Frame::iterator it = children.first; it != children.second; ++it) {
        setupFrame(*it, item);
    }
    setupDrawables(&frame, item);
}*/

/*void EasyInsert::setupDrawables(rw::kinematics::Frame* frame, QTreeWidgetItem* parent)
{
    rw::graphics::WorkCellScene::Ptr scene = getRobWorkStudio()->getView()->getWorkCellScene();

    int drawMask = getRobWorkStudio()->getView()->getDrawMask();

    const std::vector<rw::graphics::DrawableNode::Ptr>& drawables = scene->getDrawables(frame);

    typedef std::vector<rw::graphics::DrawableNode::Ptr>::const_iterator DI;
    for (DI p = drawables.begin(); p != drawables.end(); ++p) {
        rw::graphics::DrawableNode::Ptr drawable = *p;
        if( !(drawable->getMask() & drawMask) )
            continue;

        RW_ASSERT(drawable);
        QTreeWidgetItem* item = new QTreeWidgetItem(parent); // owned.

        item->setText(0, drawable->getName().c_str());
        _drawableMap[item] = drawable;
        if( drawable->getMask() & rw::graphics::DrawableNode::CollisionObject ){
            item->setIcon(0, QIcon(":/images/collision.png"));
        } else {
            item->setIcon(0, QIcon(":/images/drawable.png"));
        }
    }
}*/

std::string EasyInsert::getFrameName(const rw::kinematics::Frame& frame)
{
    std::string name = frame.getName();
    size_t index = name.find_last_of('.');
    if (index == std::string::npos)
        return frame.getName();
    else
        return name.substr(index + 1);
}

/*void EasyInsert::registerFrameItem(Frame* frame, QTreeWidgetItem* item)
{
    _frameMap.insert(make_pair(item, frame));
}*/

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
		//_settingsMap->set<std::string>("Geometries", _settingsDialog->_settingsMap->get<std::string>("Geometries", "/"));
		//_settingsMap->set<std::string>("Frames", _settingsDialog->_settingsMap->get<std::string>("Frames", "/"));
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

	_loadDialog = new dialog(st, this);

	_loadDialog->addToDialog(_loadDialog->createNameBox());
	_loadDialog->addToDialog(_loadDialog->createConfigurationBox());
	_loadDialog->addToDialog(_loadDialog->createButtonBox());
	_loadDialog->exec();


    if (_loadDialog->result() == QDialog::Accepted)
  	{
    	rw::models::WorkCell::Ptr wc = getRobWorkStudio()->getWorkCell();

        rw::models::WorkCell::Ptr dummy = rw::common::ownedPtr(new rw::models::WorkCell("dummy")); // Create dummy wc for swap
        getRobWorkStudio()->setWorkCell(dummy); // Temporarily swap out wc from rws

        try
        {
            ei::loader::add(itemText.toStdString(),wc,_loadDialog->nameLine->text().toStdString(),
                _loadDialog->doubleSpinBoxes[0]->value(),
                _loadDialog->doubleSpinBoxes[1]->value(),
                _loadDialog->doubleSpinBoxes[2]->value(),
                _loadDialog->doubleSpinBoxes[3]->value(),
                _loadDialog->doubleSpinBoxes[4]->value(),
                _loadDialog->doubleSpinBoxes[5]->value());

            getRobWorkStudio()->setWorkCell(wc); // Swap back wc into rws
        }
        catch(const std::exception& ex)
        {
            getRobWorkStudio()->setWorkCell(wc); // Swap back wc into rws
            RW_THROW("Name of device allready in use");
        }
  	}

}

void EasyInsert::cube()
{
    QString st = "Cube";
    rw::models::WorkCell::Ptr wc = getRobWorkStudio()->getWorkCell();
    _geometriDialog = new dialog(wc,st,this);

    _geometriDialog->addToDialog(_geometriDialog->createNameBox());
    _geometriDialog->addToDialog(_geometriDialog->createCheckFramesBox());
    _geometriDialog->addToDialog(_geometriDialog->createFrameSelection());
    _geometriDialog->addToDialog(_geometriDialog->createConfigurationBoxCube());
    _geometriDialog->addToDialog(_geometriDialog->createConfigurationBox());

    _geometriDialog->addToDialog(_geometriDialog->createButtonBox());

    _geometriDialog->exec();

    if (_geometriDialog->result() == QDialog::Accepted )
  	{
        if (_geometriDialog->nameLine->text().toStdString() == "") {
            RW_THROW("Give geometry a name");
        }

        rw::models::WorkCell::Ptr wc = getRobWorkStudio()->getWorkCell();

        rw::models::WorkCell::Ptr dummy = rw::common::ownedPtr(new rw::models::WorkCell("dummy")); // Create dummy wc for swap
        getRobWorkStudio()->setWorkCell(dummy); // Temporarily swap out wc from rws

        try
        {
            rw::math::Vector3D<double> displacement(
                    _geometriDialog->doubleSpinBoxes[1]->value(),
                    _geometriDialog->doubleSpinBoxes[2]->value(),
                    _geometriDialog->doubleSpinBoxes[3]->value());

            rw::math::Transform3D<double> transform(displacement);

            if (_geometriDialog->checkFrames[0]->isChecked()) // moveable frame
                ei::creator::addBox(_geometriDialog->nameLine->text().toStdString(),
                                    _geometriDialog->comboFrames->currentText().toStdString(),
                                    wc,
                                    (float)_geometriDialog->doubleSpinBoxesGeometires[0]->value(),
                                    (float)_geometriDialog->doubleSpinBoxesGeometires[1]->value(),
                                    (float)_geometriDialog->doubleSpinBoxesGeometires[2]->value(),
                                    transform);
            else // fixed frame
                ei::creator::addBox(_geometriDialog->nameLine->text().toStdString(),
                                    wc->findFrame(_geometriDialog->comboFrames->currentText().toStdString()),
                                    wc,
                                    (float)_geometriDialog->doubleSpinBoxesGeometires[0]->value(),
                                    (float)_geometriDialog->doubleSpinBoxesGeometires[1]->value(),
                                    (float)_geometriDialog->doubleSpinBoxesGeometires[2]->value(),
                                    transform);

            getRobWorkStudio()->setWorkCell(wc); // Swap back wc into rws
        }
        catch(const std::exception& ex)
        {
            getRobWorkStudio()->setWorkCell(wc); // Swap back wc into rws
            RW_THROW("Name of geometri allready in use");
        }
  	}
}

void EasyInsert::plane()
{
    QString st = "Plane";
    rw::models::WorkCell::Ptr wc = getRobWorkStudio()->getWorkCell();
    _geometriDialog = new dialog(wc,st,this);

    _geometriDialog->addToDialog(_geometriDialog->createNameBox());
    _geometriDialog->addToDialog(_geometriDialog->createCheckFramesBox());
    _geometriDialog->addToDialog(_geometriDialog->createFrameSelection());
    _geometriDialog->addToDialog(_geometriDialog->createConfigurationBox());

    _geometriDialog->addToDialog(_geometriDialog->createButtonBox());

    _geometriDialog->exec();

    if (_geometriDialog->result() == QDialog::Accepted)
    {
        if (_geometriDialog->nameLine->text().toStdString() == "") {
            RW_THROW("Give geometry a name");
        }
        rw::models::WorkCell::Ptr wc = getRobWorkStudio()->getWorkCell();

        rw::models::WorkCell::Ptr dummy = rw::common::ownedPtr(new rw::models::WorkCell("dummy")); // Create dummy wc for swap
        getRobWorkStudio()->setWorkCell(dummy); // Temporarily swap out wc from rws

        try
        {
            rw::math::Vector3D<double> displacement(
                    _geometriDialog->doubleSpinBoxes[1]->value(),
                    _geometriDialog->doubleSpinBoxes[2]->value(),
                    _geometriDialog->doubleSpinBoxes[3]->value());

            rw::math::Transform3D<double> transform(displacement);

            if (_geometriDialog->checkFrames[0]->isChecked()) // check for new moveable
                ei::creator::addPlane(_geometriDialog->nameLine->text().toStdString(),
                                    _geometriDialog->comboFrames->currentText().toStdString(),
                                    wc,
                                    transform);
            else // else fixed frame
                ei::creator::addPlane(_geometriDialog->nameLine->text().toStdString(),
                                    wc->findFrame(_geometriDialog->comboFrames->currentText().toStdString()),
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
}

void EasyInsert::sphere()
{
    QString st = "Sphere";
    rw::models::WorkCell::Ptr wc = getRobWorkStudio()->getWorkCell();
    _geometriDialog = new dialog(wc,st,this);

    _geometriDialog->addToDialog(_geometriDialog->createNameBox());
    _geometriDialog->addToDialog(_geometriDialog->createCheckFramesBox());
    _geometriDialog->addToDialog(_geometriDialog->createFrameSelection());
    _geometriDialog->addToDialog(_geometriDialog->createConfigurationBoxSphere());
    _geometriDialog->addToDialog(_geometriDialog->createConfigurationBox());

    _geometriDialog->addToDialog(_geometriDialog->createButtonBox());

    _geometriDialog->exec();

    if (_geometriDialog->result() == QDialog::Accepted)
    {
        if (_geometriDialog->nameLine->text().toStdString() == "") {
            RW_THROW("Give geometry a name");
        }

        rw::models::WorkCell::Ptr wc = getRobWorkStudio()->getWorkCell();

        rw::models::WorkCell::Ptr dummy = rw::common::ownedPtr(new rw::models::WorkCell("dummy")); // Create dummy wc for swap
        getRobWorkStudio()->setWorkCell(dummy); // Temporarily swap out wc from rws

        try
        {
            rw::math::Vector3D<double> displacement(
                    _geometriDialog->doubleSpinBoxes[1]->value(),
                    _geometriDialog->doubleSpinBoxes[2]->value(),
                    _geometriDialog->doubleSpinBoxes[3]->value());

            rw::math::Transform3D<double> transform(displacement);

            if (_geometriDialog->checkFrames[0]->isChecked()) // check for new moveable
                ei::creator::addSphere(_geometriDialog->nameLine->text().toStdString(),
                                    _geometriDialog->comboFrames->currentText().toStdString(),
                                    wc,
                                    _geometriDialog->doubleSpinBoxesGeometires[0]->value(),
                                    transform);
            else
                ei::creator::addSphere(_geometriDialog->nameLine->text().toStdString(),
                                    wc->findFrame(_geometriDialog->comboFrames->currentText().toStdString()),
                                    wc,
                                    _geometriDialog->doubleSpinBoxesGeometires[0]->value(),
                                    transform);

            getRobWorkStudio()->setWorkCell(wc); // Swap back wc into rws
        }
        catch(const std::exception& ex)
        {
            getRobWorkStudio()->setWorkCell(wc); // Swap back wc into rws
            RW_THROW("Name of device allready in use");
        }
    }
}

void EasyInsert::cone()
{
    QString st = "Cone";
    rw::models::WorkCell::Ptr wc = getRobWorkStudio()->getWorkCell();
    _geometriDialog = new dialog(wc,st,this);

    _geometriDialog->addToDialog(_geometriDialog->createNameBox());
    _geometriDialog->addToDialog(_geometriDialog->createCheckFramesBox());
    _geometriDialog->addToDialog(_geometriDialog->createFrameSelection());
    _geometriDialog->addToDialog(_geometriDialog->createConfigurationBoxCone());
    _geometriDialog->addToDialog(_geometriDialog->createConfigurationBox());

    _geometriDialog->addToDialog(_geometriDialog->createButtonBox());

    _geometriDialog->exec();

    if (_geometriDialog->result() == QDialog::Accepted)
    {
        if (_geometriDialog->nameLine->text().toStdString() == "") {
            RW_THROW("Give geometry a name");
        }

        rw::models::WorkCell::Ptr wc = getRobWorkStudio()->getWorkCell();

        rw::models::WorkCell::Ptr dummy = rw::common::ownedPtr(new rw::models::WorkCell("dummy")); // Create dummy wc for swap
        getRobWorkStudio()->setWorkCell(dummy); // Temporarily swap out wc from rws

        try
        {
            rw::math::Vector3D<double> displacement(
                    _geometriDialog->doubleSpinBoxes[1]->value(),
                    _geometriDialog->doubleSpinBoxes[2]->value(),
                    _geometriDialog->doubleSpinBoxes[3]->value());

            rw::math::Transform3D<double> transform(displacement);

            if (_geometriDialog->checkFrames[0]->isChecked()) // check for new moveable
                ei::creator::addCone(_geometriDialog->nameLine->text().toStdString(),
                                    _geometriDialog->comboFrames->currentText().toStdString(),
                                    wc,
                                    _geometriDialog->doubleSpinBoxesGeometires[0]->value(),
                                    _geometriDialog->doubleSpinBoxesGeometires[1]->value(),
                                    transform);
            else
                ei::creator::addCone(_geometriDialog->nameLine->text().toStdString(),
                                    wc->findFrame(_geometriDialog->comboFrames->currentText().toStdString()),
                                    wc,
                                    _geometriDialog->doubleSpinBoxesGeometires[0]->value(),
                                    _geometriDialog->doubleSpinBoxesGeometires[1]->value(),
                                    transform);

            getRobWorkStudio()->setWorkCell(wc); // Swap back wc into rws
        }
        catch(const std::exception& ex)
        {
            getRobWorkStudio()->setWorkCell(wc); // Swap back wc into rws
            RW_THROW("Name of device allready in use");
        }
    }
}

void EasyInsert::cylinder()
{
    QString st = "Cylinder";
    rw::models::WorkCell::Ptr wc = getRobWorkStudio()->getWorkCell();
    _geometriDialog = new dialog(wc,st,this);

    _geometriDialog->addToDialog(_geometriDialog->createNameBox());
    _geometriDialog->addToDialog(_geometriDialog->createCheckFramesBox());
    _geometriDialog->addToDialog(_geometriDialog->createFrameSelection());
    _geometriDialog->addToDialog(_geometriDialog->createConfigurationBoxCone()); //cylinder and cone uses the same parameters
    _geometriDialog->addToDialog(_geometriDialog->createConfigurationBox());

    _geometriDialog->addToDialog(_geometriDialog->createButtonBox());

    _geometriDialog->exec();

    if (_geometriDialog->result() == QDialog::Accepted)
    {
        if (_geometriDialog->nameLine->text().toStdString() == "") {
            RW_THROW("Give geometry a name");
        }

        rw::models::WorkCell::Ptr wc = getRobWorkStudio()->getWorkCell();

        rw::models::WorkCell::Ptr dummy = rw::common::ownedPtr(new rw::models::WorkCell("dummy")); // Create dummy wc for swap
        getRobWorkStudio()->setWorkCell(dummy); // Temporarily swap out wc from rws

        try
        {
            rw::math::Vector3D<double> displacement(
                    _geometriDialog->doubleSpinBoxes[1]->value(),
                    _geometriDialog->doubleSpinBoxes[2]->value(),
                    _geometriDialog->doubleSpinBoxes[3]->value());

            rw::math::Transform3D<double> transform(displacement);

            if (_geometriDialog->checkFrames[0]->isChecked()) // check for new moveable
                ei::creator::addCylinder(_geometriDialog->nameLine->text().toStdString(),
                                    _geometriDialog->comboFrames->currentText().toStdString(),
                                    wc,
                                    _geometriDialog->doubleSpinBoxesGeometires[0]->value(),
                                    _geometriDialog->doubleSpinBoxesGeometires[1]->value(),
                                    transform);
            else
                ei::creator::addCylinder(_geometriDialog->nameLine->text().toStdString(),
                                    wc->findFrame(_geometriDialog->comboFrames->currentText().toStdString()),
                                    wc,
                                    _geometriDialog->doubleSpinBoxesGeometires[0]->value(),
                                    _geometriDialog->doubleSpinBoxesGeometires[1]->value(),
                                    transform);

            getRobWorkStudio()->setWorkCell(wc); // Swap back wc into rws
        }
        catch(const std::exception& ex)
        {
            getRobWorkStudio()->setWorkCell(wc); // Swap back wc into rws
            RW_THROW("Name of device allready in use");
        }
    }
}

void EasyInsert::tube()
{
    QString st = "Tube";
    rw::models::WorkCell::Ptr wc = getRobWorkStudio()->getWorkCell();
    _geometriDialog = new dialog(wc,st,this);

    _geometriDialog->addToDialog(_geometriDialog->createNameBox());
    _geometriDialog->addToDialog(_geometriDialog->createCheckFramesBox());
    _geometriDialog->addToDialog(_geometriDialog->createFrameSelection());
    _geometriDialog->addToDialog(_geometriDialog->createConfigurationBoxTube());
    _geometriDialog->addToDialog(_geometriDialog->createConfigurationBox());

    _geometriDialog->addToDialog(_geometriDialog->createButtonBox());

    _geometriDialog->exec();

    if (_geometriDialog->result() == QDialog::Accepted)
    {
        if (_geometriDialog->nameLine->text().toStdString() == "") {
            RW_THROW("Give geometry a name");
        }

        rw::models::WorkCell::Ptr wc = getRobWorkStudio()->getWorkCell();

        rw::models::WorkCell::Ptr dummy = rw::common::ownedPtr(new rw::models::WorkCell("dummy")); // Create dummy wc for swap
        getRobWorkStudio()->setWorkCell(dummy); // Temporarily swap out wc from rws

        try
        {
            rw::math::Vector3D<double> displacement(
                    _geometriDialog->doubleSpinBoxes[1]->value(),
                    _geometriDialog->doubleSpinBoxes[2]->value(),
                    _geometriDialog->doubleSpinBoxes[3]->value());

            rw::math::Transform3D<double> transform(displacement);

            if (_geometriDialog->checkFrames[0]->isChecked()) // check for new moveable
                ei::creator::addTube(_geometriDialog->nameLine->text().toStdString(),
                                    _geometriDialog->comboFrames->currentText().toStdString(),
                                    wc,
                                    _geometriDialog->doubleSpinBoxesGeometires[0]->value(),
                                    _geometriDialog->doubleSpinBoxesGeometires[1]->value(),
                                    _geometriDialog->doubleSpinBoxesGeometires[2]->value(),
                                    transform);
            else
                ei::creator::addTube(_geometriDialog->nameLine->text().toStdString(),
                                    wc->findFrame(_geometriDialog->comboFrames->currentText().toStdString()),
                                    wc,
                                    _geometriDialog->doubleSpinBoxesGeometires[0]->value(),
                                    _geometriDialog->doubleSpinBoxesGeometires[1]->value(),
                                    _geometriDialog->doubleSpinBoxesGeometires[2]->value(),
                                    transform);


            getRobWorkStudio()->setWorkCell(wc); // Swap back wc into rws
        }
        catch(const std::exception& ex)
        {
            getRobWorkStudio()->setWorkCell(wc); // Swap back wc into rws
            RW_THROW("Name of device allready in use");
        }
    }
}

void EasyInsert::fixedFrame()
{
    QString st = "Fixed Frame";
    rw::models::WorkCell::Ptr wc = getRobWorkStudio()->getWorkCell();
    _geometriDialog = new dialog(wc,st,this);

    _geometriDialog->addToDialog(_geometriDialog->createNameBox());
    //_geometriDialog->addToDialog(_geometriDialog->createCheckFramesBox());
    _geometriDialog->addToDialog(_geometriDialog->createFrameSelection());
    _geometriDialog->addToDialog(_geometriDialog->createConfigurationBox());

    _geometriDialog->addToDialog(_geometriDialog->createButtonBox());

    _geometriDialog->exec();

    if (_geometriDialog->result() == QDialog::Accepted)
    {
        if (_geometriDialog->nameLine->text().toStdString() == "") {
            RW_THROW("Give frame a name");
        }

        rw::models::WorkCell::Ptr wc = getRobWorkStudio()->getWorkCell();

        rw::models::WorkCell::Ptr dummy = rw::common::ownedPtr(new rw::models::WorkCell("dummy")); // Create dummy wc for swap
        getRobWorkStudio()->setWorkCell(dummy); // Temporarily swap out wc from rws

        try
        {
            rw::math::Vector3D<double> displacement(
                    _geometriDialog->doubleSpinBoxes[1]->value(),
                    _geometriDialog->doubleSpinBoxes[2]->value(),
                    _geometriDialog->doubleSpinBoxes[3]->value());

            rw::math::Transform3D<double> transform(displacement);

            ei::creator::addFixedFrame(wc,
                                _geometriDialog->nameLine->text().toStdString(),
                                _geometriDialog->comboFrames->currentText().toStdString(),
                                transform);

            getRobWorkStudio()->setWorkCell(wc); // Swap back wc into rws
        }
        catch(const std::exception& ex)
        {
            getRobWorkStudio()->setWorkCell(wc); // Swap back wc into rws
            RW_THROW("Name of frame allready in use");
        }
    }
}

void EasyInsert::movableFrame()
{
    QString st = "Movable Frame";
    rw::models::WorkCell::Ptr wc = getRobWorkStudio()->getWorkCell();
    _geometriDialog = new dialog(wc,st,this);

    _geometriDialog->addToDialog(_geometriDialog->createNameBox());
    //_geometriDialog->addToDialog(_geometriDialog->createCheckFramesBox());
    _geometriDialog->addToDialog(_geometriDialog->createFrameSelection());
    _geometriDialog->addToDialog(_geometriDialog->createConfigurationBox());

    _geometriDialog->addToDialog(_geometriDialog->createButtonBox());

    _geometriDialog->exec();

    if (_geometriDialog->result() == QDialog::Accepted)
    {
        if (_geometriDialog->nameLine->text().toStdString() == "") {
            RW_THROW("Give frame a name");
        }

        rw::models::WorkCell::Ptr wc = getRobWorkStudio()->getWorkCell();

        rw::models::WorkCell::Ptr dummy = rw::common::ownedPtr(new rw::models::WorkCell("dummy")); // Create dummy wc for swap
        getRobWorkStudio()->setWorkCell(dummy); // Temporarily swap out wc from rws

        try
        {
            rw::math::Vector3D<double> displacement(
                    _geometriDialog->doubleSpinBoxes[1]->value(),
                    _geometriDialog->doubleSpinBoxes[2]->value(),
                    _geometriDialog->doubleSpinBoxes[3]->value());

            rw::math::Transform3D<double> transform(displacement);

            ei::creator::addMovableFrame(wc,
                                _geometriDialog->nameLine->text().toStdString(),
                                _geometriDialog->comboFrames->currentText().toStdString(),
                                transform);

            getRobWorkStudio()->setWorkCell(wc); // Swap back wc into rws
        }
        catch(const std::exception& ex)
        {
            getRobWorkStudio()->setWorkCell(wc); // Swap back wc into rws
            RW_THROW("Name of frame allready in use");
        }
    }
}

void EasyInsert::deleteFrame()
{
    if (_frameWidget->currentItem() != NULL) {
        QListWidgetItem* item = _frameWidget->currentItem();

        rw::models::WorkCell::Ptr wc = getRobWorkStudio()->getWorkCell();
        rw::kinematics::Frame* frame = wc->findFrame(item->text().toStdString());
        std::vector<rw::models::Object::Ptr> object = wc->getObjects();

        rw::models::WorkCell::Ptr dummy = rw::common::ownedPtr(new rw::models::WorkCell("dummy")); // Create dummy wc for swap
        getRobWorkStudio()->setWorkCell(dummy); // Temporarily swap out wc from rws

        for (size_t i = 0; i < object.size(); i++) {
            const std::vector <rw::kinematics::Frame*>& objectFrames = object[i].get()->getFrames();

            for (size_t j = 0; j < objectFrames.size(); j++) {
                if (objectFrames[j] == frame) {
                    wc->removeObject(object[i].get());
                    break;
                }
            }
        }

        if (frame->getName() != "WORLD") {
            wc->remove(frame);
        }

        getRobWorkStudio()->setWorkCell(wc); // Swap back wc into rws
    }
    else
        rw::common::Log::log().info() << "select something " << std::endl;

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
    _state = getRobWorkStudio()->getState();
    showFrameStructure();
}

/*void EasyInsert::clearTreeContent()
{
    while (_treeWidget->topLevelItemCount() > 0) {
        delete _treeWidget->takeTopLevelItem(0);
    }

    _frameMap.clear();
    _drawableMap.clear();
}*/

void EasyInsert::clearListContent()
{
    _deviceWidget->clear();
    _devFrameList.clear();
    _frameWidget->clear();
    _frames.clear();
    _objectWidget->clear();
}

void EasyInsert::stateChangedListener(const State& state)
{
    _state = getRobWorkStudio()->getState();
    bool forceUpdate = false;
    BOOST_FOREACH( EasyInsert::FrameMap::value_type p, _frameMap){
        if( rw::kinematics::Kinematics::isDAF(p.second) ){
            // test if parent changed
            if(p.second->getParent(state)!=p.second->getParent(_state)){
                forceUpdate = true;
                break;
            }
        }
    }
    if(forceUpdate)
        update();
}


#if !RWS_USE_QT5
#include <QtCore/qplugin.h>
Q_EXPORT_PLUGIN(EasyInsert);
#endif