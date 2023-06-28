#ifndef DRAWTOOL_H
#define DRAWTOOL_H

#include "drawobj.h"
#include "drawscene.h"
#include "driftinput.h"
#include "driftinputradius.h"


QT_BEGIN_NAMESPACE
class QGraphicsScene;
class QGraphicsSceneMouseEvent;
class QMenu;
class QPointF;
class QGraphicsLineItem;
class QGraphicsRectItem;
class QFont;
class QGraphicsTextItem;
class QColor;
class QAbstractGraphicsShapeItem;
QT_END_NAMESPACE

class DrawScene;
class GraphicsItem;
class GraphicsPolygonItem;
enum DrawShape
{
    selection ,
    point ,
    line ,
    arc_startEndRadius,
    arc_directRadiusAngle,
    polyline,
    polygon,
    bezier,
    rotation,

    interrput,
    merge,
    measure,
};


class DrawTool : public QObject
{
    Q_OBJECT
public:
    DrawTool(DrawShape shape);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * event , DrawScene * scene ) ;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * event , DrawScene * scene ) ;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * event , DrawScene * scene );
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event ,DrawScene *scene );

    virtual void keyPressEvent(QKeyEvent * event ,DrawScene *scene );
    virtual void keyReleaseEvent(QKeyEvent * event ,DrawScene *scene );

    virtual void movingDriftWidget(QGraphicsSceneMouseEvent * event, DrawScene *scene);
    void resetDriftWidget();

    int findSegmentId(DrawScene *scene);
    int findPointId(DrawScene *scene);

    DrawShape m_drawShape;
    bool m_hoverSizer;

    static DrawTool * findTool( DrawShape drawShape );
    static QList<DrawTool*> c_tools;
    static QPoint c_down;
    static quint32 c_nDownFlags;
    static QPoint c_last;
    static DrawShape c_drawShape;

    int    mouse_pressed_count;

    //输入坐标悬浮框
    bool inited_drift = false;
    bool inited_drift_one_param = false;
    DriftInput *driftInput = 0;
    QGraphicsRectItem * driftItem = 0;

    DriftInputRadius *driftInputRadius = 0;
    QGraphicsRectItem *driftItemRadius = 0;
    DrawScene *curScene;

};

class SelectTool : public DrawTool
{
public:
    SelectTool();
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene) override;
    QPointF initialPositions;
    QPointF opposite_;
    QGraphicsPathItem * dashRect;
};

class RotationTool : public DrawTool
{
public:
    RotationTool();
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene) override;
    qreal lastAngle;
    QGraphicsPathItem * dashRect;
};

class PointTool : public DrawTool
{
    Q_OBJECT
public:
    PointTool(DrawShape drawShape);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene) override;
    GraphicsPointItem * item;
    int last_point_id;
    int point_id;


    void addPoint(QPointF pos,DrawScene *curScene);
    void addPoint(int x, int y,DrawScene *curScene);

public slots:
    void addPointByXY(int x, int y);

    void updateOneParamByDriftInput(int param);

};

class LineTool : public DrawTool
{
    Q_OBJECT
public:
    LineTool(DrawShape shape);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene) override;
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene) override;

    void resetVariable();

    void addPointByXY(int x, int y, DrawScene *scene);

    GraphicsLineItem *line_item;
    int m_nPoints;
    QPointF initialPositions;

    QLineF linef;

    QGraphicsPathItem * dashLine;

public slots:
    void addPointByXY(int x, int y);
    void updateOneParamByDriftInput(int param);
};

class ArcTool_startEndRadius : public DrawTool
{
    Q_OBJECT
public:
    ArcTool_startEndRadius(DrawShape shape);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene) override;
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene) override;

    void resetVariable();

    GraphicsArcItem *arc_item;
    int m_nPoints;
    QPointF initialPositions;

    QLineF linef;

    QGraphicsPathItem * dashArc;
    QGraphicsPathItem * dashLine;

public slots:
    void addPointByXY(int x, int y);
    void updateOneParamByDriftInput(int param);
};

class ArcTool_directRadiusAngle : public DrawTool
{
    Q_OBJECT
public:
    ArcTool_directRadiusAngle(DrawShape shape);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene) override;
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene) override;

    virtual void movingDriftWidget(QGraphicsSceneMouseEvent *event, DrawScene *scene) override;

    void addOneArcToScene(DrawScene *scene);

    void resetVariable();

    GraphicsArcItem *arc_item;
    int m_nPoints;
    QPointF initialPositions;

    QLineF linef;

    QLineF selected_linef;
    QPoint selected_point;
    QPoint cen;
    int inputRadius = 0;
    float inputAngle = 0;
    qreal sweepLength = 0;

    QGraphicsPathItem * dashArc;
    QGraphicsPathItem * dashLine;

    QPainterPath path;
    QLineF unitLine;
    QLineF normalLine;

public slots:
    void addPointByXY(int x, int y);
    void updateOneParamByDriftInput(int param);
};

class InterrputTool : public DrawTool
{
    Q_OBJECT
public:
    InterrputTool(DrawShape shape);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene) override;
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene) override;

    virtual void movingDriftWidget(QGraphicsSceneMouseEvent *event, DrawScene *scene) override;

    void resetVariable();

    GraphicsItem *selected_item;
    int m_nPoints;
    QPointF initialPositions;

    QLineF linef;

    QLineF selected_linef;
    QPoint selected_point;

    QPoint cur_point;

    QGraphicsPathItem * dashPoint;

    QPainterPath path;


public slots:
    void addPointByXY(int x, int y);
};

class MergeTool : public DrawTool
{
    Q_OBJECT
public:
    MergeTool(DrawShape shape);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene) override;
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene) override;

    virtual void movingDriftWidget(QGraphicsSceneMouseEvent *event, DrawScene *scene) override;

    void resetVariable();

    GraphicsItem *selected_item;

    QList<GraphicsLineItem *> selected_Lineitems;

    int selected_count = 0;

    QPointF initialPositions;

    QLineF linef;

    QLineF selected_linef;

    QPoint cur_point;

    QPainterPath path;

};

class MeasureTool : public DrawTool
{
public:
    MeasureTool(DrawShape shape );
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * event , DrawScene * scene ) ;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * event , DrawScene * scene ) ;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * event , DrawScene * scene );
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event ,DrawScene *scene );
    virtual void keyPressEvent(QKeyEvent * event ,DrawScene *scene );
    virtual void keyReleaseEvent(QKeyEvent * event ,DrawScene *scene );

    void resetVariable(DrawScene *scene);
    void resetVariable();
    GraphicsMeasureItem * measure_item;
    int m_nPoints = 0;
    QPointF initialPositions;

};

#endif // DRAWTOOL_H
