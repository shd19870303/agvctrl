#include "agvctrl_tool.h"
#include <QMenu>
#include <QDebug>
#include <QTcpSocket>
#include <QHostAddress>
#include <QDateTime>
#include <QScreen>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include "checksum.h"
#include <QFile>
#include <qdir.h>
#include <QSettings>
#include <QProcess>
#define MOVE_SIZE_LABEL 50
#define MOVE_SIZE_BUTTON 60
#define MOVE_SIZE_BUTTON_COLOUM2 20
#define MOVE_SIZE_BUTTON_COLOUM3 40
#define MOVE_SIZE_BUTTON_COLOUM4 60
#define MOVE_SIZE_LINEEDIT 80
AgvCtrl_Tool::AgvCtrl_Tool(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AgvCtrl_Tool)
{
    ui->setupUi(this);
    ParameterInit();

    resize(scaleUI(451),scaleUI(449));
    setMinimumSize(size());
    setMaximumSize(size());
    ui->tabWidget->resize(scaleUI(449),scaleUI(181));

    //agv tab
    ui->label->resize(scaleUI(51),scaleUI(20));
    ui->agvctrl_tab_ip_lineEdit->resize(scaleUI(151),scaleUI(41));
    ui->label_2->resize(scaleUI(51),scaleUI(20));
    ui->agvctrl_tab_agvid_lineEdit->resize(scaleUI(71),scaleUI(41));
    ui->pushButton->resize(scaleUI(84),scaleUI(23));
    ui->pushButton_2->resize(scaleUI(84),scaleUI(23));
    ui->pushButton_3->resize(scaleUI(84),scaleUI(23));
    ui->pushButton_4->resize(scaleUI(84),scaleUI(23));
    ui->avoid_forbidden_Button->resize(scaleUI(84),scaleUI(23));
    ui->lineEdit_2->resize(scaleUI(41),scaleUI(21));
    ui->management_Button->resize(scaleUI(84),scaleUI(23));
    ui->pushButton_8->resize(scaleUI(84),scaleUI(23));
    ui->pushButton_6->resize(scaleUI(84),scaleUI(23));
    ui->pushButton_9->resize(scaleUI(84),scaleUI(23));
    ui->lineEdit_3->resize(scaleUI(41),scaleUI(21));
    ui->manageTask_Button->resize(scaleUI(84),scaleUI(23));
    ui->pushButton_12->resize(scaleUI(84),scaleUI(23));
    ui->pushButton_11->resize(scaleUI(84),scaleUI(23));
    ui->pushButton_10->resize(scaleUI(84),scaleUI(23));
    ui->lineEdit_4->resize(scaleUI(41),scaleUI(21));
    ui->pushButton_18->resize(scaleUI(84),scaleUI(23));
    ui->pushButton_19->resize(scaleUI(84),scaleUI(23));
    ui->pushButton_17->resize(scaleUI(84),scaleUI(23));
    ui->specialtask_Button->resize(scaleUI(84),scaleUI(23));
    ui->specialtask_lineEdit->resize(scaleUI(41),scaleUI(21));
    ui->agvctrl_tool_textEdit->resize(scaleUI(449),scaleUI(251));


//dev tab
    ui->label_5->resize(scaleUI(51),scaleUI(20));
    ui->agvctrl_tab_ip_lineEdit_2->resize(scaleUI(151),scaleUI(41));
    ui->label_4->resize(scaleUI(91),scaleUI(20));
    ui->agvctrl_tab_area_lineEdit->resize(scaleUI(71),scaleUI(41));
    ui->dev_Button_2->resize(scaleUI(84),scaleUI(23));
    ui->dev_Button_3->resize(scaleUI(84),scaleUI(23));
    ui->dev_Button_4->resize(scaleUI(84),scaleUI(23));
    ui->storageloc_Button->resize(scaleUI(84),scaleUI(23));
    ui->agvctrl_devtab_agvID_lineEdit->resize(scaleUI(41),scaleUI(21));
    ui->agvctrl_tabStorageID_lineEdit->resize(scaleUI(41),scaleUI(21));

    ui->dev_Button_7->resize(scaleUI(84),scaleUI(23));
    ui->dev_Button_9->resize(scaleUI(84),scaleUI(23));
    ui->dev_Button_8->resize(scaleUI(84),scaleUI(23));
    ui->dev_Button_6->resize(scaleUI(84),scaleUI(23));
    ui->foldPlate_Button->resize(scaleUI(84),scaleUI(23));

    qDebug()<<"scaleUI(1.0) ="<<scaleUI(1.0);
    if(scaleUI(1.0) > 1.0)
    {
        return;
        ui->label_2->move(ui->label_2->x()+MOVE_SIZE_LABEL,ui->label_2->y());
        ui->agvctrl_tab_agvid_lineEdit->move(ui->agvctrl_tab_agvid_lineEdit->x()+MOVE_SIZE_LABEL,ui->agvctrl_tab_agvid_lineEdit->y());
        ui->pushButton->move(ui->pushButton->x()+MOVE_SIZE_BUTTON,ui->pushButton->y());
        ui->pushButton_3->move(ui->pushButton_3->x()+MOVE_SIZE_BUTTON_COLOUM2,ui->pushButton_3->y());
        ui->pushButton_4->move(ui->pushButton_4->x()+MOVE_SIZE_BUTTON_COLOUM3,ui->pushButton_4->y());
        ui->avoid_forbidden_Button->move(ui->avoid_forbidden_Button->x()+MOVE_SIZE_BUTTON_COLOUM4,ui->avoid_forbidden_Button->y());
        ui->lineEdit_2->move(ui->lineEdit_2->x()+MOVE_SIZE_LINEEDIT,ui->lineEdit_2->y());
        ui->pushButton_8->move(ui->pushButton_8->x()+MOVE_SIZE_BUTTON_COLOUM2,ui->pushButton_8->y());
        ui->pushButton_6->move(ui->pushButton_6->x()+MOVE_SIZE_BUTTON_COLOUM3,ui->pushButton_6->y());
        ui->pushButton_9->move(ui->pushButton_9->x()+MOVE_SIZE_BUTTON_COLOUM4,ui->pushButton_9->y());
        ui->lineEdit_3->move(ui->lineEdit_3->x()+MOVE_SIZE_LINEEDIT,ui->lineEdit_3->y());
        ui->pushButton_12->move(ui->pushButton_12->x()+MOVE_SIZE_BUTTON_COLOUM2,ui->pushButton_12->y());
        ui->pushButton_11->move(ui->pushButton_11->x()+MOVE_SIZE_BUTTON_COLOUM3,ui->pushButton_11->y());
        ui->pushButton_10->move(ui->pushButton_10->x()+MOVE_SIZE_BUTTON_COLOUM4,ui->pushButton_10->y());
        ui->lineEdit_4->move(ui->lineEdit_4->x()+MOVE_SIZE_LINEEDIT,ui->lineEdit_4->y());
        ui->pushButton_19->move(ui->pushButton_19->x()+MOVE_SIZE_BUTTON_COLOUM2,ui->pushButton_19->y());
        ui->pushButton_17->move(ui->pushButton_17->x()+MOVE_SIZE_BUTTON_COLOUM3,ui->pushButton_17->y());
        ui->specialtask_Button->move(ui->specialtask_Button->x()+MOVE_SIZE_BUTTON_COLOUM4,ui->specialtask_Button->y());
        ui->specialtask_lineEdit->move(ui->specialtask_lineEdit->x()+MOVE_SIZE_LINEEDIT,ui->specialtask_lineEdit->y());




        ui->dev_Button_3->move(ui->dev_Button_3->x()+MOVE_SIZE_BUTTON_COLOUM2,ui->dev_Button_3->y());
        ui->dev_Button_4->move(ui->dev_Button_4->x()+MOVE_SIZE_BUTTON_COLOUM3,ui->dev_Button_4->y());
        ui->storageloc_Button->move(ui->storageloc_Button->x()+MOVE_SIZE_BUTTON_COLOUM4,ui->storageloc_Button->y());

        ui->dev_Button_9->move(ui->dev_Button_9->x()+MOVE_SIZE_BUTTON_COLOUM2,ui->dev_Button_9->y());
        ui->dev_Button_8->move(ui->dev_Button_8->x()+MOVE_SIZE_BUTTON_COLOUM3,ui->dev_Button_8->y());
        ui->dev_Button_6->move(ui->dev_Button_6->x()+MOVE_SIZE_BUTTON_COLOUM4,ui->dev_Button_6->y());
        ui->foldPlate_Button->move(ui->foldPlate_Button->x()+MOVE_SIZE_BUTTON_COLOUM4,ui->foldPlate_Button->y());
        ui->agvctrl_devtab_agvID_lineEdit->move(ui->agvctrl_devtab_agvID_lineEdit->x()+MOVE_SIZE_LINEEDIT,ui->agvctrl_devtab_agvID_lineEdit->y());
        ui->agvctrl_tabStorageID_lineEdit->move(ui->agvctrl_tabStorageID_lineEdit->x()+MOVE_SIZE_LINEEDIT,ui->agvctrl_tabStorageID_lineEdit->y());

    }

    QList<QScreen*> screens = QApplication::screens();
    if (screens.size() > 0) {
        QScreen* screen = screens[0];
//        connect(screen,&QScreen::logicalDotsPerInchChanged,this,&AgvCtrl_Tool::onLogicalDotsPerInchChanged);
    }

    agvctrl_tcpSocket_disconnect_timer = new QTimer(this);
    connect(agvctrl_tcpSocket_disconnect_timer,&QTimer::timeout,this,&AgvCtrl_Tool::agvctrl_tcpSocket_disconnect_timer_slot);

    init_lineEdit();
    setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowStaysOnTopHint | Qt::WindowMinimizeButtonHint);

    ui->management_Button->setStyleSheet("QPushButton::menu-indicator{image:none}");
    management_Button_Menu = new QMenu(this);
    management_Button_Act1 = new QAction("查询状态", this);
    management_Button_Act2 = new QAction("启用AGV", this);
    management_Button_Act3 = new QAction("禁用AGV", this);
    management_Button_Menu->addAction(management_Button_Act1);
    management_Button_Menu->addAction(management_Button_Act2);
    management_Button_Menu->addAction(management_Button_Act3);
    ui->management_Button->setMenu(management_Button_Menu);

    ui->specialtask_Button->setStyleSheet("QPushButton::menu-indicator{image:none}");
    specialtask_Button_Menu = new QMenu(this);
    specialtask_Button_Act1 = new QAction("插针任务", this);
    specialtask_Button_Act2 = new QAction("压力测试", this);
    specialtask_Button_Act3 = new QAction("查询测试状态", this);
    specialtask_Button_Act4 = new QAction("货叉任务", this);
    specialtask_Button_Menu->addAction(specialtask_Button_Act1);
    specialtask_Button_Menu->addAction(specialtask_Button_Act4);
    specialtask_Button_Menu->addAction(specialtask_Button_Act2);
    specialtask_Button_Menu->addAction(specialtask_Button_Act3);
    ui->specialtask_Button->setMenu(specialtask_Button_Menu);


    ui->foldPlate_Button->setStyleSheet("QPushButton::menu-indicator{image:none}");
    foldPlate_Button_Menu = new QMenu(this);
    foldPlate_Button_Act1 = new QAction("更新库位", this);
    foldPlate_Button_Act2 = new QAction("清空库位", this);
    foldPlate_Button_Menu->addAction(foldPlate_Button_Act1);
    foldPlate_Button_Menu->addAction(foldPlate_Button_Act2);
    ui->foldPlate_Button->setMenu(foldPlate_Button_Menu);

    ui->storageloc_Button->setStyleSheet("QPushButton::menu-indicator{image:none}");
    storageloc_Button_Menu = new QMenu(this);
    storageloc_Button_Act1 = new QAction("胎体设备绑定AGV", this);
    storageloc_Button_Act2 = new QAction("限定库位状态查询", this);
    storageloc_Button_Act3 = new QAction("限定库位数据库更新", this);
    storageloc_Button_Act4 = new QAction("挤出机设备绑定AGV", this);
    storageloc_Button_Menu->addAction(storageloc_Button_Act1);
    storageloc_Button_Menu->addAction(storageloc_Button_Act4);
    storageloc_Button_Menu->addAction(storageloc_Button_Act2);
    storageloc_Button_Menu->addAction(storageloc_Button_Act3);
    ui->storageloc_Button->setMenu(storageloc_Button_Menu);


    ui->resetAvoid_Button->setStyleSheet("QPushButton::menu-indicator{image:none}");
    resetAvoid_Button_Menu = new QMenu(this);
    resetAvoid_Button_Act1 = new QAction("重启1区避让", this);
    resetAvoid_Button_Act2 = new QAction("重启2区避让", this);
    resetAvoid_Button_Act3 = new QAction("重启3区避让", this);
    resetAvoid_Button_Act4 = new QAction("重启4区避让", this);
    resetAvoid_Button_Menu->addAction(resetAvoid_Button_Act1);
    resetAvoid_Button_Menu->addAction(resetAvoid_Button_Act2);
    resetAvoid_Button_Menu->addAction(resetAvoid_Button_Act3);
    resetAvoid_Button_Menu->addAction(resetAvoid_Button_Act4);
    ui->resetAvoid_Button->setMenu(resetAvoid_Button_Menu);


    ui->manageModel_Button->setStyleSheet("QPushButton::menu-indicator{image:none}");
    manageModel_Button_Menu = new QMenu(this);
    manageModel_Button_Act1 = new QAction("正常模式", this);
    manageModel_Button_Act2 = new QAction("测试模式", this);
    manageModel_Button_Act3 = new QAction("模式查询", this);
    manageModel_Button_Menu->addAction(manageModel_Button_Act1);
    manageModel_Button_Menu->addAction(manageModel_Button_Act2);
    manageModel_Button_Menu->addAction(manageModel_Button_Act3);
    ui->manageModel_Button->setMenu(manageModel_Button_Menu);


    ui->manageTask_Button->setStyleSheet("QPushButton::menu-indicator{image:none}");
    manageTask_Button_Menu = new QMenu(this);
    manageTask_Button_Act1 = new QAction("完成任务", this);
    manageTask_Button_Act2 = new QAction("从当前AGV删除任务", this);
    manageTask_Button_Act3 = new QAction("取消任务不再执行", this);
    manageTask_Button_Menu->addAction(manageTask_Button_Act1);
    manageTask_Button_Menu->addAction(manageTask_Button_Act2);
    manageTask_Button_Menu->addAction(manageTask_Button_Act3);
    ui->manageTask_Button->setMenu(manageTask_Button_Menu);


    ui->opeTask_Button->setStyleSheet("QPushButton::menu-indicator{image:none}");
    opeTask_Button_Menu = new QMenu(this);
    opeTask_Button_Act1 = new QAction("删除正在执行任务", this);
    opeTask_Button_Act2 = new QAction("完成正在执行任务", this);
    opeTask_Button_Act3 = new QAction("AGV已经取完/有货", this);
    opeTask_Button_Menu->addAction(opeTask_Button_Act1);
    opeTask_Button_Menu->addAction(opeTask_Button_Act2);
    opeTask_Button_Menu->addAction(opeTask_Button_Act3);
    ui->opeTask_Button->setMenu(opeTask_Button_Menu);

    ui->notOpeTask_Button->setStyleSheet("QPushButton::menu-indicator{image:none}");
    notOpeTask_Button_Menu = new QMenu(this);
    notOpeTask_Button_Act1 = new QAction("重做未执行的任务", this);
    notOpeTask_Button_Act2 = new QAction("完成未执行的任务", this);
    notOpeTask_Button_Act3 = new QAction("取消未执行的任务", this);
    notOpeTask_Button_Act4 = new QAction("任务提升优先级", this);
    notOpeTask_Button_Act5 = new QAction("AGV绑定任务+优先级", this);
    notOpeTask_Button_Menu->addAction(notOpeTask_Button_Act1);
    notOpeTask_Button_Menu->addAction(notOpeTask_Button_Act2);
    notOpeTask_Button_Menu->addAction(notOpeTask_Button_Act3);
    notOpeTask_Button_Menu->addAction(notOpeTask_Button_Act4);
    notOpeTask_Button_Menu->addAction(notOpeTask_Button_Act5);
    ui->notOpeTask_Button->setMenu(notOpeTask_Button_Menu);

