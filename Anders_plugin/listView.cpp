#include "listView.hpp"



listView::listView(QWidget *parent)
    : QWidget(parent)
{
    fileList = new QListView();
    fileList->setModel(dirmodel);
    fileList->setRootIndex(dirmodel->setRootPath(sPath));
    fileList->setMinimumSize(200,400);
}


listView::~listView()
{

}
