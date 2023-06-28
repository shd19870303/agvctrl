#ifndef SUBWIDGET_H
#define SUBWIDGET_H

#include <QWidget>
#include "drawscene.h"
#include "ui_subwidget.h"
#include <QHostAddress>
#include "qtvariantproperty.h"
#include <qtpropertymanager.h>
#include <qteditorfactory.h>
#include "playback_tool.h"
#include "agvctrl_tool.h"
#include "batteryinfo_widget.h"
#include "taskdialog.h"
#include <QGraphicsRectItem>
#include "protoFile/agv_mon.pb.h"
#include "protoFile/batteryMonitor.pb.h"
#include "taskspecialdialog.h"
#include "carsizewidget.h"
#include "cxjdatadialog.h"
#include "cxjbackdialog.h"
#include "station_backgrounditem.h"
#include <QFont>
class Car;
class Car_Info;
class GraphicsLineItem;
class GraphicsArcItem;
class GraphicsPointItem;
class Floating_Dialog;
class QTcpSocket;
class QUdpSocket;
class QSqlRecord;
class PlayBack_Tool;
class QMovie;
class Wait_Dialog;
class FloatingWindow;
class QGraphicsRectItem;
class QGraphicsProxyWidget;
class Float_RectItem;
class SA_BackgroundItem;
class MyDebugWidget;
//class QGraphicsRectItem;

#pragma pack(push, 1)
typedef struct
{
    char LOGO1;
    char LOGO2;
    char LOGO3;
    char LOGO4;
    char LOGO5;
    char LOGO6;
    char FLAG1;
    char FLAG2;
    char FLAG3;
    int port;
    char area;
}Send_Mon_Tcp;


typedef struct
{
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
    float outline1_x;
    float outline1_y;
    float outline2_x;
    float outline2_y;
    float outline3_x;
    float outline3_y;
    float outline4_x;
    float outline4_y;
    float outline5_x;
    float outline5_y;
    float outline6_x;
    float outline6_y;
    float outline7_x;
    float outline7_y;
    float outline8_x;
    float outline8_y;
    float outline9_x;
    float outline9_y;
    float outline10_x;
    float outline10_y;
    float outline11_x;
    float outline11_y;
    float outline12_x;
    float outline12_y;
    int outline_type;//outline type
    int seen_reflector;//seen reflector
    int use_reflector;//use reflector
}Agv_Data;

typedef struct
{
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
    //body outline  4
    float outline1_x;
    float outline1_y;
    float outline2_x;
    float outline2_y;
    float outline3_x;
    float outline3_y;
    float outline4_x;
    float outline4_y;

    //good outline  4
    float outline5_x;
    float outline5_y;
    float outline6_x;
    float outline6_y;
    float outline7_x;
    float outline7_y;
    float outline8_x;
    float outline8_y;

    short int occupy_pt[20];
    short int scan_pt[20];

    int seen_reflector;//seen reflector
    int use_reflector;//use reflector
}Agv_Data2;

typedef struct
{
    char LOGO1;
    char LOGO2;
    char LOGO3;
    char LOGO4;
    char LOGO5;
    char LOGO6;
    int FLAG;
    unsigned char agv_num;//车辆数目
}Recv_Mon_Udp_FLAG;

typedef struct
{
    char LOGO1;
    char LOGO2;
    char LOGO3;
    char LOGO4;
    char LOGO5;
    char LOGO6;
    int FLAG;
    unsigned char agv_num;//车辆数目
    Agv_Data agv_data[100];

}Recv_Mon_Udp;

typedef struct
{
    char LOGO1;
    char LOGO2;
    char LOGO3;
    char LOGO4;
    char LOGO5;
    char LOGO6;
    int FLAG;
    unsigned char agv_num;//车辆数目
    Agv_Data2 agv_data[100];

}Recv_Mon_Udp2;

