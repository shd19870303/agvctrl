#ifndef FLOAT_RECTITEM_H
#define FLOAT_RECTITEM_H

#include <QGraphicsRectItem>

class Float_RectItem : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
public:
    Float_RectItem();
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect();
};

#endif // FLOAT_RECTITEM_H
