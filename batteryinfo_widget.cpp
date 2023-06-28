#include "batteryinfo_widget.h"


BatteryInfo_Widget::BatteryInfo_Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BatteryInfo_Widget)
{
    ui->setupUi(this);
    setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
    Qt::WindowFlags windowFlag  = Qt::Dialog;
    windowFlag                  |= Qt::WindowMinimizeButtonHint;
    windowFlag                  |= Qt::WindowMaximizeButtonHint;
    windowFlag                  |= Qt::WindowCloseButtonHint;

    setWindowFlags(windowFlag);
    ui->batteryDetailWidget->setVisible(false);

}

BatteryInfo_Widget::~BatteryInfo_Widget()
{
    delete ui;
}

void BatteryInfo_Widget::on_batteryInfoButton_clicked()
{
    emit getBatteryData();
}

void BatteryInfo_Widget::on_batteryTableWidget_cellDoubleClicked(int row, int column)
{
    if(column == 0)
    {
        int tempId = ui->batteryTableWidget->item(row,column)->text().toInt();
//        qDebug()<<"电池id: "<<tempId;
        emit getBatteryDetailData(tempId);
        ui->batteryDetailWidget->setVisible(true);
    }
    else
    {
        ui->batteryDetailWidget->setVisible(false);

    }

}