//typedef struct
//{
//    char STX1;
//    char STX2;
//    int AGVID;
//}Send_Route_Tcp;
typedef struct
{
    char LOGO1;
    char LOGO2;
    char LOGO3;
    char LOGO4;
    char LOGO5;
    char LOGO6;
    int FLAG;
    int AGVID;
}Send_Route_Tcp;
typedef struct
{
    char LOGO1;
    char LOGO2;
    char LOGO3;
    char LOGO4;
    char LOGO5;
    char LOGO6;
    int FLAG;
    int route_num;
}Recv_Route_Tcp;
//    QVector<int> route_list;
//    int cur_id;
//    int nxt_id;

typedef struct
{
    int err_type;
    QString err_string_fork;
    QString err_string_lurk;
    QString err_string_diff;
}Errorinfo;

typedef struct
{
    int point_id;
    int color_flag;
    int zoomin_flag = 0;
}Occupy_Point;

typedef struct
{
    int point_id;
    int color_flag;
    int zoomin_flag = 0;
}Scan_Point;

#pragma pack(pop)

namespace Ui {
class SubWidget;
}

class SubWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SubWidget(QWidget *parent = nullptr);
    ~SubWidget();
public slots:
    void sortTableByColTask(int col);
    void sortTableByColSpecialTask(int col);

    void sortTableByColBattery(int col);
    void sortDetailByColBattery(int col);

public:
    Ui::SubWidget *ui;

    DrawScene *scene;

    QList<int> route_list;

    int routeCarId = -1;
    QUndoStack *undoStack;
    PlayBack_Tool *playback_tool;
    AgvCtrl_Tool *agvctrl_tool;
    BatteryInfo_Widget *batteryInfo_widget;
    taskDialog *taskInfoDialog;
    cxjDataDialog *cxjDataInfoDialog;
    cxjBackDialog *cxjBackInfoDialog;
    taskSpecialDialog *taskInfoSpecialDialog;
    MyDebugWidget *debugWidget;
    QTime *testTimer;

    QStringList pointList;
    QStringList actionList;

    QList<int> areaKeyList;

    //QUndoView *undoView;

    void proto_recv_data();
//    void recv_surplus_data();
    void recv_tcp();
    void f_dialog_show_slot();
    void f_dialog_hide_slot();
    QString IPV4IntegerToString(quint32 ip);
    void save_items_of_scene();
    void show_cur_or_nxt_seg();
    void change_show_cur_or_nxt_carID(int dev_id,int,int);
    void read_error_table();
//    void show_occupy_point(short int *p, int color_flag);
//    void show_occupy_point(QVector<Occupy_Point*> &vec_Occupy_Point);
    void show_occupy_point();
    void show_scan_point();
    void get_point_zoomin_carID(int carID, int ,int);

    void deal_with_car_data_proto();
    void protoTcp_has_connected();
    void protoTcp_taskData();
    void protoTcp_cxjData();
    void protoTcp_cxjBack();
    void protoTcp_routeData();
    void protoTcp_batteryData();
    void protoTcp_batteryDetailData();

    void use_timer_send_tcp_data();
    void use_timer_send_protoTcp_data();
    void abort_tcp();
    void abort_protoTcp();
    void getTaskDataButtonSlot();
    void playback_play_send_udp();
    void getTaskDataSlot();
    void getCxjDataSlot();
    void getCxjBackSlot();
    void getBatteryDataSlot();
    void getBatteryDetailDataSlot(int);
    void sleep(unsigned int msec);
    void ParameterInit();

    void ParameterSave(QString Type, QString p);
    void connect_playback_DB();
    void playback_play_Button_clicked();
    void playback_stop_Button_clicked();
    void playback_Slider_valueChanged(int value);
    void playback_confim_Button_clicked();
