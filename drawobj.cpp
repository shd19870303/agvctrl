#include "drawobj.h"
#include <QPainter>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QCursor>
#include <QDebug>
#include "drawscene.h"
#include "drawview.h"

#include <QStyleOptionGraphicsItem>


#define ZOOMIN_SIZE1 500
#define ZOOMIN_SIZE2 350
#define ZOOMIN_SIZE3 200
#define PIX_SIZE 550
ShapeMimeData::ShapeMimeData(QList<QGraphicsItem *> items)
{
    foreach (QGraphicsItem *item , items ) {
       GraphicsItem *sp = dynamic_cast<GraphicsItem*>(item);
       m_items.append(sp->duplicate());
    }

}
ShapeMimeData::~ShapeMimeData()
{
    foreach (QGraphicsItem *item , m_items ) {
        delete item;
    }
    m_items.clear();
}

QPainterPath qt_graphicsItem_shapeFromPath(const QPainterPath &path, const QPen &pen)
{
    // We unfortunately need this hack as QPainterPathStroker will set a width of 1.0
    // if we pass a value of 0.0 to QPainterPathStroker::setWidth()
    const qreal penWidthZero = qreal(0.00000001);

    if (path == QPainterPath() || pen == Qt::NoPen)
        return path;
    QPainterPathStroker ps;
    ps.setCapStyle(pen.capStyle());
    if (pen.widthF() <= 0.0)
        ps.setWidth(penWidthZero);
    else
        ps.setWidth(pen.widthF());
    ps.setJoinStyle(pen.joinStyle());
    ps.setMiterLimit(pen.miterLimit());
    QPainterPath p = ps.createStroke(path);
    p.addPath(path);
    return p;
}

static void qt_graphicsItem_highlightSelected(
    QGraphicsItem *item, QPainter *painter, const QStyleOptionGraphicsItem *option)
{
    const QRectF murect = painter->transform().mapRect(QRectF(0, 0, 1, 1));
    if (qFuzzyIsNull((float)qMax(murect.width(), murect.height())))
        return;

    const QRectF mbrect = painter->transform().mapRect(item->boundingRect());
    if (qMin(mbrect.width(), mbrect.height()) < qreal(1.0))
        return;

    qreal itemPenWidth;
    switch (item->type()) {
        case QGraphicsEllipseItem::Type:
            itemPenWidth = static_cast<QGraphicsEllipseItem *>(item)->pen().widthF();
            break;
        case QGraphicsPathItem::Type:
            itemPenWidth = static_cast<QGraphicsPathItem *>(item)->pen().widthF();
            break;
        case QGraphicsPolygonItem::Type:
            itemPenWidth = static_cast<QGraphicsPolygonItem *>(item)->pen().widthF();
            break;
        case QGraphicsRectItem::Type:
            itemPenWidth = static_cast<QGraphicsRectItem *>(item)->pen().widthF();
            break;
        case QGraphicsSimpleTextItem::Type:
            itemPenWidth = static_cast<QGraphicsSimpleTextItem *>(item)->pen().widthF();
            break;
        case QGraphicsLineItem::Type:
            itemPenWidth = static_cast<QGraphicsLineItem *>(item)->pen().widthF();
            break;
        default:
            itemPenWidth = 1.0;
    }
    const qreal pad = itemPenWidth / 2;

    const qreal penWidth = 0; // cosmetic pen

    const QColor fgcolor = option->palette.windowText().color();
    const QColor bgcolor( // ensure good contrast against fgcolor
        fgcolor.red()   > 127 ? 0 : 255,
        fgcolor.green() > 127 ? 0 : 255,
        fgcolor.blue()  > 127 ? 0 : 255);


    painter->setPen(QPen(bgcolor, penWidth, Qt::SolidLine));
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(item->boundingRect().adjusted(-pad, -pad, pad, pad));

    painter->setPen(QPen(QColor(255,0,255), 1, Qt::SolidLine));
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(item->boundingRect().adjusted(-pad, -pad, pad, pad));

}

GraphicsItem::GraphicsItem(QGraphicsItem *parent):QGraphicsItem(parent)
{
    /*
    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect;
    effect->setBlurRadius(4);
    setGraphicsEffect(effect);
   */
    m_pen=QPen(Qt::NoPen);
    m_brush= QBrush(QColor(0, 0, 255));
    m_width = m_height = 0;

    //setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    this->setAcceptHoverEvents(true);
}

GraphicsItem::~GraphicsItem()
{

}


QPixmap GraphicsItem::image(){
    QPixmap pixmap(64, 64);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    setPen(QPen(Qt::black));
    setBrush(Qt::white);
    QStyleOptionGraphicsItem *styleOption = new QStyleOptionGraphicsItem;
//    painter.translate(m_localRect.center().x(),m_localRect.center().y());
    paint(&painter,styleOption);
    delete styleOption;
    return pixmap;
}

void GraphicsItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    Q_UNUSED(event)
}

void GraphicsItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    //this->setBrush(QColor(0, 255, 0));
    //update();
}

void GraphicsItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    //this->setBrush(QColor(0, 0, 255));
    //update();
}


QVariant GraphicsItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if ( change == QGraphicsItem::ItemSelectedHasChanged ) {
        QGraphicsItemGroup *g = dynamic_cast<QGraphicsItemGroup*>(parentItem());
        if (!g)
//            setState(value.toBool() ? SelectionHandleActive : SelectionHandleOff);
            ;
        else{
            setSelected(false);
            return QVariant::fromValue<bool>(false);
        }
    }
    /*
    else if (change == ItemPositionChange && scene()) {
        // value is the new position.
        QPointF newPos = value.toPointF();
        QRectF rect = scene()->sceneRect();
        if (!rect.contains(newPos)) {
            // Keep the item inside the scene rect.
            newPos.setX(qMin(rect.right()-boundingRect().width()/2, qMax(newPos.x(), rect.left()+boundingRect().width()/2)));
            newPos.setY(qMin(rect.bottom()-boundingRect().height()/2, qMax(newPos.y(), rect.top()+boundingRect().height()/2)));
            return newPos;
        }
    }
    */
    return QGraphicsItem::itemChange(change, value);
}

GraphicsPointItem::GraphicsPointItem(const QRect & rect, bool isRound, QGraphicsItem * parent)
    :GraphicsItem(parent)
    ,m_isRound(isRound)
    ,m_fRatioX(1/10.0)
    ,m_fRatioY(1/3.0)
{
    m_width = rect.width();
    m_height = rect.height();
    m_initialRect = rect;
    m_localRect = m_initialRect;
    m_localRect = rect;
    m_originPoint = QPointF(0,0);
//    scale(1,-1);
    setTransform(QTransform::fromScale(1,-1),true);

    setZValue(0.1);
    pix_store.load(":/images/home3_32x32.png");
    pix_back_stop_charge.load(":/images/home2_32x32.png");
    pix_charge.load(":/images/charge.png");
    pix_door.load(":/images/door.png");
    //updatehandles();
//    image_PickUp_Ru.load(QString::fromUtf8(":/icons/images/PickUp_Ru.png"));
//    image_PickUp_Chu.load(QString::fromUtf8(":/icons/images/PickUp_Chu.png"));
//    image_PickDown_Ru.load(QString::fromUtf8(":/icons/images/PickDown_Ru.png"));
//    image_PickDown_Chu.load(QString::fromUtf8(":/icons/images/PickDown_Chu.png"));
//    image_Standby_Chu.load(QString::fromUtf8(":/icons/images/Standby_Chu.png"));
//    image_Standby_Ru.load(QString::fromUtf8(":/icons/images/Standby_Ru.png"));

//    picture_PickUp_Ru.load(QString::fromUtf8(":/icons/images/PickUp_Ru.png"));
//    picture_PickUp_Chu.load(QString::fromUtf8(":/icons/images/PickUp_Chu.png"));
//    picture_PickDown_Ru.load(QString::fromUtf8(":/icons/images/PickDown_Ru.png"));
//    picture_PickDown_Chu.load(QString::fromUtf8(":/icons/images/PickDown_Chu.png"));
//    picture_Standby_Chu.load(QString::fromUtf8(":/icons/images/Standby_Chu.png"));
//    picture_Standby_Ru.load(QString::fromUtf8(":/icons/images/Standby_Ru.png"));


    pixmap_PickUp_Ru.load(QString::fromUtf8(":/icons/images/PickUp_Ru.png"));
    pixmap_PickUp_Chu.load(QString::fromUtf8(":/icons/images/PickUp_Chu.png"));
    pixmap_PickDown_Ru.load(QString::fromUtf8(":/icons/images/PickDown_Ru.png"));
    pixmap_PickDown_Chu.load(QString::fromUtf8(":/icons/images/PickDown_Chu.png"));
    pixmap_Standby_Chu.load(QString::fromUtf8(":/icons/images/Standby_Chu.png"));
    pixmap_Standby_Ru.load(QString::fromUtf8(":/icons/images/Standby_Ru.png"));

    storeFont.setPointSize(650);
    storeFont.setBold(false);

    taskFont.setPointSize(500);
    taskFont.setBold(false);

}

int GraphicsPointItem::type() const
{
    return Type;
}

QRectF GraphicsPointItem::boundingRect() const
{
    return m_localRect;
}

QPainterPath GraphicsPointItem::shape() const
{
    QPainterPath path;

    path.addRect(rect());

    //return qt_graphicsItem_shapeFromPath(path,pen());
    return path;
}
void GraphicsPointItem::control(int dir, const QPointF & delta)
{
    QPointF local = mapFromParent(delta);
    switch (dir) {
    case 9:
    {
        QRectF delta1 = rect();
        int y = local.y();
        if(y> delta1.center().y() )
            y = delta1.center().y();
        if(y<delta1.top())
            y=delta1.top();
        int H= delta1.height();
        if(H==0)
            H=1;
        m_fRatioY = std::abs(((float)(delta1.top()-y)))/H;
    }
        break;
    case 10:
    {
        QRectF delta1 = rect();
        int x = local.x();
        if(x < delta1.center().x() )
            x = delta1.center().x();
        if(x>delta1.right())
            x=delta1.right();
        int W= delta1.width();
        if(W==0)
            W=1;
        m_fRatioX = std::abs(((float)(delta1.right()-x)))/W;
        break;
    }
    case 11:
    {
//        setTransform(transform().translate(-local.x(),-local.y()));
//        setTransformOriginPoint(local.x(),local.y());
//        setTransform(transform().translate(local.x(),local.y()));
        m_originPoint = local;
    }
        break;
   default:
        break;
    }
    prepareGeometryChange();
    //updatehandles();
}

void GraphicsPointItem::stretch(int handle , double sx, double sy, const QPointF & origin)
{
    QTransform trans  ;
//    switch (handle) {
//    case Right:
//    case Left:
//        sy = 1;
//        break;
//    case Top:
//    case Bottom:
//        sx = 1;
//        break;
//    default:
//        break;
//    }
    opposite_ = origin;

    trans.translate(origin.x(),origin.y());
    trans.scale(sx,sy);
    trans.translate(-origin.x(),-origin.y());

    prepareGeometryChange();
    m_localRect = trans.mapRect(m_initialRect);
    m_width = m_localRect.width();
    m_height = m_localRect.height();
    //updatehandles();
}

void GraphicsPointItem::updateCoordinate()
{

    QPointF pt1,pt2,delta;

    pt1 = mapToScene(transformOriginPoint());
    pt2 = mapToScene(m_localRect.center());
    delta = pt1 - pt2;

    if (!parentItem() ){
        prepareGeometryChange();
        m_localRect = QRectF(-m_width/2,-m_height/2,m_width,m_height);
        m_width = m_localRect.width();
        m_height = m_localRect.height();
        setTransform(transform().translate(delta.x(),delta.y()));
        setTransformOriginPoint(m_localRect.center());
        moveBy(-delta.x(),-delta.y());
        setTransform(transform().translate(-delta.x(),-delta.y()));
        opposite_ = QPointF(0,0);
        //updatehandles();
    }
    m_initialRect = m_localRect;
}

void GraphicsPointItem::move(const QPointF &point)
{
    moveBy(point.x(),point.y());
}

QGraphicsItem *GraphicsPointItem::duplicate() const
{
    GraphicsPointItem * item = new GraphicsPointItem( rect().toRect(),m_isRound);
    item->m_width = width();
    item->m_height = height();
    item->setPos(pos().x(),pos().y());
    item->setPen(pen());
    item->setBrush(brush());
    item->setTransform(transform());
    item->setTransformOriginPoint(transformOriginPoint());
    item->setRotation(rotation());
    item->setScale(scale());
    item->setZValue(zValue()+0.1);
    item->m_fRatioY = m_fRatioY;
    item->m_fRatioX = m_fRatioX;
    item->updateCoordinate();
    return item;
}

bool GraphicsPointItem::loadFromPb(agv_map::Point point_pb)
{
    map_point_pb = point_pb;

    setPos((qreal)map_point_pb.mutable_basic()->x(),(qreal)map_point_pb.mutable_basic()->y());
    return true;
}

