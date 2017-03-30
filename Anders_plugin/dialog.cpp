
#include "dialog.hpp"

dialog::dialog(QString dialog, QWidget *parent)
    : QDialog(parent)
{
    mainLayout = new QVBoxLayout();

    setLayout(mainLayout);

    setWindowTitle(dialog);

}


dialog::dialog(QString filePath, QString dialog, QWidget *parent)
    : QDialog(parent)
{
    mainLayout = new QVBoxLayout();

    setLayout(mainLayout);

    setWindowTitle(dialog);

}

void dialog::addToDialog(QWidget *widget)
{
    mainLayout->addWidget(widget);
    setLayout(mainLayout);
}

QWidget* dialog::createButtonBox()
{
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(deleteLater()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(deleteLater()));

    return buttonBox;
}

QWidget* dialog::createNameBox()
{
    nameBox = new QGroupBox(tr("Name"));
    QHBoxLayout *layout = new QHBoxLayout;

    nameLine = new QLineEdit();
    layout->addWidget(nameLine);

    nameBox->setLayout(layout);

    return nameBox;
}

QWidget* dialog::createConfigurationBox()
{
    configuration = new QGroupBox(tr("Configurations:"));
    QGridLayout *layout = new QGridLayout();
    for (int i = 0; i < 3; ++i)
    {
        if ( i == 1 )
            labels[i] = new QLabel(tr("X"));
        else if ( i == 2 )
            labels[i] = new QLabel(tr("Y"));
        else
            labels[i] = new QLabel(tr("Z"));

        doubleSpinBoxes[i] = new QDoubleSpinBox();
        layout->addWidget(labels[i], i + 1, 0);
        layout->addWidget(doubleSpinBoxes[i], i + 1, 1);
    }

    layout->setColumnStretch(1, 10);
    layout->setColumnStretch(2, 20);
    configuration->setLayout(layout);

    return configuration;
}

void dialog::reject()
{
    if (0) {
        QDialog::reject();
    }
}

dialog::~dialog()
{

}