//    void floatingwindow_show_slot(int carId, float pos_x, float pos_y, int mission_id, char mission_type, int end_id, QString errorStr, int idle_wait_ts, int avoid_set_ts, int manual_on_ts, int softstop_on_ts, int warning_on_ts, char state, char status, char softstop, int stopStatus, int stopLevel, int stopByAGVId, int stopReason, int speedLevel, int speedByAGVId);
    void floatingwindow_show_slot(unsigned int dev_id, unsigned char dev_type, int mission_id, char mission_type, float pos_x, float pos_y, float pos_ang, char dir, float speed, char state, char status, int err_type, float battery, int loadnum, int cur_segment, int nxt_segment, unsigned int ip, long long timestamp, int online, int start_id, int end_id, float hyd_height, char protection1, char protection2, char protection3, char softstop, int idle_wait_ts, int avoid_set_ts, int manual_on_ts, int softstop_on_ts, int warning_on_ts, int stopStatus, int stopLevel, int stopByAGVId, int stopReason, int speedLevel, int speedByAGVId,QString errorStr, int confPoint, int mouse_x, int mouse_y);
    void changeFloatWindowCarId(unsigned int dev_id);
    void floatingwindow_hide_slot();
    void showRoute(int carId);
    void init_config();
    void init_tcplineEdit();
    void init_tableWidget();
    bool pingOk(QString sIp);
    void recover_route_slot();
    void recover_area_slot();
    void control_SAItem(int flag);
    void carSizeInfoSlot(QString carType, QString customCarL, QString customCarW);
    void agv_track_show(agv_mon::Mon_Data &mon_data);
    void update_lift_tableWidget(agv_mon::Mon_Data &mon_data);

    int last_lift_cnt = 0;

    int has_save_items_of_scene = 0;
    int avoid_Area_flag = 0;
    int floatwindow_flag = 0;
    int saBackground_flag = 0;
    QByteArray playback_udp_array;
    QByteArray playback_tcp_array;
    int plagback_start_id = 0;
    int select_id = 0;
    int slider_id = 0;
    int playbacking = 0;
    int speedDisplayMode = 0;
    bool errorInfoSwitch = true;
    bool isFullScreenState = false;
    bool sortUpDown = true;
    int selectCarID = -1;
    int floatWindowCarId = -1;
    int agv_track_flag = 0;
    QMovie *flash_gif;
    Wait_Dialog *wait_dialog;
    int myport;
    FloatingWindow *floatingwindow;
    Float_RectItem *floatItem;
    QGraphicsProxyWidget * floatProxy;
    QVector<GraphicsLineItem*> vec_LineItem;
    QVector<GraphicsArcItem*> vec_ArcItem;
    QVector<GraphicsPointItem*> vec_PointItem;
    QVector<GraphicsAreaItem*> vec_AreaItem;
    QList<Station_BackgroundItem*> list_Station_BackgroundItem;
    QVector<QVector<int> > occupyPointID_vec;
    QVector<QVector<int> > scanPointID_vec;

    QVector<Car*> vec_Car;
    QString left_string;
    QString right_string;
    SA_BackgroundItem *sa_backgrounditem;
    Station_BackgroundItem *station_backgrounditem612;
    Station_BackgroundItem *station_backgrounditem613;
    Station_BackgroundItem *station_backgrounditem614;
    Station_BackgroundItem *station_backgrounditem615;
    Station_BackgroundItem *station_backgrounditem616;
    Station_BackgroundItem *station_backgrounditem617;
    Station_BackgroundItem *station_backgrounditem607;
    Station_BackgroundItem *station_backgrounditem608;
    Station_BackgroundItem *station_backgrounditem609;
    Station_BackgroundItem *station_backgrounditem610;
    int station_background_flag = 0;
    QGraphicsRectItem *displayPointObviousRectItem;
    CarSizeWidget *carSizeWidget;
    QFont tableWidgetfont;