bool GraphicsPointItem::loadFromSql(QSqlQuery *sql_query)
{
//    m_isRound = (xml->name() == tr("roundrect"));
//    if ( m_isRound ){
//        m_fRatioX = xml->attributes().value(tr("rx")).toDouble();
//        m_fRatioY = xml->attributes().value(tr("ry")).toDouble();
//    }
//    readBaseAttributes(sql_query);
//    updateCoordinate();

    map_point.id = sql_query->value(0).toInt();
    memcpy(map_point.name, sql_query->value(1).toString().toLatin1().data(), 5);
    map_point.x = sql_query->value(2).toFloat();
    map_point.y = sql_query->value(3).toFloat();
    map_point.QRcode = sql_query->value(4).toInt();
    map_point.type = sql_query->value(5).toInt();
    map_point.stype = sql_query->value(6).toInt();
    map_point.atype = sql_query->value(7).toInt();
    map_point.dist = sql_query->value(8).toInt();
    map_point.chargeDir = sql_query->value(9).toInt();
    map_point.Infrared = sql_query->value(10).toFloat();
    map_point.angle = sql_query->value(11).toInt();

    for(int i = 0; i < sizeof(map_point.parm)/sizeof(map_point.parm[0]); i++)
    {
        map_point.parm[i] = sql_query->value(12 + i).toDouble();
    }

    setPos((qreal)map_point.x,(qreal)map_point.y);
    return true;
}

bool GraphicsPointItem::saveToSql(QSqlQuery *sql_query)
{
//    if ( m_isRound ){
//        xml->writeStartElement(tr("roundrect"));
//        xml->writeAttribute(tr("rx"),QString("%1").arg(m_fRatioX));
//        xml->writeAttribute(tr("ry"),QString("%1").arg(m_fRatioY));
//    }
//    else
//        xml->writeStartElement(tr("rect"));

//    writeBaseAttributes(sql_query);

    QString insert_point = QString("insert into POINT values(%1,'%2',%3,%4,%5,%6,%7,%8,%9,%10,%11,%12,%13,%14,%15"
                                   ",%16,%17,%18,%19,%20,%21,%22,%23,%24,%25,%26,%27,%28,%29,%30,%31,%32,%33,%34"
                                   ",%35,%36,%37,%38,%39,%40,%41,%42);")
            .arg(map_point.id)
            .arg(QString(map_point.name).mid(0,5))
            .arg((qreal)map_point.x)
            .arg((qreal)map_point.y)
            .arg(map_point.QRcode)
            .arg((int)map_point.type)
            .arg(map_point.stype)
            .arg(map_point.atype)
            .arg(map_point.dist)
            .arg(map_point.chargeDir)
            .arg((qreal)map_point.Infrared)
            .arg(map_point.angle)
            .arg(map_point.parm[0]).arg(map_point.parm[1])
            .arg(map_point.parm[2]).arg(map_point.parm[3])
            .arg(map_point.parm[4]).arg(map_point.parm[5])
            .arg(map_point.parm[6]).arg(map_point.parm[7])
            .arg(map_point.parm[8]).arg(map_point.parm[9])
            .arg(map_point.parm[10]).arg(map_point.parm[11])
            .arg(map_point.parm[12]).arg(map_point.parm[13])
            .arg(map_point.parm[14]).arg(map_point.parm[15])
            .arg(map_point.parm[16]).arg(map_point.parm[17])
            .arg(map_point.parm[18]).arg(map_point.parm[19])
            .arg(map_point.parm[20]).arg(map_point.parm[21])
            .arg(map_point.parm[22]).arg(map_point.parm[23])
            .arg(map_point.parm[24]).arg(map_point.parm[25])
            .arg(map_point.parm[26]).arg(map_point.parm[27])
            .arg(map_point.parm[28]).arg(map_point.parm[29])
            ;

    qDebug() << insert_point;

    if(!sql_query->exec(insert_point))
    {
        qDebug()<<sql_query->lastError();
    }

    return true;
}

void GraphicsPointItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
//    qDebug()<<"point id: "<<map_point_pb.basic().id();
    emit sendPointID(map_point_pb.basic().id());
}


static
QRectF RecalcBounds(const QPolygonF&  pts)
{
    QRectF bounds(pts[0], QSize(0, 0));
    for (int i = 1; i < pts.count(); ++i)
    {
        if (pts[i].x() < bounds.left())
            bounds.setLeft(pts[i].x());
        if (pts[i].x() > bounds.right())
            bounds.setRight(pts[i].x());
        if (pts[i].y() < bounds.top())
            bounds.setTop(pts[i].y());
        if (pts[i].y() > bounds.bottom())
            bounds.setBottom (pts[i].y());
    }
    bounds = bounds.normalized();
    return bounds;
}

#if 1
void GraphicsPointItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
//    if(map_point.id == 3032)
//    {
//        qDebug()<<"3032 : scan_point_flag ="<<scan_point_flag;
//        qDebug()<<"3032 : zoomin_flag ="<<zoomin_flag;
//        qDebug()<<"3032 : occupy_point_flag ="<<occupy_point_flag;
//    }
//    if(map_point.id == 3033)
//    {
//        qDebug()<<"3033 : scan_point_flag ="<<scan_point_flag;
//        qDebug()<<"3033 : zoomin_flag ="<<zoomin_flag;
//        qDebug()<<"3033 : occupy_point_flag ="<<occupy_point_flag;
//    }
    DrawView * drawView = dynamic_cast<DrawView *>((dynamic_cast<DrawScene *>(scene())->view()));
    int map_point_width = drawView->mapConfig[drawView->cur_map_index].map_point_width;
//    m_localRect = QRect(-map_point_width/2,-map_point_width/2,map_point_width,map_point_width);
    map_point_pb.mutable_basic()->set_x(pos().x());
    map_point_pb.mutable_basic()->set_y(pos().y());

    if(scan_point_flag == 1)
    {
        if(zoomin_flag ==1)
        {
            painter->setBrush(QColor(0, 8, 215,255));
//            painter->drawEllipse(QRectF(-(rect().width()+ZOOMIN_SIZE1)/2,-(rect().height()+ZOOMIN_SIZE1)/2,rect().width()+ZOOMIN_SIZE1,rect().height()+ZOOMIN_SIZE1).toRect());
            painter->drawEllipse(QRectF(-(rect().width()*2)/2,-(rect().height()*2)/2,rect().width()*2,rect().height()*2).toRect());
            painter->setBrush(Qt::white);
            painter->drawEllipse(QRectF(-(rect().width()*0.5)/2,-(rect().height()*0.5)/2,rect().width()*0.5,rect().height()*0.5).toRect());
        }
        else
        {
            painter->setBrush(QColor(0, 8, 215,255));
//            painter->drawEllipse(QRectF(-(rect().width()*1.6)/2,-(rect().height()*1.6)/2,rect().width()*1.6,rect().height()*1.6).toRect());
            painter->drawEllipse(QRectF(-(rect().width()*1.6)/2,-(rect().height()*1.6)/2,rect().width()*1.6,rect().height()*1.6).toRect());
            painter->setBrush(Qt::white);
//            painter->drawEllipse(rect().toRect());
            painter->drawEllipse(QRectF(-(rect().width()*0.5)/2,-(rect().height()*0.5)/2,rect().width()*0.5,rect().height()*0.5).toRect());

        }
    }
    switch (occupy_point_flag) {
    case 0:
        if(zoomin_flag ==1)
        {
            painter->setBrush(QColor(130, 130, 130,255));
            painter->drawEllipse(QRectF(-(rect().width()*1.6)/2,-(rect().height()*1.6)/2,rect().width()*1.6,rect().height()*1.6).toRect());
            painter->setBrush(Qt::white);
            painter->drawEllipse(QRectF(-(rect().width()*0.5)/2,-(rect().height()*0.5)/2,rect().width()*0.5,rect().height()*0.5).toRect());
        }
        else
        {
            painter->setBrush(QColor(130, 130, 130,255));
            painter->drawEllipse(QRectF(-(rect().width()*1.4)/2,-(rect().height()*1.4)/2,rect().width()*1.4,rect().height()*1.4).toRect());
            painter->setBrush(Qt::white);
            painter->drawEllipse(QRectF(-(rect().width()*0.5)/2,-(rect().height()*0.5)/2,rect().width()*0.5,rect().height()*0.5).toRect());
        }

        break;
    case 1:
        if(zoomin_flag ==1)
        {
            painter->setBrush(QColor(255, 0, 0,255));
            painter->drawEllipse(QRectF(-(rect().width()*1.6)/2,-(rect().height()*1.6)/2,rect().width()*1.6,rect().height()*1.6).toRect());
            painter->setBrush(Qt::white);
            painter->drawEllipse(QRectF(-(rect().width()*0.5)/2,-(rect().height()*0.5)/2,rect().width()*0.5,rect().height()*0.5).toRect());
        }
        else
        {
            painter->setBrush(QColor(255, 0, 0,255));
            painter->drawEllipse(QRectF(-(rect().width()*1.4)/2,-(rect().height()*1.4)/2,rect().width()*1.4,rect().height()*1.4).toRect());
            painter->setBrush(Qt::white);
            painter->drawEllipse(QRectF(-(rect().width()*0.5)/2,-(rect().height()*0.5)/2,rect().width()*0.5,rect().height()*0.5).toRect());
        }

        break;
    case 2:
        if(zoomin_flag ==1)
        {
            painter->setBrush(QColor(215, 120, 24,255));
            painter->drawEllipse(QRectF(-(rect().width()*1.6)/2,-(rect().height()*1.6)/2,rect().width()*1.6,rect().height()*1.6).toRect());
            painter->setBrush(Qt::white);
            painter->drawEllipse(QRectF(-(rect().width()*0.5)/2,-(rect().height()*0.5)/2,rect().width()*0.5,rect().height()*0.5).toRect());
        }
        else
        {
            painter->setBrush(QColor(215, 120, 24,255));
            painter->drawEllipse(QRectF(-(rect().width()*1.4)/2,-(rect().height()*1.4)/2,rect().width()*1.4,rect().height()*1.4).toRect());
            painter->setBrush(Qt::white);
            painter->drawEllipse(QRectF(-(rect().width()*0.5)/2,-(rect().height()*0.5)/2,rect().width()*0.5,rect().height()*0.5).toRect());
        }

        break;
    case 3:
        if(zoomin_flag ==1)
        {
            painter->setBrush(QColor(215, 215, 130,255));
            painter->drawEllipse(QRectF(-(rect().width()*1.6)/2,-(rect().height()*1.6)/2,rect().width()*1.6,rect().height()*1.6).toRect());
            painter->setBrush(Qt::white);
            painter->drawEllipse(QRectF(-(rect().width()*0.5)/2,-(rect().height()*0.5)/2,rect().width()*0.5,rect().height()*0.5).toRect());
        }
        else
        {
            painter->setBrush(QColor(215, 215, 130,255));
            painter->drawEllipse(QRectF(-(rect().width()*1.4)/2,-(rect().height()*1.4)/2,rect().width()*1.4,rect().height()*1.4).toRect());
            painter->setBrush(Qt::white);
            painter->drawEllipse(QRectF(-(rect().width()*0.5)/2,-(rect().height()*0.5)/2,rect().width()*0.5,rect().height()*0.5).toRect());
        }

        break;
    case 4:
        if(zoomin_flag ==1)
        {
            painter->setBrush(QColor(73, 215, 130,255));
            painter->drawEllipse(QRectF(-(rect().width()*1.6)/2,-(rect().height()*1.6)/2,rect().width()*1.6,rect().height()*1.6).toRect());
            painter->setBrush(Qt::white);
            painter->drawEllipse(QRectF(-(rect().width()*0.5)/2,-(rect().height()*0.5)/2,rect().width()*0.5,rect().height()*0.5).toRect());
        }
        else
        {
            painter->setBrush(QColor(73, 215, 130,255));
            painter->drawEllipse(QRectF(-(rect().width()*1.4)/2,-(rect().height()*1.4)/2,rect().width()*1.4,rect().height()*1.4).toRect());
            painter->setBrush(Qt::white);
            painter->drawEllipse(QRectF(-(rect().width()*0.5)/2,-(rect().height()*0.5)/2,rect().width()*0.5,rect().height()*0.5).toRect());
        }

        break;
    case 5:
        if(zoomin_flag ==1)
        {
            painter->setBrush(QColor(255, 46, 169,255));
            painter->drawEllipse(QRectF(-(rect().width()*1.6)/2,-(rect().height()*1.6)/2,rect().width()*1.6,rect().height()*1.6).toRect());
            painter->setBrush(Qt::white);
            painter->drawEllipse(QRectF(-(rect().width()*0.5)/2,-(rect().height()*0.5)/2,rect().width()*0.5,rect().height()*0.5).toRect());
        }
        else
        {
//            painter->setBrush(QColor(0, 61, 215,255));
            painter->setBrush(QColor(255, 46, 169,255));
            painter->drawEllipse(QRectF(-(rect().width()*1.4)/2,-(rect().height()*1.4)/2,rect().width()*1.4,rect().height()*1.4).toRect());
            painter->setBrush(Qt::white);
            painter->drawEllipse(QRectF(-(rect().width()*0.5)/2,-(rect().height()*0.5)/2,rect().width()*0.5,rect().height()*0.5).toRect());
        }

        break;
    case 6:
        if(zoomin_flag ==1)
        {
            painter->setBrush(QColor(0, 215, 196,255));
            painter->drawEllipse(QRectF(-(rect().width()*1.6)/2,-(rect().height()*1.6)/2,rect().width()*1.6,rect().height()*1.6).toRect());
            painter->setBrush(Qt::white);
            painter->drawEllipse(QRectF(-(rect().width()*0.5)/2,-(rect().height()*0.5)/2,rect().width()*0.5,rect().height()*0.5).toRect());
        }
        else
        {
            painter->setBrush(QColor(0, 215, 196,255));
            painter->drawEllipse(QRectF(-(rect().width()*1.4)/2,-(rect().height()*1.4)/2,rect().width()*1.4,rect().height()*1.4).toRect());
            painter->setBrush(Qt::white);
            painter->drawEllipse(QRectF(-(rect().width()*0.5)/2,-(rect().height()*0.5)/2,rect().width()*0.5,rect().height()*0.5).toRect());
        }

        break;
    case 7:
        if(zoomin_flag ==1)
        {
            painter->setBrush(QColor(177, 3, 215,255));
            painter->drawEllipse(QRectF(-(rect().width()*1.6)/2,-(rect().height()*1.6)/2,rect().width()*1.6,rect().height()*1.6).toRect());
            painter->setBrush(Qt::white);
            painter->drawEllipse(QRectF(-(rect().width()*0.5)/2,-(rect().height()*0.5)/2,rect().width()*0.5,rect().height()*0.5).toRect());
        }
        else
        {
            painter->setBrush(QColor(177, 3, 215,255));
            painter->drawEllipse(QRectF(-(rect().width()*1.4)/2,-(rect().height()*1.4)/2,rect().width()*1.4,rect().height()*1.4).toRect());
            painter->setBrush(Qt::white);
            painter->drawEllipse(QRectF(-(rect().width()*0.5)/2,-(rect().height()*0.5)/2,rect().width()*0.5,rect().height()*0.5).toRect());
        }

        break;
    default:painter->setBrush(Qt::NoBrush);

        break;
    }
   map_point.x = pos().x();
   map_point.y = pos().y();
