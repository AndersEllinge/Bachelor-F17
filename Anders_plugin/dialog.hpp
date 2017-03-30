#ifndef dialog_H
#define dialog_H

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


private slots:

private:
    void reject();

    QGroupBox *nameBox;
    QGroupBox *configuration;
    QLabel *labels[3];
    QDoubleSpinBox *doubleSpinBoxes[3];
    QLineEdit *nameLine;
    QVBoxLayout *mainLayout;



    QDialogButtonBox *buttonBox;

    QString fPath;
};

#endif