private:

    QSqlTableModel *db_connect1_model;
    QSqlTableModel *db_playback_connect_model;

    double temparea;
    void createPropertyEditor();
    Floating_Dialog *f_dialog;
    QHostAddress rec_udp;
    QString right_ip;
    //QtTreePropertyBrowser *propertyEditor;
    QtVariantPropertyManager *variantManager;
    QtEnumPropertyManager *enumManager;
    QGraphicsItem *currentItem;

    QMap<QtProperty *, QString> propertyToId;
    QMap<QString, QtVariantProperty *> idToProperty;
    QMap<QString, bool> idToExpanded;

    Send_Mon_Tcp send_mon_tcp;
    Send_Route_Tcp send_route_tcp;
    Recv_Mon_Udp_FLAG recv_mon_udp_FLAG;
    Recv_Mon_Udp recv_mon_udp;
    Recv_Mon_Udp2 recv_mon_udp2;
    Recv_Route_Tcp recv_route_tcp;
    QUdpSocket *udpSocket;
    QUdpSocket *protoUdpSocket;
    QTcpSocket *tcpSocket;
    QTcpSocket *protoTcpSocket;
    QTcpSocket *protoTcpSocketTask;
    QTcpSocket *protoTcpSocketCxjData;
    QTcpSocket *protoTcpSocketCxjBack;
    QTcpSocket *protoTcpSocketRoute;
    QTcpSocket *protoTcpSocketBattery;
    QTcpSocket *protoTcpSocketBatteryDetail;

    QTcpSocket *route_tcpSocket;
    QByteArray heartArray;
    QByteArray task_heartArray;
    QByteArray cxjData_heartArray;
    QByteArray cxjBack_heartArray;
    QByteArray route_heartArray;
    QByteArray battery_heartArray;


    agv_mon::Mon_Ask mon_ask;
    agv_mon::Mon_Data mon_data;
    agv_mon::Task_Data task_data;
    agv_mon::Dev_Data dev_data;

    agv_mon::Mon_Route route_data;
    batteryMonitor::BatteryData batterydata;
    batteryMonitor::ProfileBtyData profilebtydata;
    batteryMonitor::DetailBtyData detailbtydata;

    agv_mon::Mon_Data test_mon_data;
    char surplus_data[1000];
    int car_id_flag;
    int current_car_num;
    int carNum;
    int first_read_udp_flag = 0;
    int tcp_has_connected_flag = 0;
    int mysterious_button_flag = 0;
    int point_zoomin_carID = -1;


//    QVector<Car_Info*> vec_Car_Info;
    QTimer *send_tcp_timer;
    QTimer *send_protoTcp_timer;
    QTimer *abort_tcp_timer;
    QTimer *abort_protoTcp_timer;
    QTimer *playback_play_timer;
    QTimer *getTaskData_timer;


    void save_ip_to_config(QString ip_text);
    void playback_delay_changed(QString delay_text);
    double rgv_area_id;
    int seg_maxID;
    int point_maxID;
    int area_maxID;
    unsigned int show_cur_or_nxt_carID = -1;
    int error_sum = 0;
    int error_insert_row = 0;
    QSqlDatabase playback_db;



    QVector<Errorinfo*> vec_ErrorInfo;
    QVector<Occupy_Point*> vec_Occupy_Point;
    QVector<Scan_Point*> vec_Scan_Point;


    void addVariantProperty(int type, QVariant value, const QString &name, const QString &id, QtProperty *groupItem);
    void addEnumProperty(QStringList styleNames, int value, const QString &name, const QString &id, QtProperty *groupItem);
public:
    QList<int> spSize;
    QList<int> sp2Size;
    QList<int> sp3Size;

    QString fileName;
    QString colorName;
    QString routeColorName;
    QString ip_text;
    QString area_status;
    QString font;
    QString font_S;
    QString seg_w;
    QString fw_status;
    QString arrow;
    QString sa_bg;
    QString SDM;
    QString arrow2;
    QString X_MIN;
    QString X_MAX;
    QString Y_MIN;
    QString Y_MAX;
    QString MAP_ID;
    QString START_ANGLE;
    QString LOGISTICS_MAX;
    QString Station_B_F;

    int oL;
    int bATTERYHOLDER;
    int bETA;
    int bTY;
    int cHARGING;
    int dIRECTION;
    int eND;
    int fORKLIGHT;
    int hEART15MIN;
    int hEART1MIN;
    int hEART5MIN;
    int hEIGHT;
    int iP;
    int pOINTC;
    int pOINTN;
    int sAFEB;
    int sAFEF;
    int sPEED;
    int sTART;
    int tYPEC;
    int vERSION;
    int vERSION_TS;
    int bINDAREA;
    int sTALIMIT;
    int xCOORD;
    int yCOORD;
    int aNGLE;
signals:
    void send_area(double);
    void return_window_xy(int,int);
    void addToMidArea(SubWidget *);
    void sendMessage(QString);