//   painter->setPen(pen());
//   painter->setBrush(brush());
   QPen temp_pen;
   temp_pen.setWidth(6);
   temp_pen.setColor(QColor(40, 40, 40));
   painter->setPen(temp_pen);
   painter->setBrush(Qt::NoBrush);
   double rx,ry;
   if(m_fRatioX<=0)
      rx=0;
   else {
       rx = m_width * m_fRatioX + 0.5;
   }
   if ( m_fRatioY <=0 )
       ry = 0;
   else
       ry = m_height * m_fRatioY + 0.5;
   if ( m_isRound )
   {
        painter->drawRoundedRect(rect(),rx,ry);
   }
   else
   {
       painter->save();

       if (option->state & QStyle::State_Selected)  //old
       {
           painter->setBrush(QColor(0,255,0));
       }
       else if(isEndpoint == true)
       {
           painter->setBrush(QColor(155,48,255));
       }
       else if(map_point_pb.mutable_basic()->disable() == 1) //禁用站点
       {
            painter->setBrush(QColor(135,206,255));
       }
       else
       {
           if(map_point_pb.mutable_basic()->type() == 8)
           {
                painter->setBrush(QColor(255,64,64));
           }
           else
           {
//                painter->setBrush(brush());
                painter->setBrush(Qt::NoBrush);
           }
       }

//       if (option->state & QStyle::State_Selected)
//       {
//           painter->setBrush(QColor(0,255,0));
//       }
//       else if(isEndpoint == true)
//       {
//           painter->setBrush(QColor(155,48,255));
//       }
//       else{
//           if(map_point_pb.mutable_basic()->disable() == 1) //禁用站点
//           {
//                painter->setBrush(QColor(135,206,255));
//           }
//           else
//           {
//               if(map_point_pb.mutable_basic()->type() == 8)
//               {
//                    painter->setBrush(QColor(255,64,64));
//               }
//               else
//               {
//                   if(map_point_pb.avo_param_size() > 0)
//                   {
//                       painter->setBrush(QColor(139,0,19));
//                   }
//                   else if(map_point_pb.ctl_param_size() > 0)
//                   {
//                       painter->setBrush(QColor(238,121,66));
//                   }
//                   else if(map_point_pb.agv_param().contains(2))    //key = 2; 手动不能放置
//                   {
//                       painter->setBrush(QColor(139,125,107));
//                   }
//                   else
//                   {
//                       painter->setBrush(brush());
//                   }
//               }
//           }
//       }
//       painter->drawRect(rect().toRect());

       QFont font;



//       painter->drawRect(rect().toRect());
//       if(map_point.type == 5)  old
       if(map_point_pb.mutable_basic()->type() == 5)
       {
           painter->setBrush(QColor(245,149,0));
           painter->drawEllipse(QRectF(-(rect().width()*1.2)/2,-(rect().height()*1.2)/2,rect().width()*1.2,rect().height()*1.2).toRect());
       }
       else if(map_point_pb.mutable_basic()->type() == 3)
       {   
           painter->drawPixmap(QRectF(-(rect().width()+PIX_SIZE)/2,-(rect().height()+PIX_SIZE)/2,rect().width()+PIX_SIZE,rect().height()+PIX_SIZE).toRect(),pix_store);
       }
       else if(map_point_pb.mutable_basic()->type() == 7)
       {
           painter->drawPixmap(QRectF(-(rect().width()+PIX_SIZE)/2,-(rect().height()+PIX_SIZE)/2,rect().width()+PIX_SIZE,rect().height()+PIX_SIZE).toRect(),pix_back_stop_charge);
       }
       else if(map_point_pb.mutable_basic()->type() == 2)
       {
           painter->drawPixmap(QRectF(-(rect().width()+PIX_SIZE)/2,-(rect().height()+PIX_SIZE)/2,rect().width()+PIX_SIZE,rect().height()+PIX_SIZE).toRect(),pix_charge);
       }
       else if(map_point_pb.mutable_basic()->type() == 9)
       {
           painter->drawPixmap(QRectF(-(rect().width()+PIX_SIZE)/2,-(rect().height()+PIX_SIZE)/2,rect().width()+PIX_SIZE,rect().height()+PIX_SIZE).toRect(),pix_door);
       }
       else
       {
//           qDebug()<<"ellipse";
           painter->drawEllipse(rect().toRect());
       }


//       QFont font;
//       painter->setPen(QPen(Qt::SolidLine));
//       painter->scale(1,-1);

       if(map_point_pb.mutable_basic()->type() == 0)
       {
           painter->setFont(point_font);
           if(point_font_flag == 1)
           {
//               painter->drawText(QPoint(rect().center().x(),rect().center().y() - 120),QString("%1").arg(map_point.id));
               painter->drawText(QPoint(rect().center().x(),rect().center().y() - 120),QString("%1").arg(map_point_pb.basic().id()));

           }
       }
       else if((int)map_point.type == 2)
       {
           painter->setFont(point_font);
           if(point_font_flag == 1)
           {
               painter->drawText(QPoint(rect().center().x(),rect().center().y() - 120),QString("C:%1").arg(map_point_pb.basic().id()));
           }
       }
//       else if((int)map_point.type == 3)
//       {
//           painter->setFont(point_font);
//           if(point_font_flag == 1)
//           {
//               painter->drawText(QPoint(rect().center().x(),rect().center().y() - 120),QString("T:%1").arg(map_point.id));
//           }
//       }

       else if(map_point_pb.mutable_basic()->type() == 3 || map_point_pb.mutable_basic()->type() == 6 || map_point_pb.mutable_basic()->type() == 7)
       {
           painter->setFont(point_font);
           if(point_font_flag == 1)
           {
               painter->drawText(QPoint(rect().center().x(),rect().center().y() - 120),QString("T:%1").arg(map_point_pb.basic().id()));
           }
       }
//       else if((int)map_point.type == 5)
//       {
//           painter->setFont(point_font);
//           if(point_font_flag == 1)
//           {
//               painter->drawText(QPoint(rect().center().x(),rect().center().y() - 120),QString("S:%1").arg(map_point.id));
//               painter->drawText(QPoint(rect().center().x(),rect().center().y() - 240),QString("%1").arg(map_point.name));

//           }
//       }
       else if(map_point_pb.mutable_basic()->type() == 5)
       {
           painter->setFont(point_font);
           if(point_font_flag == 1)
           {
               painter->drawText(QPoint(rect().center().x(),rect().center().y() - 120),QString("S:%1").arg(map_point_pb.basic().id()));
               painter->setPen(QColor(139,0,0));
               painter->drawText(QPoint(rect().center().x(),rect().center().y() - 320),QString("%1").arg(QString::fromStdString(map_point_pb.basic().name())).mid(0,5));
           }


       }
//       else if((int)map_point.type == 8)
//       {
//           painter->setFont(point_font);
//           if(point_font_flag == 1)
//           {
//               painter->drawText(QPoint(rect().center().x(),rect().center().y() - 120),QString("A:%1").arg(map_point.id));
//           }
//       }
//       else if((int)map_point.type == 9)
//       {
//           painter->setFont(point_font);
//           if(point_font_flag == 1)
//           {
//               painter->drawText(QPoint(rect().center().x(),rect().center().y() - 120),QString("D:%1").arg(map_point.id));
//           }
//       }
       else if(map_point_pb.mutable_basic()->type() == 8)
       {
           painter->setFont(point_font);
           if(point_font_flag == 1)
           {
               painter->drawText(QPoint(rect().center().x(),rect().center().y() - 120),QString("A:%1").arg(map_point_pb.basic().id()));
           }
       }
       else if(map_point_pb.mutable_basic()->type() == 9)
       {
           painter->setFont(point_font);
           if(point_font_flag == 1)
           {
               painter->drawText(QPoint(rect().center().x(),rect().center().y() - 120),QString("D:%1").arg(map_point_pb.basic().id()));
           }
       }

//       else{
//           painter->setFont(point_font);
//           if(point_font_flag == 1)
//           {
//               painter->drawText(QPoint(rect().center().x(),rect().center().y() - 120),QString("%1").arg(map_point.id));
//           }
//       }
       else if(map_point_pb.mutable_basic()->type() == 12)
       {
           painter->setFont(point_font);
           if(point_font_flag == 1)
           {
               painter->drawText(QPoint(rect().center().x(),rect().center().y() - 120),QString("IN_C:%1").arg(map_point_pb.basic().id()));
           }
       }
//       else if(map_point_pb.mutable_basic()->type() == 13)
//       {
//           int map_front_size = drawView->mapConfig[drawView->cur_map_index].map_front_size;
//           font.setPointSize(map_front_size);
//           font.setFamily("Courier New");
//           painter->setFont(font);
//           painter->drawText(QPoint(rect().center().x(),rect().center().y() - 120),QString("LIFT:%1").arg(map_point_pb.basic().id()));
//           painter->drawPixmap(QRect(rect().center().x() + 100,rect().center().y() + 100,500,500),pixmap_Lift);
//       }
       else if(map_point_pb.mutable_basic()->type() == 14)
       {
           painter->setFont(point_font);
           if(point_font_flag == 1)
           {
               painter->drawText(QPoint(rect().center().x(),rect().center().y() - 120),QString("W:%1").arg(map_point_pb.basic().id()));
           }
       }
       else if(map_point_pb.mutable_basic()->type() == 15)
       {
           painter->setFont(point_font);
           if(point_font_flag == 1)
           {
               painter->drawText(QPoint(rect().center().x(),rect().center().y() - 120),QString("U:%1").arg(map_point_pb.basic().id()));
           }
       }
       else if(map_point_pb.mutable_basic()->type() == 16)
       {
           painter->setFont(point_font);
           if(point_font_flag == 1)
           {
               painter->drawText(QPoint(rect().center().x(),rect().center().y() - 120),QString("CHK_LIFT:%1").arg(map_point_pb.basic().id()));
           }
       }
       else{
           painter->setFont(point_font);
           if(point_font_flag == 1)
           {
               painter->drawText(QPoint(rect().center().x(),rect().center().y() - 120),QString("%1").arg(map_point_pb.basic().id()));
           }
       }
       painter->restore();
   }

//   painter->setPen(Qt::blue);
//   painter->drawLine(QLine(QPoint(opposite_.x()-6,opposite_.y()),QPoint(opposite_.x()+6,opposite_.y())));
//   painter->drawLine(QLine(QPoint(opposite_.x(),opposite_.y()-6),QPoint(opposite_.x(),opposite_.y()+6)));


   if (option->state & QStyle::State_Selected)
       qt_graphicsItem_highlightSelected(this, painter, option);

/*

   QPolygonF pts;
   pts<<m_localRect.topLeft()<<m_localRect.topRight()<<m_localRect.bottomRight()<<m_localRect.bottomLeft();
   pts = mapToScene(pts);
   QRectF bound = RecalcBounds(pts);


   qDebug()<<m_localRect<<bound;
    pts.clear();
   pts<<bound.topLeft()<<bound.topRight()<<bound.bottomRight()<<bound.bottomLeft();
   pts = mapFromScene(pts);
   if ( scene() ){
   painter->save();
   painter->setPen(Qt::blue);
   painter->setBrush(Qt::NoBrush);
   painter->drawPolygon(pts);
   painter->restore();
   }
*/


}
#endif





GraphicsPolygonItem::GraphicsPolygonItem(QGraphicsItem *parent)
    :GraphicsItem(parent)
{
    // handles
    m_points.clear();
    m_pen = QPen(Qt::black);
}


int GraphicsPolygonItem::type() const
{
    return Type;
}

