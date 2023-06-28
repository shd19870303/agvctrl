#include "taskdialog.h"


taskDialog::taskDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::taskDialog)
{
    ui->setupUi(this);
    Qt::WindowFlags windowFlag  = Qt::Dialog;
    windowFlag                  |= Qt::WindowMinimizeButtonHint;
    windowFlag                  |= Qt::WindowMaximizeButtonHint;
    windowFlag                  |= Qt::WindowCloseButtonHint;

    setWindowFlags(windowFlag);

}

taskDialog::~taskDialog()
{
    delete ui;
}

void taskDialog::on_getTaskButton_clicked()
{
    sendTaskFlag = true;
    emit getTaskData();
}
