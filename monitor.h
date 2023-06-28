#ifndef MONITOR_H
#define MONITOR_H

#include <QWidget>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QTimer>

#include "drawview.h"

#include "checksum.h"

#include "simtask.h"



#pragma pack(push, 1)

typedef struct RecvPackage{
    char header1;
    char header2;
    char project_id;
    char map_id;
    unsigned char rgv_num;
};

typedef struct MonitorHeartbeat{
    char STX1 = 0xAD;
    char STX2 = 0xAE;
    int    monitorUdpPort = 9001;
    char   rgvAreaId;
    uint16_t crc;
};

#pragma pack(pop)

typedef struct IdFlash{
    bool isflash = false;
    qint64 count = 0;
    bool showing = false;
};

namespace Ui {
class Monitor;
}

class SendTaskThread;

class Monitor : public QWidget
{
    Q_OBJECT

public:
    explicit Monitor(QWidget *parent = nullptr);
    ~Monitor();

    void setGraphicsView(DrawView * activeView);

private:
    Ui::Monitor *ui;
    QByteArray recvPackageArray;


    QTimer *timer;

    int test_mileage = 0;

    QTcpSocket  *tcpSocketHeartbeat;    //监控发送心跳TCP
    QTimer *timerHeartbeat;             //监控发送心跳定时器

public:
    QUdpSocket *udpSocket;
    QTcpSocket  *tcpSocket;   

    QHostAddress IpAddress;

    DrawView * activeView = 0;

    QVector<GraphicsRgvItem> vecRgvItem;


    QMap<int , int > mapId;
    QMap<int , IdFlash > mapIdFlash;
    QMap<int , int > lastMapId;


signals:
    void monitor_connected_signal(QHostAddress IpAddress);
    void monitor_disconnected_signal();


public slots:
    void recvUdpData();
    void updateValue();

    void timerSendHeartbeat();

private slots:
    void on_pushBtn_connect_clicked();
    void on_pushBtn_disconnect_clicked();

};

#endif // MONITOR_H