QRectF GraphicsPolygonItem::boundingRect() const
{
    return shape().controlPointRect();
}

QPainterPath GraphicsPolygonItem::shape() const
{
    QPainterPath path;
    path.addPolygon(m_points);
    path.closeSubpath();
    return qt_graphicsItem_shapeFromPath(path,pen());
}

void GraphicsPolygonItem::addPoint(const QPointF &point)
{
    m_points.append(mapFromScene(point));
    int dir = m_points.count();
//    SizeHandleRect *shr = new SizeHandleRect(this, dir+Left, true);
//    shr->setState(SelectionHandleActive);
    //    m_handles.push_back(shr);
}

void GraphicsPolygonItem::endPoint(const QPointF &point)
{
    Q_UNUSED(point)
    int nPoints = m_points.count();
    if( nPoints > 2 && (m_points[nPoints-1] == m_points[nPoints-2] ||
        m_points[nPoints-1].x() - 1 == m_points[nPoints-2].x() &&
        m_points[nPoints-1].y() == m_points[nPoints-2].y())){
//        delete m_handles[Left + nPoints-1];
        m_points.remove(nPoints-1);
//        m_handles.resize(Left + nPoints-1);
    }
    m_initialPoints = m_points;
}

void GraphicsPolygonItem::control(int dir, const QPointF &delta)
{
    QPointF pt = mapFromScene(delta);
//    if ( dir <= Left ) return ;
//    m_points[dir - Left -1] = pt;
    prepareGeometryChange();
    m_localRect = m_points.boundingRect();
    m_width = m_localRect.width();
    m_height = m_localRect.height();
    m_initialPoints = m_points;
    updatehandles();
}

void GraphicsPolygonItem::stretch(int handle, double sx, double sy, const QPointF &origin)
{
    QTransform trans;
//    switch (handle) {
//    case Right:
//    case Left:
//        sy = 1;
//        break;
//    case Top:
//    case Bottom:
//        sx = 1;
//        break;
//    default:
//        break;
//    }
    trans.translate(origin.x(),origin.y());
    trans.scale(sx,sy);
    trans.translate(-origin.x(),-origin.y());

    prepareGeometryChange();
    m_points = trans.map(m_initialPoints);
    m_localRect = m_points.boundingRect();
    m_width = m_localRect.width();
    m_height = m_localRect.height();
    updatehandles();
}

void GraphicsPolygonItem::updateCoordinate()
{
    QPointF pt1,pt2,delta;
    QPolygonF pts = mapToScene(m_points);
    if (parentItem()==NULL)
    {
        pt1 = mapToScene(transformOriginPoint());
        pt2 = mapToScene(boundingRect().center());
        delta = pt1 - pt2;

        for (int i = 0; i < pts.count() ; ++i )
            pts[i]+=delta;

        prepareGeometryChange();

        m_points = mapFromScene(pts);
        m_localRect = m_points.boundingRect();
        m_width = m_localRect.width();
        m_height = m_localRect.height();

        setTransform(transform().translate(delta.x(),delta.y()));
        //setTransformOriginPoint(boundingRect().center());
        moveBy(-delta.x(),-delta.y());
        setTransform(transform().translate(-delta.x(),-delta.y()));
        updatehandles();
    }
    m_initialPoints = m_points;
}

bool GraphicsPolygonItem::loadFromSql(QSqlQuery *sql_query)
{

}

bool GraphicsPolygonItem::saveToSql(QSqlQuery *sql_query)
{

}

QGraphicsItem *GraphicsPolygonItem::duplicate() const
{
    GraphicsPolygonItem * item = new GraphicsPolygonItem( );
    item->m_width = width();
    item->m_height = height();
    item->m_points = m_points;

    for ( int i = 0 ; i < m_points.size() ; ++i ){
 //       item->m_handles.push_back(new SizeHandleRect(item,Left+i+1,true));
    }

    item->setPos(pos().x(),pos().y());
    item->setPen(pen());
    item->setBrush(brush());
    item->setTransform(transform());
    item->setTransformOriginPoint(transformOriginPoint());
    item->setRotation(rotation());
    item->setScale(scale());
    item->setZValue(zValue()+0.1);
    item->updateCoordinate();
    return item;
}

void GraphicsPolygonItem::updatehandles()
{
//    GraphicsItem::updatehandles();

//    for ( int i = 0 ; i < m_points.size() ; ++i ){
//        m_handles[Left+i]->move(m_points[i].x() ,m_points[i].y() );
    //    }
}

void GraphicsPolygonItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    QColor c = brushColor();
    QLinearGradient result(boundingRect().topLeft(), boundingRect().topRight());
    result.setColorAt(0, c.darker(150));
    result.setColorAt(0.5, c.lighter(200));
    result.setColorAt(1, c.darker(150));
    painter->setBrush(result);

    painter->setPen(pen());
    painter->drawPolygon(m_points);

    if (option->state & QStyle::State_Selected)
        qt_graphicsItem_highlightSelected(this, painter, option);
}



GraphicsLineItem::GraphicsLineItem(QGraphicsItem *parent)
    :GraphicsPolygonItem(parent)
{
    m_pen = QPen(QColor(54,54,54));
    m_pen.setWidth(15);
    m_pen.setBrush(QColor(54,54,54));
    m_points.clear();
}


int GraphicsLineItem::type() const
{
    return Type;
}


QPainterPath GraphicsLineItem::shape() const
{
    QPainterPath path;
    if(m_points.size() > 1)
    {
        path.moveTo(m_points.at(0));
        path.lineTo(m_points.at(1));
    }
    return qt_graphicsItem_shapeFromPath(path,pen());
}

QGraphicsItem *GraphicsLineItem::duplicate() const
{
    GraphicsLineItem * item = new GraphicsLineItem();
    item->m_width = width();
    item->m_height = height();
    item->m_points = m_points;
    item->m_initialPoints = m_initialPoints;
    item->setPos(pos().x(),pos().y());
    item->setPen(pen());
    item->setBrush(brush());
    item->setTransform(transform());
    item->setTransformOriginPoint(transformOriginPoint());
    item->setRotation(rotation());
    item->setScale(scale());
    item->setZValue(zValue()+0.1);
    item->updateCoordinate();
    return item;
}

void GraphicsLineItem::addPoint(const QPointF &point)
{
    m_points.append(mapFromScene(point));
    int dir = m_points.count();
//    SizeHandleRect *shr = new SizeHandleRect(this, dir+Left, dir == 1 ? false : true);
//    shr->setState(SelectionHandleActive);
//    m_handles.push_back(shr);
}

void GraphicsLineItem::endPoint(const QPointF &point)
{
    Q_UNUSED(point);
    int nPoints = m_points.count();
//    if( nPoints > 2 && (m_points[nPoints-1] == m_points[nPoints-2] ||
//        m_points[nPoints-1].x() - 1 == m_points[nPoints-2].x() &&
//        m_points[nPoints-1].y() == m_points[nPoints-2].y())){
//        delete m_handles[ nPoints-1];
//        m_points.remove(nPoints-1);
//        m_handles.resize(nPoints-1);
//    }
    m_initialPoints = m_points;
}

QPointF GraphicsLineItem::opposite(int handle)
{
    QPointF pt;
//    switch (handle) {
//    case Right:
//    case Left:
//    case Top:
//    case LeftTop:
//    case RightTop:
//        pt = m_handles[1]->pos();
//        break;
//    case RightBottom:
//    case LeftBottom:
//    case Bottom:
//        pt = m_handles[0]->pos();
//        break;
//     }
    return pt;
}


void GraphicsLineItem::stretch(int handle, double sx, double sy, const QPointF &origin)
{
    QTransform trans;
//    switch (handle) {
//    case Right:
//    case Left:
//        sy = 1;
//        break;
//    case Top:
//    case Bottom:
//        sx = 1;
//        break;
//    default:
//        break;
//    }
    trans.translate(origin.x(),origin.y());
    trans.scale(sx,sy);
    trans.translate(-origin.x(),-origin.y());

    prepareGeometryChange();
//    m_points = trans.map(m_initialPoints);
//    m_localRect = m_points.boundingRect();
    m_width = m_localRect.width();
    m_height = m_localRect.height();
    updatehandles();
}

bool GraphicsLineItem::loadFromPb(agv_map::Segment segment_pb)
{
    map_segment_pb = segment_pb;

    setPos(map_segment_pb.mutable_basic()->start_x(),map_segment_pb.mutable_basic()->start_y());
    addPoint(QPointF(map_segment_pb.mutable_basic()->start_x(),map_segment_pb.mutable_basic()->start_y()));
    addPoint(QPointF(map_segment_pb.mutable_basic()->end_x(),map_segment_pb.mutable_basic()->end_y()));

    return true;
}

bool GraphicsLineItem::loadFromSql(QSqlQuery *sql_query)
{
//    readBaseAttributes(xml);
//    while(xml->readNextStartElement()){
//        if (xml->name()=="point"){
//            qreal x = xml->attributes().value("x").toDouble();
//            qreal y = xml->attributes().value("y").toDouble();
//            m_points.append(QPointF(x,y));
//            int dir = m_points.count();
//            SizeHandleRect *shr = new SizeHandleRect(this, dir+Left, dir == 1 ? false : true);
//            m_handles.push_back(shr);
//            xml->skipCurrentElement();
//        }else
//            xml->skipCurrentElement();
//    }
//    updatehandles();
    map_segment.id = sql_query->value(0).toInt();
    map_segment.start_x = sql_query->value(1).toFloat();
    map_segment.start_y = sql_query->value(2).toFloat();
    map_segment.end_x = sql_query->value(3).toFloat();
    map_segment.end_y = sql_query->value(4).toFloat();
    map_segment.cen_x = sql_query->value(5).toFloat();
    map_segment.cen_y = sql_query->value(6).toFloat();    
    map_segment.lenght = sql_query->value(7).toFloat();
    map_segment.type = sql_query->value(8).toInt();
    map_segment.radius = sql_query->value(9).toFloat();
    map_segment.dirmode = sql_query->value(10).toInt();
    map_segment.start_ang = sql_query->value(11).toFloat();
    map_segment.end_ang = sql_query->value(12).toFloat();
    map_segment.Tmode = sql_query->value(13).toInt();
    map_segment.AgvType1 = sql_query->value(14).toInt();
    map_segment.AgvType2 = sql_query->value(15).toInt();
    map_segment.AgvType3 = sql_query->value(16).toInt();
    map_segment.allowId = sql_query->value(17).toInt();
    map_segment.blockId = sql_query->value(18).toInt();
    map_segment.detection = sql_query->value(19).toInt();
    map_segment.speed_limit = sql_query->value(20).toInt();

    for(unsigned i = 0; i < sizeof(map_segment.reserved)/sizeof(map_segment.reserved[0]); i++)
    {
        map_segment.reserved[i] = sql_query->value(21 + i).toDouble();
    }

    map_segment.start_id = sql_query->value(22 + sizeof(map_segment.reserved)/sizeof(map_segment.reserved[0])).toInt();
    map_segment.end_id = sql_query->value(23 + sizeof(map_segment.reserved)/sizeof(map_segment.reserved[0])).toInt();
    map_segment.disable = sql_query->value(24 + sizeof(map_segment.reserved)/sizeof(map_segment.reserved[0])).toInt();
    map_segment.lock = sql_query->value(25 + sizeof(map_segment.reserved)/sizeof(map_segment.reserved[0])).toInt();

    //map_segment.start_id = sql_query->value(51).toInt();
    //map_segment.end_id = sql_query->value(52).toInt();


    setPos(map_segment.start_x,map_segment.start_y);
    addPoint(QPointF(map_segment.start_x,map_segment.start_y));
    addPoint(QPointF(map_segment.end_x,map_segment.end_y));

    //qDebug() << map_point.x << map_point.y;

    return true;
}