#if 1 //new
    connect(opeTask_Button_Act1,&QAction::triggered,
            [=]()
            {
                cmdID = 103;//通过任务号删除任务
            }
            );
    connect(opeTask_Button_Act1,&QAction::triggered,this,&AgvCtrl_Tool::tcp_connectToHost);


    connect(manageTask_Button_Act2,&QAction::triggered,
            [=]()
            {
                cmdID = 104;//通过agvid删除任务
            }
            );
    connect(manageTask_Button_Act2,&QAction::triggered,this,&AgvCtrl_Tool::tcp_connectToHost);

    connect(opeTask_Button_Act2,&QAction::triggered,
            [=]()
            {
                cmdID = 105;
            }
            );
    connect(opeTask_Button_Act2,&QAction::triggered,this,&AgvCtrl_Tool::tcp_connectToHost);

    connect(notOpeTask_Button_Act1,&QAction::triggered,
            [=]()
            {
                cmdID = 107;
            }
            );
    connect(notOpeTask_Button_Act1,&QAction::triggered,this,&AgvCtrl_Tool::tcp_connectToHost);

    connect(notOpeTask_Button_Act2,&QAction::triggered,
            [=]()
            {
                cmdID = 108;
            }
            );
    connect(notOpeTask_Button_Act2,&QAction::triggered,this,&AgvCtrl_Tool::tcp_connectToHost);

    connect(notOpeTask_Button_Act3,&QAction::triggered,
            [=]()
            {
                cmdID = 109;
            }
            );
    connect(notOpeTask_Button_Act3,&QAction::triggered,this,&AgvCtrl_Tool::tcp_connectToHost);

    connect(notOpeTask_Button_Act4,&QAction::triggered,
            [=]()
            {
                cmdID = 115;
            }
            );
    connect(notOpeTask_Button_Act4,&QAction::triggered,this,&AgvCtrl_Tool::tcp_connectToHost);

    connect(notOpeTask_Button_Act5,&QAction::triggered,
            [=]()
            {
                cmdID = 116;
            }
            );
    connect(notOpeTask_Button_Act5,&QAction::triggered,this,&AgvCtrl_Tool::tcp_connectToHost);

    connect(management_Button_Act2,&QAction::triggered,
            [=]()
            {
                cmdID = 305;
            }
            );
    connect(management_Button_Act2,&QAction::triggered,this,&AgvCtrl_Tool::tcp_connectToHost);
