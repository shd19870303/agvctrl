#ifndef DRAWOBJ_H
#define DRAWOBJ_H

#include <QGraphicsItem>
#include <QtCore/QObject>
#include <QGraphicsSceneMouseEvent>
#include <QVector>
#include <QGraphicsScene>
#include <QList>
#include <QCursor>
#include <vector>
#include <QMimeData>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

#include <QPicture>
#include "protoFile/agv_map.pb.h"
#include "map.h"
#include "sizehandle.h"


#include <QtMath>

enum SegmentType
{
    LINE = 0,
    ARC,
};

QPainterPath qt_graphicsItem_shapeFromPath(const QPainterPath &path, const QPen &pen);

#define MAXCOM_F(a, b) ((a)-(b)>0.00001) ? true : false

class ShapeMimeData : public QMimeData
{
    Q_OBJECT
public:
    ShapeMimeData(QList<QGraphicsItem *> items);
    ~ShapeMimeData();
    QList<QGraphicsItem *> items() const;
private:
    QList<QGraphicsItem *> m_items;
};


class GraphicsItem :public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    explicit GraphicsItem(QGraphicsItem * parent );
    ~GraphicsItem();

    //virtual ~GraphicsItem(){}
    virtual QString displayName() const { return QString("GraphicsItem");}
    virtual void control(int dir, const QPointF & delta) {Q_UNUSED(dir) Q_UNUSED(delta)}
    virtual void stretch(int, double, double, const QPointF){}
    virtual QRectF rect() const {return m_localRect;}
    virtual void updateCoordinate() {}
    virtual void move(const QPointF & point ) {Q_UNUSED(point)}
    virtual QGraphicsItem * duplicate() const {return nullptr;}
    virtual bool loadFromSql(QSqlQuery *sql_query) = 0;
    virtual bool saveToSql(QSqlQuery *sql_query) = 0;

    QColor brushColor() const {return m_brush.color();}
    QBrush brush() const {return m_brush;}
    QPen   pen() const {return m_pen;}
    QColor penColor() const {return m_pen.color();}\

    void setPen(const QPen &pen) {m_pen = pen;}
    void setBrush(const QBrush & brush) {m_brush = brush;}
    void setBrushColor(const QColor &color) {m_brush.setColor(color);}
    qreal width() const {return m_width;}
    void setWidth(qreal width)
    {
        m_width = width ;
        updateCoordinate();
    }
    qreal  height() const {return m_height;}
    void   setHeight ( qreal height )
    {
        m_height = height ;
        updateCoordinate();
    }


    enum { Type = UserType + 1 };
    virtual QPixmap image();

signals:
    void selectedChange(QGraphicsItem * item);

protected:
    QBrush m_brush;
    QPen   m_pen;

    qreal m_width;
    qreal m_height;
public:
    QRectF m_localRect;
protected:
    QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value);
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
};


class GraphicsPointItem : public GraphicsItem
{
    Q_OBJECT
public:
    GraphicsPointItem(const QRect & rect, bool isRound = false, QGraphicsItem * parent = nullptr);
    enum { Type = UserType + 2 };
    int type() const ;
    QRectF boundingRect() const;
    QPainterPath shape() const;
    void control(int dir, const QPointF & delta);
    void stretch(int handle, double sx, double sy, const QPointF & origin);
    QRectF rect() const {  return m_localRect;}

    void updateCoordinate();
    void move(const QPointF & point);
    QGraphicsItem * duplicate() const;

    QString displayName() const { return tr("point");}

    bool loadFromPb(agv_map::Point point_pb);

    virtual bool loadFromSql(QSqlQuery *sql_query);
    virtual bool saveToSql(QSqlQuery *sql_query);

protected:
    //void updatehandles();
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    bool m_isRound;
    qreal m_fRatioY;
    qreal m_fRatioX;
    QRectF m_initialRect;
    QPointF opposite_;
    QPointF m_originPoint;
public:
    MapPoint map_point;

    agv_map::Point map_point_pb;

