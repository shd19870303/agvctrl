#include "floating_dialog.h"
#include "ui_floating_dialog.h"
#include <QPainter>
#include <QDebug>
Floating_Dialog::Floating_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Floating_Dialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
}

Floating_Dialog::~Floating_Dialog()
{
    delete ui;
}

void Floating_Dialog::set_widgets_info(int dev_id, int device_type, int mission_id, int mission_type, int speed, int state, int status, int error_type, int battery, int timestamp)
{
//    if(ui->float_tableWidget->item(0,0) == nullptr)
//    {
//        qDebug()<<"its nullptr";
//    }
//    else
//    {
//        qDebug()<<ui->float_tableWidget->item(0,0)->text();
//    }

//    qDebug()<<QString::number(dev_id);
//    qDebug()<<"set_widgets_info   dev_id ="<<dev_id;
    ui->float_tableWidget->item(0,0)->setText(QString::number(dev_id));
    qDebug()<<"mytext"<<ui->float_tableWidget->item(0,0)->text();
//    ui->float_tableWidget->item(1,0)->setText(QString::number(device_type));
//    ui->float_tableWidget->item(2,0)->setText(QString::number(mission_id));
//    ui->float_tableWidget->item(3,0)->setText(QString::number(mission_type));
//    ui->float_tableWidget->item(4,0)->setText(QString::number(speed));
//    ui->float_tableWidget->item(5,0)->setText(QString::number(state));
//    ui->float_tableWidget->item(6,0)->setText(QString::number(status));
//    ui->float_tableWidget->item(7,0)->setText(QString::number(error_type));
//    ui->float_tableWidget->item(8,0)->setText(QString::number(battery));
//    ui->float_tableWidget->item(9,0)->setText(QString::number(timestamp));
}

void Floating_Dialog::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    // QColor最后一个参数代表alpha通道，一般用作透明度
    painter.fillRect(rect(), QColor(50, 50, 50, 0));
}
