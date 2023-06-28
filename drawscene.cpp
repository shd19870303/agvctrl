#include "drawscene.h"
#include<QGraphicsSceneMouseEvent>
#include<QGraphicsRectItem>
#include <QDebug>
#include <QKeyEvent>
#include <vector>
#include <QPainter>

GridTool::GridTool(const QSize & grid , const QSize & space )
    :m_sizeGrid(grid)
    ,m_sizeGridSpace(1000,1000)
{
}

void GridTool::paintGrid(QPainter *painter, const QRect &rect)
{
    QColor c(Qt::darkCyan);
    QPen p(c);
    p.setStyle(Qt::DashLine);
    p.setWidthF(5);
    painter->setPen(p);

    painter->save();
    painter->setRenderHints(QPainter::Antialiasing,false);

    painter->fillRect(rect,Qt::white);

    for(int x = rect.left(); x < rect.right(); x += (int)(m_sizeGridSpace.width()))
    {
        painter->drawLine(x,rect.top(),x,rect.bottom());
    }
    for(int y = rect.top(); y < rect.bottom(); y += (int)(m_sizeGridSpace.height()))
    {
        painter->drawLine(rect.left(),y,rect.right(),y);
    }

    p.setStyle(Qt::SolidLine);
    p.setColor(Qt::black);
    painter->drawLine(rect.right(),rect.top(),rect.right(),rect.bottom());
    painter->drawLine(rect.left(),rect.bottom(),rect.right(),rect.bottom());

    //draw shadow
//    QColor c1(Qt::black);
//    painter->fillRect(QRect(rect.right()+1,rect.top()+2,2,rect.height()),c1.dark(200));
//    painter->fillRect(QRect(rect.left()+2,rect.bottom()+2,rect.width(),2),c1.dark(200));

    painter->restore();
}

DrawScene::DrawScene(QObject *parent)
    :QGraphicsScene(parent)
{
    m_view = nullptr;
    m_dx=m_dy=0;
    m_grid = new GridTool();

//    QGraphicsItem * item = addRect(QRectF(0,0,0,0));
//    item->setAcceptHoverEvents(true);
}

DrawScene::~DrawScene()
{
    delete m_grid;
}

void DrawScene::mouseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    switch( mouseEvent->type() ){
    case QEvent::GraphicsSceneMousePress:
        QGraphicsScene::mousePressEvent(mouseEvent);
        break;
    case QEvent::GraphicsSceneMouseMove:
        QGraphicsScene::mouseMoveEvent(mouseEvent);
        break;
    case QEvent::GraphicsSceneMouseRelease:
        QGraphicsScene::mouseReleaseEvent(mouseEvent);
        break;
    }
}

void DrawScene::drawGrid(bool show_grid)
{
    foreach (QGraphicsItem *item , items())
    {
        if(item->type() == QGraphicsLineItem::Type)
        {
            removeItem(item);
        }
    }

    QRect rect = sceneRect().toRect();
    QColor c(Qt::darkCyan);
    QPen p(c);
    p.setStyle(Qt::DashLine);
    p.setWidthF(5);
    if(show_grid)
    {
        for(int x = rect.left(); x < rect.right(); x += (int)(m_grid->m_sizeGridSpace.width()))
        {
            //painter->drawLine(x,rect.top(),x,rect.bottom());
            QLineF line(x, rect.top(), x, rect.bottom());
            QGraphicsLineItem* tmpVertical = new QGraphicsLineItem(line);
            tmpVertical->setZValue(-1.0);

            tmpVertical->setPen(p);

            //gridItermVec.push_back(tmpVertical);

            addItem(tmpVertical);
        }
        for(int y = rect.top(); y < rect.bottom(); y += (int)(m_grid->m_sizeGridSpace.height()))
        {
            //painter->drawLine(rect.left(),y,rect.right(),y);
            QLineF line(rect.left(), y, rect.right(), y);
            QGraphicsLineItem* tmpHorizontal = new QGraphicsLineItem(line);
            tmpHorizontal->setZValue(-1.0);
            tmpHorizontal->setPen(p);
            //gridItermVec.push_back(tmpHorizontal);

            addItem(tmpHorizontal);
        }
    }

}

void DrawScene::drawBackground(QPainter *painter, const QRectF &rect)
{
    //qDebug() << sceneRect();
    QGraphicsScene::drawBackground(painter,rect);

//    painter->fillRect(sceneRect(),Qt::white);//场景矩形色
    painter->fillRect(sceneRect(),scene_color);//场景矩形色


    if( m_grid ){
        if(show_grid)
        {
            //m_grid->paintGrid(painter,sceneRect().toRect());
            //drawGrid(show_grid);
        }
    }
}

void DrawScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    //qDebug() << sceneRect() << mouseEvent->scenePos();
    if(sceneRect().contains(mouseEvent->scenePos()))
    {
        DrawTool * tool = DrawTool::findTool( DrawTool::c_drawShape );
        if ( tool )
            tool->mousePressEvent(mouseEvent,this);
    }

    if(mouseEvent->button() == Qt::LeftButton)
    {
        if(itemAt(mouseEvent->scenePos(),QTransform()) == nullptr)
        {
            emit recover_route();
        }
    }
    else if(mouseEvent->button() == Qt::RightButton)
    {
        emit recover_area();

//        if(itemAt(mouseEvent->scenePos()) == nullptr)
//        {
//        }
    }

}

void DrawScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(sceneRect().contains(mouseEvent->scenePos()))
    {
        DrawTool * tool = DrawTool::findTool( DrawTool::c_drawShape );
        if ( tool )
            tool->mouseMoveEvent(mouseEvent,this);
    }
    else{
        view()->setCursor(Qt::ArrowCursor);
    }

}

void DrawScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    DrawTool * tool = DrawTool::findTool( DrawTool::c_drawShape );
    if ( tool )
        tool->mouseReleaseEvent(mouseEvent,this);
}

void DrawScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(sceneRect().contains(mouseEvent->scenePos()))
    {
        DrawTool * tool = DrawTool::findTool( DrawTool::c_drawShape );
        if ( tool )
            tool->mouseDoubleClickEvent(mouseEvent,this);
    }
}

void DrawScene::keyPressEvent(QKeyEvent *e)
{
    DrawTool * tool = DrawTool::findTool( DrawTool::c_drawShape );
    if ( tool )
    {
        tool->keyPressEvent(e,this);
    }
    QGraphicsScene::keyPressEvent(e);
}

void DrawScene::keyReleaseEvent(QKeyEvent *e)
{
    DrawTool * tool = DrawTool::findTool( DrawTool::c_drawShape );
    if ( tool )
    {
        tool->keyReleaseEvent(e,this);
    }
    QGraphicsScene::keyReleaseEvent(e);
}