public slots:

    void set_temparea(double);
    void deleteItem();
    void itemSelected();
    void itemMoved(QGraphicsItem * item , const QPointF & oldPosition );
    void itemAdded(QList<QGraphicsItem *> myItems);
    void deleteAdded(QList<QGraphicsItem *> items );
    void itemRotate(QGraphicsItem * item , const qreal oldAngle, QPoint center );
    void itemResize(QGraphicsItem * item , int handle , const QPointF& scale );
    void itemControl(QGraphicsItem * item , int handle , const QPointF & newPos , const QPointF& lastPos_ );

    void valueChanged(QtProperty *property, const QVariant &value);
    void valueChangedEnum(QtProperty *property, int value);

    void addProperty(QtVariantProperty *property, const QString &id);
    void updateExpandState();

    void actionList_TableViewUpdate(QVector<MapAction> vecAction);

    void findOccupyCar(int pointID);
    void findScanCar(int pointID);
//    void monitor_connected(QHostAddress IpAddress);
    void monitor_disconnected();
//    void change_car_id_flag(int car_id);
    void get_window_xy();
//    qint64 send_tcp();
    void send_route_tcp_func(int,int,int);
    void proto_send_route_tcp_func(int,int,int);
    void select_tablewidget_value(int,int,int);
    void get_area_value(double);
    void get_seg_maxID(int max);
    void get_point_maxID(int max);
    void get_area_maxID(int max);
    void tablewidget_sort(int col);
    void displayPointObvious(float x, float y);
    void station_background_slot();
protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *event);
private slots:
//    void on_carNumber_combobox_currentIndexChanged(int index);




    void on_carInfo_tableWidget_cellDoubleClicked(int row, int column);
    void on_tcp_disconnect_Button_clicked();

    void on_mysteriousButton_clicked();

    void on_stop_playback_Button_clicked();
    void on_proto_tcp_connect_Button_clicked();


public slots:
    void on_checkBox_OL_stateChanged(int arg1);
//    void on_checkBox_error_stateChanged(int arg1);
    void on_checkBox_battery_stateChanged(int arg1);
    void on_checkBox_cTime_stateChanged(int arg1);
//    void on_checkBox_load_stateChanged(int arg1);
    void on_checkBox_startId_stateChanged(int arg1);
    void on_checkBox_endId_stateChanged(int arg1);
    void on_checkBox_hight_stateChanged(int arg1);
//    void on_checkBox_posX_stateChanged(int arg1);
//    void on_checkBox_posY_stateChanged(int arg1);
//    void on_checkBox_angle_stateChanged(int arg1);
    void on_checkBox_dir_stateChanged(int arg1);
    void on_checkBox_speed_stateChanged(int arg1);
//    void on_checkBox_curseg_stateChanged(int arg1);
//    void on_checkBox_nxtseg_stateChanged(int arg1);
    void on_checkBox_curpoint_stateChanged(int arg1);
    void on_checkBox_nxtpoint_stateChanged(int arg1);
    void on_checkBox_fSafe_stateChanged(int arg1);
    void on_checkBox_bSafe_stateChanged(int arg1);
    void on_checkBox_forkLight_stateChanged(int arg1);
    void on_checkBox_xcoord_stateChanged(int arg1);
    void on_checkBox_ycoord_stateChanged(int arg1);
    void on_checkBox_angle_stateChanged(int arg1);
    void on_checkBox_bindarea_stateChanged(int arg1);
    void on_checkBox_stalimitf_stateChanged(int arg1);
public slots:

    void on_checkBox_carCT_stateChanged(int arg1);
    void on_checkBox_1min_stateChanged(int arg1);
#if 1
    void on_checkBox_IPaddr_stateChanged(int arg1);


    void on_checkBox_typeA_stateChanged(int arg1);
    void on_checkBox_batteryZD_stateChanged(int arg1);

    void on_checkBox_5min_stateChanged(int arg1);
    void on_checkBox_15min_stateChanged(int arg1);
    void on_checkBox_carVersion_stateChanged(int arg1);
    void on_checkBox_versionType_stateChanged(int arg1);


#endif
};

#endif // SUBWIDGET_H