bool GraphicsLineItem::saveToSql(QSqlQuery *sql_query)
{
//    xml->writeStartElement("line");
//    writeBaseAttributes(xml);
//    for ( int i = 0 ; i < m_points.count();++i){
//        xml->writeStartElement("point");
//        xml->writeAttribute("x",QString("%1").arg(m_points[i].x()));
//        xml->writeAttribute("y",QString("%1").arg(m_points[i].y()));
//        xml->writeEndElement();
//    }
//    xml->writeEndElement();
    QString insert_point = QString("insert into SEGMENT (ID,STARTX, STARTY, ENDX, ENDY, CENX, CENY,"
                                   "SEGLEN, TYPE, RADIUS, DIR_MODE, ANG_START, ANG_END, TMODE, AGVTYPE,"
                                   "AGVTYPE1, AGVTYPE2, AGVTYPE3, ALLOW_ID, BLOCK_ID,DETECTION, SpeedLimit,"
                                   "reserved0,reserved1,reserved2,reserved3,reserved4,reserved5,reserved6,"
                                   "reserved7,reserved8,reserved9,reserved10,reserved11,reserved12,reserved13,"
                                   "reserved14,reserved15,reserved16,reserved17,reserved18,reserved19,reserved20,"
                                   "reserved21,reserved22,reserved23,reserved24,reserved25,reserved26,reserved27,"
                                   "reserved28,reserved29,START_ID,END_ID,Disable,Lock) "
                                   "values(%1,%2,%3,%4,%5,%6,%7,%8,%9,%10,%11,%12,%13,%14,%15,%16,%17,%18,%19,%20,"
                                   "%21,%22,%23,%24,%25,%26,%27,%28,%29,%30,%31,%32,%33,%34,%35,%36,%37,%38,%39,%40,"
                                   "%41,%42,%43,%44,%45,%46,%47,%48,%49,%50,%51,%52,%53,%54,%55,%56);")
            .arg(map_segment.id)
            .arg(map_segment.start_x)
            .arg(map_segment.start_y)
            .arg(map_segment.end_x)
            .arg(map_segment.end_y)
            .arg(map_segment.cen_x)
            .arg(map_segment.cen_y)
            .arg(map_segment.lenght)
            .arg(map_segment.type)
            .arg(map_segment.radius)
            .arg(map_segment.dirmode)
            .arg(map_segment.start_ang)
            .arg(map_segment.end_ang)
            .arg(map_segment.Tmode)
            .arg(map_segment.AgvType)
            .arg(map_segment.AgvType1)
            .arg(map_segment.AgvType2)
            .arg(map_segment.AgvType3)
            .arg(map_segment.allowId)
            .arg(map_segment.blockId)
            .arg(map_segment.detection)
            .arg(map_segment.speed_limit)
            .arg(map_segment.reserved[0])
            .arg(map_segment.reserved[1])
            .arg(map_segment.reserved[2])
            .arg(map_segment.reserved[3])
            .arg(map_segment.reserved[4])
            .arg(map_segment.reserved[5])
            .arg(map_segment.reserved[6])
            .arg(map_segment.reserved[7])
            .arg(map_segment.reserved[8])
            .arg(map_segment.reserved[9])
            .arg(map_segment.reserved[10])
            .arg(map_segment.reserved[11])
            .arg(map_segment.reserved[12])
            .arg(map_segment.reserved[13])
            .arg(map_segment.reserved[14])
            .arg(map_segment.reserved[15])
            .arg(map_segment.reserved[16])
            .arg(map_segment.reserved[17])
            .arg(map_segment.reserved[18])
            .arg(map_segment.reserved[19])
            .arg(map_segment.reserved[20])
            .arg(map_segment.reserved[21])
            .arg(map_segment.reserved[22])
            .arg(map_segment.reserved[23])
            .arg(map_segment.reserved[24])
            .arg(map_segment.reserved[25])
            .arg(map_segment.reserved[26])
            .arg(map_segment.reserved[27])
            .arg(map_segment.reserved[28])
            .arg(map_segment.reserved[29])
            .arg(map_segment.start_id)
            .arg(map_segment.end_id)
            .arg(map_segment.disable)
            .arg(map_segment.lock)
            ;

    qDebug() << insert_point;

    if(!sql_query->exec(insert_point))
    {
        qDebug()<<sql_query->lastError();
    }

    return true;
}

void GraphicsLineItem::updatehandles()
{
//    for ( int i = 0 ; i < m_points.size() ; ++i ){
//        m_handles[i]->move(m_points[i].x() ,m_points[i].y() );
//    }
}

void GraphicsLineItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

//    int map_segment_width = dynamic_cast<DrawView *>(dynamic_cast<DrawScene *>(scene())->view())->mapConfig.map_segment_width;
    DrawView *drawView = dynamic_cast<DrawView *>(dynamic_cast<DrawScene *>(scene())->view());

    int map_segment_width = drawView->mapConfig[drawView->cur_map_index].map_segment_width;

//    if(map_segment.disable == 1)
//    {
//        qDebug()<<"id = "<<map_segment.id<<"disable 等于 1";
//    }

    painter->setPen(pen());
    painter->setBrush(Qt::NoBrush);

//    if(map_segment.disable == 1)
//    {
//        m_pen.setStyle(Qt::DotLine);
//    }
//    else
//    {
//        m_pen.setStyle(Qt::SolidLine);
//    }


    if(interrput_merge_tag == false)
    {
        if(map_segment_pb.mutable_basic()->disable() == 1)
        {
            m_pen.setStyle(Qt::DotLine);
        }
        else
        {
            m_pen.setStyle(Qt::SolidLine);
        }
    }
    else
    {
        m_pen.setStyle(Qt::DashLine);
    }

//    if(interrput_merge_tag == false)
//    {
//        if(map_segment.disable == 1)
//        {
//            m_pen.setStyle(Qt::DotLine);
//        }
//        else
//        {
//            m_pen.setStyle(Qt::SolidLine);
//        }
//    }
//    else
//    {
//        m_pen.setStyle(Qt::DashLine);
//    }

    painter->setPen(pen());

    if (option->state & QStyle::State_Selected)
    {
        //qt_graphicsItem_highlightSelected(this, painter, option);
        m_pen.setColor(QColor(0,255,0));
        setPen(m_pen);
    }
    else if(route_flag == 1)
    {
        m_pen.setColor(routeColor);
//        m_pen.setWidth(40);
        m_pen.setWidth(custom_line_width);
        setPen(m_pen);
    }
    else if(route_flag == 2)
    {
        m_pen.setColor(QColor(255,0,0));
//        m_pen.setWidth(80);
        m_pen.setWidth(custom_line_width*2);
        setPen(m_pen);
    }
    else if(route_flag == 3)
    {
        m_pen.setColor(QColor(0,0,255));
//        m_pen.setWidth(80);
        m_pen.setWidth(custom_line_width*2);
        setPen(m_pen);
    }
    else if(map_segment_pb.basic().disable() == 1)
    {
        color = QColor(130,130,130);
        m_pen.setColor(color);
        m_pen.setWidth(custom_line_width);
        setPen(m_pen);
    }
    else{
        m_pen.setColor(QColor(40,40,40));
        m_pen.setWidth(custom_line_width);
        setPen(m_pen);
    }

    if ( m_points.size() > 1)
    {
        painter->drawLine(m_points.at(0),m_points.at(1));
    }

    linef.setPoints(mapToScene(m_points.at(0)),mapToScene(m_points.at(1)));

    float start_ang = 360.0 - linef.angle();
    float end_ang = 360.0 - linef.angle();
    //转换坐标系角度 y轴正方向为0°
//    if(0.0f <= start_ang && start_ang < 90.0f)
//    {
//        start_ang += 270.0f;
//    }
//    else
//    {
//        start_ang -= 90.0f;
//    }

    //转换正常坐标系角度±180
    if(start_ang >= 180.0f)
    {
        start_ang -= 360.0f;
    }
    end_ang = start_ang;

    map_segment_pb.mutable_basic()->set_start_x(linef.x1());
    map_segment_pb.mutable_basic()->set_start_y(linef.y1());
    map_segment_pb.mutable_basic()->set_end_x(linef.x2());
    map_segment_pb.mutable_basic()->set_end_y(linef.y2());
    map_segment_pb.mutable_basic()->set_cen_x(0);
    map_segment_pb.mutable_basic()->set_cen_y(0);
    map_segment_pb.mutable_basic()->set_radius(0);
    map_segment_pb.mutable_basic()->set_seglen(qRound(linef.length()));
    map_segment_pb.mutable_basic()->set_type(0);
//    map_segment_pb.mutable_basic()->set_start_angle(start_ang);
//    map_segment_pb.mutable_basic()->set_end_angle(end_ang);

    linef.setP1(m_points.at(0));
    linef.setP2(m_points.at(1));

    if(map_segment_pb.mutable_basic()->dir_mode() != 0)
    {
        painter->save();
        painter->setPen(QPen(Qt::NoPen));
        painter->setBrush(QBrush(QColor(0,139,0)));
        QLineF v;
        if(map_segment_pb.mutable_basic()->dir_mode() == 1)
        {
            v = linef.unitVector();
        }
        else if(map_segment_pb.mutable_basic()->dir_mode() == 2)
        {
            v = linef.unitVector().normalVector().normalVector();
        }
        else{
            v = linef.unitVector().normalVector().normalVector();
        }
        v.setLength(custom_l);        // 改变单位向量的大小，实际就是改变箭头长度
        //v.translate(QPointF(linef.center()));
        v.translate((linef.dx()-v.dx())/2,(linef.dy()-v.dy())/2);
        QLineF n = v.normalVector(); // 法向量
        n.setLength(n.length() * custom_n/100); // 这里设定箭头的宽度
        QLineF n2 = n.normalVector().normalVector(); // 两次法向量运算以后，就得到一个反向的法向量
        QPolygonF marrow_points;
        marrow_points.append(v.p2());
        marrow_points.append(n.p2());
        marrow_points.append(n2.p2());
        painter->drawPolygon(marrow_points);
        painter->restore();
    }





}






GraphicsArcItem::GraphicsArcItem(QGraphicsItem *parent)
{
    m_pen = QPen(QColor(180,180,180));
    m_pen.setWidth(5);
    m_pen.setBrush(QColor(180,180,180));
    m_points.clear();
    R = 0;
    Radius = 0;
    startAngle = 0;
    sweepLength = 0;
//    scale(1,-1);
    setTransform(QTransform::fromScale(1,-1),true);
}


int GraphicsArcItem::type() const
{
    return Type;
}

void GraphicsArcItem::get_arc_path(float radius, int loading)
{
    if(m_points.size() > 1)
    {
        arc_path = QPainterPath ();
        QLineF L;
        L.setPoints(m_points.at(0),m_points.at(1));
        int R = 0;
        QPointF cen;

        if(loading == 0)  //  鼠标画地图时
        {
            R = qRound( (0.5 * L.length() * (1 + qAbs((caculateCrossLengthPtToLine(L.p1(), L.p2(), cur_point))/L.length()))) );
            if(caculateCrossLengthPtToLine(L.p1(), L.p2(), cur_point) > 0)
            {
                cen = getEllipseCoors(L.p1(), L.p2(),R)[0].toPoint();
            }
            else
            {
                cen = getEllipseCoors(L.p1(), L.p2(),R)[1].toPoint();
            }

            if(radius > (L.length()/2))  //悬浮框输入半径时（radius不等于0）
            {
                R = qRound(radius);
                if(QLineF(this->cen, getEllipseCoors(L.p1(), L.p2(),R)[0]).length() >
                        QLineF(this->cen, getEllipseCoors(L.p1(), L.p2(),R)[1]).length())
                {
                    cen = getEllipseCoors(L.p1(), L.p2(),R)[1].toPoint();
                }
                else
                {
                    cen = getEllipseCoors(L.p1(), L.p2(),R)[0].toPoint();
                }
            }
        }
        else if(loading == 1)  //加载地图时
        {
            R = qRound(radius);
            if(QLineF(mapFromScene( QPointF(map_segment_pb.mutable_basic()->cen_x(),map_segment_pb.mutable_basic()->cen_y())), getEllipseCoors(L.p1(), L.p2(),R)[0]).length() >
                    QLineF(mapFromScene( QPointF(map_segment_pb.mutable_basic()->cen_x(),map_segment_pb.mutable_basic()->cen_y())), getEllipseCoors(L.p1(), L.p2(),R)[1]).length())
            {
                cen = getEllipseCoors(L.p1(), L.p2(),R)[1].toPoint();
            }
            else
            {
                cen = getEllipseCoors(L.p1(), L.p2(),R)[0].toPoint();
            }

            cen = mapFromScene(QPointF(map_segment_pb.mutable_basic()->cen_x(),map_segment_pb.mutable_basic()->cen_y()));
        }

        //qDebug() <<"R:" << R << "  cen:" << cen << "   pointToline:" << caculateCrossLengthPtToLine(L.p1(), L.p2(), cur_point);

        QLineF r0(L.p1(),cur_point);
        QLineF r1(cen,L.p1());
        QLineF r2(cen,L.p2());
        qreal startAngle = 0,sweepLength = 0;

        startAngle = r1.angle();
        sweepLength = r2.angle() - r1.angle();
        if(sweepLength > 180.0)
        {
            sweepLength -= 360.0;
        }
        else if(sweepLength < -180.0)
        {
            sweepLength += 360.0;
        }
        arc_path.moveTo(m_points.at(0));

        arc_path.arcTo(cen.x()-R,cen.y()-R,2*R,2*R,startAngle,sweepLength);

        this->cen = cen;
        Radius = R;
        rectf = QRectF(cen.x()-R,cen.y()-R,2*R,2*R);
        this->startAngle = startAngle;
        this->sweepLength = sweepLength;


        //qDebug() << mapToScene( path.pointAtPercent(0.5));

        //qDebug() << "r1 ang:" << r1.angle() << "  r2 ang:" << r2.angle() << "  r0 ang:" << r0.angle() << "  L ang:" << L.angle()
        //         << "  dist:" << caculateCrossLengthPtToLine(L.p1(), L.p2(), cur_point)
        //         << "  sweepLength:" << sweepLength;
    }
}

QPainterPath GraphicsArcItem::shape() const
{
    return qt_graphicsItem_shapeFromPath(arc_path,pen());
}

QGraphicsItem *GraphicsArcItem::duplicate() const
{
    GraphicsArcItem * item = new GraphicsArcItem();
    item->m_width = width();
    item->m_height = height();
    item->m_points = m_points;
    item->m_initialPoints = m_initialPoints;
    item->setPos(pos().x(),pos().y());
    item->setPen(pen());
    item->setBrush(brush());
    item->setTransform(transform());
    item->setTransformOriginPoint(transformOriginPoint());
    item->setRotation(rotation());
    item->setScale(scale());
    item->setZValue(zValue()+0.1);
    item->updateCoordinate();
    return item;
}