    QPixmap pix_store;
    QPixmap pix_back_stop_charge;
    QPixmap pix_charge;
    QPixmap pix_door;
    QPixmap pix2;

    QRectF target;
    QRectF storeRect;

    qreal angle;
    int  clock_dir;
    int  store_state = 0;
    int occupy_point_flag = -1;
    int scan_point_flag = -1;
    int zoomin_flag = -1;
    int point_font_flag = 0;
    QFont point_font;
//    int have_used_flag = 0;  //0：未使用     1：已使用
//    QImage  image_PickUp_Chu;
//    QImage  image_PickUp_Ru;
//    QImage  image_PickDown_Chu;
//    QImage  image_PickDown_Ru;
//    QImage  image_Standby_Chu;
//    QImage  image_Standby_Ru;

//    QPicture picture_PickUp_Chu;
//    QPicture picture_PickUp_Ru;
//    QPicture picture_PickDown_Chu;
//    QPicture picture_PickDown_Ru;
//    QPicture picture_Standby_Chu;
//    QPicture picture_Standby_Ru;

    QPixmap   pixmap_PickUp_Chu;
    QPixmap   pixmap_PickUp_Ru;
    QPixmap   pixmap_PickDown_Chu;
    QPixmap   pixmap_PickDown_Ru;
    QPixmap   pixmap_Standby_Chu;
    QPixmap   pixmap_Standby_Ru;

    QFont storeFont;
    QFont taskFont;

    int taskNum = 0;

    bool isEndpoint = false;    //
signals:
    void sendPointID(int);
};





class GraphicsPolygonItem : public GraphicsItem
{
public:
    GraphicsPolygonItem(QGraphicsItem *parent = nullptr);

    enum { Type = UserType + 3 };
    virtual int type() const;
    QRectF boundingRect() const;
    QPainterPath shape() const;
    virtual void addPoint(const QPointF & point);
    virtual void endPoint(const QPointF & point);
    void control(int dir, const QPointF & delta);
    void stretch(int handle, double sx, double sy, const QPointF & origin);
    void updateCoordinate();
    virtual bool loadFromSql(QSqlQuery * sql_query );
    virtual bool saveToSql( QSqlQuery * sql_query );
    QString displayName() const { return tr("polygon"); }
    QGraphicsItem *duplicate() const;
protected:
    void updatehandles();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
public:
    QPolygonF m_points;
    QPolygonF m_initialPoints;

};


class GraphicsLineItem : public GraphicsPolygonItem
{
public:
    GraphicsLineItem(QGraphicsItem * parent = nullptr);
    enum { Type = UserType + 4 };
    int type() const;
    QPainterPath shape() const ;
    QGraphicsItem * duplicate() const ;
    void addPoint(const QPointF &point);
    void endPoint(const QPointF &point);
    virtual QPointF opposite(int handle);
    void updateCoordinate() { m_initialPoints = m_points;}
    void stretch(int handle, double sx, double sy, const QPointF & origin);


    bool loadFromPb(agv_map::Segment segment_pb) ;

    virtual bool loadFromSql(QSqlQuery *sql_query) ;
    virtual bool saveToSql(QSqlQuery *sql_query) ;

    QString displayName() const {return tr("line");}

protected:
    void updatehandles();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) ;

public:
    MapSegment map_segment;
    agv_map::Segment map_segment_pb;

    QLineF linef;
    bool interrput_merge_tag = false;   //打断时显示虚线
    int route_flag = 0;
    int custom_line_width = 10;
    float custom_n = 35;
    float custom_l = 200;
    QColor color;
    QColor routeColor= QColor(0,255,0);
};

class GraphicsArcItem : public GraphicsPolygonItem
{
public:
    GraphicsArcItem(QGraphicsItem * parent = nullptr);