#endif


#if 0
    //old
    connect(management_Button_Act1,&QAction::triggered,
            [=]()
            {
                send_tcp_type = 1;
                tcp_FLAG_value = 306;
                tcp_recv_FLAG = 306;
                tcp_recv_type = 1;
            }
            );
    connect(management_Button_Act1,&QAction::triggered,this,&AgvCtrl_Tool::tcp_connectToHost);


    connect(management_Button_Act2,&QAction::triggered,
            [=]()
            {
                send_tcp_type = 1;
                tcp_FLAG_value = 305;
                tcp_recv_FLAG = 305;
                tcp_recv_type = 1;
            }
            );
    connect(management_Button_Act2,&QAction::triggered,this,&AgvCtrl_Tool::tcp_connectToHost);


    connect(management_Button_Act3,&QAction::triggered,
            [=]()
            {
                send_tcp_type = 1;
                tcp_FLAG_value = 304;
                tcp_recv_FLAG = 304;
                tcp_recv_type = 1;
            }
            );
    connect(management_Button_Act3,&QAction::triggered,this,&AgvCtrl_Tool::tcp_connectToHost);


    connect(specialtask_Button_Act1,&QAction::triggered,
            [=]()
            {
                send_tcp_type = 2;
                tcp_FLAG_value = 321;
                tcp_recv_FLAG = 321;
                tcp_recv_type = 1;
            }
            );
    connect(specialtask_Button_Act1,&QAction::triggered,this,&AgvCtrl_Tool::tcp_connectToHost);


    connect(specialtask_Button_Act2,&QAction::triggered,
            [=]()
            {
                send_tcp_type = 2;
                tcp_FLAG_value = 322;
                tcp_recv_FLAG = 322;
                tcp_recv_type = 1;
            }
            );
    connect(specialtask_Button_Act2,&QAction::triggered,this,&AgvCtrl_Tool::tcp_connectToHost);

    connect(specialtask_Button_Act4,&QAction::triggered,
            [=]()
            {
                send_tcp_type = 2;
                tcp_FLAG_value = 323;
                tcp_recv_FLAG = 323;
                tcp_recv_type = 1;
            }
            );
    connect(specialtask_Button_Act4,&QAction::triggered,this,&AgvCtrl_Tool::tcp_connectToHost);

    connect(specialtask_Button_Act3,&QAction::triggered,this,&AgvCtrl_Tool::specialtask_Button_Act3_slot);


    connect(foldPlate_Button_Act1,&QAction::triggered,
            [=]()
            {
                send_tcp_type = 3;
                tcp_FLAG_value = 517;
                tcp_recv_FLAG = 517;
                tcp_recv_type = 1;
            }
            );
    connect(foldPlate_Button_Act1,&QAction::triggered,this,&AgvCtrl_Tool::tcp_connectToHost);

    connect(foldPlate_Button_Act2,&QAction::triggered,
            [=]()
            {
                send_tcp_type = 3;
                tcp_FLAG_value = 518;
                tcp_recv_FLAG = 518;
                tcp_recv_type = 1;
            }
            );
    connect(foldPlate_Button_Act2,&QAction::triggered,this,&AgvCtrl_Tool::tcp_connectToHost);

    connect(storageloc_Button_Act1,&QAction::triggered,
            [=]()
            {
                send_tcp_type = 4;
                tcp_FLAG_value = 519;
                tcp_recv_FLAG = 519;
                tcp_recv_type = 1;
            }
            );
    connect(storageloc_Button_Act1,&QAction::triggered,this,&AgvCtrl_Tool::tcp_connectToHost);

    connect(storageloc_Button_Act4,&QAction::triggered,
            [=]()
            {
                send_tcp_type = 6;
                tcp_FLAG_value = 519;
                tcp_recv_FLAG = 519;
                tcp_recv_type = 1;
            }
            );
    connect(storageloc_Button_Act4,&QAction::triggered,this,&AgvCtrl_Tool::tcp_connectToHost);

    connect(storageloc_Button_Act2,&QAction::triggered,
            [=]()
            {
                send_tcp_type = 5;
                tcp_FLAG_value = 512;
                tcp_recv_FLAG = 512;
                tcp_recv_type = 100;
            }
            );
    connect(storageloc_Button_Act2,&QAction::triggered,this,&AgvCtrl_Tool::tcp_connectToHost);

    connect(storageloc_Button_Act3,&QAction::triggered,
            [=]()
            {
                send_tcp_type = 5;
                tcp_FLAG_value = 716;
                tcp_recv_FLAG = 716;
                tcp_recv_type = 1;
            }
            );
    connect(storageloc_Button_Act3,&QAction::triggered,this,&AgvCtrl_Tool::tcp_connectToHost);
