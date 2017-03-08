#ifndef LISTVIEW_HPP
#define LISTVIEW_HPP

#include <QtCore>
#include <QtGui>
#include <QListView>

class QDragEnterEvent;
class QDropEvent;
class QMouseEvent;

class listView : public QWidget
{

Q_OBJECT

public:
    listView(QWidget *parent);
    ~listView();

signals:

protected:

private:
    QString sPath = "/home/osboxes/Desktop/Git/Bachelor-F17/Anders_plugin";
    QFileSystemModel *dirmodel;
    QListView *fileList;
};
#endif
