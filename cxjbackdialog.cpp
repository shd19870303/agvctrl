#include "cxjbackdialog.h"


cxjBackDialog::cxjBackDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::cxjBackDialog)
{
    ui->setupUi(this);
    Qt::WindowFlags windowFlag  = Qt::Dialog;
    windowFlag                  |= Qt::WindowMinimizeButtonHint;
    windowFlag                  |= Qt::WindowMaximizeButtonHint;
    windowFlag                  |= Qt::WindowCloseButtonHint;

    setWindowFlags(windowFlag);
}

cxjBackDialog::~cxjBackDialog()
{
    delete ui;
}

void cxjBackDialog::on_getCxjBackButton_clicked()
{
    emit getCxjBack();
}
