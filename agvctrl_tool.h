#ifndef AGVCTRL_TOOL_H
#define AGVCTRL_TOOL_H

#include <QWidget>
#include "ui_agvctrl_tool.h"
#include <QTimer>
#include <QSqlDatabase>
#include "protoFile/agv_mon.pb.h"
class QMenu;
class QTcpSocket;
namespace Ui {
class AgvCtrl_Tool;
}

#pragma pack(push, 1)
typedef struct
{
    char LOGO1;//暂时修改测试
    char LOGO2;
    char LOGO3;
    char LOGO4;
    char LOGO5;
    char LOGO6;
    int FLAG;
}Send_Ctrl_Tcp0;

typedef struct
{
    char LOGO1;
    char LOGO2;
    char LOGO3;
    char LOGO4;
    char LOGO5;
    char LOGO6;
    int FLAG;
    int param1;
}Send_Ctrl_Tcp1;

typedef struct
{
    char LOGO1;
    char LOGO2;
    char LOGO3;
    char LOGO4;
    char LOGO5;
    char LOGO6;
    int FLAG;
    int param1;
    int param2;
}Send_Ctrl_Tcp2;

typedef struct
{
    char LOGO1;
    char LOGO2;
    char LOGO3;
    char LOGO4;
    char LOGO5;
    char LOGO6;
    int FLAG;
    int param1;
    int param2;
    int param3;
}Send_Ctrl_Tcp3;

typedef struct
{
    char LOGO1;
    char LOGO2;
    char LOGO3;
    char LOGO4;
    char LOGO5;
    char LOGO6;
    char agv_status;
}Recv_Ctrl_Tcp_char;

#pragma pack(pop)

class AgvCtrl_Tool : public QWidget
{
    Q_OBJECT

public:
    explicit AgvCtrl_Tool(QWidget *parent = nullptr);
    ~AgvCtrl_Tool();
    void send_ctrl_tcp_slot();
    void tcp_connectToHost();
    void recv_ctrl_tcp();
    void init_lineEdit();
    void on_avoid_forbidden_Button_clicked();
    void onLogicalDotsPerInchChanged(qreal dpi);
    void agvctrl_tcpSocket_disconnect_timer_slot();
    int confirmOperation(int flag);
    void specialtask_Button_Act3_slot();
    void ParameterInit();
public:
    Ui::AgvCtrl_Tool *ui;
    QMenu *management_Button_Menu;
    QMenu *specialtask_Button_Menu;
    QMenu *foldPlate_Button_Menu;
    QMenu *storageloc_Button_Menu;
    QMenu *resetAvoid_Button_Menu;
    QMenu *manageModel_Button_Menu;
    QMenu *manageTask_Button_Menu;
    QMenu *opeTask_Button_Menu;
    QMenu *notOpeTask_Button_Menu;
    QTimer *agvctrl_tcpSocket_disconnect_timer;
    QString stressTestDBName;
    QSqlDatabase getStressTestDBName_db;
    QSqlDatabase getStressTestInfo_db;
    QSqlDatabase getmapDBName_db;
    QSqlDatabase getmapPBDBName_db;
    QString mapDBName;

    QByteArray ctrl_Array;
    QString ini_ip;

signals:
    void download_map_finished(QString file);


private slots:
    void on_singleTaskButton_clicked();

    void on_serialTaskButton_clicked();

    void on_forcibleTackButton_clicked();

    void on_getmap_clicked();

private:
    void findChild(const QObject &o);
    double scaleUI(int spec);

    double m_preRate;
    double m_scaleFromPreRate;
private:

    QAction *management_Button_Act1;
    QAction *management_Button_Act2;
    QAction *management_Button_Act3;
    QAction *specialtask_Button_Act1;
    QAction *specialtask_Button_Act2;
    QAction *specialtask_Button_Act3;
    QAction *specialtask_Button_Act4;
    QAction *foldPlate_Button_Act1;
    QAction *foldPlate_Button_Act2;
    QAction *storageloc_Button_Act1;
    QAction *storageloc_Button_Act2;
    QAction *storageloc_Button_Act3;
    QAction *storageloc_Button_Act4;
    QAction *resetAvoid_Button_Act1;
    QAction *resetAvoid_Button_Act2;
    QAction *resetAvoid_Button_Act3;
    QAction *resetAvoid_Button_Act4;
    QAction *manageModel_Button_Act1;
    QAction *manageModel_Button_Act2;
    QAction *manageModel_Button_Act3;
    QAction *manageTask_Button_Act1;
    QAction *manageTask_Button_Act2;
    QAction *manageTask_Button_Act3;
    QAction *opeTask_Button_Act1;
    QAction *opeTask_Button_Act2;
    QAction *opeTask_Button_Act3;
    QAction *notOpeTask_Button_Act1;
    QAction *notOpeTask_Button_Act2;
    QAction *notOpeTask_Button_Act3;
    QAction *notOpeTask_Button_Act4;
    QAction *notOpeTask_Button_Act5;


    Send_Ctrl_Tcp0 send_ctrl_tcp0;
    Send_Ctrl_Tcp1 send_ctrl_tcp1;
    Send_Ctrl_Tcp2 send_ctrl_tcp2;
    Send_Ctrl_Tcp3 send_ctrl_tcp3;
    Recv_Ctrl_Tcp_char recv_ctrl_tcp_char;
    QTcpSocket *agvctrl_tcpSocket;
    int send_tcp_type = -1;
    int tcp_FLAG_value = -1;
    int tcp_recv_type = -1;//1 char*1
    int tcp_recv_FLAG = -1;

    int cmdID = -1;
    agv_mon::COMD comd;
};

#endif // AGVCTRL_TOOL_H
