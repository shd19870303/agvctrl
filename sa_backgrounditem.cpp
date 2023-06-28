#include "sa_backgrounditem.h"
#include <QPainter>
#include <QKeyEvent>
#include <QDebug>
SA_BackgroundItem::SA_BackgroundItem()
{
//    pix.load(":/images/SA_1767_3487.png");
//    pix.load("./Image/SA_1767_3487.png");
    pix.load("./SA.png");
    grabKeyboard();
}

QRectF SA_BackgroundItem::boundingRect() const
{
    return QRectF(-17675,-33060,35350,66120);
}

void SA_BackgroundItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->scale(1,-1);
    painter->drawPixmap(boundingRect().toRect(),pix);
}

//void SA_BackgroundItem::keyPressEvent(QKeyEvent *event)
//{
//    switch (event->key()) {
//    case Qt::Key_Up:
//            qDebug()<<"上";
//            qDebug()<<"x ="<<this->x()<<"y ="<<this->y();
//        break;
//    default:
//        break;
//    }
//}
