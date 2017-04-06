#ifndef dialog_H
#define dialog_H

#include <RobWorkStudioConfig.hpp>
#include <rws/RobWorkStudioPlugin.hpp>

#include <QDialog>
#include <QtWidgets>
#include <QScrollArea>

class QCheckBox;
class QLabel;
class QDoubleSpinBox;
class QPushButton;
class QLineEdit;
class QMenuBar;
class QGroupBox;
class QDialogButtonBox;
class QMenu;

class dialog : public QDialog
{
    Q_OBJECT

public:
    dialog(QString dialog,QWidget *parent = 0);
    dialog(QString filePath, QString dialog, QWidget *parent = 0);
    ~dialog();

    void addToDialog(QWidget *widget);
    QWidget* createButtonBox();
    QWidget* createNameBox();
    QWidget* createConfigurationBox();
    QWidget* createLibSettingsBox();

    rw::common::PropertyMap _propMap;
    rw::common::PropertyMap *_settingsMap;



private:
    void reject();
    void setupSettings();

    QGroupBox *nameBox;
    QGroupBox *configuration;
    QLabel *labels[3];
    QDoubleSpinBox *doubleSpinBoxes[3];

    QLineEdit *nameLine;
    QVBoxLayout *mainLayout;

    QLineEdit *pathLine[3];

    QDialogButtonBox *buttonBox;

    QString fPath;

private slots:
    void setDirectoryDialog(int i);

signals:
    void clicked(int);


};

#endif
