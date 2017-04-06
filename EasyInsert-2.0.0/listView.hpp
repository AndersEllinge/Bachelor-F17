#ifndef LISTVIEW_HPP
#define LISTVIEW_HPP

#include <QListView>
#include <QFileSystemModel>


class listView : public QListView
{

Q_OBJECT

public:
    listView(QWidget* Parent);
    ~listView();

    void updateSystemModelPath(QString path);


private slots:

signals:

protected:

private:

    //void mousePressEvent(QMouseEvent *event);


    QString sPath = "/home/osboxes/Desktop/Git/Bachelor-F17/Anders_plugin";
    //QFileSystemModel *dirmodel;
    QListView *fileList;



};
#endif