    enum { Type = UserType + 5 };
    int type() const;
    void get_arc_path(float radius,int loading);
    QPainterPath shape() const;
    QGraphicsItem * duplicate() const ;
    void addPoint(const QPointF &point);
    void endPoint(const QPointF &point);
    virtual QPointF opposite(int handle);
    void updateCoordinate() { m_initialPoints = m_points;}
    void stretch(int handle, double sx, double sy, const QPointF & origin);

    bool loadFromPb(agv_map::Segment segment_pb) ;

    virtual bool loadFromSql(QSqlQuery *sql_query) ;
    virtual bool saveToSql(QSqlQuery *sql_query) ;

    QString displayName() const {return tr("arc");}

    QVector<QPointF> getEllipseCoors(const QPointF &r1, const QPointF &r2, float r);
    qreal caculateCrossLengthPtToLine(const QPointF & pt1,const  QPointF & pt2,  QPointF pt3);

protected:
    void updatehandles();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) ;

public:
    QPainterPath arc_path;
    QPointF cur_point;
    qreal R,startAngle,sweepLength,Radius;
    QPointF cen;
    QRectF rectf;
    MapSegment map_segment;
    agv_map::Segment map_segment_pb;


    QLineF linef;
    bool interrput_merge_tag = false;
    int route_flag = 0;
    int custom_line_width = 5;
    float custom_n = 35;
    float custom_l = 200;
    QColor color;
    QColor routeColor= QColor(0,255,0);
};

class GraphicsRgvItem : public GraphicsItem
{
public:
    GraphicsRgvItem(QGraphicsItem * parent = nullptr);
    ~GraphicsRgvItem();
    enum { Type = UserType + 6 };
    virtual int type() const;
    QRectF boundingRect() const;

    virtual bool loadFromSql(QSqlQuery *sql_query) ;
    virtual bool saveToSql(QSqlQuery *sql_query) ;

    void move(const QPointF & point);

    QString displayName() const { return tr("Rgv"); }
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
public:
    RgvInfo rgv_info;
    QPointF position;
    qreal   angle;
    char    rgv_state = 0;

    QPixmap pixmap_Default;
    QPixmap pixmap_RgvOffline;
    QPixmap pixmap_RgvOffline_load;
    QPixmap pixmap_RgvManual;
    QPixmap pixmap_RgvManual_load;
    QPixmap pixmap_RgvWarning;
    QPixmap pixmap_RgvWarning_load;
    QPixmap pixmap_RgvMoving;
    QPixmap pixmap_RgvMoving_load;
    QPixmap pixmap_RgvPickDown;
    QPixmap pixmap_RgvPickDown_load;
    QPixmap pixmap_RgvPickUp;
    QPixmap pixmap_RgvPickUp_load;
    QPixmap pixmap_RgvStandby;
    QPixmap pixmap_RgvStandby_load;

};

class GraphicsMeasureItem : public GraphicsPolygonItem
{
public:
    GraphicsMeasureItem(QGraphicsItem *parent = nullptr);

    enum { Type = UserType + 7 };
    virtual int type() const;
    QRectF boundingRect() const;
    QPainterPath shape() const;
    virtual void addPoint(const QPointF & point);
    virtual void endPoint(const QPointF & point);
    void control(int dir, const QPointF & delta);
    void stretch(int handle, double sx, double sy, const QPointF & origin);
    void updateCoordinate();
    virtual bool loadFromSql(QSqlQuery * sql_query );
    virtual bool saveToSql( QSqlQuery * sql_query );
    QString displayName() const { return tr("measure"); }
    QGraphicsItem *duplicate() const;
protected:
    void updatehandles();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

};

class GraphicsAreaItem : public GraphicsPolygonItem
{
public:
    GraphicsAreaItem(QGraphicsItem *parent = nullptr);

    enum { Type = UserType + 8 };
    virtual int type() const;
    QRectF boundingRect() const;
    QPainterPath shape() const;
    virtual void addPoint(const QPointF & point);
    virtual void endPoint(const QPointF & point);
    void control(int dir, const QPointF & delta);
    void stretch(int handle, double sx, double sy, const QPointF & origin);
    void updateCoordinate();
    bool loadFromPb(agv_map::Area area_pb );