#endif

    agvctrl_tcpSocket = new QTcpSocket(this);
    connect(agvctrl_tcpSocket,&QTcpSocket::connected,this,&AgvCtrl_Tool::send_ctrl_tcp_slot);
    connect(agvctrl_tcpSocket,&QTcpSocket::readyRead,this,&AgvCtrl_Tool::recv_ctrl_tcp);
}

AgvCtrl_Tool::~AgvCtrl_Tool()
{
    delete ui;
}

void AgvCtrl_Tool::send_ctrl_tcp_slot()
{
#if 1 //new
    ctrl_Array.clear();
    comd.clear_value();
    char STX1 = 0x88;
    char STX2 = 0x89;
    int len;
    comd.set_flag(cmdID);

    switch (cmdID) {
    case 103:   comd.add_value(ui->opeTaskEdit->text().toInt());break;
    case 104:   comd.add_value(ui->agvctrl_tab_agvid_lineEdit->curText().toInt());break;
    case 305:   comd.add_value(ui->agvctrl_tab_agvid_lineEdit->curText().toInt());break;
    default:    comd.add_value(-999);break;
    }


    len = comd.ByteSizeLong() + 1 + 1 + 4 + 1 + 1;
    QByteArray tempArray;
    tempArray.resize(comd.ByteSizeLong());
    comd.SerializeToArray(tempArray.data(),comd.ByteSizeLong());
    ctrl_Array.append(STX1);
    ctrl_Array.append(STX2);
    QByteArray lenArray;
    lenArray.resize(sizeof(len));
    memcpy(lenArray.data(),&len,sizeof(len));
    ctrl_Array.append(lenArray);
    ctrl_Array.append(tempArray);
    uint16_t crc16 = crc_calculate((uint8_t*)tempArray.data(),tempArray.size());
    char CKA = (crc16 & 0x00FF);
    char CKB = ((crc16 >> 8) & 0x00FF);
    ctrl_Array.append(CKA);
    ctrl_Array.append(CKB);

    qint64 temp = agvctrl_tcpSocket->write(ctrl_Array);
    qDebug()<<"agvctrl send :"<<temp;
    agvctrl_tcpSocket_disconnect_timer->start(5000);

#endif


}