void GraphicsArcItem::addPoint(const QPointF &point)
{
    m_points.append(mapFromScene(point));
    int dir = m_points.count();
//    SizeHandleRect *shr = new SizeHandleRect(this, dir+Left, dir == 1 ? false : true);
//    shr->setState(SelectionHandleActive);
//    m_handles.push_back(shr);
}

void GraphicsArcItem::endPoint(const QPointF &point)
{
    Q_UNUSED(point);
    int nPoints = m_points.count();
//    if( nPoints > 2 && (m_points[nPoints-1] == m_points[nPoints-2] ||
//        m_points[nPoints-1].x() - 1 == m_points[nPoints-2].x() &&
//        m_points[nPoints-1].y() == m_points[nPoints-2].y())){
//        delete m_handles[ nPoints-1];
//        m_points.remove(nPoints-1);
//        m_handles.resize(nPoints-1);
//    }
    m_initialPoints = m_points;
}

QPointF GraphicsArcItem::opposite(int handle)
{
    QPointF pt;
//    switch (handle) {
//    case Right:
//    case Left:
//    case Top:
//    case LeftTop:
//    case RightTop:
//        pt = m_handles[1]->pos();
//        break;
//    case RightBottom:
//    case LeftBottom:
//    case Bottom:
//        pt = m_handles[0]->pos();
//        break;
//     }
    return pt;
}

void GraphicsArcItem::stretch(int handle, double sx, double sy, const QPointF &origin)
{
    QTransform trans;
//    switch (handle) {
//    case Right:
//    case Left:
//        sy = 1;
//        break;
//    case Top:
//    case Bottom:
//        sx = 1;
//        break;
//    default:
//        break;
//    }
    trans.translate(origin.x(),origin.y());
    trans.scale(sx,sy);
    trans.translate(-origin.x(),-origin.y());

    prepareGeometryChange();
//    m_points = trans.map(m_initialPoints);
//    m_localRect = m_points.boundingRect();
    m_width = m_localRect.width();
    m_height = m_localRect.height();
    updatehandles();
}

bool GraphicsArcItem::loadFromPb(agv_map::Segment segment_pb)
{
    map_segment_pb = segment_pb;

    setPos(map_segment_pb.mutable_basic()->start_x(),map_segment_pb.mutable_basic()->start_y());
    addPoint(QPointF(map_segment_pb.mutable_basic()->start_x(),map_segment_pb.mutable_basic()->start_y()));
    addPoint(QPointF(map_segment_pb.mutable_basic()->end_x(),map_segment_pb.mutable_basic()->end_y()));

    get_arc_path(map_segment_pb.mutable_basic()->radius(), 1);

    return true;
}

bool GraphicsArcItem::loadFromSql(QSqlQuery *sql_query)
{

//    updatehandles();
    map_segment.id = sql_query->value(0).toInt();
    map_segment.start_x = sql_query->value(1).toFloat();
    map_segment.start_y = sql_query->value(2).toFloat();
    map_segment.end_x = sql_query->value(3).toFloat();
    map_segment.end_y = sql_query->value(4).toFloat();
    map_segment.cen_x = sql_query->value(5).toFloat();
    map_segment.cen_y = sql_query->value(6).toFloat();
    map_segment.lenght = sql_query->value(7).toFloat();
    map_segment.type = sql_query->value(8).toInt();
    map_segment.radius = sql_query->value(9).toFloat();
    map_segment.dirmode = sql_query->value(10).toInt();
    map_segment.start_ang = sql_query->value(11).toFloat();
    map_segment.end_ang = sql_query->value(12).toFloat();
    map_segment.Tmode = sql_query->value(13).toInt();
    map_segment.AgvType1 = sql_query->value(14).toInt();
    map_segment.AgvType2 = sql_query->value(15).toInt();
    map_segment.AgvType3 = sql_query->value(16).toInt();
    map_segment.allowId = sql_query->value(17).toInt();
    map_segment.blockId = sql_query->value(18).toInt();
    map_segment.detection = sql_query->value(19).toInt();
    map_segment.speed_limit = sql_query->value(20).toInt();

    for(unsigned i = 0; i < sizeof(map_segment.reserved)/sizeof(map_segment.reserved[0]); i++)
    {
        map_segment.reserved[i] = sql_query->value(21 + i).toDouble();
    }
    //map_segment.start_id = sql_query->value(51).toInt();
    //map_segment.end_id = sql_query->value(52).toInt();

    map_segment.start_id = sql_query->value(22 + sizeof(map_segment.reserved)/sizeof(map_segment.reserved[0])).toInt();
    map_segment.end_id = sql_query->value(23 + sizeof(map_segment.reserved)/sizeof(map_segment.reserved[0])).toInt();
    map_segment.disable = sql_query->value(24 + sizeof(map_segment.reserved)/sizeof(map_segment.reserved[0])).toInt();
    map_segment.lock = sql_query->value(25 + sizeof(map_segment.reserved)/sizeof(map_segment.reserved[0])).toInt();

    setPos(map_segment.start_x,map_segment.start_y);
    addPoint(QPointF(map_segment.start_x,map_segment.start_y));
    addPoint(QPointF(map_segment.end_x,map_segment.end_y));

    get_arc_path(map_segment.radius, 1);

    return true;
}

bool GraphicsArcItem::saveToSql(QSqlQuery *sql_query)
{

    QString insert_point = QString("insert into SEGMENT (ID,STARTX, STARTY, ENDX, ENDY, CENX, CENY,"
                                   "SEGLEN, TYPE, RADIUS, DIR_MODE, ANG_START, ANG_END, TMODE,AGVTYPE,"
                                   "AGVTYPE1, AGVTYPE2, AGVTYPE3, ALLOW_ID, BLOCK_ID,DETECTION, SpeedLimit,"
                                   "reserved0,reserved1,reserved2,reserved3,reserved4,reserved5,reserved6,"
                                   "reserved7,reserved8,reserved9,reserved10,reserved11,reserved12,reserved13,"
                                   "reserved14,reserved15,reserved16,reserved17,reserved18,reserved19,reserved20,"
                                   "reserved21,reserved22,reserved23,reserved24,reserved25,reserved26,reserved27,"
                                   "reserved28,reserved29,START_ID,END_ID,Disable,Lock) "
                                   "values(%1,%2,%3,%4,%5,%6,%7,%8,%9,%10,%11,%12,%13,%14,%15,%16,%17,%18,%19,%20,"
                                   "%21,%22,%23,%24,%25,%26,%27,%28,%29,%30,%31,%32,%33,%34,%35,%36,%37,%38,%39,%40,"
                                   "%41,%42,%43,%44,%45,%46,%47,%48,%49,%50,%51,%52,%53,%54,%55,%56);")
            .arg(map_segment.id)
            .arg(map_segment.start_x)
            .arg(map_segment.start_y)
            .arg(map_segment.end_x)
            .arg(map_segment.end_y)
            .arg(map_segment.cen_x)
            .arg(map_segment.cen_y)
            .arg(map_segment.lenght)
            .arg(map_segment.type)
            .arg(map_segment.radius)
            .arg(map_segment.dirmode)
            .arg(map_segment.start_ang)
            .arg(map_segment.end_ang)
            .arg(map_segment.Tmode)
            .arg(map_segment.AgvType)
            .arg(map_segment.AgvType1)
            .arg(map_segment.AgvType2)
            .arg(map_segment.AgvType3)
            .arg(map_segment.allowId)
            .arg(map_segment.blockId)
            .arg(map_segment.detection)
            .arg(map_segment.speed_limit)
            .arg(map_segment.reserved[0])
            .arg(map_segment.reserved[1])
            .arg(map_segment.reserved[2])
            .arg(map_segment.reserved[3])
            .arg(map_segment.reserved[4])
            .arg(map_segment.reserved[5])
            .arg(map_segment.reserved[6])
            .arg(map_segment.reserved[7])
            .arg(map_segment.reserved[8])
            .arg(map_segment.reserved[9])
            .arg(map_segment.reserved[10])
            .arg(map_segment.reserved[11])
            .arg(map_segment.reserved[12])
            .arg(map_segment.reserved[13])
            .arg(map_segment.reserved[14])
            .arg(map_segment.reserved[15])
            .arg(map_segment.reserved[16])
            .arg(map_segment.reserved[17])
            .arg(map_segment.reserved[18])
            .arg(map_segment.reserved[19])
            .arg(map_segment.reserved[20])
            .arg(map_segment.reserved[21])
            .arg(map_segment.reserved[22])
            .arg(map_segment.reserved[23])
            .arg(map_segment.reserved[24])
            .arg(map_segment.reserved[25])
            .arg(map_segment.reserved[26])
            .arg(map_segment.reserved[27])
            .arg(map_segment.reserved[28])
            .arg(map_segment.reserved[29])
            .arg(map_segment.start_id)
            .arg(map_segment.end_id)
            .arg(map_segment.disable)
            .arg(map_segment.lock)
            ;

    qDebug() << insert_point;

    if(!sql_query->exec(insert_point))
    {
        qDebug()<<sql_query->lastError();
    }

    return true;
}

QVector<QPointF> GraphicsArcItem::getEllipseCoors(const QPointF &r1, const QPointF &r2, float r)
{
    double c1=0.0f, c2=0.0f, A=0.0f, B=0.0f, C=0.0f, x0=0.0f, y0=0.0f, x1=0.0f, y1=0.0f;
    if (MAXCOM_F(qAbs(r2.x()-r1.x()),0.0f)) {
        c1 = (pow(r2.x(),2)-pow(r1.x(),2)+pow(r2.y(),2)-pow(r1.y(),2))/2/(r2.x()-r1.x());
        c2 = (r2.y()-r1.y())/(r2.x()-r1.x());
        A = 1.0+pow(c2, 2);
        B = 2*(r1.x()-c1)*c2-2*r1.y();
        C = pow((r1.x()-c1), 2)+pow(r1.y(), 2)-pow(r, 2);
        //如何b^2-4ac <= 0,则赋值为0.0f
        double b_ac = sqrt(B*B-4*A*C);
        if (qIsNaN(b_ac)) b_ac = 0.0f;
        y0 = (-B+b_ac)/(2*A);
        x0 = c1-c2*y0;
        y1 = (-B-b_ac)/2/A;
        x1 = c1-c2*y1;
    } else {
        float d = sqrt(pow((r2.y()-r1.y()), 2)+pow((r2.x()-r1.x()), 2));
        d = sqrt(pow(r, 2)-pow(d/2, 2));
        y0 = (r1.y()+r2.y())/2;
        x0 = (r1.x()+d);
        y1 = (r1.y()+r2.y())/2;
        x1 = (r1.x()-d);
    }

    QVector<QPointF> centorf;
    centorf<<QPointF(x0, y0);
    centorf<<QPointF(x1, y1);
    return centorf;
}

qreal GraphicsArcItem::caculateCrossLengthPtToLine(const QPointF &pt1, const QPointF &pt2,  QPointF pt3)
{
    // 直线垂直的情况;
    if (pt1.x() == pt2.x())
    {
        return pt3.x() - pt1.x();
    }
    else
    {
        qreal angle = QLineF(pt1, pt2).angleTo(QLineF(pt1, pt3));
        qreal leng = sin(angle / 180 * M_PI) * (QLineF(pt1, pt3).length());

        return leng;
    }
}

void GraphicsArcItem::updatehandles()
{
//    for ( int i = 0 ; i < m_points.size() ; ++i ){
//        m_handles[i]->move(m_points[i].x() ,m_points[i].y() );
//    }
}

void GraphicsArcItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

//    int map_segment_width = dynamic_cast<DrawView *>(dynamic_cast<DrawScene *>(scene())->view())->mapConfig.map_segment_width;
    DrawView *drawView = dynamic_cast<DrawView *>(dynamic_cast<DrawScene *>(scene())->view());

    int map_segment_width = drawView->mapConfig[drawView->cur_map_index].map_segment_width;


//    get_arc_path(map_segment.radius);
//    map_segment.lenght = arc_path.length();
    painter->setPen(pen());
    painter->setBrush(Qt::NoBrush);

    if(map_segment.disable == 1)
    {
        m_pen.setStyle(Qt::DotLine);
    }
    else
    {
        m_pen.setStyle(Qt::SolidLine);
    }

