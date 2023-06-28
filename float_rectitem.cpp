#include "float_rectitem.h"
#include <QPainter>
Float_RectItem::Float_RectItem()
{

}

void Float_RectItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
//    double scaleFactor = 1.0/painter->matrix().m11();
//    double scaleFactor2 = 1.0/painter->matrix().m22();
    double scaleFactor = 1.0/painter->transform().m11();
    double scaleFactor2 = 1.0/painter->transform().m22();

//    painter->scale(scaleFactor, scaleFactor2);
////    __super::paint(painter, option, widget);
//    QGraphicsRectItem::paint(painter, option, widget);
//    painter->drawRect(boundingRect().toRect());
}

QRectF Float_RectItem::boundingRect()
{
  return QRectF(0,0,244,128);
}