void AgvCtrl_Tool::tcp_connectToHost()
{
    agvctrl_tcpSocket->connectToHost(QHostAddress(ui->agvctrl_tab_ip_lineEdit->curText()),9106); //new
#if 0
    if(send_tcp_type == 3)
    {
        agvctrl_tcpSocket->connectToHost(QHostAddress(ui->agvctrl_tab_ip_lineEdit_2->curText()),9106);
    }
    else
    {
        agvctrl_tcpSocket->connectToHost(QHostAddress(ui->agvctrl_tab_ip_lineEdit->curText()),9106);
    }
#endif
}

void AgvCtrl_Tool::recv_ctrl_tcp()
{
    if(tcp_recv_type == 1)
    {
        qint64 temp = agvctrl_tcpSocket->read((char*)&recv_ctrl_tcp_char,sizeof(recv_ctrl_tcp_char));

        switch (tcp_recv_FLAG) {
        case 304:
            if(recv_ctrl_tcp_char.agv_status == '0')
            {
                ui->agvctrl_tool_textEdit->append(QString("%2 设置AGV %1 禁用状态").arg(ui->agvctrl_tab_agvid_lineEdit->curText()).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));
            }
            else if(recv_ctrl_tcp_char.agv_status == '1')
            {
                ui->agvctrl_tool_textEdit->append(QString("%2 设置AGV %1 禁用失败").arg(ui->agvctrl_tab_agvid_lineEdit->curText()).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));
            }
            else
            {
                ui->agvctrl_tool_textEdit->append(QString("返回值错误"));
            }
            break;
        case 305:
            if(recv_ctrl_tcp_char.agv_status == '0')
            {
                ui->agvctrl_tool_textEdit->append(QString("%2 设置AGV %1 启用状态").arg(ui->agvctrl_tab_agvid_lineEdit->curText()).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));
            }
            else if(recv_ctrl_tcp_char.agv_status == '1')
            {
                ui->agvctrl_tool_textEdit->append(QString("%2 设置AGV %1 启用失败").arg(ui->agvctrl_tab_agvid_lineEdit->curText()).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));
            }
            else
            {
                ui->agvctrl_tool_textEdit->append(QString("返回值错误"));
            }
            break;
        case 306:
            if(recv_ctrl_tcp_char.agv_status == '0')
            {
                ui->agvctrl_tool_textEdit->append(QString("%2 查询AGV %1 为禁用状态").arg(ui->agvctrl_tab_agvid_lineEdit->curText()).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));
            }
            else if(recv_ctrl_tcp_char.agv_status == '1')
            {
                ui->agvctrl_tool_textEdit->append(QString("%2 查询AGV %1 为启用状态").arg(ui->agvctrl_tab_agvid_lineEdit->curText()).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));
            }
            else
            {
                ui->agvctrl_tool_textEdit->append(QString("返回值错误"));
            }
            break;
        case 321:
            if(recv_ctrl_tcp_char.agv_status == '0')
            {
                ui->agvctrl_tool_textEdit->append(QString("%2 设置AGV %1 插针任务").arg(ui->agvctrl_tab_agvid_lineEdit->curText()).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));
            }
            else if(recv_ctrl_tcp_char.agv_status == '1')
            {
                ui->agvctrl_tool_textEdit->append(QString("%2 设置AGV %1 插针任务失败").arg(ui->agvctrl_tab_agvid_lineEdit->curText()).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));
            }
            else
            {
                ui->agvctrl_tool_textEdit->append(QString("返回值错误"));
            }
            break;
        case 322:
            if(recv_ctrl_tcp_char.agv_status == '0')
            {
                ui->agvctrl_tool_textEdit->append(QString("%2 设置AGV %1 压力测试").arg(ui->agvctrl_tab_agvid_lineEdit->curText()).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));
            }
            else if(recv_ctrl_tcp_char.agv_status == '1')
            {
                ui->agvctrl_tool_textEdit->append(QString("%2 设置AGV %1 压力测试失败").arg(ui->agvctrl_tab_agvid_lineEdit->curText()).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));
            }
            else
            {
                ui->agvctrl_tool_textEdit->append(QString("返回值错误"));
            }
            break;
        case 323:
            if(recv_ctrl_tcp_char.agv_status == '0')
            {
                ui->agvctrl_tool_textEdit->append(QString("%2 设置AGV %1 货叉任务").arg(ui->agvctrl_tab_agvid_lineEdit->curText()).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));
            }
            else if(recv_ctrl_tcp_char.agv_status == '1')
            {
                ui->agvctrl_tool_textEdit->append(QString("%2 设置AGV %1 货叉任务失败").arg(ui->agvctrl_tab_agvid_lineEdit->curText()).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));
            }
            else
            {
                ui->agvctrl_tool_textEdit->append(QString("返回值错误"));
            }
            break;
        case 517:
            if(recv_ctrl_tcp_char.agv_status == '0')
            {
                ui->agvctrl_tool_textEdit->append(QString("%1 拆叠盘库位更新成功 ").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));
            }
            else if(recv_ctrl_tcp_char.agv_status == '1')
            {
                ui->agvctrl_tool_textEdit->append(QString("%1 拆叠盘库位更新失败").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));
            }
            else
            {
                ui->agvctrl_tool_textEdit->append(QString("返回值错误"));
            }
            break;
        case 518:
            if(recv_ctrl_tcp_char.agv_status == '0')
            {
                ui->agvctrl_tool_textEdit->append(QString("%1 拆叠盘库位清空成功 ").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));
            }
            else if(recv_ctrl_tcp_char.agv_status == '1')
            {
                ui->agvctrl_tool_textEdit->append(QString("%1 拆叠盘库位清空失败").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));
            }
            else
            {
                ui->agvctrl_tool_textEdit->append(QString("返回值错误"));
            }
            break;
        case 519:
            if(recv_ctrl_tcp_char.agv_status == '0')
            {
                ui->agvctrl_tool_textEdit->append(QString("%1 绑定AGV: %2 完成 ").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).arg(ui->agvctrl_devtab_agvID_lineEdit->text()));
            }
            else if(recv_ctrl_tcp_char.agv_status == '1')
            {
                ui->agvctrl_tool_textEdit->append(QString("%1 绑定AGV: %2 失败 ").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).arg(ui->agvctrl_devtab_agvID_lineEdit->text()));
            }
            else
            {
                ui->agvctrl_tool_textEdit->append(QString("返回值错误"));
            }
            break;

        case 716:
            if(recv_ctrl_tcp_char.agv_status == '0')
            {
                ui->agvctrl_tool_textEdit->append(QString("%1 更新 %2 库位限定数据库完成 ").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).arg(ui->agvctrl_tab_ip_lineEdit_2->curText()));
            }
            else if(recv_ctrl_tcp_char.agv_status == '1')
            {
                ui->agvctrl_tool_textEdit->append(QString("%1 更新 %2 库位限定数据库失败 ").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).arg(ui->agvctrl_tab_ip_lineEdit_2->curText()));
            }
            else
            {
                ui->agvctrl_tool_textEdit->append(QString("返回值错误"));
            }
            break;
        default:
            break;
        }

    }
    else if(tcp_recv_type == 100)//返回字符串
    {
        QByteArray returnStrArray = agvctrl_tcpSocket->readAll();
        QString tempStr = QString(returnStrArray.data());
        ui->agvctrl_tool_textEdit->append(QString("%1 获取 %2 数据:\n%3 ").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).arg(ui->agvctrl_tab_ip_lineEdit_2->curText()).arg(tempStr));
    }
    agvctrl_tcpSocket->disconnectFromHost();
    agvctrl_tcpSocket->close();
    agvctrl_tcpSocket_disconnect_timer->stop();
}

