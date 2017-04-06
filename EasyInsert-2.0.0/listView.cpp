#include "listView.hpp"
#include "EasyInsert.hpp"

#include <QtCore>
#include <QtGui>
#include <QListView>

#include <rws/RobWorkStudio.hpp>


listView::listView(QWidget* parent)
    : QListView(parent)
{
    //dirmodel = new QFileSystemModel(this);
    //setModel(dirmodel);
    //setMinimumSize(200,400);
    ResizeMode(true);
    //setRootIndex(dirmodel->setRootPath(sPath));
	move(5,25);

}


listView::~listView()
{

}

void listView::updateSystemModelPath(QString path)
{
    //setRootIndex(dirmodel->setRootPath(path));
}

/*void listView::mousePressEvent(QMouseEvent *event)
{
    // get the buttons type
   Qt::MouseButtons mouseButton = event->buttons();
   // only the left mouse buton
   if( mouseButton == Qt::LeftButton )
   {
      //clear selection
      clearSelection();
      //select item at cursor position
      setCurrentIndex(indexAt(event->pos()));;
  }

}*/
