#ifndef DRAWSCENE_H
#define DRAWSCENE_H

#include <QGraphicsScene>
#include <QGraphicsView>
#include "drawtool.h"

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
class QMenu;
class QPointF;
class QGraphicsLineItem;
class QFont;
class QGraphicsTextItem;
class QColor;
class QKeyEvent;
class QPainter;
QT_END_NAMESPACE

class GridTool
{
public:
    GridTool(const QSize &grid = QSize(3200,2400), const QSize & space = QSize(1000,1000));
    void paintGrid(QPainter *painter,const QRect & rect );
//protected:
    QSize m_sizeGrid;
    QSize m_sizeGridSpace;
};

//class GraphicsItemGroup;

class DrawScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit DrawScene(QObject *parent = nullptr);
    ~DrawScene();
    void setView(QGraphicsView * view ) { m_view = view ; }
    QGraphicsView * view() { return m_view; }
    void mouseEvent(QGraphicsSceneMouseEvent *mouseEvent );
//    GraphicsItemGroup * createGroup(const QList<QGraphicsItem *> &items ,bool isAdd = true);
//    void destroyGroup(QGraphicsItemGroup *group);

    void drawGrid(bool show_grid);

signals:
    void itemMoved(QGraphicsItem * item, const QPointF & oldPosition );
    void itemRotate(QGraphicsItem *item, const qreal oldAngle, QPoint center);
    void itemAdded(QList<QGraphicsItem *> myItems);
    void deleteAdded(QGraphicsItem *item);
    void itemResize(QGraphicsItem * item , int handle , const QPointF& scale );
    void itemControl(QGraphicsItem * item , int handle , const QPointF & newPos , const QPointF& lastPos_ );
    void recover_route();
    void recover_area();
protected:
    void drawBackground(QPainter *painter, const QRectF &rect) Q_DECL_OVERRIDE;
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) Q_DECL_OVERRIDE;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *e) Q_DECL_OVERRIDE;
    void keyReleaseEvent(QKeyEvent *e) Q_DECL_OVERRIDE;

    QGraphicsView * m_view;

    qreal m_dx;
    qreal m_dy;
    bool  m_moved;
    GridTool *m_grid;

public:
    bool show_grid = false;
    QColor scene_color = Qt::white;
};

#endif // DRAWSCENE_H