int AgvCtrl_Tool::confirmOperation(int flag)
{
    switch (flag) {
    case 517:return QMessageBox::question(this,"",QString("是否确定更新库位:\n区域/楼层:%1\n库位ID:%2").arg(ui->agvctrl_tab_area_lineEdit->curText()).arg(ui->agvctrl_tabStorageID_lineEdit->text()));
        break;
    case 518:return QMessageBox::question(this,"",QString("是否确定清空库位:\n区域/楼层:%1\n库位ID:%2").arg(ui->agvctrl_tab_area_lineEdit->curText()).arg(ui->agvctrl_tabStorageID_lineEdit->text()));
        break;
    case 519:
        if(send_tcp_type == 4)
        {
            return QMessageBox::question(this,"",QString("是否确定胎体设备绑定:\n区域/楼层:%1\nAGV ID:%2").arg(ui->agvctrl_tab_area_lineEdit->curText()).arg(ui->agvctrl_devtab_agvID_lineEdit->text()));
        }
        else if(send_tcp_type == 6)
        {
            return QMessageBox::question(this,"",QString("是否确定挤出机设备绑定:\n区域/楼层:%1\nAGV ID:%2").arg(ui->agvctrl_tab_area_lineEdit->curText()).arg(ui->agvctrl_devtab_agvID_lineEdit->text()));
        }
        break;
    default:
        break;
    }
}

