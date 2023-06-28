#ifndef DRAWVIEW_H
#define DRAWVIEW_H
#include <QGraphicsView>
#include "rulebar.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include "protoFile/agv_map.pb.h"
#include "drawobj.h"
#include <QSqlTableModel>
//#include "view.h"

//class QMouseEvent;

#pragma pack(push, 1)

typedef struct SendPack{
    char STX1 = 0xA9;
    char STX2 = 0xAA;
    char area = 0;
    char ETX1 = 0xB9;
    char ETX2 = 0xBA;
};

typedef struct RecvPack{
    char STX1;
    char STX2;
    char snum;   //站点数量
    char anum;   //弧线数量
    char unused;
    char clock;
    int  len;
};

typedef struct StationPackage{
    char area;
    char unused;
    uint16_t sNo;
    char locality;
    char direction;
    char type;
    char unused2;
    int32_t stationpos;
};
typedef struct ArcPackage{
    int startMileage;
    int endMileage;
    int angle;
    char clock;
    char unused;
};

#pragma pack(pop)

class DrawView : public QGraphicsView
{
    Q_OBJECT
public:
    DrawView(QWidget *parent);
    bool readsizefromSQL = true;
    void zoomIn();
    void zoomOut();
    void self_adaption();
    void clockwise();
    void anti_clockwise();

    void newFile();
    bool loadFile(const QString &fileName);
    bool save();
    bool saveAs();
    bool saveFile(const QString &fileName);
    QString userFriendlyCurrentFile();
    QString currentFile() { return curFile; }
    void setModified( bool value ) { modified = value ; }
    bool isModified() const { return modified; }

    void find(int type, QString str_id);
    void selfCheckMap();

    bool maybeSave();

    void updateAssistPoint();
    QList<MapPoint> createPoint(QVector<MapPoint> vecPoint, QVector<MapSegment> vecSegment);

    void clearMeasureLine();

    void moveItems(int dx, int dy);
    void copyItems(int dx, int dy);

    void rotateItems(int center_x, int center_y, float angle);
    void scene_add_AreaItem();
    void scene_remove_AreaItem();
    void ParameterSave(QString Type, QString p);

signals:
    void positionChanged(int x , int y );
    void send_area_value(double);
    void send_seg_maxID(int);
    void send_point_maxID(int);
    void send_area_maxID(int);
    void send_point_xy(float,float);

    void actionListUpdateSignal(QMap<int, agv_map::Fpoint> mapAction);

protected:
//    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

    void mouseMoveEvent(QMouseEvent * event) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent *event) override;
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    void scrollContentsBy(int dx, int dy) Q_DECL_OVERRIDE;
    void updateRuler();
    QtRuleBar *m_hruler;
    QtRuleBar *m_vruler;
    QtCornerBox * box;

private:

    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);
//    void loadCanvas( QSqlQuery *sql_query,QSqlDatabase *db);
    void loadCanvas( agv_map::MAP_ARR Map_Arr_pb );

    qreal convertAngle(qreal srcAngle);

    bool isUntitled;
    bool modified;

    QPoint m_lastMousePos;


    QSqlTableModel *db_config_model;


public:
    QString curFile;

    QPainterPath path;
    QVector<MapPoint> vecPoint;
    QVector<MapSegment> vecSegment;
    QVector<MapArea> vecArea;

    QVector<MapPoint> vecPointBackup;
    QVector<MapSegment> vecSegmentBackup;
    QVector<MapAction> vecAction;
    QVector<MapArea> vecAreaBackup;

    QVector<GraphicsAreaItem*> vec_AreaItem;

    QMap<int, Map_Config>  mapConfig;

    agv_map::MAP_ARR mapArr_pb;

    QMap<int, QMap<int, GraphicsPointItem *> > map_mapPointItem;
    QMap<int, QMap<int, GraphicsLineItem *> > map_mapLineItem;
    QMap<int, QMap<int, GraphicsArcItem *> >  map_mapArcItem;
    QMap<int, QMap<int, GraphicsAreaItem *> > map_mapAreaItem;
    QMap<int, QMap<int, agv_map::Fpoint> > map_mapAction;

    int cur_map_index = 1;
    QVector<int> vecMapIndex;
    //QLabel *label_test;

//    Map_Config  mapConfig;  旧
    int area_flag = 0;
};

#endif // DRAWVIEW_H
