#include "cxjdatadialog.h"


cxjDataDialog::cxjDataDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::cxjDataDialog)
{
    ui->setupUi(this);
    Qt::WindowFlags windowFlag  = Qt::Dialog;
    windowFlag                  |= Qt::WindowMinimizeButtonHint;
    windowFlag                  |= Qt::WindowMaximizeButtonHint;
    windowFlag                  |= Qt::WindowCloseButtonHint;

    setWindowFlags(windowFlag);
}

cxjDataDialog::~cxjDataDialog()
{
    delete ui;
}

void cxjDataDialog::on_getCxjDataButton_clicked()
{
    emit getCxjData();
}
