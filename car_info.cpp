#include "car_info.h"
#include <QFont>
#include <QPainter>
Car_Info::Car_Info(qreal x, qreal y)
{
    setZValue(5);
    this->x = x;
    this->y = y;

}

void Car_Info::set_info(int car_id, int task_id)
{
    this->car_id = car_id;
    this->task_id = task_id;
}

QRectF Car_Info::boundingRect() const
{
    return QRectF(-16,-8,32,16);

}

void Car_Info::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setRenderHint(QPainter::Antialiasing);
    QFont font1;
    font1.setPointSize(5);
    font1.setBold(true);
    painter->setFont(font1);
    painter->setPen(QPen(Qt::blue));
    painter->drawText(boundingRect(), Qt::AlignCenter, tr("%1\n%2").arg(car_id).arg(task_id));
}
