#include "station_backgrounditem.h"
#include <QPainter>
#include <QDebug>
#include <QFont>


Station_BackgroundItem::Station_BackgroundItem(QString num)
{
    number = num;
}

QRectF Station_BackgroundItem::boundingRect() const
{
    return QRectF(-50000,-50000,100000,100000);
}

void Station_BackgroundItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->scale(1,-1);
    QPen pen;
    pen.setColor(QColor(196,49,49));
    painter->setPen(pen);

    QFont font = painter->font();
    font.setPixelSize(8000);
    painter->setFont(font);

    painter->drawText(boundingRect().toRect(), Qt::AlignCenter, number);

}
