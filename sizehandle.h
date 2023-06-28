#ifndef SIZEHANDLE_H
#define SIZEHANDLE_H

#include <QGraphicsRectItem>
#ifndef SIZEHANDLE
#define SIZEHANDLE

#include <QGraphicsRectItem>
#include <QList>

QT_BEGIN_NAMESPACE
class QColor;
class QFocusEvent;
class QGraphicsItem;
class QGraphicsScene;
class QGraphicsSceneMouseEvent;
class QGraphicsSceneHoverEvent;
QT_END_NAMESPACE


enum { SELECTION_HANDLE_SIZE = 200, SELECTION_MARGIN = 10 }; //节点宽度大小
enum SelectionHandleState { SelectionHandleOff, SelectionHandleInactive, SelectionHandleActive };    //显示状态
enum { Handle_None = 0 , LeftTop , Top, RightTop, Right, RightBottom, Bottom, LeftBottom, Left };    //边框节点所在位置

class SizeHandleRect :public QGraphicsRectItem
{
public:

    SizeHandleRect(QGraphicsItem* parent , int d , bool control = false );
    int dir() const  { return m_dir; }
    void setState(SelectionHandleState st);   //设置当前状态
    void move(qreal x, qreal y );
protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *e );
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *e );
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    const int m_dir;            //唯一标识
    bool   m_controlPoint;      //是否是矩形框的节点还是多线段的节点
    SelectionHandleState m_state;   //显示状态
    QColor borderColor;         //绘色
};


#endif // SIZEHANDLE


class SizeHandle : public QGraphicsRectItem
{
public:
    SizeHandle();
};

#endif // SIZEHANDLE_H