void AgvCtrl_Tool::specialtask_Button_Act3_slot()
{
    if(stressTestDBName.isEmpty())
    {
        getStressTestDBName_db = QSqlDatabase::addDatabase("QMYSQL","getStressTestDBName_connection");
        getStressTestDBName_db.setHostName(ui->agvctrl_tab_ip_lineEdit->curText());
        getStressTestDBName_db.setUserName("manager");
        getStressTestDBName_db.setPassword("AgvManager");
        getStressTestDBName_db.setDatabaseName("agvtransfer_log");
        if(!getStressTestDBName_db.open())
        {
            QMessageBox::warning(this,"agvtransfer_log数据库打开失败",getStressTestDBName_db.lastError().text());
            return ;
        }
        else
        {
            qDebug()<<"agvtransfer_log数据库打开成功";
            QSqlQuery query1(getStressTestDBName_db);
            QString tempstr = QString("select dbname from %1;").arg("CTL_DB_NAME");
            query1.exec(tempstr);
            if(query1.next() == true)
            {
                qDebug()<<"name is :"<<query1.value(0).toString();
                stressTestDBName = query1.value(0).toString();
            }
            else
            {
                QMessageBox::warning(this,"","dbname未找到");
            }
        }
    }
    else
    {
        qDebug()<<"已经找到daname:"<<stressTestDBName;
    }


    getStressTestInfo_db = QSqlDatabase::addDatabase("QMYSQL","getStressTestInfo_db_connection");
    getStressTestInfo_db.setHostName(ui->agvctrl_tab_ip_lineEdit->curText());
    getStressTestInfo_db.setUserName("manager");
    getStressTestInfo_db.setPassword("AgvManager");
    getStressTestInfo_db.setDatabaseName(stressTestDBName);
    if(!getStressTestInfo_db.open())
    {
        QMessageBox::warning(this, stressTestDBName+"数据库打开失败",getStressTestInfo_db.lastError().text());
        return ;
    }
    else
    {
        qDebug()<<stressTestDBName+"数据库打开成功";

//        QSqlQuery testquery(getmapPBDBName_db);
//        QString teststr = QString("select creatdate from EVT_WBS_TASK;");
//        testquery.exec(teststr);
//        testquery.next();
//        qDebug()<<"testquery creatdate is : "<<testquery.value(0);
//        qDebug()<<"ui->dateEdit->dateTime().toTime_t() is :"<<ui->dateEdit->dateTime().toTime_t();

        QSqlQuery query2(getStressTestInfo_db);
        QString tempstr = QString("select stress_test_num,stress_test_max_num from CTL_AGV_TEST where  agvid=%1;").arg(ui->agvctrl_tab_agvid_lineEdit->curText());
        query2.exec(tempstr);
        qDebug()<<"query2 size :"<<query2.size();
        query2.next();
        ui->agvctrl_tool_textEdit->append(QString("%1 AGV: %2 \n压力测试次数: %3\n压力测试最大次数: %4").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).arg(ui->agvctrl_tab_agvid_lineEdit->curText()).arg(query2.value(0).toInt()).arg(query2.value(1).toInt()));

    }
}

void AgvCtrl_Tool::init_lineEdit()
{
    QStringList list;
    list<<ini_ip<<QString("192.168.66.234")<<QString("172.16.115.6")<<QString("172.16.115.7")<<QString("172.16.115.9")<<QString("172.16.251.234")
       <<QString("192.168.1.220")<<QString("192.168.1.223")<<QString("192.168.1.234")<<QString("192.168.8.234")<<QString("192.168.227.140")<<QString("10.217.204.88");
    ui->comboBox->addItems(list);
    ui->comboBox_3->addItems(list);
    ui->comboBox_4->addItems(list);
    ui->agvctrl_tab_ip_lineEdit->setUserList(list);
    ui->agvctrl_tab_ip_lineEdit_2->setUserList(list);

    QStringList list2;
    for (int i = 0; i < 50; ++i) {
        list2<<QString("%1").arg(i+1);
    }
    ui->agvctrl_tab_agvid_lineEdit->setUserList(list2);
    QStringList list3;
    list3<<QString("1")<<QString("2")<<QString("3")<<QString("4")<<QString("5")<<QString("6")<<QString("7")<<QString("8")<<QString("9")<<QString("10");
    ui->agvctrl_tab_area_lineEdit->setUserList(list3);
    ui->comboBox_5->addItems(list3);
    QStringList list4;
    list4<<QString("0");

}




void AgvCtrl_Tool::on_avoid_forbidden_Button_clicked()
{
    management_Button_Menu->exec(QPoint(QCursor::pos().x(), QCursor::pos().y()));
}

void AgvCtrl_Tool::onLogicalDotsPerInchChanged(qreal dpi)
{
    m_scaleFromPreRate = (double)dpi / 96 / m_preRate;
    m_preRate = (double)dpi / 96;
    findChild(*this);
}

