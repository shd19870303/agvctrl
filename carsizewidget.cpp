#include "carsizewidget.h"
#include "ui_carsizewidget.h"

CarSizeWidget::CarSizeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CarSizeWidget)
{
    ui->setupUi(this);
}

CarSizeWidget::~CarSizeWidget()
{
    delete ui;
}

void CarSizeWidget::on_confimButton_clicked()
{
    emit sendCarSizeInfo(ui->carTypeBox->currentText(), ui->carLEdit->text(), ui->carWEdit->text());
}

void CarSizeWidget::on_initButton_clicked()
{
    emit sendCarSizeInfo(ui->carTypeBox->currentText(), QString("-1"), QString("-1"));
}
