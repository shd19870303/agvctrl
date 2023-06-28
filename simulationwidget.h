#ifndef SIMULATIONWIDGET_H
#define SIMULATIONWIDGET_H

#include <QWidget>
#include <QStandardItemModel>
#include <QDebug>
#include <QUdpSocket>
#include <QTcpSocket>
#include "drawview.h"

#include "checksum.h"

#include "simtask.h"

#pragma pack(push, 1)

typedef struct SendTaskPackage{
    char header1;
    char header2;
    long long    timestamp;
    int     task_num;
};

#pragma pack(pop)

namespace Ui {
class SimulationWidget;
}

class SendTaskThread;

class SimulationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SimulationWidget(QWidget *parent = nullptr);
    ~SimulationWidget();

    void setGraphicsView(DrawView * activeView);

    QStandardItemModel * add_task_item_model;

signals:
    void addTaskManualSignal(int startSatId, int endSatId, int cntDown);

private slots:
    void on_pushBtn_add_clicked();

    void on_pushBtn_addToList_clicked();

    void on_pushBtn_del_clicked();

    void on_pushButton_open_clicked();

    void on_pushButton_save_clicked();

    void on_pushBtn_SimulateStart_clicked();
    void on_pushBtn_SimulatePause_clicked();
    void on_pushBtn_SimulateStop_clicked();
    void on_pushBtn_TaskClear_clicked();

    void getIpAddress(QHostAddress IpAddress);

    void timerUpdateTaskList();

    void deleteTaskById(int taskID);

    void monitor_disconnected();

    void on_pushBtn_CreatTaskToFile_clicked();

private:
    Ui::SimulationWidget *ui;

public:
    DrawView * activeView = 0;
    simTask *simulation;
    SendTaskThread *sendTaskThread;
    QHostAddress IpAddress;
    QUdpSocket *udpSocket;

    QTimer *updateListTimer;
};

class SendTaskThread : public QThread
{
    Q_OBJECT
public:
    //构造函数
    SendTaskThread(SimulationWidget *parent = nullptr);

    SimulationWidget * simulationWidget;

    void run(void);

signals:
    void taskListUpdateSignal();
};

#endif // SIMULATIONWIDGET_H