    virtual bool loadFromSql(QSqlQuery * sql_query );
    virtual bool saveToSql( QSqlQuery * sql_query );
    QString displayName() const { return tr("Area"); }
    QGraphicsItem *duplicate() const;
    virtual int handleCount() const { return m_handles.size();}
    int collidesWithHandle( const QPointF & point ) const
    {
        const Handles::const_reverse_iterator hend =  m_handles.rend();
        for (Handles::const_reverse_iterator it = m_handles.rbegin(); it != hend; ++it)
        {
            QPointF pt = (*it)->mapFromScene(point);
            if ((*it)->contains(pt) ){
                return (*it)->dir();
            }
        }
        return Handle_None;
    }
    virtual QPointF handlePos( int handle ) const
    {
        const Handles::const_reverse_iterator hend =  m_handles.rend();
        for (Handles::const_reverse_iterator it = m_handles.rbegin(); it != hend; ++it)
        {
            if ((*it)->dir() == handle ){
                return (*it)->pos();
            }
        }
        return QPointF();
    }
    int swapHandle(int handle, const QPointF& scale ) const
    {
        int dir = Handle_None;
        if ( scale.x() < 0 || scale.y() < 0 ){
            switch (handle) {
            case RightTop:
                if ( scale.x() < 0 && scale.y() < 0 )
                    dir = LeftBottom;
                else if ( scale.x() > 0 && scale.y() < 0 )
                    dir = RightBottom;
                else
                    dir = LeftTop;
                break;
            case RightBottom:
                if ( scale.x() < 0 && scale.y() < 0 )
                    dir = LeftTop;
                else if ( scale.x() > 0 && scale.y() < 0 )
                    dir = RightTop;
                else
                    dir = LeftBottom;
                break;
            case LeftBottom:
                if ( scale.x() < 0 && scale.y() < 0 )
                    dir = RightTop;
                else if ( scale.x() > 0 && scale.y() < 0 )
                    dir = LeftTop;
                else
                    dir = RightBottom;
                break;
            case LeftTop:
                if ( scale.x() < 0 && scale.y() < 0 )
                    dir = RightBottom;
                else if ( scale.x() > 0 && scale.y() < 0 )
                    dir = LeftBottom;
                else
                    dir = RightTop;
                break;
            case Right:
                if (scale.x() < 0 )
                    dir = Left;
                break;
            case Left:
                if (scale.x() < 0 )
                    dir = Right;
                break;
            case Top:
                if (scale.y()<0)
                    dir = Bottom;
                break;
            case Bottom:
                if (scale.y()<0)
                   dir = Top;
                break;
            }
        }
        return dir;
    }

    virtual QPointF opposite( int handle ) {
        QPointF pt;
        switch (handle) {
        case Right:
            pt = m_handles.at(Left-1)->pos();
            break;
        case RightTop:
            pt = m_handles[LeftBottom-1]->pos();
            break;
        case RightBottom:
            pt = m_handles[LeftTop-1]->pos();
            break;
        case LeftBottom:
            pt = m_handles[RightTop-1]->pos();
            break;
        case Bottom:
            pt = m_handles[Top-1]->pos();
            break;
        case LeftTop:
            pt = m_handles[RightBottom-1]->pos();
            break;
        case Left:
            pt = m_handles[Right-1]->pos();
            break;
        case Top:
            pt = m_handles[Bottom-1]->pos();
            break;
         }
        return pt;
    }

    typedef std::vector<SizeHandleRect*> Handles;
    Handles m_handles;

    MapArea map_area;
    agv_map::Area map_area_pb;
    bool find = false;    // 被查找后高亮显示

protected:
    void updatehandles();
    QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value);
    void setState(SelectionHandleState st)
    {
        const Handles::iterator hend =  m_handles.end();
        for (Handles::iterator it = m_handles.begin(); it != hend; ++it)
            (*it)->setState(st);
    }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
};
#endif // DRAWOBJ_H