//    if(interrput_merge_tag == false)
//    {
//        if(map_segment.disable == 1)
//        {
//            m_pen.setStyle(Qt::DotLine);
//        }
//        else
//        {
//            m_pen.setStyle(Qt::SolidLine);
//        }
//    }
//    else
//    {
//        m_pen.setStyle(Qt::DashLine);
//    }

    if (option->state & QStyle::State_Selected)
    {
        //qt_graphicsItem_highlightSelected(this, painter, option);
        m_pen.setColor(QColor(0,255,0));
        setPen(m_pen);
    }
    else if(route_flag == 1)
    {
        m_pen.setColor(routeColor);
//        m_pen.setWidth(20);
        m_pen.setWidth(custom_line_width);
        setPen(m_pen);
    }
    else if(route_flag == 2)
    {
        m_pen.setColor(QColor(255,0,0));
//        m_pen.setWidth(40);
        m_pen.setWidth(custom_line_width*2);
        setPen(m_pen);
    }
    else if(route_flag == 3)
    {
        m_pen.setColor(QColor(0,0,255));
//        m_pen.setWidth(40);
        m_pen.setWidth(custom_line_width*2);
        setPen(m_pen);
    }
    else if(map_segment.disable == 1)
    {
        color = QColor(130,130,130);
        m_pen.setColor(color);
        m_pen.setWidth(custom_line_width);
        setPen(m_pen);
    }
    else{
        m_pen.setColor(QColor(40,40,40));
        m_pen.setWidth(custom_line_width);
        setPen(m_pen);
    }
    if ( m_points.size() > 1)
        painter->drawPath(shape());

    linef.setP1(m_points.at(0));
    linef.setP2(m_points.at(1));

    map_segment_pb.mutable_basic()->set_start_x(mapToScene(m_points.at(0)).x());
    map_segment_pb.mutable_basic()->set_start_y(mapToScene(m_points.at(0)).y());
    map_segment_pb.mutable_basic()->set_end_x(mapToScene(m_points.at(1)).x());
    map_segment_pb.mutable_basic()->set_end_y(mapToScene(m_points.at(1)).y());
    map_segment_pb.mutable_basic()->set_cen_x(mapToScene(cen).x());
    map_segment_pb.mutable_basic()->set_cen_y(mapToScene(cen).y());
    map_segment_pb.mutable_basic()->set_radius(Radius);
    map_segment_pb.mutable_basic()->set_seglen(qRound( arc_path.length() ));
    map_segment_pb.mutable_basic()->set_type(1);

    float start_ang = 0;
    float end_ang = 0;

    if(sweepLength > 0)
    {
        start_ang = startAngle + 90.0;
        if(start_ang >= 360.0f)
        {
            start_ang -= 360.0f;
        }
    }
    else
    {
        start_ang = startAngle - 90.0f;
        if(start_ang < 0)
        {
            start_ang += 360.0f;
        }
    }
    end_ang = start_ang + sweepLength;
    if(end_ang >= 360.0f)
    {
        end_ang -= 360.0f;
    }
    else if(end_ang < 0)
    {
        end_ang += 360.0f;
    }

    if(qAbs(start_ang - 360.0f) < 0.001f)
    {
        start_ang = 0;
    }
    if(qAbs(end_ang - 360.0f) < 0.001f)
    {
        end_ang = 0;
    }

    //转换坐标系角度 y轴正方向为0°
//    if(0.0f <= start_ang && start_ang < 90.0f)
//    {
//        start_ang += 270.0f;
//    }
//    else
//    {
//        start_ang -= 90.0f;
//    }
//    if(0.0f <= end_ang && end_ang < 90.0f)
//    {
//        end_ang += 270.0f;
//    }
//    else
//    {
//        end_ang -= 90.0f;
//    }

    //转换正常坐标系角度±180
    if(start_ang >= 180.0f)
    {
        start_ang -= 360.0f;
    }
    //转换正常坐标系角度±180
    if(end_ang >= 180.0f)
    {
        end_ang -= 360.0f;
    }

    map_segment_pb.mutable_basic()->set_start_angle(start_ang);
    map_segment_pb.mutable_basic()->set_end_angle(end_ang);



    map_segment.speed_limit = 0;

    if(map_segment_pb.mutable_basic()->dir_mode() != 0)
    {
        painter->save();
        painter->setPen(QPen(Qt::NoPen));
        painter->setBrush(QBrush(QColor(0,139,0)));
        QLineF v;
        if(map_segment_pb.mutable_basic()->dir_mode() == 1)
        {
            v = linef.unitVector();
        }
        else{
            v = linef.unitVector().normalVector().normalVector();
        }
        v.setLength(custom_l);        // 改变单位向量的大小，实际就是改变箭头长度
        //v.translate(QPointF(linef.center()));
        v.translate(arc_path.pointAtPercent(0.5));
        v.translate(-v.dx()/2,-v.dy()/2);
        QLineF n = v.normalVector(); // 法向量
        n.setLength(n.length() * custom_n/100); // 这里设定箭头的宽度
        QLineF n2 = n.normalVector().normalVector(); // 两次法向量运算以后，就得到一个反向的法向量
        QPolygonF marrow_points;
        marrow_points.append(v.p2());
        marrow_points.append(n.p2());
        marrow_points.append(n2.p2());
        painter->drawPolygon(marrow_points);
        painter->restore();
    }


}

GraphicsRgvItem::GraphicsRgvItem(QGraphicsItem * parent)
    :GraphicsItem(parent)
{
    m_localRect = QRect(-1117,-715,2234,1430);
    pixmap_Default.load(QString::fromUtf8(":/icons/images/Default.png"));
    pixmap_RgvOffline.load(QString::fromUtf8(":/icons/images/RgvOffline.png"));
    pixmap_RgvOffline_load.load(QString::fromUtf8(":/icons/images/RgvOfflineLoad.png"));
    pixmap_RgvManual.load(QString::fromUtf8(":/icons/images/RgvManual.png"));
    pixmap_RgvManual_load.load(QString::fromUtf8(":/icons/images/RgvManualLoad.png"));
    pixmap_RgvWarning.load(QString::fromUtf8(":/icons/images/RgvWarning.png"));
    pixmap_RgvWarning_load.load(QString::fromUtf8(":/icons/images/RgvWarningLoad.png"));
    pixmap_RgvMoving.load(QString::fromUtf8(":/icons/images/RgvMoving.png"));
    pixmap_RgvMoving_load.load(QString::fromUtf8(":/icons/images/RgvMovingLoad.png"));
    pixmap_RgvPickDown.load(QString::fromUtf8(":/icons/images/RgvPickDown.png"));
    pixmap_RgvPickDown_load.load(QString::fromUtf8(":/icons/images/RgvPickDownLoad.png"));
    pixmap_RgvPickUp.load(QString::fromUtf8(":/icons/images/RgvPickUp.png"));
    pixmap_RgvPickUp_load.load(QString::fromUtf8(":/icons/images/RgvPickUpLoad.png"));
    pixmap_RgvStandby.load(QString::fromUtf8(":/icons/images/RgvStandby.png"));
    pixmap_RgvStandby_load.load(QString::fromUtf8(":/icons/images/RgvStandbyLoad.png"));

    setZValue(1);
}

GraphicsRgvItem::~GraphicsRgvItem()
{

}

int GraphicsRgvItem::type() const
{
    return Type;
}

QRectF GraphicsRgvItem::boundingRect() const
{
    return m_localRect;
}

bool GraphicsRgvItem::loadFromSql(QSqlQuery *sql_query)
{

}

bool GraphicsRgvItem::saveToSql(QSqlQuery *sql_query)
{

}

void GraphicsRgvItem::move(const QPointF &point)
{
    moveBy(point.x(),point.y());
}

void GraphicsRgvItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QRect target = m_localRect.toRect();
    rgv_state = 0;
    if(rgv_info.rgv_main_state == OFFLINE)
    {
        if(rgv_info.loading == 0)
        {
            rgv_state = RGV_OFFLINE;
        }
        else{
            rgv_state = RGV_OFFLINE_LOAD;
        }
    }
    else if(rgv_info.rgv_main_state == MANUAL)
    {
        if(rgv_info.loading == 0)
        {
            rgv_state = RGV_MANUAL;
        }
        else{
            rgv_state = RGV_MANUAL_LOAD;
        }
    }
    else if(rgv_info.rgv_main_state == AUTO)
    {
        if(rgv_info.rgv_sub_state == STANDBY)
        {
            if(rgv_info.loading == 0)
            {
                rgv_state = RGV_STANDBY;
            }
            else{
                rgv_state = RGV_STANDBY_LOAD;
            }
        }
        else if(rgv_info.rgv_sub_state == MOVING)
        {
            if(rgv_info.loading == 0)
            {
                rgv_state = RGV_MOVING;
            }
            else{
                rgv_state = RGV_MOVING_LOAD;
            }
        }
        else if(rgv_info.rgv_sub_state == ERROR_STOP)
        {
            if(rgv_info.loading == 0)
            {
                rgv_state = RGV_WARNING;
            }
            else{
                rgv_state = RGV_WARNING_LOAD;
            }
        }
        else if(rgv_info.rgv_sub_state == TASKRUNNING)
        {
            if(rgv_info.task_sub_state == 1 || rgv_info.task_sub_state == 2)   //取货途中/取货过程中
            {
                if(rgv_info.loading == 0)
                {
                    rgv_state = RGV_PICKUP;
                }
                else{
                    rgv_state = RGV_PICKUP_LOAD;
                }
            }
            else if(rgv_info.task_sub_state == 3 || rgv_info.task_sub_state == 4)   //放货途中/放货过程中
            {
                if(rgv_info.loading == 0)
                {
                    rgv_state = RGV_PICKDOWN;
                }
                else{
                    rgv_state = RGV_PICKDOWN_LOAD;
                }
            }

        }

    }

    switch (rgv_state)
    {
    case DEFAULT:
        painter->drawPixmap(target,pixmap_Default);
        break;
    case RGV_OFFLINE:
        painter->drawPixmap(target,pixmap_RgvOffline);
        break;
    case RGV_OFFLINE_LOAD:
        painter->drawPixmap(target,pixmap_RgvOffline_load);
        break;
    case RGV_MANUAL:
        painter->drawPixmap(target,pixmap_RgvManual);
        break;
    case RGV_MANUAL_LOAD:
        painter->drawPixmap(target,pixmap_RgvManual_load);
        break;
    case RGV_WARNING:
        painter->drawPixmap(target,pixmap_RgvWarning);
        break;
    case RGV_WARNING_LOAD:
        painter->drawPixmap(target,pixmap_RgvWarning_load);
        break;
    case RGV_MOVING:
        painter->drawPixmap(target,pixmap_RgvMoving);
        break;
    case RGV_MOVING_LOAD:
        painter->drawPixmap(target,pixmap_RgvMoving_load);
        break;
    case RGV_PICKDOWN:
        painter->drawPixmap(target,pixmap_RgvPickDown);
        break;
    case RGV_PICKDOWN_LOAD:
        painter->drawPixmap(target,pixmap_RgvPickDown_load);
        break;
    case RGV_PICKUP:
        painter->drawPixmap(target,pixmap_RgvPickUp);
        break;
    case RGV_PICKUP_LOAD:
        painter->drawPixmap(target,pixmap_RgvPickUp_load);
        break;
    case RGV_STANDBY:
        painter->drawPixmap(target,pixmap_RgvStandby);
        break;
    case RGV_STANDBY_LOAD:
        painter->drawPixmap(target,pixmap_RgvStandby_load);
        break;
    }

    painter->save();
    QString textId = QString("%1").arg((int)rgv_info.dev_id);
    QFont font;
    font.setBold(true);
    font.setPointSize(420);
    painter->setFont(font);
//    QFontMetrics metrics(font);
//    int w=metrics.width(textId); //获取显示文本的宽度
//    int h=metrics.height(); //获取显示文本的高度
    int w = 10;
    int h = 10;
    QPen pen;
    pen.setStyle(Qt::SolidLine);
    pen.setColor(QColor(Qt::white));
    painter->setPen(pen);
    painter->scale(1,-1);

    if(rgv_info.rgv_sub_state == TASKRUNNING)
    {
        painter->drawText(QPoint(rect().center().x()-w*2,rect().center().y() + h/3),textId);

        font.setPointSize(220);
        painter->setFont(font);
        pen.setColor(QColor(Qt::white));
        painter->setPen(pen);
        painter->drawText(QPoint(rect().center().x()-w,rect().center().y() + h/3),QString("(%1)").arg((int)rgv_info.task_id));
    }
    else{
        painter->drawText(QPoint(rect().center().x()-w/2,rect().center().y() + h/3),textId);
    }
    painter->restore();
}


GraphicsMeasureItem::GraphicsMeasureItem(QGraphicsItem *parent)
    :GraphicsPolygonItem(parent)
{
    m_pen = QPen(QColor(230,54,54));
    m_pen.setWidth(15);
    setBrush(QBrush(QColor(255,255,255)));
    m_points.clear();
//    setZValue(1.0);
        setZValue(5);
//    scale(1,-1);
    setTransform(QTransform::fromScale(1,-1),true);

}

int GraphicsMeasureItem::type() const
{
    return Type;
}

QRectF GraphicsMeasureItem::boundingRect() const
{
    return shape().controlPointRect();
}

QPainterPath GraphicsMeasureItem::shape() const
{
    QPainterPath path;
    path.addPolygon(m_points);
    //path.closeSubpath();
    return qt_graphicsItem_shapeFromPath(path,pen());
}

void GraphicsMeasureItem::addPoint(const QPointF &point)
{
    m_points.append(mapFromScene(point));
    int dir = m_points.count();
//    SizeHandleRect *shr = new SizeHandleRect(this, dir+Left, true);
//    shr->setState(SelectionHandleActive);
    //    m_handles.push_back(shr);
}

void GraphicsMeasureItem::endPoint(const QPointF &point)
{

}

void GraphicsMeasureItem::control(int dir, const QPointF &delta)
{

}

void GraphicsMeasureItem::stretch(int handle, double sx, double sy, const QPointF &origin)
{

}

void GraphicsMeasureItem::updateCoordinate()
{

}

bool GraphicsMeasureItem::loadFromSql(QSqlQuery *sql_query)
{

}

bool GraphicsMeasureItem::saveToSql(QSqlQuery *sql_query)
{

}

