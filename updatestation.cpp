#include "updatestation.h"
#include "ui_updatestation.h"
//#include <QRegExpValidator>
#include <QRegularExpressionValidator>
UpdateStation::UpdateStation(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UpdateStation)
{
    ui->setupUi(this);

    QRegularExpression regx("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b");
    QValidator* validator = new QRegularExpressionValidator(regx, ui->lineEditIP);
    ui->lineEditIP->setValidator(validator);
    ui->lineEditIP->setText("192.168.227.165");
}

UpdateStation::~UpdateStation()
{
    delete ui;
}

QString UpdateStation::getInputIP()
{
    return ui->lineEditIP->text();
}
