#ifndef SA_BACKGROUNDITEM_H
#define SA_BACKGROUNDITEM_H

#include <QGraphicsItem>

class SA_BackgroundItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    SA_BackgroundItem();
protected:
    QRectF boundingRect() const ;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr);
//    void keyPressEvent(QKeyEvent *event);
private:
    QPixmap pix;

};

#endif // SA_BACKGROUNDITEM_H
