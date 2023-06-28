#include "drawtool.h"
#include "drawobj.h"
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QGraphicsLineItem>
#include <QGraphicsProxyWidget>
#include <QtMath>

#include <QMessageBox>

#define PI 3.1416

QList<DrawTool*> DrawTool::c_tools;
QPoint DrawTool::c_down;
QPoint DrawTool::c_last;
quint32 DrawTool::c_nDownFlags;

//bool DrawTool::inited_drift = false;
//DriftInput * DrawTool::driftInput = 0;
//QGraphicsLineItem * DrawTool::driftItem = 0;

DrawShape DrawTool::c_drawShape = selection;

static SelectTool selectTool;
 PointTool   pointTool(point);
 LineTool   lineTool(line);
static ArcTool_startEndRadius   arcTool_startEndRadius(arc_startEndRadius);
static ArcTool_directRadiusAngle arcTool_directRadiusAngle(arc_directRadiusAngle);

static InterrputTool interrputTool(interrput);
static MergeTool     mergeTool(merge);
static MeasureTool   measureTool(measure);

//static PolygonTool lineTool(line);
//static PolygonTool polygonTool(polygon);
//static PolygonTool bezierTool(bezier);
//static PolygonTool polylineTool(polyline);

//static RotationTool rotationTool;

enum SelectMode
{
    none,
    netSelect,
    move,
    size,
    rotate,
    editor,
};

SelectMode selectMode = none;

static void setCursor(DrawScene * scene , const QCursor & cursor )
{
    QGraphicsView * view = scene->view();
    if (view)
        view->setCursor(cursor);
}

DrawTool::DrawTool(DrawShape shape)
{
    m_drawShape = shape ;
    m_hoverSizer = false;
    c_tools.push_back(this);
    mouse_pressed_count = 0;
}

void DrawTool::mousePressEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{
    c_down = event->scenePos().toPoint();
    c_last = event->scenePos().toPoint();
}

void DrawTool::mouseMoveEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{
    c_last = event->scenePos().toPoint();
    curScene = scene;
}

void DrawTool::mouseReleaseEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{
//    if (event->scenePos().toPoint() == c_down )
//        c_drawShape = selection;
    setCursor(scene,Qt::ArrowCursor);
    scene->update();
}

void DrawTool::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{

}

void DrawTool::keyPressEvent(QKeyEvent *event, DrawScene *scene)
{
    //qDebug() << "DrawTool::keyPressEvent" << event->key();
    event->ignore();
}

void DrawTool::keyReleaseEvent(QKeyEvent *event, DrawScene *scene)
{
    //qDebug() << "DrawTool::keyReleaseEvent" << event->key();
    event->ignore();
}