void AgvCtrl_Tool::agvctrl_tcpSocket_disconnect_timer_slot()
{
    agvctrl_tcpSocket->disconnectFromHost();
    agvctrl_tcpSocket->close();
}



void AgvCtrl_Tool::findChild(const QObject &o)
{
    for (int i=0; i<o.children().size(); ++i) {
        QWidget *pWidget = qobject_cast<QWidget *>(o.children().at(i));
        if (pWidget != nullptr) {
            qDebug() << pWidget->width() << pWidget->height();
            pWidget->resize(pWidget->width()*m_scaleFromPreRate, pWidget->height()*m_scaleFromPreRate);
            findChild(*(o.children().at(i)));
        }
    }
}

double AgvCtrl_Tool::scaleUI(int spec)
{
    double rate = 0;
    if (0 == (int)rate) {
        QList<QScreen*> screens = QApplication::screens();
        if (screens.size() > 0) {
            QScreen* screen = screens[0];
            double dpi = screen->logicalDotsPerInch();
            rate = dpi / 96.0;

            if (rate < 1.1) {
                rate = 1.0;
            } else if (rate < 1.4) {
                rate = 1.25;
            } else if (rate < 1.6) {
                rate = 1.5;
            } else if (rate < 1.8) {
                rate = 1.75;
            } else {
                rate = 2.0;
            }
        }
    }
    m_preRate = rate;
    return double(spec * rate);
}



void AgvCtrl_Tool::on_singleTaskButton_clicked()
{
    cmdID = 402;
    tcp_connectToHost();
}

void AgvCtrl_Tool::on_serialTaskButton_clicked()
{
    cmdID = 403;
    tcp_connectToHost();
}

void AgvCtrl_Tool::on_forcibleTackButton_clicked()
{
    cmdID = 405;
    tcp_connectToHost();
}

bool pingOk(QString sIp)
{
       if (sIp.isEmpty()) return false;

       QString sCmd = QString("ping %1").arg(sIp);

       QProcess proc;
       proc.start(sCmd);
       proc.waitForReadyRead(200);
       proc.waitForFinished(200);

       QString sRet = proc.readAll();

       bool bPing = (sRet.indexOf("TTL", 0, Qt::CaseInsensitive) >= 0);
       proc.finished(0);
       proc.terminate();

       return bPing;
}


void AgvCtrl_Tool::on_getmap_clicked()
{
    if(pingOk(ui->comboBox->currentText()) != true)
    {
        ui->agvctrl_tool_textEdit->append("ping "+ui->comboBox->currentText()+QString("失败.\r\n"));
        return;
    }
    ui->getmap->setEnabled(false);
    if(mapDBName.isEmpty())
    {
        getmapDBName_db = QSqlDatabase::addDatabase("QMYSQL","getmapDBName_connection");
        getmapDBName_db.setHostName(ui->comboBox->currentText());
        getmapDBName_db.setUserName("manager");
        getmapDBName_db.setPassword("AgvManager");
        getmapDBName_db.setDatabaseName("agvtransfer_log");
        if(!getmapDBName_db.open())
        {
            QMessageBox::warning(this,"agvtransfer_log数据库打开失败",getmapDBName_db.lastError().text());
            ui->getmap->setEnabled(true);
            return ;
        }
        else
        {
            qDebug()<<"agvtransfer_log数据库打开成功";
            QSqlQuery query1(getmapDBName_db);
            QString tempstr = QString("select dbname from %1;").arg("CTL_DB_NAME");
            query1.exec(tempstr);
            if(query1.next() == true)
            {
                qDebug()<<"name is :"<<query1.value(0).toString();
                mapDBName = query1.value(0).toString();
            }
            else
            {
                QMessageBox::warning(this,"","dbname未找到");
            }
        }
    }
    else
    {
        qDebug()<<"已经找到daname:"<<mapDBName;
    }


    getmapPBDBName_db = QSqlDatabase::addDatabase("QMYSQL","getTaskInfo_connection");
    getmapPBDBName_db.setHostName(ui->comboBox->currentText());
    getmapPBDBName_db.setUserName("manager");
    getmapPBDBName_db.setPassword("AgvManager");
    getmapPBDBName_db.setDatabaseName(mapDBName);
    if(!getmapPBDBName_db.open())
    {
        QMessageBox::warning(this, mapDBName+"数据库打开失败",getmapPBDBName_db.lastError().text());
        ui->getmap->setEnabled(true);
        return ;
    }
    else
    {
        qDebug()<<mapDBName+"数据库打开成功";
        QSqlQuery query2(getmapPBDBName_db);
        QString tempstr = QString("select mapdata,remark,uploaddate from EVT_MAP_DATA where ver=0 order by id desc limit 0,1;");
        query2.exec(tempstr);
        if(query2.size() > 0)
        {
            query2.next();
            QByteArray temparr(query2.value(0).toByteArray());
//            qDebug() << "1" << query2.value(1)<< "2" << temparr.size()<< "3" << query2.value(2);
            QString filename = QDir::currentPath()+"/"+query2.value(2).toDateTime().toString("yyyyMMdd_hhmmss")+"_0.pb";
            QFile file(filename);

            if(!file.open(QIODevice::WriteOnly))
            {
                ui->getmap->setEnabled(true);
                return;
            }
            QDataStream out(&file);

            out.writeRawData(temparr.data(),temparr.size());
            ui->agvctrl_tool_textEdit->append("从"+ui->comboBox->currentText()+QString(",下载地图成功,存放:\r\n"+filename+"\r\n"));
            emit download_map_finished(filename);
        }
    }
    ui->getmap->setEnabled(true);
}

void AgvCtrl_Tool::ParameterInit()
{
    QSettings settings("Config.ini", QSettings::IniFormat);
    ini_ip = settings.value("ip").toString();

}
