#ifndef CAR_INFO_H
#define CAR_INFO_H

#include <QGraphicsItem>

class Car_Info : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    explicit Car_Info(qreal x, qreal y);
    void set_info(int car_id, int task_id);
protected:
    QRectF boundingRect() const ;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr);
private:
    int car_id;
    int task_id;
    qreal x;
    qreal y;
};

#endif // CAR_INFO_H