void DrawTool::movingDriftWidget(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{
    curScene = scene;

    if(inited_drift == false)
    {
        if(driftInput)
        {
            delete driftInput;
            driftInput = 0;
        }
        if(driftItem)
        {
            delete driftItem;
            driftItem = 0;
        }

        if(driftInputRadius)
        {
            delete driftInputRadius;
            driftInputRadius = 0;
        }

        if(driftItemRadius)
        {
            delete driftItemRadius;
            driftItemRadius = 0;
        }

        inited_drift = true;

        if(mouse_pressed_count < 2)
        {
            driftInput = new DriftInput;
            driftItem = new QGraphicsRectItem();
//            driftItem->scale(1,-1);
            driftItem->setTransform(QTransform::fromScale(1,-1),true);
            QGraphicsProxyWidget* pMyProxy = new QGraphicsProxyWidget(driftItem);
            driftItem->setPos(event->scenePos());
            driftItem->setZValue(0.2);
            pMyProxy->setWidget(driftInput);
            driftInput->ui->lineEdit_x->setFocus();
            connect(driftInput,SIGNAL(coordinateUpdate(int, int)),this,SLOT(addPointByXY(int, int)));
            scene->addItem(driftItem);
        }
        else
        {
            driftInputRadius = new DriftInputRadius;
            driftItemRadius = new QGraphicsRectItem();
//            driftItemRadius->scale(1,-1);
            driftItemRadius->setTransform(QTransform::fromScale(1,-1),true);

            QGraphicsProxyWidget* pMyProxy2 = new QGraphicsProxyWidget(driftItemRadius);
            driftItemRadius->setPos(event->scenePos());
            driftItemRadius->setZValue(0.2);
            pMyProxy2->setWidget(driftInputRadius);
            connect(driftInputRadius,SIGNAL(radiusUpdate(int)), this, SLOT(updateOneParamByDriftInput(int)));
            scene->addItem(driftItemRadius);
            inited_drift_one_param = true;
        }

    }

    if(mouse_pressed_count < 2)
    {
        if(driftInput)
        {
            if(driftInput->ui->lineEdit_x->hasFocus())
            {
                driftInput->ui->lineEdit_x->setText(QString("%1").arg((int)event->scenePos().x()));
                driftInput->ui->lineEdit_y->setText(QString("%1").arg((int)event->scenePos().y()));
                driftInput->ui->lineEdit_x->selectAll();
            }
            else if(driftInput->ui->lineEdit_y->hasFocus())
            {
                driftInput->ui->lineEdit_y->selectAll();
            }

        }
        if(driftItem)
        {
            if(driftInput && driftInput->ui->lineEdit_x->hasFocus())
            {
                driftItem->moveBy((event->scenePos() - driftItem->scenePos()).x()+100,(event->scenePos() - driftItem->scenePos()).y()-100);
            }
        }
    }
    else
    {
        if(inited_drift_one_param == false)
        {
            inited_drift = false;
        }
        if(driftInputRadius)
        {
            driftInputRadius->ui->lineEdit->setFocus();
            //driftInputRadius->ui->lineEdit->setText(QString("%1").arg(500));
            driftInputRadius->ui->lineEdit->selectAll();
        }
        if(driftItemRadius)
        {
            driftItemRadius->moveBy((event->scenePos() - driftItemRadius->scenePos()).x()+100,(event->scenePos() - driftItemRadius->scenePos()).y()-100);
        }
    }
}

void DrawTool::resetDriftWidget()
{
    if(driftInput)
    {
        driftInput->ui->lineEdit_x->setEnabled(true);
        driftInput->ui->lineEdit_x->setFocus();
        driftInput->ui->lineEdit_x->selectAll();
        driftInput->ui->lineEdit_y->setEnabled(false);
    }
}

int DrawTool::findSegmentId(DrawScene *scene)
{
    int segmentId = 0;
    QList<int> list_id;
    foreach (QGraphicsItem *item , scene->items()) {
        if(item->type() == GraphicsLineItem::Type)
        {
            GraphicsLineItem *lineItem = dynamic_cast<GraphicsLineItem *>(item);
            list_id.push_back(lineItem->map_segment.id);
        }
        else if(item->type() == GraphicsArcItem::Type)
        {
            GraphicsArcItem *arcItem = dynamic_cast<GraphicsArcItem *>(item);
            list_id.push_back(arcItem->map_segment.id);
        }
    }

    if(!list_id.isEmpty())
    {
//        qSort(list_id.begin(), list_id.end());
        std::sort(list_id.begin(), list_id.end());
        //qDebug() << list_id;
        int i = 1, j =0;
        for(i = 1; i < list_id.last(); i++)
        {
            for(j = 0; j < list_id.count(); j++)
            {
                if(i == list_id.at(j))   //判断list里是否包含
                {
                    break;
                }
            }
            if(j >= list_id.count())     // 不包含
            {
                segmentId = i;
                break;
            }
        }

        if(i >= list_id.last())
        {
            segmentId = list_id.last()+1;
        }
    }
    else{
        segmentId = 1;
    }

    return segmentId;
}

int DrawTool::findPointId(DrawScene *scene)
{
    int pointId = 0;

    QList<int> list_id;
    foreach (QGraphicsItem *item , scene->items()) {

        if(item->type() == GraphicsPointItem::Type)
        {
            GraphicsPointItem *pointItem = dynamic_cast<GraphicsPointItem *>(item);
            list_id.push_back(pointItem->map_point.id);
        }
    }

    if(!list_id.isEmpty())
    {
//        qSort(list_id.begin(), list_id.end());
        std::sort(list_id.begin(), list_id.end());
        //qDebug() << list_id;
        int i = 1, j =0;
        for(i = 1; i < list_id.last(); i++)
        {
            for(j = 0; j < list_id.count(); j++)
            {
                if(i == list_id.at(j))   //判断list里是否包含
                {
                    break;
                }
            }
            if(j >= list_id.count())     // 不包含
            {
                pointId = i;
                break;
            }
        }

        if(i >= list_id.last())
        {
            pointId = list_id.last()+1;
        }
    }
    else{
        pointId = 1;
    }

    return pointId;
}

DrawTool *DrawTool::findTool(DrawShape drawShape)
{
    QList<DrawTool*>::const_iterator iter = c_tools.constBegin();
    for ( ; iter != c_tools.constEnd() ; ++iter ){
        if ((*iter)->m_drawShape == drawShape )
            return (*iter);
    }
    return 0;
}


SelectTool::SelectTool()
    :DrawTool(selection)
{
    dashRect = 0;
    opposite_ = QPointF();
}

void SelectTool::mousePressEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{
    //qDebug() << " select mouse pressed";
    DrawTool::mousePressEvent(event,scene);
    if ( event->button() != Qt::LeftButton ) return;
    if (!m_hoverSizer)
       scene->mouseEvent(event);

    selectMode = none;
    QList<QGraphicsItem *> items = scene->selectedItems();
    GraphicsItem *item = 0;

    if ( items.count() == 1 )
    {
        item = dynamic_cast<GraphicsItem*>(items.first());
        if(item != 0 && item->type() == GraphicsRgvItem::Type)
        {
            GraphicsRgvItem * rgv_item = dynamic_cast<GraphicsRgvItem *>(item);
            //qDebug() << rgv_item->rgv_info.task_start_id << rgv_item->rgv_info.task_end_id;
            foreach (QGraphicsItem *item , scene->items()) {
                if ( item->type() == GraphicsPointItem::Type)
                {
                    GraphicsPointItem * my_point_item = dynamic_cast<GraphicsPointItem*>(item);
                    my_point_item->store_state = 0;
                    if(rgv_item->rgv_info.rgv_sub_state == TASKRUNNING)
                    {
                        if(rgv_item->rgv_info.task_start_id == my_point_item->map_point.id)
                        {
                            my_point_item->store_state = rgv_item->rgv_info.task_sub_state;
                        }
                        if(rgv_item->rgv_info.task_end_id == my_point_item->map_point.id)
                        {
                            my_point_item->store_state = rgv_item->rgv_info.task_sub_state;
                        }
                    }
                    else if(rgv_item->rgv_info.rgv_sub_state == MOVING)
                    {
                        if(rgv_item->rgv_info.task_end_id == my_point_item->map_point.id)
                        {
                            my_point_item->store_state = 5;
                        }
                    }
                }
            }

        }
    }
    else
    {
        foreach (QGraphicsItem *item , scene->items()) {
            if ( item->type() == GraphicsPointItem::Type)
            {
                GraphicsPointItem * my_point_item = dynamic_cast<GraphicsPointItem*>(item);
                my_point_item->store_state = 0;
            }
        }
    }

    if(item != 0 )
    {
        selectMode =  move;
        setCursor(scene,Qt::ClosedHandCursor);
    }
    else if(items.count() > 1)
    {
        selectMode =  move;
    }

    if(selectMode == none)
    {
        selectMode = netSelect;
        if( scene->view() )
        {
            QGraphicsView * view = scene->view();
            view->setDragMode(QGraphicsView::RubberBandDrag);
        }
    }

    if(selectMode == move && items.count() == 1)
    {
        if(dashRect)
        {
            scene->removeItem(dashRect);
            delete dashRect;
            dashRect = 0;
        }

        dashRect = new QGraphicsPathItem(item->shape());
        dashRect->setPen(Qt::DashLine);
        dashRect->setPos(item->pos());
        dashRect->setTransformOriginPoint(item->transformOriginPoint());
        dashRect->setTransform(item->transform());
        dashRect->setRotation(item->rotation());
        dashRect->setScale(item->scale());
        dashRect->setZValue(item->zValue());
        scene->addItem(dashRect);

        initialPositions = item->pos();

    }   

//    GraphicsRgvItem *rgv = new GraphicsRgvItem();
//    rgv->setPos(c_down);
//    scene->addItem(rgv);

}

void SelectTool::mouseMoveEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{
    //qDebug() << " mouseMoveEvent ";
    DrawTool::mouseMoveEvent(event,scene);

    QList<QGraphicsItem *> items = scene->selectedItems();
    GraphicsItem * item = 0;
    if( items.count() == 1)
    {
        item = dynamic_cast<GraphicsItem *>(items.first());
    }

    if(selectMode == move)
    {
        setCursor(scene, Qt::ClosedHandCursor);
        if(dashRect)
        {
            dashRect->setPos(initialPositions + c_last - c_down);
        }
    }

    if ( selectMode != size  && items.count() > 1)
    {
        scene->mouseEvent(event);

    }

    scene->mouseEvent(event);

}

void SelectTool::mouseReleaseEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{
    DrawTool::mouseReleaseEvent(event,scene);

    if(event->button() != Qt::LeftButton) return;

    QList<QGraphicsItem *> items = scene->selectedItems();

    if ( items.count() == 1 ){
        GraphicsItem * item = dynamic_cast<GraphicsItem*>(items.first());
        if ( item != 0  && selectMode == move && QLineF(c_down,c_last).length() > 100/*c_last != c_down*/ ){
 //            item->setPos(initialPositions + c_last - c_down);
 //            emit scene->itemMoved(item , c_last - c_down );
         }else if ( item !=0 && (selectMode == size || selectMode ==editor) && QLineF(c_down,c_last).length() > 100/*c_last != c_down*/ ){

            item->updateCoordinate();
        }
    }else if ( items.count() > 1 && selectMode == move && QLineF(c_down,c_last).length() > 100/*c_last != c_down*/ ){
 //         emit scene->itemMoved(NULL , c_last - c_down );
    }

    if(items.count() > 0 && selectMode == move && QLineF(c_down,c_last).length() > 100/*c_last != c_down*/)
    {
//        foreach (QGraphicsItem *item , items) {
//            if ( item->type() == GraphicsPointItem::Type )
//            {
//                GraphicsPointItem *point_item = dynamic_cast<GraphicsPointItem *>(item);
//                point_item->map_point.x = point_item->map_point.x + (c_last - c_down).x();
//                point_item->map_point.y = point_item->map_point.y + (c_last - c_down).y();
//                //point_item->setPos(point_item->map_point.x,point_item->map_point.y);
//            }
//        }
    }

    if (selectMode == netSelect )
    {

        if ( scene->view() ){
            QGraphicsView * view = scene->view();
            view->setDragMode(QGraphicsView::NoDrag);
        }
#if 0
        if ( scene->selectedItems().count() > 1 ){
            selLayer = scene->createGroup(scene->selectedItems());
            selLayer->setSelected(true);
        }
#endif
    }
    if (dashRect )
    {
        scene->removeItem(dashRect);
        delete dashRect;
        dashRect = 0;
    }
    selectMode = none;
    m_hoverSizer = false;
    opposite_ = QPointF();
    scene->mouseEvent(event);
}

RotationTool::RotationTool()
    :DrawTool(rotation)
{
    lastAngle = 0;
    dashRect = 0;
}

void RotationTool::mousePressEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{
    DrawTool::mousePressEvent(event,scene);
    if(event->button() != Qt::LeftButton) return;

    if(!m_hoverSizer)
    {
         scene->mouseEvent(event);
    }
    QList<QGraphicsItem *> items = scene->selectedItems();
    if( items.count() == 1)
    {
        GraphicsItem * item = dynamic_cast<GraphicsItem *>(items.first());
        if(item != 0)
        {
            c_drawShape = selection;
            selectTool.mousePressEvent(event,scene);
        }
    }
}

void RotationTool::mouseMoveEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{
    DrawTool::mouseMoveEvent(event,scene);
    QList<QGraphicsItem *> items = scene->selectedItems();
    if( items.count() == 1)
    {
        GraphicsItem * item = dynamic_cast<GraphicsItem *>(items.first());

    }

    scene->mouseEvent(event);
}

void RotationTool::mouseReleaseEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{
    DrawTool::mouseReleaseEvent(event,scene);
    if ( event->button() != Qt::LeftButton ) return;

    QList<QGraphicsItem *> items = scene->selectedItems();
    if(items.count() == 1)
    {
        GraphicsItem * item = dynamic_cast<GraphicsItem *>(items.first());
    }

    setCursor(scene,Qt::ArrowCursor);
    selectMode = none;
    lastAngle = 0;
    m_hoverSizer = false;
    if (dashRect ){
        scene->removeItem(dashRect);
        delete dashRect;
        dashRect = 0;
    }
    scene->mouseEvent(event);
}

PointTool::PointTool(DrawShape drawShape)
    :DrawTool(drawShape)
{
    item = 0;


}

void PointTool::mousePressEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{

    if ( event->button() != Qt::LeftButton ) return;

    scene->clearSelection();
    DrawTool::mousePressEvent(event,scene);

    addPoint(c_down.x(),c_down.y(),scene);  //在场景中添加一个点
    resetDriftWidget();
}

void PointTool::mouseMoveEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{
    //qDebug() << " PointTool::mouseMoveEvent ";

    setCursor(scene, Qt::CrossCursor);

    movingDriftWidget(event, scene);

    if(driftInput)
    {
        driftInput->ui->label->setText(tr("请输入点的坐标："));
    }

    selectTool.mouseMoveEvent(event,scene);
}

void PointTool::mouseReleaseEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{
    //DrawTool::mouseReleaseEvent(event,scene);
    selectTool.mouseReleaseEvent(event,scene);


}

void PointTool::addPoint(QPointF pos, DrawScene *curScene)
{
    addPoint(pos.toPoint().x(),pos.toPoint().y(),curScene);
}

void PointTool::addPoint(int x, int y, DrawScene *curScene)
{
    QGraphicsItem *temp_item = curScene->itemAt(QPoint(x,y),QTransform());
    if(temp_item != 0 && temp_item->type() == GraphicsPointItem::Type)
    {
        return;
    }

    last_point_id = point_id;

    item = new GraphicsPointItem(QRect(-100,-100,200,200));

    if ( item == 0) return;

    item->setPos(QPoint(x,y));
    item->map_point.x = x;
    item->map_point.y = y;

    item->map_point.id = findPointId(curScene);

    point_id = item->map_point.id;

    item->map_point.QRcode = 0;
    item->map_point.type = 0;
    item->map_point.stype = 0;
    item->map_point.atype = 0;
    item->map_point.dist = 0;
    item->map_point.chargeDir = 0;
    item->map_point.Infrared = 0;
    item->map_point.angle = 0;

    curScene->addItem(item);
    item->setSelected(true);

    if( item ){
        QList<QGraphicsItem *> myItems;
        myItems.append(item);
        emit curScene->itemAdded( myItems );
    }

    QPointF pot;
    //qDebug() << item->pos() << "mapFromScene:"<< item->mapFromScene(pot) << "mapFromParent:" <<item->mapFromParent(pot)
    //         << "mapToScene:"<< item->mapToScene(pot) << "mapToParent:" <<item->mapToParent(pot);


//    selectMode = size;

}

void PointTool::addPointByXY(int x, int y)
{
    curScene->clearSelection();

    addPoint(x, y, curScene);
}

void PointTool::updateOneParamByDriftInput(int param)
{

}


LineTool::LineTool(DrawShape shape)
    :DrawTool(shape)
{
    line_item = nullptr;
    m_nPoints = 0;
}

void LineTool::mousePressEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{
    DrawTool::mousePressEvent(event,scene);

    if( event->button() == Qt::LeftButton )
    {
        resetDriftWidget();

        if(dashLine)
        {
            scene->removeItem(dashLine);
            delete dashLine;
            dashLine = 0;
        }

        QGraphicsItem *item = scene->itemAt(event->scenePos(),QTransform());

        if(mouse_pressed_count ==0)
        {
            mouse_pressed_count++;
            line_item = new GraphicsLineItem(0);
            if(item != 0 && item->type() == GraphicsPointItem::Type)
            {
                line_item->setPos(item->pos());
                initialPositions = item->pos();
                line_item->addPoint(initialPositions);
                GraphicsPointItem * my_point_item = dynamic_cast<GraphicsPointItem *>(item);
                line_item->map_segment.start_id = my_point_item->map_point.id;
                m_nPoints++;
            }
            else
            {
                pointTool.mousePressEvent(event,scene);
                line_item->setPos(c_down);
                initialPositions = c_down;
                line_item->addPoint(c_down);
                line_item->map_segment.start_id = pointTool.point_id;
                m_nPoints++;
            }
            line_item->addPoint(c_last);
            m_nPoints++;
        }
        else if(mouse_pressed_count == 1)
        {
            mouse_pressed_count++;

            if(item != 0 && item->type() == GraphicsPointItem::Type)
            {
                //qDebug() << "select a point for line end";
                line_item->m_points.begin()[1] = line_item->mapFromScene(item->pos());

                GraphicsPointItem * my_point_item = dynamic_cast<GraphicsPointItem *>(item);
                line_item->map_segment.end_id = my_point_item->map_point.id;
            }
            else
            {
                pointTool.mousePressEvent(event,scene);
                line_item->map_segment.end_id = pointTool.point_id;
                //line_item->map_segment.end_mileage =
            }

            line_item->map_segment.id = findSegmentId(scene);

            linef.setPoints(line_item->mapToScene(line_item->m_points.at(0)),line_item->mapToScene(line_item->m_points.at(1)));

            line_item->map_segment.start_x = linef.x1();
            line_item->map_segment.start_y = linef.y1();
            line_item->map_segment.end_x = linef.x2();
            line_item->map_segment.end_y = linef.y2();
            line_item->map_segment.cen_x = 0;
            line_item->map_segment.cen_y = 0;
            line_item->map_segment.radius = 0;
            line_item->map_segment.lenght = linef.length();
            line_item->map_segment.type = 0;
            line_item->map_segment.dirmode = 0;
            line_item->map_segment.start_ang = linef.angle();
            line_item->map_segment.end_ang = linef.angle();
            line_item->map_segment.speed_limit = 0;

            scene->addItem(line_item);
            scene->clearSelection();
            line_item->setSelected(true);           

            //qDebug() << line_item->mapToScene(line_item->m_points.at(0)) << line_item->mapToScene(line_item->m_points.at(1));

        }
    }

    if(event->button() == Qt::RightButton && mouse_pressed_count == 1)
    {
        //scene->removeItem(pointTool.item);

        //emit scene->deleteAdded( pointTool.item );

        mouse_pressed_count = 0;
        m_nPoints = 0;
        line_item->m_points.clear();
    }

}

void LineTool::mouseMoveEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{
    setCursor(scene, Qt::CrossCursor);

    DrawTool::mouseMoveEvent(event,scene);

    if(mouse_pressed_count == 0)
    {
        if(driftInput)
        {
            driftInput->ui->label->setText(tr("请输入起点坐标："));
        }
    }
    if(mouse_pressed_count == 1)
    {
        if(driftInput)
        {
            driftInput->ui->label->setText(tr("请输入端点坐标："));
        }

        line_item->m_points.begin()[1] = line_item->mapFromScene(c_last);
        //qDebug() << "m_points size:"<< line_item->m_points.size() << c_last  << line_item->m_points.begin()[1];

        if(dashLine)
        {
            scene->removeItem(dashLine);
            delete dashLine;
            dashLine = 0;
        }

        dashLine = new QGraphicsPathItem(line_item->shape());
        dashLine->setPen(Qt::DashLine);
        dashLine->setPos(line_item->pos());
        dashLine->setTransformOriginPoint(line_item->transformOriginPoint());
        dashLine->setTransform(line_item->transform());
        dashLine->setRotation(line_item->rotation());
        dashLine->setScale(line_item->scale());
        dashLine->setZValue(line_item->zValue());
        scene->addItem(dashLine);

        initialPositions = line_item->pos();
    }
    else{
        if(dashLine)
        {
            scene->removeItem(dashLine);
            delete dashLine;
            dashLine = 0;
        }
    }

    movingDriftWidget(event, scene);

    scene->mouseEvent(event);

}

void LineTool::mouseReleaseEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{
    if(mouse_pressed_count > 0)
    {
        pointTool.mouseReleaseEvent(event,scene);
    }

    if(mouse_pressed_count == 2)
    {
        mouse_pressed_count = 0;
        m_nPoints = 0;
        if( line_item ){
            QList<QGraphicsItem *> myItems;
            myItems.append(line_item);
            emit scene->itemAdded( myItems );
        }
    }

    if (dashLine ){
        scene->removeItem(dashLine);
        delete dashLine;
        dashLine = 0;
    }
}

void LineTool::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{

}

void LineTool::resetVariable()
{
    mouse_pressed_count = 0;
    inited_drift = false;
    inited_drift_one_param = 0;
    line_item->m_points.clear();

    if(dashLine)
    {
        curScene->removeItem(dashLine);
        delete dashLine;
        dashLine = 0;
    }
}

void LineTool::addPointByXY(int x, int y, DrawScene *scene)
{
    if(dashLine)
    {
        scene->removeItem(dashLine);
        delete dashLine;
        dashLine = 0;
    }

    QGraphicsItem *item = scene->itemAt(QPointF(x,y),QTransform());

    if(mouse_pressed_count ==0)
    {
        mouse_pressed_count++;
        line_item = new GraphicsLineItem(0);
        if(item != 0 && item->type() == GraphicsPointItem::Type)
        {
            line_item->setPos(item->pos());
            initialPositions = item->pos();
            line_item->addPoint(initialPositions);
            GraphicsPointItem * my_point_item = dynamic_cast<GraphicsPointItem *>(item);
            line_item->map_segment.start_id = my_point_item->map_point.id;

            m_nPoints++;
        }
        else
        {
            pointTool.addPoint(x, y, scene);
            line_item->setPos(QPoint(x,y));
            initialPositions = QPoint(x,y);
            line_item->addPoint(QPoint(x,y));
            line_item->map_segment.start_id = pointTool.point_id;
            m_nPoints++;
        }

        line_item->addPoint(c_last);
        m_nPoints++;
    }
    else if(mouse_pressed_count == 1)
    {
        mouse_pressed_count++;

        if(item != 0 && item->type() == GraphicsPointItem::Type)
        {
            line_item->m_points.begin()[1] = line_item->mapFromScene(item->pos());

            GraphicsPointItem * my_point_item = dynamic_cast<GraphicsPointItem *>(item);
            line_item->map_segment.end_id = my_point_item->map_point.id;

        }
        else
        {
            pointTool.addPoint(x, y, scene);
            line_item->map_segment.end_id = pointTool.point_id;

            line_item->m_points.begin()[1] = line_item->mapFromScene(QPointF(x,y));
        }


        line_item->map_segment.id = findSegmentId(scene);

        linef.setPoints(line_item->mapToScene(line_item->m_points.at(0)),line_item->mapToScene(line_item->m_points.at(1)));

        line_item->map_segment.start_x = linef.x1();
        line_item->map_segment.start_y = linef.y1();
        line_item->map_segment.end_x = linef.x2();
        line_item->map_segment.end_y = linef.y2();
        line_item->map_segment.cen_x = 0;
        line_item->map_segment.cen_y = 0;
        line_item->map_segment.radius = 0;
        line_item->map_segment.lenght = linef.length();
        line_item->map_segment.type = 0;
        line_item->map_segment.dirmode = 0;
        line_item->map_segment.start_ang = linef.angle();
        line_item->map_segment.end_ang = linef.angle();
        line_item->map_segment.speed_limit = 0;

        scene->addItem(line_item);
        scene->clearSelection();
        line_item->setSelected(true);

        //qDebug() << line_item->mapToScene(line_item->m_points.at(0)) << line_item->mapToScene(line_item->m_points.at(1));
        if(mouse_pressed_count == 2)
        {
            mouse_pressed_count = 0;
            if( line_item ){
                QList<QGraphicsItem *> myItems;
                myItems.append(line_item);
                emit scene->itemAdded( myItems );
            }
        }
    }
}


void LineTool::addPointByXY(int x, int y)
{
    addPointByXY(x, y, curScene);

}

void LineTool::updateOneParamByDriftInput(int param)
{

}


ArcTool_startEndRadius::ArcTool_startEndRadius(DrawShape shape)
    :DrawTool(shape)
{
    arc_item = nullptr;
    m_nPoints = 0;
}

void ArcTool_startEndRadius::mousePressEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{
    DrawTool::mousePressEvent(event,scene);

    if( event->button() == Qt::LeftButton )
    {

        resetDriftWidget();

        if(dashArc)
        {
            scene->removeItem(dashArc);
            delete dashArc;
            dashArc = 0;
        }
        if(dashLine)
        {
            scene->removeItem(dashLine);
            delete dashLine;
            dashLine = 0;
        }       

        QGraphicsItem *item = scene->itemAt(event->scenePos(),QTransform());

        if(mouse_pressed_count ==0)
        {
            mouse_pressed_count++;

            arc_item = new GraphicsArcItem(0);
            if(item != 0 && item->type() == GraphicsPointItem::Type)
            {
                arc_item->setPos(item->pos());
                initialPositions = item->pos();
                arc_item->addPoint(initialPositions);
                GraphicsPointItem * my_point_item = dynamic_cast<GraphicsPointItem *>(item);
                arc_item->map_segment.start_id = my_point_item->map_point.id;

                m_nPoints++;
            }
            else
            {
                pointTool.mousePressEvent(event,scene);
                arc_item->setPos(c_down);
                initialPositions = c_down;
                arc_item->addPoint(c_down);
                arc_item->map_segment.start_id = pointTool.point_id;
                m_nPoints++;
            }

            arc_item->addPoint(c_last);
            //arc_item->cur_point = arc_item->mapFromScene(event->scenePos());
            m_nPoints++;
        }
        else if(mouse_pressed_count == 1)
        {
            mouse_pressed_count++;

            if(item != 0 && item->type() == GraphicsPointItem::Type)
            {
                //qDebug() << "select a point for line end";
                arc_item->m_points.begin()[1] = arc_item->mapFromScene(item->pos());
                GraphicsPointItem * my_point_item = dynamic_cast<GraphicsPointItem *>(item);
                arc_item->map_segment.end_id = my_point_item->map_point.id;
            }
            else
            {
                pointTool.mousePressEvent(event,scene);
                arc_item->map_segment.end_id = pointTool.point_id;
            }

        }
        else if(mouse_pressed_count == 2)
        {
            mouse_pressed_count++;           

            arc_item->map_segment.id = findSegmentId(scene);

            arc_item->map_segment.start_x = arc_item->mapToScene(arc_item->m_points.at(0)).x();
            arc_item->map_segment.start_y = arc_item->mapToScene(arc_item->m_points.at(0)).y();
            arc_item->map_segment.end_x = arc_item->mapToScene(arc_item->m_points.at(1)).x();
            arc_item->map_segment.end_y = arc_item->mapToScene(arc_item->m_points.at(1)).y();
            arc_item->map_segment.cen_x = arc_item->mapToScene(arc_item->cen).x();
            arc_item->map_segment.cen_y = arc_item->mapToScene(arc_item->cen).y();
            arc_item->map_segment.radius = arc_item->Radius;
            arc_item->map_segment.lenght = arc_item->arc_path.length();
            arc_item->map_segment.type = 1;
            arc_item->map_segment.dirmode = 0;
//            if(arc_item->sweepLength > 0)
//            {
//                arc_item->map_segment.start_ang = arc_item->startAngle + 90.0;
//            }
//            else{
//                arc_item->map_segment.start_ang = arc_item->startAngle - 90.0;
//            }
//            arc_item->map_segment.end_ang = arc_item->map_segment.start_ang + arc_item->sweepLength;
            if(arc_item->sweepLength > 0)
            {
                arc_item->map_segment.start_ang = arc_item->startAngle + 90.0;
                if(arc_item->map_segment.start_ang >= 360.0)
                {
                    arc_item->map_segment.start_ang -= 360.0;
                }
            }
            else{
                arc_item->map_segment.start_ang = arc_item->startAngle - 90.0;
                if(arc_item->map_segment.start_ang < 0)
                {
                    arc_item->map_segment.start_ang += 360.0;
                }
            }
            arc_item->map_segment.end_ang = arc_item->map_segment.start_ang + arc_item->sweepLength;
            if(arc_item->map_segment.end_ang >= 360.0)
            {
                arc_item->map_segment.end_ang -= 360.0;
            }
            else if(arc_item->map_segment.end_ang < 0)
            {
                arc_item->map_segment.end_ang += 360;
            }

            if(qAbs(arc_item->map_segment.start_ang - 360.0) < 0.001)
            {
                arc_item->map_segment.start_ang = 0;
            }
            if(qAbs(arc_item->map_segment.end_ang - 360.0) < 0.001)
            {
                arc_item->map_segment.end_ang = 0;
            }

            arc_item->map_segment.speed_limit = 0;
            scene->addItem(arc_item);
            scene->clearSelection();
            arc_item->setSelected(true);

        }
    }

    if(event->button() == Qt::RightButton && mouse_pressed_count == 1)
    {
//        scene->removeItem(pointTool.item);

//        emit scene->deleteAdded( pointTool.item );

        mouse_pressed_count = 0;
        m_nPoints = 0;
        arc_item->m_points.clear();
    }

}

void ArcTool_startEndRadius::mouseMoveEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{
    setCursor(scene, Qt::CrossCursor);

    DrawTool::mouseMoveEvent(event,scene);

    if(arc_item && (mouse_pressed_count == 2))
    {
        arc_item->cur_point = arc_item->mapFromScene(event->scenePos());
    }

    if(mouse_pressed_count == 0)
    {
        if(driftInput)
        {
            driftInput->ui->label->setText(tr("请输入起点坐标："));
        }
    }
    else if(mouse_pressed_count == 1)
    {
        if(driftInput)
        {
            driftInput->ui->label->setText(tr("请输入端点坐标："));
        }

        arc_item->m_points.begin()[1] = arc_item->mapFromScene(c_last);
        //qDebug() << "m_points size:"<< line_item->m_points.size() << c_last  << line_item->m_points.begin()[1];

        if(dashLine)
        {
            scene->removeItem(dashLine);
            delete dashLine;
            dashLine = 0;
        }
        if(dashArc)
        {
            scene->removeItem(dashArc);
            delete dashArc;
            dashArc = 0;
        }

//        dashLine = new QGraphicsPathItem(arc_item->shape());
//        dashLine->setPen(Qt::DashLine);
//        dashLine->setPos(arc_item->pos());
//        dashLine->setTransformOriginPoint(arc_item->transformOriginPoint());
//        dashLine->setTransform(arc_item->transform());
//        dashLine->setRotation(arc_item->rotation());
//        dashLine->setScale(arc_item->scale());
//        dashLine->setZValue(arc_item->zValue());
//        scene->addItem(dashLine);

        initialPositions = arc_item->pos();
    }
    else if(mouse_pressed_count == 2)
    {
        arc_item->get_arc_path(0,0);
        if(driftInputRadius)
        {
            driftInputRadius->ui->lineEdit->setText(QString("%1").arg((int)arc_item->Radius));
        }

        if(dashArc)
        {
            scene->removeItem(dashArc);
            delete dashArc;
            dashArc = 0;
        }

        dashArc = new QGraphicsPathItem(arc_item->shape());
        dashArc->setPen(Qt::DashLine);
        dashArc->setPos(arc_item->pos());
        dashArc->setTransformOriginPoint(arc_item->transformOriginPoint());
        dashArc->setTransform(arc_item->transform());
        dashArc->setRotation(arc_item->rotation());
        dashArc->setScale(arc_item->scale());
        dashArc->setZValue(arc_item->zValue());
        scene->addItem(dashArc);

    }
    else
   {
        if(dashLine)
        {
            scene->removeItem(dashLine);
            delete dashLine;
            dashLine = 0;
        }
        if(dashArc)
        {
            scene->removeItem(dashArc);
            delete dashArc;
            dashArc = 0;
        }
    }

    movingDriftWidget(event, scene);

    scene->mouseEvent(event);
}

void ArcTool_startEndRadius::mouseReleaseEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{
    if(mouse_pressed_count > 0)
    {
        pointTool.mouseReleaseEvent(event,scene);
    }

    if(mouse_pressed_count == 3)
    {
        mouse_pressed_count = 0;
        inited_drift = false;
        inited_drift_one_param = false;
        m_nPoints = 0;
        if( arc_item ){
            QList<QGraphicsItem *> myItems;
            myItems.append(arc_item);
            emit scene->itemAdded( myItems );
        }
    }

    if (dashLine ){
        scene->removeItem(dashLine);
        delete dashLine;
        dashLine = 0;
    }
}

void ArcTool_startEndRadius::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{

}

void ArcTool_startEndRadius::resetVariable()
{
    mouse_pressed_count = 0;
    inited_drift = 0;
    inited_drift_one_param = 0;

    arc_item->m_points.clear();

    if(dashArc)
    {
        curScene->removeItem(dashArc);
        delete dashArc;
        dashArc = 0;
    }
    if(dashLine)
    {
        curScene->removeItem(dashLine);
        delete dashLine;
        dashLine = 0;
    }

    if(driftInput)
    {
        delete  driftInput;
        driftInput = 0;
    }
    if(driftItem)
    {
        delete driftItem;
        driftItem = 0;
    }

    if(driftInputRadius)
    {
        delete driftInputRadius;
        driftInputRadius = 0;
    }

    if(driftItemRadius)
    {
        delete driftItemRadius;
        driftItemRadius = 0;
    }

}

void ArcTool_startEndRadius::addPointByXY(int x, int y)
{
    if(dashArc)
    {
        curScene->removeItem(dashArc);
        delete dashArc;
        dashArc = 0;
    }
    if(dashLine)
    {
        curScene->removeItem(dashLine);
        delete dashLine;
        dashLine = 0;
    }

    if(mouse_pressed_count ==0)
    {
        mouse_pressed_count++;

        arc_item = new GraphicsArcItem(0);

        pointTool.addPoint(x, y, curScene);
        arc_item->setPos(QPoint(x, y));
        initialPositions = QPoint(x, y);
        arc_item->addPoint(QPoint(x, y));
        arc_item->map_segment.start_id = pointTool.point_id;
        m_nPoints++;


        arc_item->addPoint(c_last);
        //arc_item->cur_point = arc_item->mapFromScene(event->scenePos());
        m_nPoints++;
    }
    else if(mouse_pressed_count == 1)
    {
        mouse_pressed_count++;

        pointTool.addPoint(x, y, curScene);
        arc_item->map_segment.end_id = pointTool.point_id;


    }
}

void ArcTool_startEndRadius::updateOneParamByDriftInput(int param)
{
    arc_item->get_arc_path(param,0);

    if(mouse_pressed_count == 2)
    {
        mouse_pressed_count++;

        arc_item->map_segment.id = findSegmentId(curScene);

        arc_item->map_segment.start_x = arc_item->mapToScene(arc_item->m_points.at(0)).x();
        arc_item->map_segment.start_y = arc_item->mapToScene(arc_item->m_points.at(0)).y();
        arc_item->map_segment.end_x = arc_item->mapToScene(arc_item->m_points.at(1)).x();
        arc_item->map_segment.end_y = arc_item->mapToScene(arc_item->m_points.at(1)).y();
        arc_item->map_segment.cen_x = arc_item->mapToScene(arc_item->cen).x();
        arc_item->map_segment.cen_y = arc_item->mapToScene(arc_item->cen).y();
        arc_item->map_segment.radius = arc_item->Radius;
        arc_item->map_segment.lenght = arc_item->arc_path.length();
        arc_item->map_segment.type = 1;
        arc_item->map_segment.dirmode = 0;
//            if(arc_item->sweepLength > 0)
//            {
//                arc_item->map_segment.start_ang = arc_item->startAngle + 90.0;
//            }
//            else{
//                arc_item->map_segment.start_ang = arc_item->startAngle - 90.0;
//            }
//            arc_item->map_segment.end_ang = arc_item->map_segment.start_ang + arc_item->sweepLength;
        if(arc_item->sweepLength > 0)
        {
            arc_item->map_segment.start_ang = arc_item->startAngle + 90.0;
            if(arc_item->map_segment.start_ang >= 360.0)
            {
                arc_item->map_segment.start_ang -= 360.0;
            }
        }
        else{
            arc_item->map_segment.start_ang = arc_item->startAngle - 90.0;
            if(arc_item->map_segment.start_ang < 0)
            {
                arc_item->map_segment.start_ang += 360.0;
            }
        }
        arc_item->map_segment.end_ang = arc_item->map_segment.start_ang + arc_item->sweepLength;
        if(arc_item->map_segment.end_ang >= 360.0)
        {
            arc_item->map_segment.end_ang -= 360.0;
        }
        else if(arc_item->map_segment.end_ang < 0)
        {
            arc_item->map_segment.end_ang += 360;
        }

        if(qAbs(arc_item->map_segment.start_ang - 360.0) < 0.001)
        {
            arc_item->map_segment.start_ang = 0;
        }
        if(qAbs(arc_item->map_segment.end_ang - 360.0) < 0.001)
        {
            arc_item->map_segment.end_ang = 0;
        }

        arc_item->map_segment.speed_limit = 0;
        curScene->addItem(arc_item);
        curScene->clearSelection();
        arc_item->setSelected(true);

        if(mouse_pressed_count == 3)
        {
            mouse_pressed_count = 0;
            inited_drift = false;
            inited_drift_one_param = false;
            m_nPoints = 0;
            if( arc_item ){
                QList<QGraphicsItem *> myItems;
                myItems.append(arc_item);
                emit curScene->itemAdded( myItems );
            }
        }

    }

    if(dashLine)
    {
        curScene->removeItem(dashLine);
        delete dashLine;
        dashLine = 0;
    }
    if(dashArc)
    {
        curScene->removeItem(dashArc);
        delete dashArc;
        dashArc = 0;
    }
}


ArcTool_directRadiusAngle::ArcTool_directRadiusAngle(DrawShape shape)
    :DrawTool(shape)
{
    arc_item = nullptr;
    m_nPoints = 0;
}

void ArcTool_directRadiusAngle::mousePressEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{
    DrawTool::mousePressEvent(event,scene);
    scene->mouseEvent(event);

    if( event->button() == Qt::LeftButton )
    {
        resetDriftWidget();

        if(dashArc)
        {
            scene->removeItem(dashArc);
            delete dashArc;
            dashArc = 0;
        }
        if(dashLine)
        {
            scene->removeItem(dashLine);
            delete dashLine;
            dashLine = 0;
        }

        QGraphicsItem *item = scene->itemAt(event->scenePos(),QTransform());
        if(item != 0 && item->type() == GraphicsLineItem::Type)
        {
            GraphicsLineItem *lineItem = dynamic_cast<GraphicsLineItem *>(item);
            if(mouse_pressed_count == 0)
            {
                mouse_pressed_count++;

                arc_item = new GraphicsArcItem(0);

                selected_linef.setP1(QPointF(lineItem->map_segment.start_x,lineItem->map_segment.start_y));
                selected_linef.setP2(QPointF(lineItem->map_segment.end_x,lineItem->map_segment.end_y));
            }
        }
        else if(item != 0 && item->type() == GraphicsPointItem::Type)
        {
            GraphicsPointItem *pointItem = dynamic_cast<GraphicsPointItem *>(item);
            if(mouse_pressed_count == 1)
            {
                mouse_pressed_count++;

                arc_item->setPos(item->pos());
                initialPositions = item->pos();
                arc_item->addPoint(initialPositions);
                arc_item->map_segment.start_id = pointItem->map_point.id;

                selected_point = QPoint(pointItem->map_point.x,pointItem->map_point.y);

                if(qAbs(selected_point.x() - selected_linef.x1()) < 1 && qAbs(selected_point.y() - selected_linef.y1()) < 1)
                {
                    selected_linef.setP1(selected_linef.p2());
                    selected_linef.setP2(selected_point);
                }
            }
        }

    }

}

void ArcTool_directRadiusAngle::mouseMoveEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{
    setCursor(scene, Qt::CrossCursor);

    DrawTool::mouseMoveEvent(event,scene);

    if(arc_item && (mouse_pressed_count > 1))
    {
        arc_item->cur_point = arc_item->mapFromScene(event->scenePos());
    }

    if(mouse_pressed_count == 0)
    {
        if(driftInputRadius)
        {
            driftInputRadius->ui->label->setText(tr("请选择一条直线"));
            driftInputRadius->ui->lineEdit->hide();
        }
    }
    else if(mouse_pressed_count == 1)
    {
        if(driftInputRadius)
        {
            driftInputRadius->ui->label->setText(tr("请选择直线端点"));
            driftInputRadius->ui->lineEdit->hide();
        }
    }
    else if(mouse_pressed_count == 2)
    {

        if(dashLine)
        {
            scene->removeItem(dashLine);
            delete dashLine;
            dashLine = 0;
        }
        path = QPainterPath();
        //unitLine = selected_linef.unitVector();
        qreal pointToline = arc_item->caculateCrossLengthPtToLine(selected_linef.p1(),selected_linef.p2(),event->scenePos());
        if(pointToline > 0)
        {
            normalLine = selected_linef.normalVector();
        }
        else
        {
            normalLine = selected_linef.normalVector().normalVector().normalVector();
        }
        normalLine.translate(selected_linef.dx(),selected_linef.dy());
        normalLine.setLength(qAbs(pointToline));

        path.moveTo(arc_item->mapFromScene((normalLine.p1())));
        path.lineTo(arc_item->mapFromScene(normalLine.p2()));

        //qDebug() << arc_item->mapToScene(path.pointAtPercent(0.0)) << arc_item->mapToScene(path.pointAtPercent(1.0));

        dashLine = new QGraphicsPathItem(qt_graphicsItem_shapeFromPath(path,arc_item->pen()));
        dashLine->setPen(Qt::DashLine);
        dashLine->setPos((selected_point));
        dashLine->setTransformOriginPoint(arc_item->transformOriginPoint());
        dashLine->setTransform(arc_item->transform());
        dashLine->setRotation(arc_item->rotation());
        dashLine->setScale(arc_item->scale());
        dashLine->setZValue(arc_item->zValue());
        scene->addItem(dashLine);

        if(driftInputRadius)
        {
            driftInputRadius->ui->label->setText(tr("请输入半径:"));
            driftInputRadius->ui->lineEdit->show();
            driftInputRadius->ui->lineEdit->setText(QString("%1").arg(qAbs(pointToline)));
        }

    }
    else if(mouse_pressed_count == 3)
    {
        if(dashArc)
        {
            scene->removeItem(dashArc);
            delete dashArc;
            dashArc = 0;
        }
        path = QPainterPath();
        //path.moveTo(arc_item->mapFromScene((normalLine.p1())));
        path.moveTo(normalLine.p1());
        QLineF line_start = normalLine.normalVector().normalVector();
        QLineF line_end = QLineF(cen, arc_item->mapToScene(arc_item->cur_point));

        //qDebug() << line_start.angle() << line_end.angle();

        sweepLength = line_end.angleTo(line_start);

        if(arc_item->caculateCrossLengthPtToLine(selected_linef.p1(),selected_linef.p2(),cen) > 0)
        {
            sweepLength = sweepLength - 360;
        }

        path.arcTo(cen.x()-inputRadius,cen.y()-inputRadius,2*inputRadius,2*inputRadius, line_start.angle(), -sweepLength);


        dashArc = new QGraphicsPathItem(qt_graphicsItem_shapeFromPath(arc_item->mapFromScene(path),arc_item->pen()));
        dashArc->setPen(Qt::DashLine);
        dashArc->setPos((selected_point));
        dashArc->setTransformOriginPoint(arc_item->transformOriginPoint());
        dashArc->setTransform(arc_item->transform());
        dashArc->setRotation(arc_item->rotation());
        dashArc->setScale(arc_item->scale());
        dashArc->setZValue(arc_item->zValue());
        scene->addItem(dashArc);

        if(driftInputRadius)
        {
            driftInputRadius->ui->label->setText(tr("请输入角度:"));
            driftInputRadius->ui->lineEdit->show();
            driftInputRadius->ui->lineEdit->setText(QString("%1").arg(qAbs(sweepLength)));
        }
    }


    movingDriftWidget(event, scene);
    scene->mouseEvent(event);
}

void ArcTool_directRadiusAngle::mouseReleaseEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{
    DrawTool::mouseReleaseEvent(event,scene);

    if(event->button() == Qt::LeftButton)
    {

    }
    else if(event->button() == Qt::RightButton)
    {
        resetVariable();
    }

    scene->mouseEvent(event);
}

void ArcTool_directRadiusAngle::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{

}

void ArcTool_directRadiusAngle::addPointByXY(int x, int y)
{

}

void ArcTool_directRadiusAngle::updateOneParamByDriftInput(int param)
{
    if(mouse_pressed_count == 2)
    {
        inputRadius = param;
        mouse_pressed_count++;

        normalLine.setLength(qAbs(inputRadius));

        cen = normalLine.p2().toPoint();

        if(dashLine)
        {
            curScene->removeItem(dashLine);
            delete dashLine;
            dashLine = 0;
        }
        path = QPainterPath();
        path.moveTo(arc_item->mapFromScene((normalLine.p1())));
        path.lineTo(arc_item->mapFromScene(normalLine.p2()));
        dashLine = new QGraphicsPathItem(qt_graphicsItem_shapeFromPath(path,arc_item->pen()));
        dashLine->setPen(Qt::DashLine);
        dashLine->setPos((selected_point));
        dashLine->setTransformOriginPoint(arc_item->transformOriginPoint());
        dashLine->setTransform(arc_item->transform());
        dashLine->setRotation(arc_item->rotation());
        dashLine->setScale(arc_item->scale());
        dashLine->setZValue(arc_item->zValue());
        curScene->addItem(dashLine);

        //qDebug() << "inputRadius = " << inputRadius << selected_linef << selected_point;
        if(driftInputRadius)
        {
            driftInputRadius->ui->label->setText(tr("请输入角度:"));
            driftInputRadius->ui->lineEdit->show();
        }
    }
    else if(mouse_pressed_count == 3)
    {
        inputAngle = param;
        inputAngle = inputAngle * sweepLength/qAbs(sweepLength);
        mouse_pressed_count = 0;

        path = QPainterPath();
        path.moveTo(normalLine.p1());
        QLineF line_start = normalLine.normalVector().normalVector();
        path.arcTo(cen.x()-inputRadius,cen.y()-inputRadius,2*inputRadius,2*inputRadius, line_start.angle(), -inputAngle);
        arc_item->addPoint(path.pointAtPercent(1.0));

        arc_item->arc_path = arc_item->mapFromScene(path);
        if(dashLine)
        {
            curScene->removeItem(dashLine);
            delete dashLine;
            dashLine = 0;
        }
        if(dashArc)
        {
            curScene->removeItem(dashArc);
            delete dashArc;
            dashArc = 0;
        }

        addOneArcToScene(curScene);

        pointTool.addPoint(path.pointAtPercent(1.0).x(), path.pointAtPercent(1.0).y(), curScene);

        qDebug() << "inputAngle = " << inputAngle;
    }
}

void ArcTool_directRadiusAngle::movingDriftWidget(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{
    curScene = scene;

    if(inited_drift_one_param == false)
    {
        if(driftInputRadius)
        {
            delete driftInputRadius;
            driftInputRadius = 0;
        }

        if(driftItemRadius)
        {
            delete driftItemRadius;
            driftItemRadius = 0;
        }

        inited_drift_one_param = true;
        driftInputRadius = new DriftInputRadius;
        driftItemRadius = new QGraphicsRectItem();
//        driftItemRadius->scale(1,-1);
        driftItemRadius->setTransform(QTransform::fromScale(1,-1),true);
        QGraphicsProxyWidget* pMyProxy2 = new QGraphicsProxyWidget(driftItemRadius);
        driftItemRadius->setPos(event->scenePos());
        driftItemRadius->setZValue(0.2);
        pMyProxy2->setWidget(driftInputRadius);
        connect(driftInputRadius,SIGNAL(radiusUpdate(int)), this, SLOT(updateOneParamByDriftInput(int)));
        scene->addItem(driftItemRadius);

    }

    if(driftInputRadius)
    {
        driftInputRadius->ui->lineEdit->setFocus();
        //driftInputRadius->ui->lineEdit->setText(QString("%1").arg(500));
        driftInputRadius->ui->lineEdit->selectAll();
    }
    if(driftItemRadius)
    {
        driftItemRadius->moveBy((event->scenePos() - driftItemRadius->scenePos()).x()+100,(event->scenePos() - driftItemRadius->scenePos()).y()-100);
    }


}

void ArcTool_directRadiusAngle::addOneArcToScene(DrawScene *scene)
{
    arc_item->map_segment.id = findSegmentId(scene);

    arc_item->map_segment.start_x = arc_item->mapToScene(arc_item->m_points.at(0)).x();
    arc_item->map_segment.start_y = arc_item->mapToScene(arc_item->m_points.at(0)).y();
    arc_item->map_segment.end_x = arc_item->mapToScene(arc_item->m_points.at(1)).x();
    arc_item->map_segment.end_y = arc_item->mapToScene(arc_item->m_points.at(1)).y();
    arc_item->map_segment.cen_x = arc_item->mapToScene(arc_item->cen).x();
    arc_item->map_segment.cen_y = arc_item->mapToScene(arc_item->cen).y();
    arc_item->map_segment.radius = arc_item->Radius;
    arc_item->map_segment.lenght = arc_item->arc_path.length();
    arc_item->map_segment.type = 1;
    arc_item->map_segment.dirmode = 0;

    if(arc_item->sweepLength > 0)
    {
        arc_item->map_segment.start_ang = arc_item->startAngle + 90.0;
        if(arc_item->map_segment.start_ang >= 360.0)
        {
            arc_item->map_segment.start_ang -= 360.0;
        }
    }
    else{
        arc_item->map_segment.start_ang = arc_item->startAngle - 90.0;
        if(arc_item->map_segment.start_ang < 0)
        {
            arc_item->map_segment.start_ang += 360.0;
        }
    }
    arc_item->map_segment.end_ang = arc_item->map_segment.start_ang + arc_item->sweepLength;
    if(arc_item->map_segment.end_ang >= 360.0)
    {
        arc_item->map_segment.end_ang -= 360.0;
    }
    else if(arc_item->map_segment.end_ang < 0)
    {
        arc_item->map_segment.end_ang += 360;
    }

    if(qAbs(arc_item->map_segment.start_ang - 360.0) < 0.001)
    {
        arc_item->map_segment.start_ang = 0;
    }
    if(qAbs(arc_item->map_segment.end_ang - 360.0) < 0.001)
    {
        arc_item->map_segment.end_ang = 0;
    }

    arc_item->map_segment.speed_limit = 0;
    scene->addItem(arc_item);
    scene->clearSelection();
    arc_item->setSelected(true);
}

void ArcTool_directRadiusAngle::resetVariable()
{
    mouse_pressed_count = 0;
    inited_drift = false;
    inited_drift_one_param = false;
    arc_item->m_points.clear();
    if(driftInputRadius)
    {
        delete driftInputRadius;
        driftInputRadius = 0;
    }

    if(driftItemRadius)
    {
        delete driftItemRadius;
        driftItemRadius = 0;
    }

    if(driftInput)
    {
        delete driftInput;
        driftInput = 0;
    }

    if(driftItem)
    {
        delete driftItem;
        driftItem = 0;
    }

    if(dashLine)
    {
        curScene->removeItem(dashLine);
        delete dashLine;
        dashLine = 0;
    }
    if(dashArc)
    {
        curScene->removeItem(dashArc);
        delete dashArc;
        dashArc = 0;
    }

}

InterrputTool::InterrputTool(DrawShape shape)
    :DrawTool(shape)
{
    selected_item = nullptr;
    m_nPoints = 0;
}

void InterrputTool::mousePressEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{
    DrawTool::mousePressEvent(event,scene);
    scene->mouseEvent(event);

    if( event->button() == Qt::LeftButton )
    {
        addPointByXY(cur_point.x(), cur_point.y());

        if(mouse_pressed_count == 0)
        {
            QGraphicsItem *item = scene->itemAt(event->scenePos(),QTransform());
            if(item != 0 && item->type() == GraphicsLineItem::Type)
            {
                GraphicsLineItem *lineItem = dynamic_cast<GraphicsLineItem *>(item);
                mouse_pressed_count++;
                selected_linef.setP1(QPointF(lineItem->map_segment.start_x,lineItem->map_segment.start_y));
                selected_linef.setP2(QPointF(lineItem->map_segment.end_x,lineItem->map_segment.end_y));
                path = lineItem->shape();
                inited_drift_one_param = false;

                if(dashPoint)
                {
                    scene->removeItem(dashPoint);
                    delete dashPoint;
                    dashPoint = 0;
                }
                GraphicsPointItem point_item(QRect(-100,-100,200,200));
                dashPoint = new QGraphicsPathItem(point_item.shape());
                //dashPoint->setPen(Qt::DashLine);
                dashPoint->setPos(event->scenePos());
                scene->addItem(dashPoint);
                lineItem->interrput_merge_tag = true;
                selected_item = lineItem;
            }
            else if(item != 0 && item->type() == GraphicsArcItem::Type)
            {
                GraphicsArcItem *arcItem = dynamic_cast<GraphicsArcItem *>(item);
                mouse_pressed_count++;
                path = arcItem->shape();
                inited_drift_one_param = false;

                if(dashPoint)
                {
                    scene->removeItem(dashPoint);
                    delete dashPoint;
                    dashPoint = 0;
                }
                GraphicsPointItem point_item(QRect(-100,-100,200,200));
                dashPoint = new QGraphicsPathItem(point_item.shape());
                //dashPoint->setPen(Qt::DashLine);
                //dashPoint->setPos(event->scenePos());
                scene->addItem(dashPoint);
                arcItem->interrput_merge_tag = true;
                selected_item = arcItem;
            }
        }
    }
    else if(event->button() == Qt::RightButton)
    {
        resetVariable();
    }
}

void InterrputTool::mouseMoveEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{
    DrawTool::mouseMoveEvent(event,scene);

    movingDriftWidget(event, scene);
    scene->mouseEvent(event);

    if(dashPoint)
    {
        cur_point = event->scenePos().toPoint();

        if(selected_item->type() == GraphicsLineItem::Type)
        {
            // (y2-y1)x + (x1-x2)y + (x2*y1 - x1*y2) = 0
            //      Ax     +      By     +             C           =0
            qreal A = selected_linef.y2() - selected_linef.y1();
            qreal B = selected_linef.x1() - selected_linef.x2();
            qreal C = selected_linef.x2() * selected_linef.y1() - selected_linef.x1() * selected_linef.y2();

            if(selected_linef.y1() > selected_linef.y2())
            {
                if(cur_point.y() > selected_linef.y1())
                {
                    cur_point.setY(selected_linef.y1());
                }
                else if(cur_point.y() < selected_linef.y2())
                {
                    cur_point.setY(selected_linef.y2());
                }
            }
            else
            {
                if(cur_point.y() > selected_linef.y2())
                {
                    cur_point.setY(selected_linef.y2());
                }
                else if(cur_point.y() < selected_linef.y1())
                {
                    cur_point.setY(selected_linef.y1());
                }
            }

            if(selected_linef.x1() > selected_linef.x2())
            {
                if(cur_point.x() > selected_linef.x1())
                {
                    cur_point.setX(selected_linef.x1());
                }
                else if(cur_point.x() < selected_linef.x2())
                {
                    cur_point.setX(selected_linef.x2());
                }
            }
            else
            {
                if(cur_point.x() > selected_linef.x2())
                {
                    cur_point.setX(selected_linef.x2());
                }
                else if(cur_point.x() < selected_linef.x1())
                {
                    cur_point.setX(selected_linef.x1());
                }
            }

            if(qAbs(A) < 0.0001)
            {
                cur_point.setY(-C/B);
            }
            else if(qAbs(B) < 0.0001)
            {
                cur_point.setX(-C/A);
            }
            else
            {
                if(qAbs(-A/B) > 1)
                {
                    cur_point.setX((B * cur_point.y() + C)/(-A));
                }
                else
                {
                    cur_point.setY((A * cur_point.x() + C)/(-B));
                }
            }
        }
        else if(selected_item->type() == GraphicsArcItem::Type)
        {
            cur_point = selected_item->mapToScene(path).pointAtPercent(0.5).toPoint();
        }


        dashPoint->setPos(cur_point);
    }

}

void InterrputTool::mouseReleaseEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{
    DrawTool::mouseReleaseEvent(event,scene);

    if(event->button() != Qt::LeftButton) return;

    scene->mouseEvent(event);
}

void InterrputTool::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{

}

void InterrputTool::movingDriftWidget(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{
    curScene = scene;

    if(inited_drift_one_param == false)
    {
        if(driftInput)
        {
            delete driftInput;
            driftInput = 0;
        }
        if(driftItem)
        {
            delete driftItem;
            driftItem = 0;
        }

        if(driftInputRadius)
        {
            delete driftInputRadius;
            driftInputRadius = 0;
        }
        if(driftItemRadius)
        {
            delete driftItemRadius;
            driftItemRadius = 0;
        }

        inited_drift_one_param = true;

        if(mouse_pressed_count == 0)
        {
            driftInputRadius = new DriftInputRadius;
            driftInputRadius->ui->label->setText(tr("请选择要打断的片段"));
            driftInputRadius->ui->lineEdit->hide();

            driftItemRadius = new QGraphicsRectItem();
//            driftItemRadius->scale(1,-1);
            driftItemRadius->setTransform(QTransform::fromScale(1,-1),true);

            QGraphicsProxyWidget* pMyProxy2 = new QGraphicsProxyWidget(driftItemRadius);
            driftItemRadius->setPos(event->scenePos());
            driftItemRadius->setZValue(0.2);
            pMyProxy2->setWidget(driftInputRadius);
            scene->addItem(driftItemRadius);
        }
        else if(mouse_pressed_count == 1)
        {
            driftInput = new DriftInput;
            driftInput->ui->label->setText("请输入断点坐标");
            driftItem = new QGraphicsRectItem();
//            driftItem->scale(1,-1);
            driftItem->setTransform(QTransform::fromScale(1,-1),true);

            QGraphicsProxyWidget* pMyProxy = new QGraphicsProxyWidget(driftItem);
            driftItem->setPos(event->scenePos());
            driftItem->setZValue(0.2);
            pMyProxy->setWidget(driftInput);
            driftInput->ui->lineEdit_x->setFocus();
            connect(driftInput,SIGNAL(coordinateUpdate(int, int)),this,SLOT(addPointByXY(int, int)));
            scene->addItem(driftItem);
        }
    }


    if(mouse_pressed_count == 0)
    {
        if(driftInputRadius)
        {
            driftInputRadius->ui->lineEdit->setFocus();
            //driftInputRadius->ui->lineEdit->setText(QString("%1").arg(500));
            driftInputRadius->ui->lineEdit->selectAll();
        }
        if(driftItemRadius)
        {
            driftItemRadius->moveBy((event->scenePos() - driftItemRadius->scenePos()).x()+100,(event->scenePos() - driftItemRadius->scenePos()).y()-100);
        }
    }
    else if(mouse_pressed_count == 1)
    {
        if(driftInput)
        {
            if(driftInput->ui->lineEdit_x->hasFocus())
            {
                driftInput->ui->lineEdit_x->setText(QString("%1").arg((int)cur_point.x()));
                driftInput->ui->lineEdit_y->setText(QString("%1").arg((int)cur_point.y()));
                driftInput->ui->lineEdit_x->selectAll();
            }
            else if(driftInput->ui->lineEdit_y->hasFocus())
            {
                driftInput->ui->lineEdit_y->selectAll();
            }

        }
        if(driftItem)
        {
            if(driftInput && driftInput->ui->lineEdit_x->hasFocus())
            {
                driftItem->moveBy((event->scenePos() - driftItem->scenePos()).x()+100,(event->scenePos() - driftItem->scenePos()).y()-100);
            }
        }
    }

}

void InterrputTool::resetVariable()
{
    inited_drift = false;
    inited_drift_one_param = false;
    mouse_pressed_count = 0;
    if(dashPoint)
    {
        curScene->removeItem(dashPoint);
        delete dashPoint;
        dashPoint = 0;
    }

    if(selected_item->type() == GraphicsLineItem::Type)
    {
        GraphicsLineItem * tempItem = dynamic_cast<GraphicsLineItem *>(selected_item);
        tempItem->interrput_merge_tag = false;
    }
    else if(selected_item->type() == GraphicsArcItem::Type)
    {
        GraphicsArcItem * tempItem = dynamic_cast<GraphicsArcItem *>(selected_item);
        tempItem->interrput_merge_tag = false;
    }

}

void InterrputTool::addPointByXY(int x, int y)
{
    if(mouse_pressed_count == 1)
    {
        mouse_pressed_count = 0;
        curScene->removeItem(selected_item);

        if(selected_item->type() == GraphicsLineItem::Type)
        {
            lineTool.addPointByXY(selected_linef.x1(),selected_linef.y1(),curScene);
            lineTool.addPointByXY(cur_point.x(),cur_point.y(),curScene);
            lineTool.addPointByXY(cur_point.x(),cur_point.y(),curScene);
            lineTool.addPointByXY(selected_linef.x2(),selected_linef.y2(),curScene);
        }
        else if(selected_item->type() == GraphicsArcItem::Type)
        {
            pointTool.addPoint(cur_point.x(),cur_point.y(),curScene);
            GraphicsArcItem *arcItem1 = new GraphicsArcItem(0);

            GraphicsArcItem *item = dynamic_cast<GraphicsArcItem *>(selected_item);
            arcItem1->setPos(item->map_segment.start_x,item->map_segment.start_y);
            arcItem1->addPoint(QPointF(item->map_segment.start_x,item->map_segment.start_y));
            arcItem1->addPoint(QPointF(cur_point.x(),cur_point.y()));
            arcItem1->map_segment.id = findSegmentId(curScene);            
            arcItem1->map_segment.radius = item->map_segment.radius;
            arcItem1->map_segment.dirmode = item->map_segment.dirmode;
            //arcItem1->map_segment.start_ang = item->map_segment.start_ang;
            arcItem1->map_segment.speed_limit = item->map_segment.speed_limit;
            arcItem1->map_segment.start_id = item->map_segment.start_id;

            arcItem1->get_arc_path(arcItem1->map_segment.radius, 1);
            arcItem1->cen = item->cen;
            arcItem1->map_segment.lenght = arcItem1->mapToScene(arcItem1->arc_path).length();
            arcItem1->map_segment.end_id = pointTool.point_id;
            //arcItem1->map_segment.end_ang = arcItem1->mapToScene(arcItem1->arc_path).angleAtPercent(1.0);
            curScene->addItem(arcItem1);

            GraphicsArcItem *arcItem2 = new GraphicsArcItem(0);
            arcItem2->setPos(item->map_segment.start_x,item->map_segment.start_y);
            arcItem2->addPoint(QPointF(cur_point.x(),cur_point.y()));
            arcItem2->addPoint(QPointF(item->map_segment.end_x,item->map_segment.end_y));
            arcItem2->map_segment.id = findSegmentId(curScene);
            arcItem2->map_segment.radius = item->map_segment.radius;
            arcItem2->map_segment.dirmode = item->map_segment.dirmode;
            //arcItem2->map_segment.start_ang = arcItem1->mapToScene(arcItem1->arc_path).angleAtPercent(0.0);
            arcItem2->map_segment.speed_limit = item->map_segment.speed_limit;
            arcItem2->map_segment.start_id = pointTool.point_id;
            arcItem2->get_arc_path(arcItem2->map_segment.radius, 1);
            arcItem2->cen = item->cen;
            arcItem2->map_segment.lenght = arcItem2->mapToScene(arcItem2->arc_path).length();
            arcItem2->map_segment.end_id = item->map_segment.end_id;
            //arcItem2->map_segment.end_ang = item->map_segment.end_ang;
            curScene->addItem(arcItem2);

            //qDebug() << arcItem1->arc_path.elementCount() << arcItem2->arc_path.elementCount() << item->arc_path.elementCount();


        }

        resetVariable();
    }
}

MergeTool::MergeTool(DrawShape shape)
    :DrawTool(shape)
{

}

void MergeTool::mousePressEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{
    DrawTool::mousePressEvent(event,scene);
    scene->mouseEvent(event);

    if(event->button() == Qt::LeftButton)
    {
        QGraphicsItem *item = scene->itemAt(event->scenePos(),QTransform());
        if(item != 0 && item->type() == GraphicsLineItem::Type)
        {
            GraphicsLineItem *lineItem = dynamic_cast<GraphicsLineItem *>(item);            

            if(selected_count == 0)
            {
                inited_drift_one_param = false;
                selected_item = lineItem;
                lineItem->interrput_merge_tag = true;
                selected_Lineitems.push_back(lineItem);
                selected_count++;

                selected_linef.setP1(QPointF(lineItem->map_segment.start_x,lineItem->map_segment.start_y));
                selected_linef.setP2(QPointF(lineItem->map_segment.end_x,lineItem->map_segment.end_y));
                path = lineItem->shape();
            }
            else
            {
                if(qAbs(lineItem->map_segment.start_ang - selected_Lineitems.last()->map_segment.end_ang) < 1
                        || qAbs(qAbs(lineItem->map_segment.start_ang - selected_Lineitems.last()->map_segment.end_ang)-180) <1)  // 判断直线的斜率是否一致
                {
                    for(int i = 0; i < selected_Lineitems.size(); i++)
                    {
                        if ((qAbs(lineItem->map_segment.start_x - selected_Lineitems[i]->map_segment.end_x) < 2
                                && qAbs(lineItem->map_segment.start_y - selected_Lineitems[i]->map_segment.end_y) < 2)
                            || (qAbs(lineItem->map_segment.start_x - selected_Lineitems[i]->map_segment.start_x) < 2
                                && qAbs(lineItem->map_segment.start_y - selected_Lineitems[i]->map_segment.start_y) < 2)
                            || (qAbs(lineItem->map_segment.end_x - selected_Lineitems[i]->map_segment.end_x) < 2
                                && qAbs(lineItem->map_segment.end_y - selected_Lineitems[i]->map_segment.end_y) < 2)
                            || (qAbs(lineItem->map_segment.end_x - selected_Lineitems[i]->map_segment.start_x) < 2
                                && qAbs(lineItem->map_segment.end_y - selected_Lineitems[i]->map_segment.start_y) < 2))  //判断选择得直线是否与已选择的直线相连
                        {
                            if(lineItem->interrput_merge_tag == false)   // 该直线还没有被选中
                            {
                                inited_drift_one_param = false;
                                selected_item = lineItem;
                                lineItem->interrput_merge_tag = true;
                                selected_Lineitems.push_back(lineItem);
                                selected_count++;

                                break;
                            }
                        }
                    }

                    if(lineItem->interrput_merge_tag == false)
                    {
                        QMessageBox::critical(scene->view(),"Error","   直线不相连，请重新选择    ");
                    }

                }
                else
                {
                    QMessageBox::critical(scene->view(),"Error","   直线斜率不一致，请重新选择    ");
                }
            }
        }

    }
    else if(event->button() == Qt::RightButton)
    {
        resetVariable();
    }
}

void MergeTool::mouseMoveEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{
    DrawTool::mouseMoveEvent(event,scene);
    scene->mouseEvent(event);


    movingDriftWidget(event, scene);
}

void MergeTool::mouseReleaseEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{
    DrawTool::mouseReleaseEvent(event,scene);

    scene->mouseEvent(event);
}

void MergeTool::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{
    int addedLine = 0;
    for(int i = 0; i < selected_Lineitems.size(); i++)
    {
        bool startConnect = false;
        bool endConnect = false;

        for(int j = 0; j < selected_Lineitems.size(); j++)
        {
            if(j != i)
            {
                if (qAbs(selected_Lineitems[i]->map_segment.start_x - selected_Lineitems[j]->map_segment.end_x) < 2
                        && qAbs(selected_Lineitems[i]->map_segment.start_y - selected_Lineitems[j]->map_segment.end_y) < 2)
                {
                    QGraphicsItem *item = scene->itemAt(QPointF(selected_Lineitems[i]->map_segment.start_x,selected_Lineitems[i]->map_segment.start_y),QTransform());
                    if(item->type() == GraphicsPointItem::Type)
                    {
                        GraphicsPointItem *pointItem = dynamic_cast<GraphicsPointItem *>(item);
                        scene->removeItem(pointItem);
                    }
                    startConnect = true;
                }
                else if(qAbs(selected_Lineitems[i]->map_segment.start_x - selected_Lineitems[j]->map_segment.start_x) < 2
                        && qAbs(selected_Lineitems[i]->map_segment.start_y - selected_Lineitems[j]->map_segment.start_y) < 2)
                {
                    QGraphicsItem *item = scene->itemAt(QPointF(selected_Lineitems[i]->map_segment.start_x,selected_Lineitems[i]->map_segment.start_y),QTransform());
                    if(item->type() == GraphicsPointItem::Type)
                    {
                        GraphicsPointItem *pointItem = dynamic_cast<GraphicsPointItem *>(item);
                        scene->removeItem(pointItem);
                    }
                    startConnect = true;
                }

                if(qAbs(selected_Lineitems[i]->map_segment.end_x - selected_Lineitems[j]->map_segment.end_x) < 2
                        && qAbs(selected_Lineitems[i]->map_segment.end_y - selected_Lineitems[j]->map_segment.end_y) < 2)
                {
                    QGraphicsItem *item = scene->itemAt(QPointF(selected_Lineitems[i]->map_segment.end_x,selected_Lineitems[i]->map_segment.end_y),QTransform());
                    if(item->type() == GraphicsPointItem::Type)
                    {
                        GraphicsPointItem *pointItem = dynamic_cast<GraphicsPointItem *>(item);
                        scene->removeItem(pointItem);
                    }
                    endConnect = true;
                }
                else if(qAbs(selected_Lineitems[i]->map_segment.end_x - selected_Lineitems[j]->map_segment.start_x) < 2
                        && qAbs(selected_Lineitems[i]->map_segment.end_y - selected_Lineitems[j]->map_segment.start_y) < 2)
                {
                    QGraphicsItem *item = scene->itemAt(QPointF(selected_Lineitems[i]->map_segment.end_x,selected_Lineitems[i]->map_segment.end_y),QTransform());
                    if(item->type() == GraphicsPointItem::Type)
                    {
                        GraphicsPointItem *pointItem = dynamic_cast<GraphicsPointItem *>(item);
                        scene->removeItem(pointItem);
                    }
                    endConnect = true;
                }
            }

        }

        scene->removeItem(selected_Lineitems[i]);

        if(startConnect == false && endConnect == true)
        {
            lineTool.addPointByXY(selected_Lineitems[i]->map_segment.start_x,selected_Lineitems[i]->map_segment.start_y,scene);
        }
        else if(startConnect == true && endConnect == false)
        {
            lineTool.addPointByXY(selected_Lineitems[i]->map_segment.end_x,selected_Lineitems[i]->map_segment.end_y,scene);
        }
        else if(startConnect == false && endConnect == false)
        {
            QMessageBox::critical(scene->view(),"Error","   直线不相连，合并失败    ");
        }

    }

    resetVariable();
}

void MergeTool::movingDriftWidget(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{
    if(inited_drift_one_param == false)
    {
        if(driftInputRadius)
        {
            delete driftInputRadius;
            driftInputRadius = 0;
        }
        if(driftItemRadius)
        {
            delete driftItemRadius;
            driftItemRadius = 0;
        }

        inited_drift_one_param = true;

        driftInputRadius = new DriftInputRadius;
        if(selected_count == 0)
        {
            driftInputRadius->ui->label->setText(tr("请选择要合并的直线片段"));
        }
        else
        {
            driftInputRadius->ui->label->setText(tr("已选择 %1 条直线片段").arg(selected_count));
        }
        driftInputRadius->ui->lineEdit->hide();

        driftItemRadius = new QGraphicsRectItem();
//        driftItemRadius->scale(1,-1);
        driftItemRadius->setTransform(QTransform::fromScale(1,-1),true);

        QGraphicsProxyWidget* pMyProxy2 = new QGraphicsProxyWidget(driftItemRadius);
        driftItemRadius->setPos(event->scenePos());
        driftItemRadius->setZValue(0.2);
        pMyProxy2->setWidget(driftInputRadius);
        scene->addItem(driftItemRadius);
    }

    if(driftInputRadius)
    {
        driftInputRadius->ui->lineEdit->setFocus();
        //driftInputRadius->ui->lineEdit->setText(QString("%1").arg(500));
        driftInputRadius->ui->lineEdit->selectAll();
    }
    if(driftItemRadius)
    {
        driftItemRadius->moveBy((event->scenePos() - driftItemRadius->scenePos()).x()+100,(event->scenePos() - driftItemRadius->scenePos()).y()-100);
    }
}

void MergeTool::resetVariable()
{
    inited_drift_one_param = false;
    selected_count = 0;
    if(driftInputRadius)
    {
        delete driftInputRadius;
        driftInputRadius = 0;
    }
    if(driftItemRadius)
    {
        delete driftItemRadius;
        driftItemRadius = 0;
    }

    for(int i = 0; i < selected_Lineitems.size(); i++)
    {
        selected_Lineitems[i]->interrput_merge_tag = false;
    }

    selected_Lineitems.clear();

}

MeasureTool::MeasureTool(DrawShape shape)
    :DrawTool(shape)
{
    measure_item = NULL;
    m_nPoints = 0;
}

void MeasureTool::mousePressEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{
    DrawTool::mousePressEvent(event,scene);
    scene->mouseEvent(event);
    qDebug() << "MeasureTool::mousePressEvent";

    if( event->button() == Qt::LeftButton )
    {
        QGraphicsItem *item = scene->itemAt(event->scenePos(),QTransform());

        if(mouse_pressed_count == 0)
        {
            mouse_pressed_count++;
            measure_item = new GraphicsMeasureItem(0);
            if(item != 0 && item->type() == GraphicsPointItem::Type)
            {
                measure_item->setPos(item->pos());
                initialPositions = item->pos();
                measure_item->addPoint(initialPositions);
                m_nPoints++;
            }
            else
            {
                measure_item->setPos(c_down);
                initialPositions = c_down;
                measure_item->addPoint(c_down);
                m_nPoints++;
            }

            measure_item->addPoint(c_last);
            m_nPoints++;

            scene->addItem(measure_item);
        }
        else if(mouse_pressed_count > 0)
        {
            mouse_pressed_count++;
            if(item != 0 && item->type() == GraphicsPointItem::Type)
            {
                //measure_item->m_points.begin()[mouse_pressed_count - 1] = item->mapFromScene(item->pos());
                measure_item->addPoint(item->pos());
            }
            else
            {
                measure_item->addPoint(c_down);
            }
            //scene->removeItem(measure_item);
            //scene->addItem(measure_item);
        }
        //qDebug() << measure_item->m_points;
    }
}

void MeasureTool::mouseMoveEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{
    setCursor(scene, Qt::CrossCursor);
    DrawTool::mouseMoveEvent(event,scene);


    if(mouse_pressed_count > 0)
    {
        measure_item->m_points.begin()[mouse_pressed_count] = measure_item->mapFromScene(c_last);
        scene->removeItem(measure_item);
        scene->addItem(measure_item);
    }

    scene->mouseEvent(event);
    scene->update();
}

void MeasureTool::mouseReleaseEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{
    DrawTool::mouseReleaseEvent(event,scene);
    scene->mouseEvent(event);
}

void MeasureTool::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{
    DrawTool::mouseDoubleClickEvent(event,scene);
    scene->mouseEvent(event);
}

void MeasureTool::keyPressEvent(QKeyEvent *event, DrawScene *scene)
{
    //qDebug() << "MeasureTool::keyPressEvent" << event->key() << Qt::Key_Escape;
    if(event->key() == Qt::Key_Escape)
    {
        resetVariable(scene);
        scene->update();
    }
}

void MeasureTool::keyReleaseEvent(QKeyEvent *event, DrawScene *scene)
{

}

void MeasureTool::resetVariable(DrawScene *scene)
{
    if(mouse_pressed_count == 1)
    {
        scene->removeItem(measure_item);
        measure_item->m_points.clear();
    }
    else if(mouse_pressed_count > 1)
    {
        measure_item->m_points.removeLast();
    }

    mouse_pressed_count = 0;
}

void MeasureTool::resetVariable()
{
    if(mouse_pressed_count == 1)
    {
        curScene->removeItem(measure_item);
        measure_item->m_points.clear();
    }
    else if(mouse_pressed_count > 1)
    {
        measure_item->m_points.removeLast();
    }

    mouse_pressed_count = 0;
}
