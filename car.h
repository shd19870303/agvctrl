#ifndef CAR_H
#define CAR_H

#include <QGraphicsItem>
#include <QPixmap>
#include <QPolygonF>
#include <QFont>
class Floating_Dialog;
class Car : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    explicit Car(qreal x ,qreal y ,int flag);
    int get_id();
    int get_dev_type();
    qreal get_pos_x();
    qreal get_pos_y();
    void set_id(int);
    void set_xy(qreal,qreal);
    void set_f_dialog_xy(int x, int y);
    void set_f_dialog_info(int dev_id, int device_type, int mission_id, int mission_type, int speed, int state, int status, int error_type, int battery, int timestamp);
    void set_polygon_points(int polygon_num, int type, QPointF point1, QPointF point2, QPointF point3, QPointF point4);
    void set_car_values(unsigned int dev_id,
            unsigned char dev_type,
            int mission_id,
            char mission_type,
            float pos_x,
            float pos_y,
            float pos_ang,
            char dir,
            float speed,
            char state,
            char status,
            int err_type,
            float battery,
            int loadnum,
            int cur_segment,
            int nxt_segment,
            unsigned int ip,
            long long timestamp,
            int online,
            int start_id,
            int end_id,
            float hyd_height,
            char protection1,
            char protection2,
            char protection3,
            char softstop, int idle_wait_ts, int avoid_set_ts, int manual_on_ts, int softstop_on_ts, int warning_on_ts, int stopStatus, int stopLevel, int stopByAGVId, int stopReason, int speedLevel, int speedByAGVId ,int confPoint);
    int flag601 = 601;
    int flag602 = 602;
    int port9103 = 9103;
    int port9104 = 9104;
    QFont font;
    int f_show_flag = 0;
    int hasAddToScene_flag = 0;
    int color_flag;
    bool protoHasRoute = false;
    QString errorStr = "";
    QRectF carRectF;
    int customCarL = -1;
    int customCarW = -1;
    int transparency = 255;
signals:
    void car_id(int,int,int);
    void get_f_d_xy();
    void showRoute(int);
//    void f_dialog_show();
//    void f_dialog_hide();
    void floatwindow_show(unsigned int dev_id, unsigned char dev_type, int mission_id, char mission_type, float pos_x, float pos_y, float pos_ang, char dir, float speed, char state, char status, int err_type, float battery, int loadnum, int cur_segment, int nxt_segment, unsigned int ip, long long timestamp, int online, int start_id, int end_id, float hyd_height, char protection1, char protection2, char protection3, char softstop, int idle_wait_ts, int avoid_set_ts, int manual_on_ts, int softstop_on_ts, int warning_on_ts, int stopStatus, int stopLevel, int stopByAGVId, int stopReason, int speedLevel, int speedByAGVId, QString errorStr, int confPoint, int mouse_x, int mouse_y);
    void floatwindow_hide();
protected:
    QRectF boundingRect() const ;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
//    bool eventFilter(QObject *watched, QEvent *event);
private:
    QPixmap pix;
//    QImage pix;
    QPixmap pixmapToShow;
    unsigned int dev_id;//AGV ID
    unsigned char dev_type;//device type
    int mission_id;//current mission ID
    char mission_type;//0:no goods    1:get goods   2:put goods
    float pos_x;//positon x
    float pos_y;//position y
    float pos_ang;//positon angle
    char dir;//moving direction
    float speed;//current speed
    char state;//0:offline   1:auto   2:warning   3:error   4:manual
    char status;//main status
    int err_type;//error type;
    float battery;//battery capacity
    int loadnum;//load number
    int cur_segment;//current segment
    int nxt_segment;//next segment
    unsigned int ip;//ip address
    long long timestamp;//update timestamp
    int online;//-2:deleted   -1:init   0:offline  1:online
    int start_id;//mission start id
    int end_id;//mission end id
    float hyd_height;//hydraulic height
    char protection1;//safety protection area status                front safety protection
    char protection2;//safety protection area status                back safety protection
    char protection3;//infrared detection status                        infrared detection
    char softstop;//1:age on soft status mode
    int idle_wait_ts;
    int avoid_set_ts;
    int manual_on_ts;
    int softstop_on_ts;
    int warning_on_ts;
    int stopStatus;
    int stopLevel;
    int stopByAGVId;
    int stopReason;
    int speedLevel;
    int speedByAGVId;
    int confPoint;
    qreal x;
    qreal y;
    Floating_Dialog *f_dialog;
    int f_dialog_x;
    int f_dialog_y;

    int polygon_num;
    QPolygonF polygon1;
    QPolygonF polygon2;
    QPolygonF polygon3;
//    int f_dialog_is_Show = 0;//0:悬浮框未显示  1:悬浮框已显示
};

#endif // CAR_H
