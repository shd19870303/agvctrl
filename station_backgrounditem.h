#ifndef STATION_BACKGROUNDITEM_H
#define STATION_BACKGROUNDITEM_H

#include <QGraphicsItem>


class Station_BackgroundItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    Station_BackgroundItem(QString num);
protected:
    QRectF boundingRect() const ;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr);
//    void keyPressEvent(QKeyEvent *event);
private:
    QString number;

};

#endif // STATION_BACKGROUNDITEM_H