QGraphicsItem *GraphicsMeasureItem::duplicate() const
{

}

void GraphicsMeasureItem::updatehandles()
{

}

void GraphicsMeasureItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(pen());
    painter->drawPolyline(m_points);

    painter->save();
    painter->setBrush(QBrush(QColor(255,255,255)));

    QFont font;
    font.setPointSize(100);

    QFontMetrics metrics(font);
    painter->setFont(font);

    QPainterPath path;
    path.moveTo(m_points.at(0));
    for(int i = 0; i < m_points.count(); i++)
    {
        painter->drawEllipse(m_points[i],80,80);


        painter->save();
        QPen pen = QPen(QColor(230,54,54));
        pen.setWidth(8);
        painter->setPen(pen);
        painter->setOpacity(0.8);
        if(i > 0)
        {
            //QLineF line(m_points.at(i-1),m_points.at(i));
            //QString textNum = QString("%1mm").arg((int)line.length());
            path.lineTo(m_points.at(i));
            QString textNum = QString("%1mm").arg((int)path.length());
            painter->drawRect(metrics.boundingRect(textNum).translated(m_points[i].toPoint()+QPoint(100,200)));
            painter->drawText(m_points[i]+QPoint(100,200),QString("%1").arg(textNum));
        }
        painter->restore();


    }
    painter->restore();

}




GraphicsAreaItem::GraphicsAreaItem(QGraphicsItem *parent)
    :GraphicsPolygonItem(parent)
{
    m_handles.reserve(Left);
    for (int i = LeftTop; i <= Left; ++i) {
        SizeHandleRect *shr = new SizeHandleRect(this,i);
        m_handles.push_back(shr);
    }

    m_pen = QPen(QColor(30,144,255));
    m_pen.setWidth(2);
    setBrush(QBrush(QColor(181, 181, 181)));
    m_points.clear();
    //setZValue(1.0);
//    scale(1,-1);
    setTransform(QTransform::fromScale(1,-1),true);

    m_width = m_height = 0;
}

int GraphicsAreaItem::type() const
{
    return Type;
}

QRectF GraphicsAreaItem::boundingRect() const
{
    return shape().controlPointRect();
}

QPainterPath GraphicsAreaItem::shape() const
{
    QPainterPath path;
    path.addPolygon(m_points);
    path.closeSubpath();
    return qt_graphicsItem_shapeFromPath(path,pen());
}

void GraphicsAreaItem::addPoint(const QPointF &point)
{
    m_points.append(mapFromScene(point));
    int dir = m_points.count();
    SizeHandleRect *shr = new SizeHandleRect(this, dir+Left, true);
    shr->setState(SelectionHandleActive);
    m_handles.push_back(shr);
}

void GraphicsAreaItem::endPoint(const QPointF &point)
{
    Q_UNUSED(point);
    int nPoints = m_points.count();
    delete m_handles[Left + nPoints-1];
    m_points.remove(nPoints-1);
    m_handles.resize(Left + nPoints-1);
    m_initialPoints = m_points;
    updatehandles();
}

void GraphicsAreaItem::control(int dir, const QPointF &delta)
{
    QPointF pt = mapFromScene(delta);
    if ( dir <= Left ) return ;
    m_points[dir - Left -1] = pt;
    prepareGeometryChange();
    m_localRect = m_points.boundingRect();
    m_width = m_localRect.width();
    m_height = m_localRect.height();
    m_initialPoints = m_points;
    updatehandles();

}

void GraphicsAreaItem::stretch(int handle, double sx, double sy, const QPointF &origin)
{
    QTransform trans;
    switch (handle) {
    case Right:
    case Left:
        sy = 1;
        break;
    case Top:
    case Bottom:
        sx = 1;
        break;
    default:
        break;
    }
    trans.translate(origin.x(),origin.y());
    trans.scale(sx,sy);
    trans.translate(-origin.x(),-origin.y());

    prepareGeometryChange();
    m_points = trans.map(m_initialPoints);
    m_localRect = m_points.boundingRect();
    m_width = m_localRect.width();
    m_height = m_localRect.height();
    updatehandles();
}

void GraphicsAreaItem::updateCoordinate()
{
    QPointF pt1,pt2,delta;
    QPolygonF pts = mapToScene(m_points);
    if (parentItem()==NULL)
    {
        pt1 = mapToScene(transformOriginPoint());
        pt2 = mapToScene(boundingRect().center());
        delta = pt1 - pt2;

        for (int i = 0; i < pts.count() ; ++i )
            pts[i]+=delta;

        prepareGeometryChange();

        m_points = mapFromScene(pts);
        m_localRect = m_points.boundingRect();
        m_width = m_localRect.width();
        m_height = m_localRect.height();

        setTransform(transform().translate(delta.x(),delta.y()));
        //setTransformOriginPoint(boundingRect().center());
        moveBy(-delta.x(),-delta.y());
        setTransform(transform().translate(-delta.x(),-delta.y()));
        updatehandles();
    }
    m_initialPoints = m_points;
}

bool GraphicsAreaItem::loadFromPb(agv_map::Area area_pb)
{
    map_area_pb = area_pb;

    setPos(QPoint(map_area_pb.coord(0).x(),map_area_pb.coord(0).y()));

    for(int i = 0;  i < map_area_pb.coord_size(); i++)
    {
        addPoint(QPoint(map_area_pb.coord(i).x(),map_area_pb.coord(i).y()));
    }

    updatehandles();
    m_initialPoints = m_points;

    setState(SelectionHandleOff);

    return true;
}

bool GraphicsAreaItem::loadFromSql(QSqlQuery *sql_query)
{

    map_area.id = sql_query->value(0).toInt();
    map_area.type = sql_query->value(1).toInt();
    for(unsigned i = 0; i < sizeof(map_area.gateway)/sizeof(map_area.gateway[0]); i++)
    {
        map_area.gateway[i] = sql_query->value(2 + i).toInt();
    }
    map_area.point_sum = sql_query->value(12).toInt();
    for(unsigned i = 0; i < sizeof(map_area.point)/sizeof(map_area.point[0]); i++)
    {
        map_area.point[i].x = sql_query->value(13 + i*2).toFloat();
        map_area.point[i].y = sql_query->value(14 + i*2).toFloat();
    }

    setPos(QPoint(map_area.point[0].x,map_area.point[0].y));
    for(int i = 0;  i < map_area.point_sum; i++)
    {
        addPoint(QPoint(map_area.point[i].x,map_area.point[i].y));
    }

    updatehandles();
    m_initialPoints = m_points;

    setState(SelectionHandleOff);

    return true;
}

bool GraphicsAreaItem::saveToSql(QSqlQuery *sql_query)
{
    QString insert_point = QString("insert into CLUED (ID,TYPE, GATEWAY0, GATEWAY1, GATEWAY2, GATEWAY3, GATEWAY4,"
                                   "GATEWAY5, GATEWAY6, GATEWAY7, GATEWAY8, GATEWAY9, NUMBER, X1,"
                                   "Y1, X2, Y2, X3, Y3,X4, Y4, X5, Y5, X6, Y6, X7, Y7, X8, Y8, X9, Y9, X10, Y10,"
                                   "X11, Y11, X12, Y12, X13, Y13, X14, Y14, X15, Y15, X16, Y16, X17, Y17, X18, Y18,"
                                   "X19 , Y19, X20, Y20) "
                                   "values(%1,%2,%3,%4,%5,%6,%7,%8,%9,%10,%11,%12,%13,%14,%15,%16,%17,%18,%19,%20,"
                                   "%21,%22,%23,%24,%25,%26,%27,%28,%29,%30,%31,%32,%33,%34,%35,%36,%37,%38,%39,%40,"
                                   "%41,%42,%43,%44,%45,%46,%47,%48,%49,%50,%51,%52,%53);")
            .arg(map_area.id)
            .arg(map_area.type)
            .arg(map_area.gateway[0])
            .arg(map_area.gateway[1])
            .arg(map_area.gateway[2])
            .arg(map_area.gateway[3])
            .arg(map_area.gateway[4])
            .arg(map_area.gateway[5])
            .arg(map_area.gateway[6])
            .arg(map_area.gateway[7])
            .arg(map_area.gateway[8])
            .arg(map_area.gateway[9])
            .arg(map_area.point_sum)
            .arg(map_area.point[0].x).arg(map_area.point[0].y)
            .arg(map_area.point[1].x).arg(map_area.point[1].y)
            .arg(map_area.point[2].x).arg(map_area.point[2].y)
            .arg(map_area.point[3].x).arg(map_area.point[3].y)
            .arg(map_area.point[4].x).arg(map_area.point[4].y)
            .arg(map_area.point[5].x).arg(map_area.point[5].y)
            .arg(map_area.point[6].x).arg(map_area.point[6].y)
            .arg(map_area.point[7].x).arg(map_area.point[7].y)
            .arg(map_area.point[8].x).arg(map_area.point[8].y)
            .arg(map_area.point[9].x).arg(map_area.point[9].y)
            .arg(map_area.point[10].x).arg(map_area.point[10].y)
            .arg(map_area.point[11].x).arg(map_area.point[11].y)
            .arg(map_area.point[12].x).arg(map_area.point[12].y)
            .arg(map_area.point[13].x).arg(map_area.point[13].y)
            .arg(map_area.point[14].x).arg(map_area.point[14].y)
            .arg(map_area.point[15].x).arg(map_area.point[15].y)
            .arg(map_area.point[16].x).arg(map_area.point[16].y)
            .arg(map_area.point[17].x).arg(map_area.point[17].y)
            .arg(map_area.point[18].x).arg(map_area.point[18].y)
            .arg(map_area.point[19].x).arg(map_area.point[19].y)
            ;

//    qDebug() << insert_point;

    if(!sql_query->exec(insert_point))
    {
        qDebug()<<sql_query->lastError();
    }

    return true;
}

QGraphicsItem *GraphicsAreaItem::duplicate() const
{

}

void GraphicsAreaItem::updatehandles()
{
    const QRectF &geom = this->boundingRect();

    const Handles::iterator hend =  m_handles.end();
    for (Handles::iterator it = m_handles.begin(); it != hend; ++it) {
        SizeHandleRect *hndl = *it;
        switch (hndl->dir()) {
        case LeftTop:
            hndl->move(geom.x() , geom.y() );
            break;
        case Top:
            hndl->move(geom.x() + geom.width() / 2 , geom.y() );
            break;
        case RightTop:
            hndl->move(geom.x() + geom.width() , geom.y() );
            break;
        case Right:
            hndl->move(geom.x() + geom.width() , geom.y() + geom.height() / 2 );
            break;
        case RightBottom:
            hndl->move(geom.x() + geom.width() , geom.y() + geom.height() );
            break;
        case Bottom:
            hndl->move(geom.x() + geom.width() / 2 , geom.y() + geom.height() );
            break;
        case LeftBottom:
            hndl->move(geom.x(), geom.y() + geom.height());
            break;
        case Left:
            hndl->move(geom.x(), geom.y() + geom.height() / 2);
            break;
        default:
            break;
        }
    }

    for ( int i = 0 ; i < m_points.size() ; ++i ){
        m_handles[Left+i]->move(m_points[i].x() ,m_points[i].y() );
    }
}

QVariant GraphicsAreaItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if ( change == QGraphicsItem::ItemSelectedHasChanged ) {
        QGraphicsItemGroup *g = dynamic_cast<QGraphicsItemGroup*>(parentItem());
        if (!g)
            setState(value.toBool() ? SelectionHandleActive : SelectionHandleOff);
        else{
            setSelected(false);
            return QVariant::fromValue<bool>(false);
        }
    }
    /*
    else if (change == ItemPositionChange && scene()) {
        // value is the new position.
        QPointF newPos = value.toPointF();
        QRectF rect = scene()->sceneRect();
        if (!rect.contains(newPos)) {
            // Keep the item inside the scene rect.
            newPos.setX(qMin(rect.right()-boundingRect().width()/2, qMax(newPos.x(), rect.left()+boundingRect().width()/2)));
            newPos.setY(qMin(rect.bottom()-boundingRect().height()/2, qMax(newPos.y(), rect.top()+boundingRect().height()/2)));
            return newPos;
        }
    }
    */
    return QGraphicsItem::itemChange(change, value);
}

void GraphicsAreaItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (option->state & QStyle::State_Selected)
    {
        qt_graphicsItem_highlightSelected(this, painter, option);
        setBrush(QBrush(QColor(151, 255, 255)));
    }
    else
    {
        if(find == false)
        {
            setBrush(QBrush(QColor(181, 181, 181)));
        }
        else
        {
            setBrush(QBrush(QColor(255,165,255)));
        }
    }

    map_area.point_sum = m_points.size();
    for(unsigned i = 0; i < sizeof(map_area.point)/sizeof(map_area.point[0]); i++)
    {
        if(i < map_area.point_sum)
        {
            map_area.point[i].x = mapToScene(m_points[i]).x();
            map_area.point[i].y = mapToScene(m_points[i]).y();
        }
        else
        {
            map_area.point[i].x = 0;
            map_area.point[i].y = 0;
        }
    }

    painter->setBrush(brush());
    painter->setPen(pen());
    painter->setOpacity(0.2);
    painter->drawPolygon(m_points);

}

void GraphicsAreaItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::RightButton)
    {
        qDebug()<<"避让区右键";
    }
}
