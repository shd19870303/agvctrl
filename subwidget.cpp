#include "subwidget.h"
#include "commands.h"
#include "drawtool.h"
#include "car.h"
#include "car_info.h"
#include <QGraphicsRectItem>
//#include <QGraphicsView>
#include "drawview.h"
#include "floating_dialog.h"
#include <QCursor>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QHostAddress>
#include <QSqlRecord>
#include <QSqlDatabase>
#include <QInputDialog>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QMessageBox>
#include <QSqlQuery>
#include <QVariant>
#include <QString>
#include <QDebug>
#include <QProcess>
#include <stdio.h>
#include <QSqlTableModel>
#include <QDateTime>
#include <QMovie>
#include "wait_dialog.h"
#include <QGraphicsProxyWidget>
#include "floatingwindow.h"
#include "float_rectitem.h"
#include "checksum.h"
#include "sa_backgrounditem.h"
#include <QPalette>
#include "taskdialog.h"
#include "mydebugwidget.h"
#include "tablenumberitem.h"
#include <QList>

#define CARID 0
#define TASKID 1
#define TIME 2
#define TYPET 3
#define STATE 4
#define STATUS 5
#define SOFTSTOP 6
#define ONLINE 7
#define ERROR 8
#define BATTERY 9
#define CHARGING 10
#define LOAD 11
#define START 12
#define END 13
#define HEIGHT 14
#define X 15
#define Y 16
#define ANGLE 17
#define DIRECTION 18
#define SPEED 19
#define ROUTEC 20
#define ROUTEN 21
#define POINTC 22
#define POINTN 23
#define SAFEF 24
#define SAFEB 25
#define FORKLIGHT 26
#define IP 27
#define TYPEC 28
#define BATTERYHOLDER 29
#define HEART1MIN 30
#define HEART5MIN 31
#define HEART15MIN 32
#define VERSION 33
#define VERSION_TS 34
#define BETA 35
#define BINDAREA 36
#define STALIMIT 37
SubWidget::SubWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SubWidget)
{
    ui->setupUi(this);
    ui->tcp_connect_Button->setVisible(false);


    tableWidgetfont.setPixelSize(12);
    ui->carInfo_tableWidget->setFont(tableWidgetfont);
    ui->carInfo_tableWidget->horizontalHeader()->setFont(tableWidgetfont);
    testTimer = new QTime;

    debugWidget = new MyDebugWidget;
    carSizeWidget = new CarSizeWidget;
    connect(carSizeWidget,&CarSizeWidget::sendCarSizeInfo,this,&SubWidget::carSizeInfoSlot);

    playback_tool = new PlayBack_Tool;
    agvctrl_tool = new AgvCtrl_Tool;
    sa_backgrounditem = new SA_BackgroundItem;
    station_backgrounditem612 = new Station_BackgroundItem(QString("612"));
    station_backgrounditem613 = new Station_BackgroundItem(QString("613"));
    station_backgrounditem614 = new Station_BackgroundItem(QString("614"));
    station_backgrounditem615 = new Station_BackgroundItem(QString("615"));
    station_backgrounditem616 = new Station_BackgroundItem(QString("616"));
    station_backgrounditem617 = new Station_BackgroundItem(QString("617"));
    station_backgrounditem607 = new Station_BackgroundItem(QString("607"));
    station_backgrounditem608 = new Station_BackgroundItem(QString("608"));
    station_backgrounditem609 = new Station_BackgroundItem(QString("609"));
    station_backgrounditem610 = new Station_BackgroundItem(QString("610"));


    init_tableWidget();



    connect(playback_tool,&PlayBack_Tool::clicked_playback_connect_Button,this,&SubWidget::connect_playback_DB);
    connect(playback_tool,&PlayBack_Tool::clicked_playback_play_Button,this,&SubWidget::playback_play_Button_clicked);
    connect(playback_tool,&PlayBack_Tool::clicked_playback_stop_Button,this,&SubWidget::playback_stop_Button_clicked);
    connect(playback_tool,&PlayBack_Tool::playback_Slider_valueChanged,this,&SubWidget::playback_Slider_valueChanged);
    connect(playback_tool,&PlayBack_Tool::clicked_playback_confirm_Button,this,&SubWidget::playback_confim_Button_clicked);
    connect(ui->graphicsView,&DrawView::send_area_value,this,&SubWidget::get_area_value);
    connect(ui->graphicsView,&DrawView::send_seg_maxID,this,&SubWidget::get_seg_maxID);
    connect(ui->graphicsView,&DrawView::send_point_maxID,this,&SubWidget::get_point_maxID);
    connect(ui->graphicsView,&DrawView::send_area_maxID,this,&SubWidget::get_area_maxID);

    init_tcplineEdit();

    current_car_num = -1;
    vec_Car.clear();
    vec_LineItem.clear();
    vec_ArcItem.clear();
    vec_ErrorInfo.clear();
    send_tcp_timer = new QTimer(this);
    connect(send_tcp_timer,&QTimer::timeout,this,&SubWidget::use_timer_send_tcp_data);
    send_protoTcp_timer = new QTimer(this);
    connect(send_protoTcp_timer,&QTimer::timeout,this,&SubWidget::use_timer_send_protoTcp_data);

    abort_tcp_timer = new QTimer(this);
    connect(abort_tcp_timer,&QTimer::timeout,this,&SubWidget::abort_tcp);
    abort_protoTcp_timer = new QTimer(this);
    connect(abort_protoTcp_timer,&QTimer::timeout,this,&SubWidget::abort_protoTcp);

    getTaskData_timer = new QTimer(this);
    connect(getTaskData_timer,&QTimer::timeout,this,&SubWidget::getTaskDataButtonSlot);

    playback_play_timer = new QTimer(this);
    connect(playback_play_timer,&QTimer::timeout,this,&SubWidget::playback_play_send_udp);


    connect(playback_tool->ui->playback_delay_lineEdit,&UserNameCtrl::sig_TextChanged,this,&SubWidget::playback_delay_changed);
    udpSocket = new QUdpSocket(this);
    protoUdpSocket = new QUdpSocket(this);


    tcpSocket = new QTcpSocket(this);
    connect(tcpSocket,&QTcpSocket::readyRead,
            [=]()
            {
            }
    );
    protoTcpSocket = new QTcpSocket(this);
    connect(protoTcpSocket,&QTcpSocket::connected,this,&SubWidget::protoTcp_has_connected);

    protoTcpSocketTask = new QTcpSocket(this);
    connect(protoTcpSocketTask,&QTcpSocket::readyRead,this,&SubWidget::protoTcp_taskData);
    connect(protoTcpSocketTask,&QTcpSocket::readyRead,
            [=]()
    {
    }
    );

    protoTcpSocketCxjData = new QTcpSocket(this);
    connect(protoTcpSocketCxjData,&QTcpSocket::readyRead,this,&SubWidget::protoTcp_cxjData);
    connect(protoTcpSocketCxjData,&QTcpSocket::readyRead,
            [=]()
    {
    }
    );

    protoTcpSocketCxjBack = new QTcpSocket(this);
    connect(protoTcpSocketCxjBack,&QTcpSocket::readyRead,this,&SubWidget::protoTcp_cxjBack);
    connect(protoTcpSocketCxjBack,&QTcpSocket::readyRead,
            [=]()
    {
    }
    );

    protoTcpSocketBattery = new QTcpSocket(this);
    connect(protoTcpSocketBattery,&QTcpSocket::readyRead,this,&SubWidget::protoTcp_batteryData);
    connect(protoTcpSocketBattery,&QTcpSocket::readyRead,
            [=]()
    {
    }
    );

    protoTcpSocketBatteryDetail = new QTcpSocket(this);
    connect(protoTcpSocketBatteryDetail,&QTcpSocket::readyRead,this,&SubWidget::protoTcp_batteryDetailData);
    connect(protoTcpSocketBatteryDetail,&QTcpSocket::readyRead,
            [=]()
    {
    }
    );

    protoTcpSocketRoute = new QTcpSocket(this);
    connect(protoTcpSocketRoute,&QTcpSocket::readyRead,this,&SubWidget::protoTcp_routeData);
    connect(protoTcpSocketRoute,&QTcpSocket::readyRead,
            [=]()
    {
    }
    );


    route_tcpSocket = new QTcpSocket(this);
    connect(route_tcpSocket,&QTcpSocket::readyRead,this,&SubWidget::recv_tcp);



    scene = new DrawScene(this);
    connect(scene,&DrawScene::recover_route,this,&SubWidget::recover_route_slot);
    connect(scene,&DrawScene::recover_area,this,&SubWidget::recover_area_slot);

    QRectF rc = QRectF(0,0,20000,10000);
    scene->setSceneRect(rc);
    scene->setBackgroundBrush(scene->scene_color);//场景背景色

    ui->graphicsView->setScene(scene);
    scene->setView(ui->graphicsView);

    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView->setCacheMode(QGraphicsView::CacheBackground);
    ui->graphicsView->setOptimizationFlags(QGraphicsView::DontSavePainterState);
    ui->graphicsView->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    //view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    ui->graphicsView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    // move orign point to leftbottom
    ui->graphicsView->setTransform(ui->graphicsView->transform().scale(1,-1));


    connect(scene, SIGNAL(selectionChanged()),
            this, SLOT(itemSelected()));

    connect(scene,SIGNAL(itemAdded(QList<QGraphicsItem *>)),
            this, SLOT(itemAdded(QList<QGraphicsItem *>)));
//    connect(scene,SIGNAL(deleteAdded(QGraphicsItem*)),
//            this, SLOT(deleteAdded(QGraphicsItem*)));

    connect(scene,SIGNAL(itemMoved(QGraphicsItem*,QPointF)),
            this,SLOT(itemMoved(QGraphicsItem*,QPointF)));

    connect(scene,SIGNAL(itemRotate(QGraphicsItem*,qreal,QPoint)),
            this,SLOT(itemRotate(QGraphicsItem*,qreal,QPoint)));

//    connect(scene,SIGNAL(itemResize(QGraphicsItem* , int , const QPointF&)),
//            this,SLOT(itemResize(QGraphicsItem*,int,QPointF)));

//    connect(scene,SIGNAL(itemControl(QGraphicsItem* , int , const QPointF&,const QPointF&)),
//            this,SLOT(itemControl(QGraphicsItem*,int,QPointF,QPointF)));

//    undoStack = new QUndoStack(this);
//    undoView = new QUndoView(undoStack);
//    undoView->setWindowTitle(tr("Command List"));
//    undoView->setAttribute(Qt::WA_QuitOnClose, true);



    createPropertyEditor();

    showMaximized();



    ui->tcp_disconnect_Button->setEnabled(false);

    ui->splitter_3->setStretchFactor(0,3);

    QHeaderView *headerGoods = ui->carInfo_tableWidget->horizontalHeader();
    headerGoods->setSortIndicator(0, Qt::AscendingOrder);
    headerGoods->setSortIndicatorShown(true);
    headerGoods->setSectionsClickable(true);
    connect(headerGoods,SIGNAL(sectionClicked(int)),this,SLOT(tablewidget_sort(int)));


    ui->tcp_disconnect_Button->setVisible(false);
    read_error_table();


    init_config();


    ui->stop_playback_Button->setVisible(false);
    wait_dialog = new Wait_Dialog;
    flash_gif = new QMovie(":/images/waiting.gif");
    wait_dialog->ui->label_waiting->setMovie(flash_gif);


    floatingwindow = new FloatingWindow;
    floatItem = new Float_RectItem;
    floatItem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    floatProxy = new QGraphicsProxyWidget(floatItem);
    floatItem->setPos(0,0);
    floatItem->setZValue(5);
    floatProxy->setWidget(floatingwindow);
    scene->addItem(floatItem);
    floatItem->hide();


    list_Station_BackgroundItem.append(station_backgrounditem612);
    list_Station_BackgroundItem.append(station_backgrounditem613);
    list_Station_BackgroundItem.append(station_backgrounditem614);
    list_Station_BackgroundItem.append(station_backgrounditem615);
    list_Station_BackgroundItem.append(station_backgrounditem616);
    list_Station_BackgroundItem.append(station_backgrounditem617);
    list_Station_BackgroundItem.append(station_backgrounditem607);
    list_Station_BackgroundItem.append(station_backgrounditem608);
    list_Station_BackgroundItem.append(station_backgrounditem609);
    list_Station_BackgroundItem.append(station_backgrounditem610);

    if(station_background_flag == 1)
    {
        for(int i=0; i<list_Station_BackgroundItem.size(); i++)
        {
            scene->addItem(list_Station_BackgroundItem[i]);
        }

        station_backgrounditem612->setPos(120118,99334);
        station_backgrounditem613->setPos(142284,99334);
        station_backgrounditem614->setPos(163966,99334);
        station_backgrounditem615->setPos(187439,99334);
        station_backgrounditem616->setPos(209046,99334);
        station_backgrounditem617->setPos(232008,99334);
        station_backgrounditem607->setPos(120588,73520);
        station_backgrounditem608->setPos(147627,73520);
        station_backgrounditem609->setPos(174467,73520);
        station_backgrounditem610->setPos(201202,73520);
    }


}

SubWidget::~SubWidget()
{
    delete ui;
//    undoStack->clear();
    delete undoStack;

    DrawTool * tool = dynamic_cast<DrawTool *>(DrawTool::findTool( DrawTool::c_drawShape ));
    tool->inited_drift = false;
    tool->inited_drift_one_param = false;
    if(tool->driftInput)
    {
        delete tool->driftInput;
        tool->driftInput = 0;
    }
    if(tool->driftItem)
    {
        delete tool->driftItem;
        tool->driftItem = 0;
    }

    if(tool->driftInputRadius)
    {
        delete tool->driftInputRadius;
        tool->driftInputRadius = 0;
    }

    if(tool->driftItemRadius)
    {
        delete tool->driftItemRadius;
        tool->driftItemRadius = 0;
    }
}

void SubWidget::sortTableByColTask(int col)
{
    if(sortUpDown)
        {
            taskInfoDialog->ui->tasktWidget->sortItems(col, Qt::AscendingOrder);
            sortUpDown=false;
        }
        else
        {
            taskInfoDialog->ui->tasktWidget->sortItems(col, Qt::DescendingOrder);
            sortUpDown=true;
    }
}

void SubWidget::sortTableByColSpecialTask(int col)
{
    if(sortUpDown)
        {
            taskInfoSpecialDialog->ui->taskSpecialTable->sortItems(col, Qt::AscendingOrder);
            sortUpDown=false;
        }
        else
        {
            taskInfoSpecialDialog->ui->taskSpecialTable->sortItems(col, Qt::DescendingOrder);
            sortUpDown=true;
    }
}

void SubWidget::sortTableByColBattery(int col)
{
    if(sortUpDown)
        {
            batteryInfo_widget->ui->batteryTableWidget->sortItems(col, Qt::AscendingOrder);
            sortUpDown=false;
        }
        else
        {
            batteryInfo_widget->ui->batteryTableWidget->sortItems(col, Qt::DescendingOrder);
            sortUpDown=true;
    }
}

void SubWidget::sortDetailByColBattery(int col)
{
    if(sortUpDown)
        {
            batteryInfo_widget->ui->batteryDetailWidget->sortItems(col, Qt::AscendingOrder);
            sortUpDown=false;
        }
        else
        {
            batteryInfo_widget->ui->batteryDetailWidget->sortItems(col, Qt::DescendingOrder);
            sortUpDown=true;
    }
}



void SubWidget::protoTcp_batteryData()
{
    QByteArray allArray;
    allArray = protoTcpSocketBattery->readAll();
    protoTcpSocketBattery->disconnectFromHost();
    protoTcpSocketBattery->close();
    profilebtydata.ParseFromArray(allArray.data()+6,allArray.size()-8);
    int batsnum = profilebtydata.bats_size();
    batteryInfo_widget->ui->batteryTableWidget->setRowCount(batsnum);

    for(int i = 0; i < batsnum; i++)
    {
        if(batteryInfo_widget->ui->batteryTableWidget->item(i,0) == nullptr)
        {
            batteryInfo_widget->ui->batteryTableWidget->setItem(i, 0, new QTableWidgetItem());
            batteryInfo_widget->ui->batteryTableWidget->item(i,0)->setData(Qt::DisplayRole , profilebtydata.bats(i).bty_tag());

            batteryInfo_widget->ui->batteryTableWidget->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(profilebtydata.bats(i).bty_rfid())));
            batteryInfo_widget->ui->batteryTableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(profilebtydata.bats(i).inagv())));
            batteryInfo_widget->ui->batteryTableWidget->setItem(i, 3, new QTableWidgetItem(QString::number(profilebtydata.bats(i).instation())));
            batteryInfo_widget->ui->batteryTableWidget->setItem(i, 4, new QTableWidgetItem(QString::number(profilebtydata.bats(i).stationspace())));
            batteryInfo_widget->ui->batteryTableWidget->setItem(i, 5, new QTableWidgetItem(QString::number(profilebtydata.bats(i).power())+QString("%")));
            batteryInfo_widget->ui->batteryTableWidget->setItem(i, 6, new QTableWidgetItem(QString::number(profilebtydata.bats(i).useavgtime())));
            batteryInfo_widget->ui->batteryTableWidget->setItem(i, 7, new QTableWidgetItem(QString::number(profilebtydata.bats(i).chargeavgtime())));
            batteryInfo_widget->ui->batteryTableWidget->setItem(i, 8, new QTableWidgetItem(QString::number(profilebtydata.bats(i).idleavgtime())));
            batteryInfo_widget->ui->batteryTableWidget->setItem(i, 9, new QTableWidgetItem(QString::number(profilebtydata.bats(i).addwaterinter())));
            batteryInfo_widget->ui->batteryTableWidget->setItem(i, 10, new QTableWidgetItem(QString::fromStdString(profilebtydata.bats(i).addwatertime())));
            batteryInfo_widget->ui->batteryTableWidget->setItem(i, 11, new QTableWidgetItem(QString::fromStdString(profilebtydata.bats(i).startusetime())));

            for(int j = 0; j <  batteryInfo_widget->ui->batteryTableWidget->columnCount(); j++)
            {
                batteryInfo_widget->ui->batteryTableWidget->item(i,j)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            }
        }
        else
        {
            batteryInfo_widget->ui->batteryTableWidget->item(i,0)->setData(Qt::DisplayRole , profilebtydata.bats(i).bty_tag());
            batteryInfo_widget->ui->batteryTableWidget->item(i,1)->setText(QString::fromStdString(profilebtydata.bats(i).bty_rfid()));
            batteryInfo_widget->ui->batteryTableWidget->item(i,2)->setText(QString::number(profilebtydata.bats(i).inagv()));
            batteryInfo_widget->ui->batteryTableWidget->item(i,3)->setText(QString::number(profilebtydata.bats(i).instation()));
            batteryInfo_widget->ui->batteryTableWidget->item(i,4)->setText(QString::number(profilebtydata.bats(i).stationspace()));
            batteryInfo_widget->ui->batteryTableWidget->item(i,5)->setText(QString::number(profilebtydata.bats(i).power()));
            batteryInfo_widget->ui->batteryTableWidget->item(i,6)->setText(QString::number(profilebtydata.bats(i).useavgtime()));
            batteryInfo_widget->ui->batteryTableWidget->item(i,7)->setText(QString::number(profilebtydata.bats(i).chargeavgtime()));
            batteryInfo_widget->ui->batteryTableWidget->item(i,8)->setText(QString::number(profilebtydata.bats(i).idleavgtime()));
            batteryInfo_widget->ui->batteryTableWidget->item(i,9)->setText(QString::number(profilebtydata.bats(i).addwaterinter()));
            batteryInfo_widget->ui->batteryTableWidget->item(i,10)->setText(QString::fromStdString(profilebtydata.bats(i).addwatertime()));
            batteryInfo_widget->ui->batteryTableWidget->item(i,11)->setText(QString::fromStdString(profilebtydata.bats(i).startusetime()));
         }


    }

}

void SubWidget::protoTcp_batteryDetailData()
{
    QByteArray allArray;
    allArray = protoTcpSocketBatteryDetail->readAll();
    protoTcpSocketBatteryDetail->disconnectFromHost();
    protoTcpSocketBatteryDetail->close();
    detailbtydata.ParseFromArray(allArray.data()+6,allArray.size()-8);
    int batsnum = detailbtydata.items_size();
    batteryInfo_widget->ui->batteryDetailWidget->setRowCount(batsnum);

    for(int i = 0; i < batsnum; i++)
    {
        if(batteryInfo_widget->ui->batteryDetailWidget->item(i,0) == nullptr)
        {
            batteryInfo_widget->ui->batteryDetailWidget->setItem(i, 0, new QTableWidgetItem());
            batteryInfo_widget->ui->batteryDetailWidget->item(i,0)->setData(Qt::DisplayRole , detailbtydata.items(i).bty_tag());

            batteryInfo_widget->ui->batteryDetailWidget->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(detailbtydata.items(i).bty_rfid())));
            batteryInfo_widget->ui->batteryDetailWidget->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(detailbtydata.items(i).evtitem())));
            batteryInfo_widget->ui->batteryDetailWidget->setItem(i, 3, new QTableWidgetItem(QString::number(detailbtydata.items(i).agv())));
            batteryInfo_widget->ui->batteryDetailWidget->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(detailbtydata.items(i).starttime())));
            batteryInfo_widget->ui->batteryDetailWidget->setItem(i, 5, new QTableWidgetItem(QString::fromStdString(detailbtydata.items(i).endtime())));
            batteryInfo_widget->ui->batteryDetailWidget->setItem(i, 6, new QTableWidgetItem(QString::fromStdString(detailbtydata.items(i).intertime())));


            for(int j = 0; j <  batteryInfo_widget->ui->batteryDetailWidget->columnCount(); j++)
            {
                batteryInfo_widget->ui->batteryDetailWidget->item(i,j)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            }
        }
        else
        {
            batteryInfo_widget->ui->batteryDetailWidget->item(i,0)->setData(Qt::DisplayRole , detailbtydata.items(i).bty_tag());
            batteryInfo_widget->ui->batteryDetailWidget->item(i,1)->setText(QString::fromStdString(detailbtydata.items(i).bty_rfid()));
            batteryInfo_widget->ui->batteryDetailWidget->item(i,2)->setText(QString::fromStdString(detailbtydata.items(i).evtitem()));
            batteryInfo_widget->ui->batteryDetailWidget->item(i,3)->setText(QString::number(detailbtydata.items(i).agv()));
            batteryInfo_widget->ui->batteryDetailWidget->item(i,4)->setText(QString::fromStdString(detailbtydata.items(i).starttime()));
            batteryInfo_widget->ui->batteryDetailWidget->item(i,5)->setText(QString::fromStdString(detailbtydata.items(i).endtime()));
            batteryInfo_widget->ui->batteryDetailWidget->item(i,6)->setText(QString::fromStdString(detailbtydata.items(i).intertime()));

         }


    }

}

void SubWidget::protoTcp_taskData()
{
    QByteArray allArray;
    allArray = protoTcpSocketTask->readAll();
    protoTcpSocketTask->disconnectFromHost();
    protoTcpSocketTask->close();
    task_data.ParseFromArray(allArray.data()+6,allArray.size()-8);
    //qDebug()<<"task num:"<<task_data.taskinfo_size();
    if(task_data.taskinfo_size()<1)
    {
        return;
    }

    int tasknum = task_data.taskinfo_size();
    taskInfoDialog->ui->tasktWidget->setRowCount(tasknum);

    for(int i = 0; i < tasknum; i++)
    {
        if(taskInfoDialog->ui->tasktWidget->item(i,0) == nullptr)
        {
            taskInfoDialog->ui->tasktWidget->setItem(i, 0, new QTableWidgetItem(QString::number(task_data.taskinfo(i).wms_id())));
            taskInfoDialog->ui->tasktWidget->setItem(i, 1, new QTableWidgetItem(QString::number(task_data.taskinfo(i).mes_id())));
            taskInfoDialog->ui->tasktWidget->setItem(i, 2, new QTableWidgetItem(QString::number(task_data.taskinfo(i).area())));
            if(task_data.taskinfo(i).status() == 0)
            {
                taskInfoDialog->ui->tasktWidget->setItem(i, 3, new QTableWidgetItem(QString("未执行")));
            }
            else if(task_data.taskinfo(i).status() == 1)
            {
                taskInfoDialog->ui->tasktWidget->setItem(i, 3, new QTableWidgetItem(QString("已执行")));
            }
            else if(task_data.taskinfo(i).status() == 2)
            {
                taskInfoDialog->ui->tasktWidget->setItem(i, 3, new QTableWidgetItem(QString("取货中")));
            }
            else if(task_data.taskinfo(i).status() == 3)
            {
                taskInfoDialog->ui->tasktWidget->setItem(i, 3, new QTableWidgetItem(QString("放货中")));
            }
            else
            {
                taskInfoDialog->ui->tasktWidget->setItem(i, 3, new QTableWidgetItem(QString::number(task_data.taskinfo(i).status())));
            }
            taskInfoDialog->ui->tasktWidget->setItem(i, 4, new QTableWidgetItem(QString::number(task_data.taskinfo(i).agv_id())));

            taskInfoDialog->ui->tasktWidget->setItem(i, 5, new QTableWidgetItem(QString("%1(%2)").arg(QString::fromStdString(task_data.taskinfo(i).start().name())).arg(QString::number(task_data.taskinfo(i).start().id()))));
            taskInfoDialog->ui->tasktWidget->setItem(i, 6, new QTableWidgetItem(QString::fromStdString(task_data. taskinfo(i).start().area_name())));
            taskInfoDialog->ui->tasktWidget->setItem(i, 7, new QTableWidgetItem(QString("%1(%2)").arg(QString::fromStdString(task_data.taskinfo(i).end().name())).arg(QString::number(task_data.taskinfo(i).end().id()))));
            taskInfoDialog->ui->tasktWidget->setItem(i, 8, new QTableWidgetItem(QString::fromStdString(task_data. taskinfo(i).end().area_name())));
            taskInfoDialog->ui->tasktWidget->setItem(i, 9, new QTableWidgetItem(QString::fromStdString(task_data. taskinfo(i).pallet_no())));
            if((long long)task_data.taskinfo(i).create_time() < 0)
            {
                taskInfoDialog->ui->tasktWidget->setItem(i, 10, new QTableWidgetItem(QString("-1")));
            }
            else if((long long)task_data.taskinfo(i).create_time() == 0)
            {
                taskInfoDialog->ui->tasktWidget->setItem(i, 10, new QTableWidgetItem(QString("0")));
            }
            else
            {
                taskInfoDialog->ui->tasktWidget->setItem(i, 10, new QTableWidgetItem(QDateTime::fromSecsSinceEpoch((long long)task_data.taskinfo(i).create_time()).toString("yyyy-MM-dd hh:mm:ss")));
            }

            if((long long)task_data.taskinfo(i).excute_time() < 0)
            {
                taskInfoDialog->ui->tasktWidget->setItem(i, 11, new QTableWidgetItem(QString("-1")));
            }
            else if((long long)task_data.taskinfo(i).excute_time() == 0)
            {
                taskInfoDialog->ui->tasktWidget->setItem(i, 11, new QTableWidgetItem(QString("0")));
            }
            else
            {
                taskInfoDialog->ui->tasktWidget->setItem(i, 11, new QTableWidgetItem(QDateTime::fromSecsSinceEpoch((long long)task_data.taskinfo(i).excute_time()).toString("yyyy-MM-dd hh:mm:ss")));
            }

            if((long long)task_data.taskinfo(i).get_time() < 0)
            {
                taskInfoDialog->ui->tasktWidget->setItem(i, 12, new QTableWidgetItem(QString("-1")));
            }
            else if((long long)task_data.taskinfo(i).get_time() == 0)
            {
                taskInfoDialog->ui->tasktWidget->setItem(i, 12, new QTableWidgetItem(QString("0")));
            }
            else
            {
                taskInfoDialog->ui->tasktWidget->setItem(i, 12, new QTableWidgetItem(QDateTime::fromSecsSinceEpoch((long long)task_data.taskinfo(i).get_time()).toString("yyyy-MM-dd hh:mm:ss")));
            }
            taskInfoDialog->ui->tasktWidget->setItem(i, 13, new QTableWidgetItem(QString::number(task_data.taskinfo(i).priority())));
            taskInfoDialog->ui->tasktWidget->setItem(i, 14, new QTableWidgetItem(QString::number(task_data.taskinfo(i).agv_expect())));

            if(task_data.taskinfo(i).status() == 1)
            {
                taskInfoDialog->ui->tasktWidget->setItem(i, 15, new QTableWidgetItem(QString("-")));
            }
            else
            {
                switch (task_data.taskinfo(i).delay_type()) {
                case 0:
                    taskInfoDialog->ui->tasktWidget->setItem(i, 15, new QTableWidgetItem(QString("无延迟")));
                    break;
                case 11:
                    taskInfoDialog->ui->tasktWidget->setItem(i, 15, new QTableWidgetItem(QString("暂不执行,取放货位置相同的重复任务")));
                    break;
                case 12:
                    taskInfoDialog->ui->tasktWidget->setItem(i, 15, new QTableWidgetItem(QString("暂不执行,AGV在执行此库位的其他任务")));
                    break;
                case 21:
                    taskInfoDialog->ui->tasktWidget->setItem(i, 15, new QTableWidgetItem(QString("暂停放货,取货位置后置光电显示有货")));
                    break;
                case 22:
                    taskInfoDialog->ui->tasktWidget->setItem(i, 15, new QTableWidgetItem(QString("暂停放货,放货位置后置光电显示有货")));
                    break;
                case 31:
                    taskInfoDialog->ui->tasktWidget->setItem(i, 15, new QTableWidgetItem(QString("暂不执行,托盘库位无库存")));
                    break;
                case 41:
                    taskInfoDialog->ui->tasktWidget->setItem(i, 15, new QTableWidgetItem(QString("暂不执行,库位任务延迟执行")));
                    break;
                default:
                    taskInfoDialog->ui->tasktWidget->setItem(i, 15, new QTableWidgetItem(QString::number(task_data.taskinfo(i).delay_type())));
                    break;
                }

            }

            for(int j = 0; j <  taskInfoDialog->ui->tasktWidget->columnCount(); j++)
            {
                taskInfoDialog->ui->tasktWidget->item(i,j)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            }
        }
        else
        {
            taskInfoDialog->ui->tasktWidget->item(i,0)->setText(QString::number(task_data.taskinfo(i).wms_id()));
            taskInfoDialog->ui->tasktWidget->item(i,1)->setText(QString::number(task_data.taskinfo(i).mes_id()));
            taskInfoDialog->ui->tasktWidget->item(i,2)->setText(QString::number(task_data.taskinfo(i).area()));
            if(task_data.taskinfo(i).status() == 0)
            {
                taskInfoDialog->ui->tasktWidget->item(i,3)->setText(QString("未执行"));
            }
            else if(task_data.taskinfo(i).status() == 1)
            {
                taskInfoDialog->ui->tasktWidget->item(i,3)->setText(QString("已执行"));
            }
            else if(task_data.taskinfo(i).status() == 2)
            {
                taskInfoDialog->ui->tasktWidget->item(i,3)->setText(QString("取货中"));
            }
            else if(task_data.taskinfo(i).status() == 3)
            {
                taskInfoDialog->ui->tasktWidget->item(i,3)->setText(QString("放货中"));
            }
            else
            {
                taskInfoDialog->ui->tasktWidget->item(i,3)->setText(QString::number(task_data.taskinfo(i).status()));
            }
            taskInfoDialog->ui->tasktWidget->item(i,4)->setText(QString::number(task_data.taskinfo(i).agv_id()));
            taskInfoDialog->ui->tasktWidget->item(i,5)->setText(QString("%1(%2)").arg(QString::fromStdString(task_data.taskinfo(i).start().name())).arg(QString::number(task_data.taskinfo(i).start().id())));
            taskInfoDialog->ui->tasktWidget->item(i,6)->setText(QString::fromStdString(task_data. taskinfo(i).start().area_name()));
            taskInfoDialog->ui->tasktWidget->item(i,7)->setText(QString("%1(%2)").arg(QString::fromStdString(task_data.taskinfo(i).end().name())).arg(QString::number(task_data.taskinfo(i).end().id())));
            taskInfoDialog->ui->tasktWidget->item(i,8)->setText(QString::fromStdString(task_data. taskinfo(i).end().area_name()));
            taskInfoDialog->ui->tasktWidget->item(i,9)->setText(QString::fromStdString(task_data.taskinfo(i).pallet_no()));

            if((long long)task_data.taskinfo(i).create_time() < 0)
            {
                taskInfoDialog->ui->tasktWidget->item(i,10)->setText(QString("-1"));
            }
            else if((long long)task_data.taskinfo(i).create_time() == 0)
            {
                taskInfoDialog->ui->tasktWidget->item(i,10)->setText(QString("0"));
            }
            else
            {
                taskInfoDialog->ui->tasktWidget->item(i,10)->setText(QDateTime::fromSecsSinceEpoch((long long)task_data.taskinfo(i).create_time()).toString("yyyy-MM-dd hh:mm:ss"));
            }

            if((long long)task_data.taskinfo(i).excute_time() < 0)
            {
                taskInfoDialog->ui->tasktWidget->item(i,11)->setText(QString("-1"));
            }
            else if((long long)task_data.taskinfo(i).excute_time() == 0)
            {
                taskInfoDialog->ui->tasktWidget->item(i,11)->setText(QString("0"));
            }
            else
            {
                taskInfoDialog->ui->tasktWidget->item(i,11)->setText(QDateTime::fromSecsSinceEpoch((long long)task_data.taskinfo(i).excute_time()).toString("yyyy-MM-dd hh:mm:ss"));
            }

            if((long long)task_data.taskinfo(i).get_time() < 0)
            {
                taskInfoDialog->ui->tasktWidget->item(i,12)->setText(QString("-1"));
            }
            else if((long long)task_data.taskinfo(i).get_time() == 0)
            {
                taskInfoDialog->ui->tasktWidget->item(i,12)->setText(QString("0"));
            }
            else
            {
                taskInfoDialog->ui->tasktWidget->item(i,12)->setText(QDateTime::fromSecsSinceEpoch((long long)task_data.taskinfo(i).get_time()).toString("yyyy-MM-dd hh:mm:ss"));
            }
            taskInfoDialog->ui->tasktWidget->item(i,13)->setText(QString::number(task_data.taskinfo(i).priority()));
            taskInfoDialog->ui->tasktWidget->item(i,14)->setText(QString::number(task_data.taskinfo(i).agv_expect()));
            if(task_data.taskinfo(i).status() == 1)
            {
                taskInfoDialog->ui->tasktWidget->item(i,15)->setText(QString("-"));

            }
            else
            {
                switch (task_data.taskinfo(i).delay_type()) {
                case 0:
                    taskInfoDialog->ui->tasktWidget->item(i,15)->setText(QString("无延迟"));
                    break;
                case 11:
                    taskInfoDialog->ui->tasktWidget->item(i,15)->setText(QString("暂不执行,取放货位置相同的重复任务"));
                    break;
                case 12:
                    taskInfoDialog->ui->tasktWidget->item(i,15)->setText(QString("暂不执行,AGV在执行此库位的其他任务"));
                    break;
                case 21:
                    taskInfoDialog->ui->tasktWidget->item(i,15)->setText(QString("暂停放货,取货位置后置光电显示有货"));
                    break;
                case 22:
                    taskInfoDialog->ui->tasktWidget->item(i,15)->setText(QString("暂停放货,放货位置后置光电显示有货"));
                    break;
                case 31:
                    taskInfoDialog->ui->tasktWidget->item(i,15)->setText(QString("暂不执行,托盘库位无库存"));
                    break;
                case 41:
                    taskInfoDialog->ui->tasktWidget->item(i,15)->setText(QString("暂不执行,库位任务延迟执行"));
                    break;
                default:
                    taskInfoDialog->ui->tasktWidget->item(i,15)->setText(QString::number(task_data.taskinfo(i).delay_type()));
                    break;
                }

            }

        }
    }

    getTaskData_timer->stop();
    taskInfoDialog->ui->getTaskButton->setEnabled(true);

}

void SubWidget::protoTcp_cxjData()
{
    QByteArray allArray;
    allArray = protoTcpSocketCxjData->readAll();
    protoTcpSocketCxjData->disconnectFromHost();
    protoTcpSocketCxjData->close();
    dev_data.ParseFromArray(allArray.data()+6,allArray.size()-8);

    int cxjDataNum = dev_data.cxjdata_size();
    if(cxjDataNum < 1)
    {
        cxjDataInfoDialog->ui->querylabel->setText("没有可用数据");
    }
    cxjDataInfoDialog->ui->cxjDatatWidget->setRowCount(cxjDataNum);

    for(int i = 0; i < cxjDataNum; i++)
    {
        if(cxjDataInfoDialog->ui->cxjDatatWidget->item(i,0) == nullptr)
        {
            cxjDataInfoDialog->ui->cxjDatatWidget->setItem(i, 0, new QTableWidgetItem());
            cxjDataInfoDialog->ui->cxjDatatWidget->item(i,0)->setData(Qt::DisplayRole , dev_data.cxjdata(i).cxjid());
            if(dev_data.cxjdata(i).cxjtype() == 0)
            {
                cxjDataInfoDialog->ui->cxjDatatWidget->setItem(i, 1, new QTableWidgetItem(QString("空盘")));
            }
            else if(dev_data.cxjdata(i).cxjtype() == 1)
            {
                cxjDataInfoDialog->ui->cxjDatatWidget->setItem(i, 1, new QTableWidgetItem(QString("实盘")));
            }
            else if(dev_data.cxjdata(i).cxjtype() == 2)
            {
                cxjDataInfoDialog->ui->cxjDatatWidget->setItem(i, 1, new QTableWidgetItem(QString("二合一")));
            }
            else
            {
                cxjDataInfoDialog->ui->cxjDatatWidget->setItem(i, 1, new QTableWidgetItem(QString::number(dev_data.cxjdata(i).cxjtype())));
            }
            cxjDataInfoDialog->ui->cxjDatatWidget->setItem(i, 2, new QTableWidgetItem(QString::number(dev_data.cxjdata(i).storeid())));
            if(dev_data.cxjdata(i).goods_sig() == 0)
            {
                if(dev_data.cxjdata(i).cxjtype() == 0)
                {
                    cxjDataInfoDialog->ui->cxjDatatWidget->setItem(i, 3, new QTableWidgetItem(QString("有空盘")));
                }
                else if(dev_data.cxjdata(i).cxjtype() == 1)
                {
                    cxjDataInfoDialog->ui->cxjDatatWidget->setItem(i, 3, new QTableWidgetItem(QString("有实盘")));
                }
                else
                {
                    cxjDataInfoDialog->ui->cxjDatatWidget->setItem(i, 3, new QTableWidgetItem(QString::number(dev_data.cxjdata(i).cxjtype())));
                }
            }
            else if(dev_data.cxjdata(i).goods_sig() == 1)
            {
                if(dev_data.cxjdata(i).cxjtype() == 0)
                {
                    cxjDataInfoDialog->ui->cxjDatatWidget->setItem(i, 3, new QTableWidgetItem(QString("无空盘")));
                }
                else if(dev_data.cxjdata(i).cxjtype() == 1)
                {
                    cxjDataInfoDialog->ui->cxjDatatWidget->setItem(i, 3, new QTableWidgetItem(QString("无实盘")));
                }
                else
                {
                    cxjDataInfoDialog->ui->cxjDatatWidget->setItem(i, 3, new QTableWidgetItem(QString::number(dev_data.cxjdata(i).cxjtype())));
                }
            }
            else
            {
                cxjDataInfoDialog->ui->cxjDatatWidget->setItem(i, 3, new QTableWidgetItem(QString::number(dev_data.cxjdata(i).goods_sig())));
            }
            if(dev_data.cxjdata(i).inright_sig() == 0)
            {
                cxjDataInfoDialog->ui->cxjDatatWidget->setItem(i, 4, new QTableWidgetItem(QString("未到位")));
            }
            else if(dev_data.cxjdata(i).inright_sig() == 1)
            {
                cxjDataInfoDialog->ui->cxjDatatWidget->setItem(i, 4, new QTableWidgetItem(QString("到位")));
            }
            else if(dev_data.cxjdata(i).inright_sig() == 2)
            {
                cxjDataInfoDialog->ui->cxjDatatWidget->setItem(i, 4, new QTableWidgetItem(QString("无平台")));
            }
            else
            {
                cxjDataInfoDialog->ui->cxjDatatWidget->setItem(i, 4, new QTableWidgetItem(QString::number(dev_data.cxjdata(i).inright_sig())));
            }
            cxjDataInfoDialog->ui->cxjDatatWidget->setItem(i, 5, new QTableWidgetItem(QString::number(dev_data.cxjdata(i).all_sig())));
            if(dev_data.cxjdata(i).agv_in_flag() == 0)
            {
                cxjDataInfoDialog->ui->cxjDatatWidget->setItem(i, 6, new QTableWidgetItem(QString("占用")));
            }
           else if(dev_data.cxjdata(i).agv_in_flag() == 1)
            {
                cxjDataInfoDialog->ui->cxjDatatWidget->setItem(i, 6, new QTableWidgetItem(QString("未占用")));
            }
            else
            {
                cxjDataInfoDialog->ui->cxjDatatWidget->setItem(i, 6, new QTableWidgetItem(QString::number(dev_data.cxjdata(i).agv_in_flag())));
            }
            cxjDataInfoDialog->ui->cxjDatatWidget->setItem(i, 7, new QTableWidgetItem(QString::number(dev_data.cxjdata(i).cxj_heart())));
            if(dev_data.cxjdata(i).agv_id() == -1)
            {
                cxjDataInfoDialog->ui->cxjDatatWidget->setItem(i, 8, new QTableWidgetItem(QString("无")));
            }
            else
            {
                cxjDataInfoDialog->ui->cxjDatatWidget->setItem(i, 8, new QTableWidgetItem(QString::number(dev_data.cxjdata(i).agv_id())));
            }
            cxjDataInfoDialog->ui->cxjDatatWidget->setItem(i, 9, new QTableWidgetItem(QString::number(dev_data.cxjdata(i).wms_task())));

            for(int j = 0; j <  cxjDataInfoDialog->ui->cxjDatatWidget->columnCount(); j++)
            {
                cxjDataInfoDialog->ui->cxjDatatWidget->item(i,j)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            }
        }
        else  //setText
        {
            cxjDataInfoDialog->ui->cxjDatatWidget->item(i,0)->setData(Qt::DisplayRole , dev_data.cxjdata(i).cxjid());
            if(dev_data.cxjdata(i).cxjtype() == 0)
            {
                cxjDataInfoDialog->ui->cxjDatatWidget->item(i,1)->setText(QString("空盘"));
            }
            else if(dev_data.cxjdata(i).cxjtype() == 1)
            {
                cxjDataInfoDialog->ui->cxjDatatWidget->item(i,1)->setText(QString("实盘"));
            }
            else if(dev_data.cxjdata(i).cxjtype() == 2)
            {
                cxjDataInfoDialog->ui->cxjDatatWidget->item(i,1)->setText(QString("二合一"));
            }
            else
            {
                cxjDataInfoDialog->ui->cxjDatatWidget->item(i,1)->setText(QString::number(dev_data.cxjdata(i).cxjtype()));
            }
            cxjDataInfoDialog->ui->cxjDatatWidget->item(i,2)->setText(QString::number(dev_data.cxjdata(i).storeid()));

            if(dev_data.cxjdata(i).goods_sig() == 0)
            {
                if(dev_data.cxjdata(i).cxjtype() == 0)
                {
                    cxjDataInfoDialog->ui->cxjDatatWidget->item(i,3)->setText(QString("有空盘"));
                }
                else if(dev_data.cxjdata(i).cxjtype() == 1)
                {
                    cxjDataInfoDialog->ui->cxjDatatWidget->item(i,3)->setText(QString("有实盘"));
                }
                else
                {
                    cxjDataInfoDialog->ui->cxjDatatWidget->item(i,3)->setText(QString::number(dev_data.cxjdata(i).cxjtype()));
                }
            }
            else if(dev_data.cxjdata(i).goods_sig() == 1)
            {
                if(dev_data.cxjdata(i).cxjtype() == 0)
                {
                    cxjDataInfoDialog->ui->cxjDatatWidget->item(i,3)->setText(QString("无空盘"));
                }
                else if(dev_data.cxjdata(i).cxjtype() == 1)
                {
                    cxjDataInfoDialog->ui->cxjDatatWidget->item(i,3)->setText(QString("无实盘"));
                }
                else
                {
                    cxjDataInfoDialog->ui->cxjDatatWidget->item(i,3)->setText(QString::number(dev_data.cxjdata(i).cxjtype()));
                }
            }
            else
            {
                cxjDataInfoDialog->ui->cxjDatatWidget->item(i,3)->setText(QString::number(dev_data.cxjdata(i).goods_sig()));
            }
            if(dev_data.cxjdata(i).inright_sig() == 0)
            {
                cxjDataInfoDialog->ui->cxjDatatWidget->item(i,4)->setText(QString("未到位"));
            }
            else if(dev_data.cxjdata(i).inright_sig() == 1)
            {
                cxjDataInfoDialog->ui->cxjDatatWidget->item(i,4)->setText(QString("到位"));
            }
            else if(dev_data.cxjdata(i).inright_sig() == 2)
            {
                cxjDataInfoDialog->ui->cxjDatatWidget->item(i,4)->setText(QString("无平台"));
            }
            else
            {
                cxjDataInfoDialog->ui->cxjDatatWidget->item(i,4)->setText(QString::number(dev_data.cxjdata(i).inright_sig()));
            }
            cxjDataInfoDialog->ui->cxjDatatWidget->item(i,5)->setText(QString::number(dev_data.cxjdata(i).all_sig()));
            if(dev_data.cxjdata(i).agv_in_flag() == 0)
            {
                cxjDataInfoDialog->ui->cxjDatatWidget->item(i,6)->setText(QString("占用"));

            }
           else if(dev_data.cxjdata(i).agv_in_flag() == 1)
            {
                cxjDataInfoDialog->ui->cxjDatatWidget->item(i,6)->setText(QString("未占用"));
            }
            else
            {
                cxjDataInfoDialog->ui->cxjDatatWidget->item(i,6)->setText(QString::number(dev_data.cxjdata(i).agv_in_flag()));
            }
            cxjDataInfoDialog->ui->cxjDatatWidget->item(i,7)->setText(QString::number(dev_data.cxjdata(i).cxj_heart()));
            if(dev_data.cxjdata(i).agv_id() == -1)
            {
                cxjDataInfoDialog->ui->cxjDatatWidget->item(i,8)->setText(QString("无"));
            }
            else
            {
                cxjDataInfoDialog->ui->cxjDatatWidget->item(i,8)->setText(QString::number(dev_data.cxjdata(i).agv_id()));
            }
            cxjDataInfoDialog->ui->cxjDatatWidget->item(i,9)->setText(QString::number(dev_data.cxjdata(i).wms_task()));

        }
    }
}

void SubWidget::protoTcp_cxjBack()
{
    QByteArray allArray;
    allArray = protoTcpSocketCxjBack->readAll();
    protoTcpSocketCxjBack->disconnectFromHost();
    protoTcpSocketCxjBack->close();
    dev_data.ParseFromArray(allArray.data()+6,allArray.size()-8);

    int cxjBackNum = dev_data.cxjback_size();
    if(cxjBackNum < 1)
    {
        cxjBackInfoDialog->ui->querylabel->setText("没有可用数据");
    }
    cxjBackInfoDialog->ui->cxjBacktWidget->setRowCount(cxjBackNum);

    for(int i = 0; i < cxjBackNum; i++)
    {
        if(cxjBackInfoDialog->ui->cxjBacktWidget->item(i,0) == nullptr)
        {
            cxjBackInfoDialog->ui->cxjBacktWidget->setItem(i, 0, new QTableWidgetItem());
            cxjBackInfoDialog->ui->cxjBacktWidget->item(i,0)->setData(Qt::DisplayRole , dev_data.cxjback(i).storeid());
            cxjBackInfoDialog->ui->cxjBacktWidget->setItem(i, 1, new QTableWidgetItem(QString::number(dev_data.cxjback(i).area())));
            if(dev_data.cxjback(i).type() == 0)
            {
                cxjBackInfoDialog->ui->cxjBacktWidget->setItem(i, 2, new QTableWidgetItem(QString("立库取放位")));

            }
            else if(dev_data.cxjback(i).type() == 1)
            {
                cxjBackInfoDialog->ui->cxjBacktWidget->setItem(i, 2, new QTableWidgetItem(QString("RFID回空位")));

            }
            else if(dev_data.cxjback(i).type() == 2)
            {
                cxjBackInfoDialog->ui->cxjBacktWidget->setItem(i, 2, new QTableWidgetItem(QString("光电回空位")));

            }
            else
            {
                cxjBackInfoDialog->ui->cxjBacktWidget->setItem(i, 2, new QTableWidgetItem(QString::number(dev_data.cxjback(i).type())));
            }
            cxjBackInfoDialog->ui->cxjBacktWidget->setItem(i, 3, new QTableWidgetItem(QString::number(dev_data.cxjback(i).task_no())));
            cxjBackInfoDialog->ui->cxjBacktWidget->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(dev_data.cxjback(i).rfid())));
            cxjBackInfoDialog->ui->cxjBacktWidget->setItem(i, 5, new QTableWidgetItem(IPV4IntegerToString((quint32)dev_data.cxjback(i).plcip())));
            cxjBackInfoDialog->ui->cxjBacktWidget->setItem(i, 6, new QTableWidgetItem(QDateTime::fromSecsSinceEpoch(dev_data.cxjback(i).ts()).toString("yyyy-MM-dd hh:mm:ss")));

            for(int j = 0; j <  cxjBackInfoDialog->ui->cxjBacktWidget->columnCount(); j++)
            {
                cxjBackInfoDialog->ui->cxjBacktWidget->item(i,j)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            }
        }
        else
        {
            cxjBackInfoDialog->ui->cxjBacktWidget->item(i,0)->setData(Qt::DisplayRole , dev_data.cxjback(i).storeid());
            cxjBackInfoDialog->ui->cxjBacktWidget->item(i,1)->setText(QString::number(dev_data.cxjback(i).area()));
            if(dev_data.cxjback(i).type() == 0)
            {
                cxjBackInfoDialog->ui->cxjBacktWidget->item(i,2)->setText(QString("立库取放位"));
            }
            else if(dev_data.cxjback(i).type() == 1)
            {
                cxjBackInfoDialog->ui->cxjBacktWidget->item(i,2)->setText(QString("RFID回空位"));
            }
            else if(dev_data.cxjback(i).type() == 2)
            {
                cxjBackInfoDialog->ui->cxjBacktWidget->item(i,2)->setText(QString("光电回空位"));
            }
            else
            {
                cxjBackInfoDialog->ui->cxjBacktWidget->item(i,2)->setText(QString::number(dev_data.cxjback(i).type()));
            }
            cxjBackInfoDialog->ui->cxjBacktWidget->item(i,3)->setText(QString::number(dev_data.cxjback(i).task_no()));
            cxjBackInfoDialog->ui->cxjBacktWidget->item(i,4)->setText(QString::fromStdString(dev_data.cxjback(i).rfid()));
            cxjBackInfoDialog->ui->cxjBacktWidget->item(i,5)->setText(IPV4IntegerToString((quint32)dev_data.cxjback(i).plcip()));
            cxjBackInfoDialog->ui->cxjBacktWidget->item(i,6)->setText(QDateTime::fromSecsSinceEpoch(dev_data.cxjback(i).ts()).toString("yyyy-MM-dd hh:mm:ss"));

        }
    }
}
void SubWidget::protoTcp_routeData()
{
    route_list.clear();
    QByteArray allArray;
    allArray = protoTcpSocketRoute->readAll();
    protoTcpSocketRoute->disconnectFromHost();
    protoTcpSocketRoute->close();
    route_data.ParseFromArray(allArray.data()+6,allArray.size()-8);
    //qDebug()<<"1agvroute_size"<<route_data.agvroute_size();
    if(route_data.agvroute_size() < 1)
    {
        QMessageBox::information(this," ","该车没有路径");
        return;
    }

    int route_num = route_data.agvroute(0).route_id_size();
    if(route_num < 1)
    {
        return;
    }
    int cur_id;
    int nxt_id;

    //qDebug()<<"2 route_num"<<route_num;
    for(int i = 0; i < route_num; i++)
    {
        route_list.push_back(route_data.agvroute(0).route_id(i));
    }
    cur_id = route_data.agvroute(0).cur_seg();
    nxt_id = route_data.agvroute(0).nxt_seg();
    //qDebug()<<"3 cur_id"<<cur_id;


    protoTcpSocketRoute->disconnectFromHost();
    protoTcpSocketRoute->close();
    for(int j = 0 ; j < vec_LineItem.size(); j++)
    {
        if(vec_LineItem[j] != nullptr)
        {
            vec_LineItem[j]->route_flag = 0;
        }
    }
    for(int j = 0 ; j < vec_ArcItem.size(); j++)
    {
        if(vec_ArcItem[j] != nullptr)
        {
            vec_ArcItem[j]->route_flag = 0;
        }
    }


    for(int i = 0; i < route_num; i++)
    {
        if(route_list[i] >= vec_LineItem.size() || route_list[i] < 1)
        {
            continue;
        }

        if(vec_LineItem[route_list[i]] != nullptr)
        {
            if(route_list[i] == cur_id)
            {
                vec_LineItem[route_list[i]]->route_flag = 2;
            }
            else if(route_list[i] == nxt_id)
            {
                vec_LineItem[route_list[i]]->route_flag = 3;
            }
            else
            {
                vec_LineItem[route_list[i]]->route_flag = 1;
            }
        }
        else if(vec_ArcItem[route_list[i]] != nullptr)
        {
            if(route_list[i] == cur_id)
            {
                vec_ArcItem[route_list[i]]->route_flag = 2;
            }
            else if(route_list[i] == nxt_id)
            {
                vec_ArcItem[route_list[i]]->route_flag = 3;
            }
            else
            {
                vec_ArcItem[route_list[i]]->route_flag = 1;
            }
        }
        else
        {
        }
    }
    ui->graphicsView->scene()->update();
    //qDebug()<<"4 agvroute_size"<<route_data.agvroute_size();

}

/**
 * @brief SubWidget::agv_track_show
 * graphicsView centerOn 跟踪车
 * @param mon_data
 */
void SubWidget::agv_track_show(agv_mon::Mon_Data &mon_data)
{
    if(agv_track_flag < 1)
        return;
    qreal x = 0,  y = 0,  w = 20000,  h = 15000;
    int carNum = mon_data.agvinfor_size();
    if(carNum < 1)
    {
        return;
    }
    int agv_id = -1;
    for(int i = 0 ; i < carNum; i++)
    {
        if(mon_data.agvinfor(i).agvreport().basic().id() == agv_track_flag)
        {
            agv_id = i;
            break;
        }
    }
    if(agv_id < 0) return;

    x = mon_data.agvinfor(agv_id).agvreport().agvloc().pos().x();
    y = mon_data.agvinfor(agv_id).agvreport().agvloc().pos().y();

    ui->graphicsView->centerOn(x,y);
}

void SubWidget::update_lift_tableWidget(agv_mon::Mon_Data &mon_data)
{
    int lift_cnt = mon_data.liftinfor_size();
    if(lift_cnt != last_lift_cnt)
    {
        ui->liftInfo_tableWidget->clearContents();
        ui->liftInfo_tableWidget->setRowCount(lift_cnt);
        ui->liftInfo_tableWidget->setColumnWidth(11, 120);
        ui->liftInfo_tableWidget->setColumnWidth(12, 130);

        for(int i = 0; i < lift_cnt; i++)
        {
            QTableWidgetItem    *item;
            item = new  QTableWidgetItem(QString("%1(%2)").arg(mon_data.liftinfor(i).lift_group()).arg(mon_data.liftinfor(i).lift_idx()));
            item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->liftInfo_tableWidget->setItem(i,0,item);

            item = new  QTableWidgetItem(QString("%1层").arg(mon_data.liftinfor(i).curfloor()));
            item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->liftInfo_tableWidget->setItem(i,1,item);
            QString runsignal = QString("未知");
            if(mon_data.liftinfor(i).runsignal() == 0)
                runsignal = QString("停靠");
            else if(mon_data.liftinfor(i).runsignal() == 1)
                runsignal = QString("上行");
            else if(mon_data.liftinfor(i).runsignal() == 2)
                runsignal = QString("下行");
            item = new  QTableWidgetItem(runsignal);
            item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->liftInfo_tableWidget->setItem(i,2,item);

            QString lift_agv = QString("无");
            if(mon_data.liftinfor(i).lift_agv() != -1)
                lift_agv = QString("%1号").arg(mon_data.liftinfor(i).lift_agv());
            item = new  QTableWidgetItem(lift_agv);
            item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->liftInfo_tableWidget->setItem(i,3,item);

            QString startfloor = QString("无");
            if(mon_data.liftinfor(i).startfloor() != -1)
                startfloor = QString("%1层").arg(mon_data.liftinfor(i).startfloor());
            item = new  QTableWidgetItem(startfloor);
            item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->liftInfo_tableWidget->setItem(i,4,item);

            QString endfloor = QString("无");
            if(mon_data.liftinfor(i).endfloor() != -1)
                endfloor = QString("%1层").arg(mon_data.liftinfor(i).endfloor());
            item = new  QTableWidgetItem(endfloor);
            item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->liftInfo_tableWidget->setItem(i,5,item);


            QString openinplace = QString("未知");
            if(mon_data.liftinfor(i).openinplace() == 0)
                openinplace = QString("非开门");
            else if(mon_data.liftinfor(i).openinplace() == 1)
                openinplace = QString("开门");
            item = new  QTableWidgetItem(QString("%1").arg(openinplace));
            item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->liftInfo_tableWidget->setItem(i,6,item);

            QString closeinplace = QString("未知");
            if(mon_data.liftinfor(i).closeinplace() == 0)
                closeinplace = QString("非关门");
            else if(mon_data.liftinfor(i).closeinplace() == 1)
                closeinplace = QString("关门");
            item = new  QTableWidgetItem(QString("%1").arg(closeinplace));
            item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->liftInfo_tableWidget->setItem(i,7,item);

            QString idlewait = QString("未知");
            if(mon_data.liftinfor(i).idlewait() == 0)
                idlewait = QString("非空闲");
            else if(mon_data.liftinfor(i).idlewait() == 1)
                idlewait = QString("空闲");
            item = new  QTableWidgetItem(QString("%1").arg(idlewait));
            item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->liftInfo_tableWidget->setItem(i,8,item);

            QString incall_floor = "";
            if(mon_data.liftinfor(i).incall_floor() == 0)
                incall_floor = "无内呼";
            else
            {
                QString incall_floor_str = qPrintable(QString::number(mon_data.liftinfor(i).incall_floor(),2));
                int get = 0;
                int len = incall_floor_str.size();
                for (int i = 0; i < len; ++i)
                {
                    if(incall_floor_str.at(len-i-1) == "1")
                    {
                        int floor = i+1;
                        //qDebug()<<"1"<<floor;
                        if(get == 0)
                        {
                            get = 1;
                            incall_floor.append(QString("%1").arg(floor));
                        }else
                        {
                            incall_floor.append(".");
                            incall_floor.append(QString("%1").arg(floor));
                        }

                    }
                }
                incall_floor.append("内呼");
            }
            item = new  QTableWidgetItem(QString("%1").arg(incall_floor));
            item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->liftInfo_tableWidget->setItem(i,9,item);

            QString outcall_floor = "";
            if(mon_data.liftinfor(i).outcall_floor() == 0)
                outcall_floor = "无外呼";
            else
            {
                QString outcall_floor_str = qPrintable(QString::number(mon_data.liftinfor(i).outcall_floor(),2));
                int get = 0;
                int len = outcall_floor_str.size();
                for (int i = 0; i < len; ++i)
                {
                    if(outcall_floor_str.at(len-i-1) == "1")
                    {
                        //奇数下行 偶数上行
                        //i/2+1
                        int floor = i/2+1;
                        if(get == 0)
                        {
                            get = 1;
                            outcall_floor.append(QString("%1").arg(floor));
                            if(i%2 == 0)
                                outcall_floor.append("上行");
                            else
                                outcall_floor.append("下行");
                        }else
                        {
                            outcall_floor.append(".");
                            outcall_floor.append(QString("%1").arg(floor));
                            if(i%2 == 0)
                                outcall_floor.append("上行");
                            else
                                outcall_floor.append("下行");
                        }
                    }
                }

                //incall_floor.append(outcall_floor_str);
            }
            item = new  QTableWidgetItem(QString("%1").arg(outcall_floor));
            item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->liftInfo_tableWidget->setItem(i,10,item);

            item = new  QTableWidgetItem(QString(mon_data.liftinfor(i).ip().c_str()));
            item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->liftInfo_tableWidget->setItem(i,11,item);

            item = new  QTableWidgetItem(QDateTime::fromMSecsSinceEpoch(mon_data.liftinfor(i).ts()).toString("yyyy-MM-dd hh:mm:ss"));
            item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->liftInfo_tableWidget->setItem(i,12,item);
        }
    }
    else
    {
        for(int i = 0; i < lift_cnt; i++)
        {
            ui->liftInfo_tableWidget->item(i,0)->setText(QString("%1(%2)").arg(mon_data.liftinfor(i).lift_group()).arg(mon_data.liftinfor(i).lift_idx()));

            QString curfloor = QString("无");
            if(mon_data.liftinfor(i).curfloor() != -1)
                curfloor = QString("%1层").arg(mon_data.liftinfor(i).curfloor());
            ui->liftInfo_tableWidget->item(i,1)->setText(curfloor);

            QString runsignal = QString("未知");
            if(mon_data.liftinfor(i).runsignal() == 0)
                runsignal = QString("停靠");
            else if(mon_data.liftinfor(i).runsignal() == 1)
                runsignal = QString("上行");
            else if(mon_data.liftinfor(i).runsignal() == 2)
                runsignal = QString("下行");
            ui->liftInfo_tableWidget->item(i,2)->setText(runsignal);

            QString lift_agv = QString("无");
            if(mon_data.liftinfor(i).lift_agv() != -1)
                lift_agv = QString("%1号").arg(mon_data.liftinfor(i).lift_agv());
            ui->liftInfo_tableWidget->item(i,3)->setText(lift_agv);

            QString startfloor = QString("无");
            if(mon_data.liftinfor(i).startfloor() != -1)
                startfloor = QString("%1层").arg(mon_data.liftinfor(i).startfloor());
            ui->liftInfo_tableWidget->item(i,4)->setText(startfloor);

            QString endfloor = QString("无");
            if(mon_data.liftinfor(i).endfloor() != -1)
                endfloor = QString("%1层").arg(mon_data.liftinfor(i).endfloor());
            ui->liftInfo_tableWidget->item(i,5)->setText(endfloor);
            QString openinplace = QString("未知");
            if(mon_data.liftinfor(i).openinplace() == 0)
                openinplace = QString("非开门");
            else if(mon_data.liftinfor(i).openinplace() == 1)
                openinplace = QString("开门");
            ui->liftInfo_tableWidget->item(i,6)->setText(QString("%1").arg(openinplace));
            QString closeinplace = QString("未知");
            if(mon_data.liftinfor(i).closeinplace() == 0)
                closeinplace = QString("非关门");
            else if(mon_data.liftinfor(i).closeinplace() == 1)
                closeinplace = QString("关门");
            ui->liftInfo_tableWidget->item(i,7)->setText(QString("%1").arg(closeinplace));
            QString idlewait = QString("未知");
            if(mon_data.liftinfor(i).idlewait() == 0)
                idlewait = QString("非空闲");
            else if(mon_data.liftinfor(i).idlewait() == 1)
                idlewait = QString("空闲");
            ui->liftInfo_tableWidget->item(i,8)->setText(QString("%1").arg(idlewait));

            QString incall_floor = "";
            if(mon_data.liftinfor(i).incall_floor() == 0)
                incall_floor = "无内呼";
            else
            {
                QString incall_floor_str = qPrintable(QString::number(mon_data.liftinfor(i).incall_floor(),2));
                int get = 0;
                int len = incall_floor_str.size();
                for (int i = 0; i < len; ++i)
                {
                    if(incall_floor_str.at(len-i-1) == "1")
                    {
                        int floor = i+1;
                        //qDebug()<<"2 "<<floor<<" "<<i<<" "<<incall_floor_str;
                        if(get == 0)
                        {
                            get = 1;
                            incall_floor.append(QString("%1").arg(floor));
                        }else
                        {
                            incall_floor.append(".");
                            incall_floor.append(QString("%1").arg(floor));
                        }
                    }
                }

                incall_floor.append("内呼");
                //incall_floor.append(incall_floor_str);
            }
            ui->liftInfo_tableWidget->item(i,9)->setText(QString("%1").arg(incall_floor));

            QString outcall_floor = "";
            if(mon_data.liftinfor(i).outcall_floor() == 0)
                outcall_floor = "无外呼";
            else
            {
                QString outcall_floor_str = qPrintable(QString::number(mon_data.liftinfor(i).outcall_floor(),2));
                int get = 0;
                int len = outcall_floor_str.size();
                for (int i = 0; i < len; ++i)
                {
                    if(outcall_floor_str.at(len-i-1) == "1")
                    {
                        //奇数下行 偶数上行
                        //i/2+1
                        int floor = i/2+1;
                        if(get == 0)
                        {
                            get = 1;
                            outcall_floor.append(QString("%1").arg(floor));
                            if(i%2 == 0)
                                outcall_floor.append("上行");
                            else
                                outcall_floor.append("下行");
                        }else
                        {
                            outcall_floor.append(".");
                            outcall_floor.append(QString("%1").arg(floor));
                            if(i%2 == 0)
                                outcall_floor.append("上行");
                            else
                                outcall_floor.append("下行");
                        }

                    }
                }
                //incall_floor.append(outcall_floor_str);
            }
            ui->liftInfo_tableWidget->item(i,10)->setText(QString("%1").arg(outcall_floor));
            ui->liftInfo_tableWidget->item(i,11)->setText(QString(mon_data.liftinfor(i).ip().c_str()));
            ui->liftInfo_tableWidget->item(i,12)->setText(QDateTime::fromMSecsSinceEpoch(mon_data.liftinfor(i).ts()).toString("yyyy-MM-dd HH:mm:ss"));
        }
    }

    last_lift_cnt = lift_cnt;
}

void SubWidget::proto_recv_data()
{
    while (protoUdpSocket->hasPendingDatagrams())
    {
        if(playbacking == 1)
        {
            right_ip ="::ffff:127.0.0.1";
        }
        else
        {
            right_ip ="::ffff:" + ui->tcp_lineEdit->currentText();
        }
        if(first_read_udp_flag == 0)
        {
            ui->carInfo_tableWidget->setColumnCount(38);
            ui->carInfo_tableWidget->verticalHeader()->setVisible(false);
            QStringList headers;
            if(speedDisplayMode == 0)
            {
                headers  << "车号" << "任务号"<<"时间"<< "类型(T)"<<"主状态"<<"子状态"<<"软停"<<"在线"<<"报警"<<"电量"<<"充电时间"<<"载货"
                        <<"起始站点"<<"目标站点"<<"货叉高度"<< "x坐标"<< "y坐标"<< "角度" << "方向"<<"速度(毫米/秒)"<<"当前段"<<"下一段"<<"当前站点"<<"下个站点"
                       <<"前安全防护"<<"后安全防护"<<"货叉光电"<<"IP"<< "类型(A)"<<"电池止挡"<<"1分钟心跳次数"<<"5分钟心跳次数"<<"15分钟心跳次数"
                      <<"程序版本"<<"编译时间"<<"版本类型"<<"绑定区"<<"库位限定";
            }
            else if(speedDisplayMode == 1)
            {
                headers  << "车号" << "任务号"<<"时间"<< "类型(T)"<<"主状态"<<"子状态"<<"软停"<<"在线"<<"报警"<<"电量"<<"充电时间"<<"载货"
                         <<"起始站点"<<"目标站点"<<"货叉高度"<< "x坐标"<< "y坐标"<< "角度" << "方向"<<"速度(米/分)"<<"当前段"<<"下一段"<<"当前站点"<<"下个站点"
                        <<"前安全防护"<<"后安全防护"<<"货叉光电"<<"IP"<< "类型(A)"<<"电池止挡"<<"1分钟心跳次数"<<"5分钟心跳次数"<<"15分钟心跳次数"
                          <<"程序版本"<<"编译时间"<<"版本类型"<<"绑定区"<<"库位限定";
            }
            else
            {
                headers  << "车号" << "任务号"<<"时间"<< "类型(T)"<<"主状态"<<"子状态"<<"软停"<<"在线"<<"报警"<<"电量"<<"充电时间"<<"载货h"
                         <<"起始站点"<<"目标站点"<<"货叉高度"<< "x坐标"<< "y坐标"<< "角度" << "方向"<<"速度()"<<"当前段"<<"下一段"<<"当前站点"<<"下个站点"
                        <<"前安全防护"<<"后安全防护"<<"货叉光电"<<"IP"<< "类型(A)"<<"电池止挡"<<"1分钟心跳次数"<<"5分钟心跳次数"<<"15分钟心跳次数"
                          <<"程序版本"<<"编译时间"<<"版本类型"<<"绑定区"<<"库位限定";
            }
            ui->carInfo_tableWidget->setHorizontalHeaderLabels(headers);
            ui->carInfo_tableWidget->horizontalHeader()->setMinimumSectionSize(10);

            ui->carInfo_tableWidget->horizontalHeader()->resizeSection(CARID,40);
            ui->carInfo_tableWidget->horizontalHeader()->setSectionResizeMode(CARID, QHeaderView::Fixed);

            ui->carInfo_tableWidget->horizontalHeader()->resizeSection(TASKID,100);
            ui->carInfo_tableWidget->horizontalHeader()->setSectionResizeMode(TASKID, QHeaderView::Fixed);

            ui->carInfo_tableWidget->horizontalHeader()->resizeSection(TIME,180);
            ui->carInfo_tableWidget->horizontalHeader()->setSectionResizeMode(TIME, QHeaderView::Fixed);

            ui->carInfo_tableWidget->horizontalHeader()->resizeSection(TYPET,80);
            ui->carInfo_tableWidget->horizontalHeader()->setSectionResizeMode(TYPET, QHeaderView::Fixed);

            ui->carInfo_tableWidget->horizontalHeader()->resizeSection(STATE,60);
            ui->carInfo_tableWidget->horizontalHeader()->setSectionResizeMode(STATE, QHeaderView::Fixed);

            ui->carInfo_tableWidget->horizontalHeader()->resizeSection(STATUS,60);
            ui->carInfo_tableWidget->horizontalHeader()->setSectionResizeMode(STATUS, QHeaderView::Fixed);

            ui->carInfo_tableWidget->horizontalHeader()->resizeSection(SOFTSTOP,60);
            ui->carInfo_tableWidget->horizontalHeader()->setSectionResizeMode(SOFTSTOP, QHeaderView::Fixed);

            ui->carInfo_tableWidget->horizontalHeader()->resizeSection(ONLINE,40);
            ui->carInfo_tableWidget->horizontalHeader()->setSectionResizeMode(ONLINE, QHeaderView::Fixed);

            ui->carInfo_tableWidget->horizontalHeader()->setSectionResizeMode(ERROR, QHeaderView::ResizeToContents);

            ui->carInfo_tableWidget->horizontalHeader()->resizeSection(BATTERY,40);
            ui->carInfo_tableWidget->horizontalHeader()->setSectionResizeMode(BATTERY, QHeaderView::Fixed);

            ui->carInfo_tableWidget->horizontalHeader()->resizeSection(CHARGING,120);
            ui->carInfo_tableWidget->horizontalHeader()->setSectionResizeMode(CHARGING, QHeaderView::Fixed);

            ui->carInfo_tableWidget->horizontalHeader()->resizeSection(LOAD,40);
            ui->carInfo_tableWidget->horizontalHeader()->setSectionResizeMode(LOAD, QHeaderView::Fixed);

            ui->carInfo_tableWidget->horizontalHeader()->resizeSection(START,80);
            ui->carInfo_tableWidget->horizontalHeader()->setSectionResizeMode(START, QHeaderView::Fixed);

            ui->carInfo_tableWidget->horizontalHeader()->resizeSection(END,80);
            ui->carInfo_tableWidget->horizontalHeader()->setSectionResizeMode(END, QHeaderView::Fixed);

            ui->carInfo_tableWidget->horizontalHeader()->resizeSection(HEIGHT,80);
            ui->carInfo_tableWidget->horizontalHeader()->setSectionResizeMode(HEIGHT, QHeaderView::Fixed);

            ui->carInfo_tableWidget->horizontalHeader()->resizeSection(X,100);
            ui->carInfo_tableWidget->horizontalHeader()->setSectionResizeMode(X, QHeaderView::Fixed);

            ui->carInfo_tableWidget->horizontalHeader()->resizeSection(Y,100);
            ui->carInfo_tableWidget->horizontalHeader()->setSectionResizeMode(Y, QHeaderView::Fixed);

            ui->carInfo_tableWidget->horizontalHeader()->resizeSection(ANGLE,100);
            ui->carInfo_tableWidget->horizontalHeader()->setSectionResizeMode(ANGLE, QHeaderView::Fixed);

            ui->carInfo_tableWidget->horizontalHeader()->resizeSection(DIRECTION,40);
            ui->carInfo_tableWidget->horizontalHeader()->setSectionResizeMode(DIRECTION, QHeaderView::Fixed);

            ui->carInfo_tableWidget->horizontalHeader()->resizeSection(SPEED,100);
            ui->carInfo_tableWidget->horizontalHeader()->setSectionResizeMode(SPEED, QHeaderView::Fixed);

            ui->carInfo_tableWidget->horizontalHeader()->resizeSection(ROUTEC,80);
            ui->carInfo_tableWidget->horizontalHeader()->setSectionResizeMode(ROUTEC, QHeaderView::Fixed);

            ui->carInfo_tableWidget->horizontalHeader()->resizeSection(ROUTEN,80);
            ui->carInfo_tableWidget->horizontalHeader()->setSectionResizeMode(ROUTEN, QHeaderView::Fixed);

            ui->carInfo_tableWidget->horizontalHeader()->resizeSection(POINTC,80);
            ui->carInfo_tableWidget->horizontalHeader()->setSectionResizeMode(POINTC, QHeaderView::Fixed);

            ui->carInfo_tableWidget->horizontalHeader()->resizeSection(POINTN,80);
            ui->carInfo_tableWidget->horizontalHeader()->setSectionResizeMode(POINTN, QHeaderView::Fixed);


            ui->carInfo_tableWidget->horizontalHeader()->setSectionResizeMode(SAFEF, QHeaderView::Fixed);
            ui->carInfo_tableWidget->horizontalHeader()->resizeSection(SAFEF,250);

            ui->carInfo_tableWidget->horizontalHeader()->setSectionResizeMode(SAFEB, QHeaderView::Fixed);
            ui->carInfo_tableWidget->horizontalHeader()->resizeSection(SAFEB,250);

            ui->carInfo_tableWidget->horizontalHeader()->setSectionResizeMode(FORKLIGHT, QHeaderView::Fixed);
            ui->carInfo_tableWidget->horizontalHeader()->resizeSection(FORKLIGHT,250);

            ui->carInfo_tableWidget->horizontalHeader()->resizeSection(IP,150);
            ui->carInfo_tableWidget->horizontalHeader()->setSectionResizeMode(IP, QHeaderView::Fixed);

            ui->carInfo_tableWidget->horizontalHeader()->resizeSection(TYPEC,100);
            ui->carInfo_tableWidget->horizontalHeader()->setSectionResizeMode(TYPEC, QHeaderView::Fixed);



            ui->carInfo_tableWidget->horizontalHeader()->resizeSection(BATTERYHOLDER,100);
            ui->carInfo_tableWidget->horizontalHeader()->setSectionResizeMode(TYPEC, QHeaderView::Fixed);

            ui->carInfo_tableWidget->horizontalHeader()->resizeSection(HEART1MIN,130);
            ui->carInfo_tableWidget->horizontalHeader()->setSectionResizeMode(TYPEC, QHeaderView::Fixed);

            ui->carInfo_tableWidget->horizontalHeader()->resizeSection(HEART5MIN,130);
            ui->carInfo_tableWidget->horizontalHeader()->setSectionResizeMode(TYPEC, QHeaderView::Fixed);

            ui->carInfo_tableWidget->horizontalHeader()->resizeSection(HEART15MIN,130);
            ui->carInfo_tableWidget->horizontalHeader()->setSectionResizeMode(TYPEC, QHeaderView::Fixed);

            ui->carInfo_tableWidget->horizontalHeader()->resizeSection(VERSION,130);
            ui->carInfo_tableWidget->horizontalHeader()->setSectionResizeMode(TYPEC, QHeaderView::Fixed);

            ui->carInfo_tableWidget->horizontalHeader()->resizeSection(VERSION_TS,180);
            ui->carInfo_tableWidget->horizontalHeader()->setSectionResizeMode(TYPEC, QHeaderView::Fixed);

            ui->carInfo_tableWidget->horizontalHeader()->resizeSection(BETA,100);
            ui->carInfo_tableWidget->horizontalHeader()->setSectionResizeMode(TYPEC, QHeaderView::Fixed);

            ui->carInfo_tableWidget->horizontalHeader()->resizeSection(BINDAREA,60);
            ui->carInfo_tableWidget->horizontalHeader()->setSectionResizeMode(TYPEC, QHeaderView::Fixed);

            ui->carInfo_tableWidget->horizontalHeader()->resizeSection(STALIMIT,80);
            ui->carInfo_tableWidget->horizontalHeader()->setSectionResizeMode(TYPEC, QHeaderView::Fixed);
            if(ui->checkBox_OL->checkState() == 0)
            {
                ui->carInfo_tableWidget->setColumnHidden(ONLINE,true);
            }
            if(ui->checkBox_battery->checkState() == 0)
            {
                ui->carInfo_tableWidget->setColumnHidden(BATTERY,true);
            }
            if(ui->checkBox_cTime->checkState() == 0)
            {
                ui->carInfo_tableWidget->setColumnHidden(CHARGING,true);
            }
            if(ui->checkBox_startId->checkState() == 0)
            {
                ui->carInfo_tableWidget->setColumnHidden(START,true);
            }
            if(ui->checkBox_endId->checkState() == 0)
            {
                ui->carInfo_tableWidget->setColumnHidden(END,true);
            }
            if(ui->checkBox_hight->checkState() == 0)
            {
                ui->carInfo_tableWidget->setColumnHidden(HEIGHT,true);
            }
            if(ui->checkBox_dir->checkState() == 0)
            {
                ui->carInfo_tableWidget->setColumnHidden(DIRECTION,true);
            }
            if(ui->checkBox_speed->checkState() == 0)
            {
                ui->carInfo_tableWidget->setColumnHidden(SPEED,true);
            }
            if(ui->checkBox_curpoint->checkState() == 0)
            {
                ui->carInfo_tableWidget->setColumnHidden(POINTC,true);
            }
            if(ui->checkBox_nxtpoint->checkState() == 0)
            {
                ui->carInfo_tableWidget->setColumnHidden(POINTN,true);
            }
            if(ui->checkBox_fSafe->checkState() == 0)
            {
                ui->carInfo_tableWidget->setColumnHidden(SAFEF,true);
            }
            if(ui->checkBox_bSafe->checkState() == 0)
            {
                ui->carInfo_tableWidget->setColumnHidden(SAFEB,true);
            }
            if(ui->checkBox_forkLight->checkState() == 0)
            {
                ui->carInfo_tableWidget->setColumnHidden(FORKLIGHT,true);
            }
            if(ui->checkBox_IPaddr->checkState() == 0)
            {
                ui->carInfo_tableWidget->setColumnHidden(IP,true);
            }
            if(ui->checkBox_typeA->checkState() == 0)
            {
                ui->carInfo_tableWidget->setColumnHidden(TYPEC,true);
            }
            if(ui->checkBox_batteryZD->checkState() == 0)
            {
                ui->carInfo_tableWidget->setColumnHidden(BATTERYHOLDER,true);
            }
            if(ui->checkBox_1min->checkState() == 0)
            {
                ui->carInfo_tableWidget->setColumnHidden(HEART1MIN,true);
            }
            if(ui->checkBox_5min->checkState() == 0)
            {
                ui->carInfo_tableWidget->setColumnHidden(HEART5MIN,true);
            }
            if(ui->checkBox_15min->checkState() == 0)
            {
                ui->carInfo_tableWidget->setColumnHidden(HEART15MIN,true);
            }
            if(ui->checkBox_carVersion->checkState() == 0)
            {
                ui->carInfo_tableWidget->setColumnHidden(VERSION,true);
            }
            if(ui->checkBox_carCT->checkState() == 0)
            {
                ui->carInfo_tableWidget->setColumnHidden(VERSION_TS,true);
            }
            if(ui->checkBox_versionType->checkState() == 0)
            {
                ui->carInfo_tableWidget->setColumnHidden(BETA,true);
            }
            if(ui->checkBox_xcoord->checkState() == 0)
            {
                ui->carInfo_tableWidget->setColumnHidden(X,true);
            }
            if(ui->checkBox_ycoord->checkState() == 0)
            {
                ui->carInfo_tableWidget->setColumnHidden(Y,true);
            }
            if(ui->checkBox_angle->checkState() == 0)
            {
                ui->carInfo_tableWidget->setColumnHidden(ANGLE,true);
            }
            if(ui->checkBox_bindarea->checkState() == 0)
            {
                ui->carInfo_tableWidget->setColumnHidden(BINDAREA,true);
            }
            if(ui->checkBox_stalimitf->checkState() == 0)
            {
                ui->carInfo_tableWidget->setColumnHidden(STALIMIT,true);
            }

            first_read_udp_flag = 1;
        }
        QByteArray allArray;
        allArray.resize(1024*1024*10*2);
        qint64 temp = protoUdpSocket->readDatagram(allArray.data(),allArray.size(),&rec_udp);
        if(temp < 9)
        {
            return;
        }
        allArray.resize(temp);

        mon_data.ParseFromArray(allArray.data()+6,allArray.size()-8);

        if(mon_data.agvinfor_size() < 1)
        {
            continue;
        }
        carNum = mon_data.agvinfor_size();
        occupyPointID_vec.clear();
        occupyPointID_vec.resize(carNum);
        scanPointID_vec.clear();
        scanPointID_vec.resize(carNum);


        if(rec_udp.toString() == right_ip)
        {

            if(current_car_num != carNum)
            {
                if(!vec_Car.isEmpty())
                {
                    for(int i = 0 ; i < vec_Car.size(); i ++)
                    {
                        ui->graphicsView->scene()->removeItem(vec_Car[i]);

                    }
                }
                vec_Car.clear();

                for(int i = 0 ; i < carNum; i++)
                {
                    Car *temp_car = new Car(0,0,i%8);
                    connect(temp_car,&Car::car_id,this,&SubWidget::proto_send_route_tcp_func);
                    connect(temp_car,&Car::car_id,this,&SubWidget::select_tablewidget_value);
                    connect(temp_car,&Car::floatwindow_show,this,&SubWidget::floatingwindow_show_slot);
                    connect(temp_car,&Car::floatwindow_show,this,&SubWidget::changeFloatWindowCarId);
                    connect(temp_car,&Car::floatwindow_hide,this,&SubWidget::floatingwindow_hide_slot);
                    connect(temp_car,&Car::showRoute,this,&SubWidget::showRoute);
                    connect(this,&SubWidget::return_window_xy,temp_car,&Car::set_f_dialog_xy);
                    vec_Car.push_back(temp_car);

                }

                current_car_num = carNum;
            }

#if 1  //扫描点
            vec_Occupy_Point.clear();
            vec_Scan_Point.clear();
            for(int i = 0 ; i < vec_PointItem.size(); i++)
            {
                if(vec_PointItem[i] != nullptr)
                {
                    vec_PointItem[i]->occupy_point_flag = -1;
                    vec_PointItem[i]->scan_point_flag = -1;
                    vec_PointItem[i]->zoomin_flag = -1;
                }
            }
#endif


            //报警处理
            error_sum = 0;
            error_insert_row = error_sum;
            ui->carInfo_tableWidget->setRowCount(current_car_num+error_sum);
            deal_with_car_data_proto();

            agv_track_show(mon_data);

            update_lift_tableWidget(mon_data);
#if 1  //扫描点
            if(mysterious_button_flag == 1)
            {
                show_occupy_point();
                show_scan_point();
            }

#endif
        }
        else
        {
        }

        ui->graphicsView->scene()->update();


    }

}




void SubWidget::recv_tcp()
{
    qint64 recive_recv_route_tcp = route_tcpSocket->read((char*)&recv_route_tcp,sizeof(recv_route_tcp));

    int route_list[(int)recv_route_tcp.route_num];
    int cur_id;
    int nxt_id;
    qint64 recive_route_list = route_tcpSocket->read((char*)route_list,sizeof(route_list));

    route_tcpSocket->read((char*)&cur_id,sizeof(cur_id));
    route_tcpSocket->read((char*)&nxt_id,sizeof(nxt_id));
    route_tcpSocket->disconnectFromHost();
    route_tcpSocket->close();




    for(int j = 0 ; j < vec_LineItem.size(); j++)
    {
        if(vec_LineItem[j] != nullptr)
        {
            vec_LineItem[j]->route_flag = 0;
        }
    }
    for(int j = 0 ; j < vec_ArcItem.size(); j++)
    {
        if(vec_ArcItem[j] != nullptr)
        {
            vec_ArcItem[j]->route_flag = 0;
        }
    }

    for(int i = 0; i < (int)recv_route_tcp.route_num; i++)
    {
        if(vec_LineItem[route_list[i]] != nullptr)
        {
            if(route_list[i] == cur_id)
            {
                vec_LineItem[route_list[i]]->route_flag = 2;
            }
            else if(route_list[i] == nxt_id)
            {
                vec_LineItem[route_list[i]]->route_flag = 3;
            }
            else
            {
                vec_LineItem[route_list[i]]->route_flag = 1;
            }
        }
        else if(vec_ArcItem[route_list[i]] != nullptr)
        {
            if(route_list[i] == cur_id)
            {
                vec_ArcItem[route_list[i]]->route_flag = 2;
            }
            else if(route_list[i] == nxt_id)
            {
                vec_ArcItem[route_list[i]]->route_flag = 3;
            }
            else
            {
                vec_ArcItem[route_list[i]]->route_flag = 1;
            }
        }
        else
        {
            //qDebug()<<"没有找到对应的段";
        }
    }
    ui->graphicsView->scene()->update();
}

void SubWidget::f_dialog_show_slot()
{
    f_dialog->move(QCursor::pos().x()+20,QCursor::pos().y()+20);
    f_dialog->show();
}

void SubWidget::f_dialog_hide_slot()
{
    f_dialog->hide();
}

QString SubWidget::IPV4IntegerToString(quint32 ip)
{
    return QString("%4.%3.%2.%1")
            .arg((ip >> 24) & 0xFF)
            .arg((ip >> 16) & 0xFF)
            .arg((ip >> 8) & 0xFF)
            .arg(ip & 0xFF);
}

void SubWidget::save_items_of_scene()
{
    vec_LineItem.clear();
    vec_ArcItem.clear();
    vec_PointItem.clear();
    vec_AreaItem.clear();

    vec_LineItem.resize(seg_maxID+1);
    vec_ArcItem.resize(seg_maxID+1);
    vec_PointItem.resize(point_maxID+1);
    vec_AreaItem.resize(area_maxID+1);
        foreach (QGraphicsItem *item , scene->items())
        {

            if ( item->type() == GraphicsLineItem::Type)
            {
                GraphicsLineItem * my_line_item = dynamic_cast<GraphicsLineItem*>(item);
                vec_LineItem.replace((int)(my_line_item->map_segment_pb.mutable_basic()->id()),my_line_item);

            }
            else if(item->type() == GraphicsArcItem::Type)
            {
                GraphicsArcItem * my_arc_item = dynamic_cast<GraphicsArcItem*>(item);
                vec_ArcItem.replace((int)(my_arc_item->map_segment_pb.mutable_basic()->id()),my_arc_item);

            }
            else if(item->type() == GraphicsPointItem::Type)
            {
                GraphicsPointItem * my_point_item = dynamic_cast<GraphicsPointItem*>(item);
                connect(my_point_item,&GraphicsPointItem::sendPointID,this,&SubWidget::findScanCar);

                vec_PointItem.replace((int)(my_point_item->map_point_pb.mutable_basic()->id()),my_point_item);
            }
            else if(item->type() == GraphicsAreaItem::Type)
            {
                GraphicsAreaItem * my_area_item = dynamic_cast<GraphicsAreaItem*>(item);
                vec_AreaItem.replace((int)(my_area_item->map_area_pb.mutable_basic()->id()),my_area_item);
            }
        }
    //qDebug()<<"foreach end---------";

    has_save_items_of_scene = 1;
}

void SubWidget::show_cur_or_nxt_seg()
{

}

void SubWidget::change_show_cur_or_nxt_carID(int dev_id, int, int)
{
    show_cur_or_nxt_carID = dev_id;
}

void SubWidget::read_error_table()
{
    QSqlDatabase connect1_db = QSqlDatabase::addDatabase("QSQLITE","connection1");
    connect1_db.setDatabaseName("CTL_WARN_CODE.db");
    if(!connect1_db.open()){
        QMessageBox::critical(0,"Cannot open database1","Unable to establish a database connection.",QMessageBox::Cancel);
    }

    db_connect1_model = new QSqlTableModel(this,QSqlDatabase::database("connection1"));
    db_connect1_model->setTable("CTL_WARN_CODE");
    db_connect1_model->select();
    while(db_connect1_model->canFetchMore())
    {
        db_connect1_model->fetchMore();
    }
    for(int i = 0; i<db_connect1_model->rowCount(); i++){
    QSqlRecord temp_record = db_connect1_model->record(i);
    Errorinfo *temperror = new Errorinfo;
    temperror->err_type = temp_record.value(0).toInt();
    ////qDebug()<<temp_record.value(1).toString()<<" a";
    temperror->err_string_fork = temp_record.value(1).toString();
    temperror->err_string_diff = temp_record.value(2).toString();
    temperror->err_string_lurk = temp_record.value(3).toString();
    vec_ErrorInfo.push_back(temperror);
    }
}


void SubWidget::show_occupy_point()
{
    for(int i = 0 ; i < vec_Occupy_Point.size(); i++)
    {
        if(vec_Occupy_Point[i]->point_id > -1)
        {
            if(vec_PointItem[(vec_Occupy_Point[i]->point_id)] != nullptr)
            {
                vec_PointItem[(vec_Occupy_Point[i]->point_id)]->occupy_point_flag = vec_Occupy_Point[i]->color_flag;
                vec_PointItem[(vec_Occupy_Point[i]->point_id)]->zoomin_flag = vec_Occupy_Point[i]->zoomin_flag;
            }
        }
    }
}

void SubWidget::show_scan_point()
{
    for(int i = 0 ; i < vec_Scan_Point.size(); i++)
    {
        if(vec_Scan_Point[i]->point_id > -1)
        {
            if(vec_PointItem[(vec_Scan_Point[i]->point_id)] != nullptr)
            {
                vec_PointItem[(vec_Scan_Point[i]->point_id)]->scan_point_flag = vec_Scan_Point[i]->color_flag;
                vec_PointItem[(vec_Scan_Point[i]->point_id)]->zoomin_flag = vec_Scan_Point[i]->zoomin_flag;
            }
        }
    }
}


QString sec_to_time(int ts)
{
    QString timer ;
    if(ts >= 3600)
        timer = QTime(0, 0, 0,0).addSecs(int(ts)).toString(QString::fromLatin1("HH:mm:ss"));
    else
        timer = QTime(0, 0, 0,0).addSecs(int(ts)).toString(QString::fromLatin1("mm:ss"));
    return timer;
}


void SubWidget::get_point_zoomin_carID(int carID, int, int)
{
    point_zoomin_carID = carID;
    //qDebug()<<"carID ="<<point_zoomin_carID;
}

void SubWidget::deal_with_car_data_proto()
{
    for(int j = 0; j < vec_Car.size(); j++)
    {
        qreal pos_x = mon_data.agvinfor(j).agvreport().agvloc().pos().x();
        qreal pos_y = mon_data.agvinfor(j).agvreport().agvloc().pos().y();

        vec_Car[j]->setPos(pos_x,pos_y);
        vec_Car[j]->set_xy(pos_x,pos_y);
        qreal temp_angle = mon_data.agvinfor(j).agvreport().agvloc().pos_ang();
        vec_Car[j]->setTransformOriginPoint(0, 0);
        vec_Car[j]->setRotation(temp_angle);

#if 1  //扫描点
        if(mon_data.agvinfor(j).agvreport().agvload().load() == -1)
        {

            vec_Car[j]->set_polygon_points(1,1,
                                           QPointF(mon_data.agvinfor(j).avoidinfo().agvoutline(0).x(),mon_data.agvinfor(j).avoidinfo().agvoutline(0).y()),
                                           QPointF(mon_data.agvinfor(j).avoidinfo().agvoutline(1).x(),mon_data.agvinfor(j).avoidinfo().agvoutline(1).y()),
                                           QPointF(mon_data.agvinfor(j).avoidinfo().agvoutline(2).x(),mon_data.agvinfor(j).avoidinfo().agvoutline(2).y()),
                                           QPointF(mon_data.agvinfor(j).avoidinfo().agvoutline(3).x(),mon_data.agvinfor(j).avoidinfo().agvoutline(3).y()));
        }
        else if(mon_data.agvinfor(j).agvreport().agvload().load() != -1)
        {
            vec_Car[j]->set_polygon_points(2,1,
                                           QPointF(mon_data.agvinfor(j).avoidinfo().agvoutline(0).x(),mon_data.agvinfor(j).avoidinfo().agvoutline(0).y()),
                                           QPointF(mon_data.agvinfor(j).avoidinfo().agvoutline(1).x(),mon_data.agvinfor(j).avoidinfo().agvoutline(1).y()),
                                           QPointF(mon_data.agvinfor(j).avoidinfo().agvoutline(2).x(),mon_data.agvinfor(j).avoidinfo().agvoutline(2).y()),
                                           QPointF(mon_data.agvinfor(j).avoidinfo().agvoutline(3).x(),mon_data.agvinfor(j).avoidinfo().agvoutline(3).y()));
            vec_Car[j]->set_polygon_points(2,2,
                                           QPointF(mon_data.agvinfor(j).avoidinfo().goodsoutline(0).x(),mon_data.agvinfor(j).avoidinfo().goodsoutline(0).y()),
                                           QPointF(mon_data.agvinfor(j).avoidinfo().goodsoutline(1).x(),mon_data.agvinfor(j).avoidinfo().goodsoutline(1).y()),
                                           QPointF(mon_data.agvinfor(j).avoidinfo().goodsoutline(2).x(),mon_data.agvinfor(j).avoidinfo().goodsoutline(2).y()),
                                           QPointF(mon_data.agvinfor(j).avoidinfo().goodsoutline(3).x(),mon_data.agvinfor(j).avoidinfo().goodsoutline(3).y()));
        }

        if(vec_Car[j]->hasAddToScene_flag == 0)
        {
            ui->graphicsView->scene()->addItem(vec_Car[j]);
            vec_Car[j]->hasAddToScene_flag = 1;
        }

        occupyPointID_vec[j].append(-vec_Car[j]->get_id());//取车号负数保存
            for(int i = 0 ; i < mon_data.agvinfor(j).avoidinfo().occupypoint_size() ; i++)
            {
                Occupy_Point *temp_occupy_point = new Occupy_Point;
                temp_occupy_point->point_id = mon_data.agvinfor(j).avoidinfo().occupypoint(i);
                temp_occupy_point->color_flag = vec_Car[j]->color_flag;
                if(mon_data.agvinfor(j).agvreport().basic().id() == point_zoomin_carID)
                {
                    temp_occupy_point->zoomin_flag = 1;
                }
                else
                {
                    temp_occupy_point->zoomin_flag = -1;
                }
                vec_Occupy_Point.push_back(temp_occupy_point);

                occupyPointID_vec[j].append(temp_occupy_point->point_id);
            }

        scanPointID_vec[j].append(-vec_Car[j]->get_id());//取车号负数保存
            for(int i = 0 ; i < mon_data.agvinfor(j).avoidinfo().scanpoint_size() ; i++)
            {
                Scan_Point *temp_scan_point = new Scan_Point;

                temp_scan_point->point_id = mon_data.agvinfor(j).avoidinfo().scanpoint(i);
                temp_scan_point->color_flag = 1;
                if(mon_data.agvinfor(j).agvreport().basic().id() == point_zoomin_carID)
                {
                    temp_scan_point->zoomin_flag = 1;
                }
                else
                {
                    temp_scan_point->zoomin_flag = -1;
                }
                vec_Scan_Point.push_back(temp_scan_point);

                scanPointID_vec[j].append(temp_scan_point->point_id);
            }


#endif
    #if 1
    vec_Car[j] ->set_car_values(
                mon_data.agvinfor(j).agvreport().basic().id(),
                mon_data.agvinfor(j).agvreport().basic().type(),
                mon_data.agvinfor(j).agvreport().agvtask().task_id(),
                mon_data.agvinfor(j).agvreport().agvtask().task_type(),
                mon_data.agvinfor(j).agvreport().agvloc().pos().x(),
                mon_data.agvinfor(j).agvreport().agvloc().pos().y(),
                mon_data.agvinfor(j).agvreport().agvloc().pos_ang(),
                mon_data.agvinfor(j).agvreport().agvst().dir(),
                mon_data.agvinfor(j).agvreport().agvst().speed(),
                mon_data.agvinfor(j).agvreport().agvst().state(),
                mon_data.agvinfor(j).agvreport().agvst().status(),
                mon_data.agvinfor(j).agvreport().agvst().error(),
                mon_data.agvinfor(j).agvreport().agvbat().battery(),
                mon_data.agvinfor(j).agvreport().agvload().load(),
                mon_data.agvinfor(j).agvreport().agvtask().cur_seg(),
                mon_data.agvinfor(j).agvreport().agvtask().nxt_seg(),
                mon_data.agvinfor(j).ip(),
                mon_data.agvinfor(j).ts(),
                mon_data.agvinfor(j).ol(),
                mon_data.agvinfor(j).agvreport().agvtask().start_id(),
                mon_data.agvinfor(j).agvreport().agvtask().end_id(),
                mon_data.agvinfor(j).agvreport().agvload().hyd_height(),
                0,
                0,
                0,
//                mon_data.agvinfor(j).agvreport().protection(0),
//                mon_data.agvinfor(j).agvreport().protection(1),
//                mon_data.agvinfor(j).agvreport().protection(2),
                mon_data.agvinfor(j).agvreport().agvst().softstop(),
                mon_data.agvinfor(j).timeinfo().idle_wait_ts(),
                mon_data.agvinfor(j).timeinfo().avoid_set_ts(),
                mon_data.agvinfor(j).timeinfo().manual_on_ts(),
                mon_data.agvinfor(j).timeinfo().softstop_on_ts(),
                mon_data.agvinfor(j).timeinfo().warning_on_ts(),
                mon_data.agvinfor(j).avoidinfo().stopstatus(),
                mon_data.agvinfor(j).avoidinfo().stoplevel(),
                mon_data.agvinfor(j).avoidinfo().stopbyagvid(),
                mon_data.agvinfor(j).avoidinfo().stopreason(),
                mon_data.agvinfor(j).avoidinfo().speedlevel(),
                mon_data.agvinfor(j).avoidinfo().speedbyagvid(),
                mon_data.agvinfor(j).avoidinfo().confpoint()
                );

#endif

    if(routeCarId == mon_data.agvinfor(j).agvreport().basic().id() &&
            route_data.agvroute_size() > 0)
    {
        QList<int> route_list;
        int route_num = route_data.agvroute(0).route_id_size();
        if(route_num >= 1)
        {
            for(int i = 0; i < route_num; i++)
            {
                route_list.append(route_data.agvroute(0).route_id(i));
            }

            for(int i = 0; i < route_num; i++)
            {
                if(route_list[i] >= vec_LineItem.size() || route_list[i] < 1)
                {
                    debugWidget->ui->textEditDebug->append("进入continue");
                    continue;
                }

                if(vec_LineItem[route_list[i]] != nullptr)
                {
                    if(route_list[i] == mon_data.agvinfor(j).agvreport().agvtask().cur_seg())
                    {
                        vec_LineItem[route_list[i]]->route_flag = 2;
                    }
                    else if(route_list[i] == mon_data.agvinfor(j).agvreport().agvtask().nxt_seg())
                    {
                        vec_LineItem[route_list[i]]->route_flag = 3;

                    }
                    else
                    {
                        vec_LineItem[route_list[i]]->route_flag = 1;
                    }
                }
                else if(vec_ArcItem[route_list[i]] != nullptr)
                {
                    if(route_list[i] == mon_data.agvinfor(j).agvreport().agvtask().cur_seg())
                    {
                        vec_ArcItem[route_list[i]]->route_flag = 2;
                    }
                    else if(route_list[i] == mon_data.agvinfor(j).agvreport().agvtask().nxt_seg())
                    {
                        vec_ArcItem[route_list[i]]->route_flag = 3;
                    }
                    else
                    {
                        vec_ArcItem[route_list[i]]->route_flag = 1;
                    }
                }
                else
                {
                    debugWidget->ui->textEditDebug->append("没有找到对应的段");
                }
            }

            ui->graphicsView->scene()->update();
        }
    }

        if(ui->carInfo_tableWidget->item(j+error_sum,0) == nullptr)
        {
            ui->carInfo_tableWidget->setItem(j+error_sum, CARID, new QTableWidgetItem(QString::number(mon_data.agvinfor(j).agvreport().basic().id())));
            if(mon_data.agvinfor(j).agvreport().basic().type() > 0 && mon_data.agvinfor(j).agvreport().basic().type() < 21)
            {
                ui->carInfo_tableWidget->setItem(j+error_sum, TYPEC, new QTableWidgetItem(QString("单舵轮叉车(%1)").arg(mon_data.agvinfor(j).agvreport().basic().type())));
            }
            else if(mon_data.agvinfor(j).agvreport().basic().type() > 20 && mon_data.agvinfor(j).agvreport().basic().type() < 41)
            {
                ui->carInfo_tableWidget->setItem(j+error_sum, TYPEC, new QTableWidgetItem(QString("双轮差速(%1)").arg(mon_data.agvinfor(j).agvreport().basic().type())));
            }
            else if(mon_data.agvinfor(j).agvreport().basic().type() > 40 && mon_data.agvinfor(j).agvreport().basic().type() < 61)
            {
                ui->carInfo_tableWidget->setItem(j+error_sum, TYPEC, new QTableWidgetItem(QString("双舵轮潜入(%1)").arg(mon_data.agvinfor(j).agvreport().basic().type())));
            }
            else
            {
                ui->carInfo_tableWidget->setItem(j+error_sum, TYPEC, new QTableWidgetItem(QString::number(mon_data.agvinfor(j).agvreport().basic().type())));
            }
            ui->carInfo_tableWidget->setItem(j+error_sum, TASKID, new QTableWidgetItem(QString::number(mon_data.agvinfor(j).agvreport().agvtask().task_id())));
            if(mon_data.agvinfor(j).agvreport().agvtask().task_type() == 0)
            {
                ui->carInfo_tableWidget->setItem(j+error_sum, TYPET, new QTableWidgetItem(QString("无货")));
            }
            else if(mon_data.agvinfor(j).agvreport().agvtask().task_type() == 1)
            {
                ui->carInfo_tableWidget->setItem(j+error_sum, TYPET, new QTableWidgetItem(QString("取货")));
            }
            else if(mon_data.agvinfor(j).agvreport().agvtask().task_type() == 2)
            {
                ui->carInfo_tableWidget->setItem(j+error_sum, TYPET, new QTableWidgetItem(QString("放货")));
            }
            else if(mon_data.agvinfor(j).agvreport().agvtask().task_type() == -1)
            {
                ui->carInfo_tableWidget->setItem(j+error_sum, TYPET, new QTableWidgetItem(QString("无")));
            }
            else
            {
                ui->carInfo_tableWidget->setItem(j+error_sum, TYPET, new QTableWidgetItem(QString("其他")));
            }
            ui->carInfo_tableWidget->setItem(j+error_sum, X, new QTableWidgetItem(QString::number(mon_data.agvinfor(j).agvreport().agvloc().pos().x(),'f',2)));
            ui->carInfo_tableWidget->setItem(j+error_sum, Y, new QTableWidgetItem(QString::number(mon_data.agvinfor(j).agvreport().agvloc().pos().y(),'f',2)));
            ui->carInfo_tableWidget->setItem(j+error_sum, ANGLE, new QTableWidgetItem(QString("%1°").arg(QString::number(mon_data.agvinfor(j).agvreport().agvloc().pos_ang(),'f',2))));
            if(mon_data.agvinfor(j).agvreport().agvst().dir() == 1)
            {
                ui->carInfo_tableWidget->setItem(j+error_sum, DIRECTION, new QTableWidgetItem(QString("前行")));
            }
            else if(mon_data.agvinfor(j).agvreport().agvst().dir() == -1)
            {
                ui->carInfo_tableWidget->setItem(j+error_sum, DIRECTION, new QTableWidgetItem(QString("后退")));
            }
            else
            {
                ui->carInfo_tableWidget->setItem(j+error_sum, DIRECTION, new QTableWidgetItem(QString("无")));
            }
            if(speedDisplayMode == 0)
            {
                ui->carInfo_tableWidget->setItem(j+error_sum, SPEED, new QTableWidgetItem(QString("%1").arg(QString::number(mon_data.agvinfor(j).agvreport().agvst().speed(),'f',2))));
            }
            else if(speedDisplayMode == 1)
            {
                ui->carInfo_tableWidget->setItem(j+error_sum,SPEED, new QTableWidgetItem(QString("%1").arg(QString::number(mon_data.agvinfor(j).agvreport().agvst().speed()*60/1000,'f',2))));
            }
            else
            {
                ui->carInfo_tableWidget->setItem(j+error_sum, SPEED, new QTableWidgetItem(QString("%1 ?").arg(QString::number(mon_data.agvinfor(j).agvreport().agvst().speed(),'f',2))));
            }
            if(mon_data.agvinfor(j).agvreport().agvst().state() == 1)
            {
                ui->carInfo_tableWidget->setItem(j+error_sum, STATE, new QTableWidgetItem(QString("自动")));
            }
            else if(mon_data.agvinfor(j).agvreport().agvst().state() == 2)
            {
                ui->carInfo_tableWidget->setItem(j+error_sum, STATE, new QTableWidgetItem(QString("报警")));
            }
            else if(mon_data.agvinfor(j).agvreport().agvst().state() == 3)
            {
                ui->carInfo_tableWidget->setItem(j+error_sum, STATE, new QTableWidgetItem(QString("错误")));
            }
            else if(mon_data.agvinfor(j).agvreport().agvst().state() == 4)
            {
                ui->carInfo_tableWidget->setItem(j+error_sum, STATE, new QTableWidgetItem(QString("手动")));
            }
            else if(mon_data.agvinfor(j).agvreport().agvst().state() == 5)
            {
                ui->carInfo_tableWidget->setItem(j+error_sum, STATE, new QTableWidgetItem(QString("软停")));
            }
            else
            {
                ui->carInfo_tableWidget->setItem(j+error_sum, STATE, new QTableWidgetItem(QString("未知")));
            }

            if(mon_data.agvinfor(j).agvreport().agvst().status() == 0)
            {
                ui->carInfo_tableWidget->setItem(j+error_sum, STATUS, new QTableWidgetItem(QString("运行")));
            }
            else if(mon_data.agvinfor(j).agvreport().agvst().status() == 1)
            {
                ui->carInfo_tableWidget->setItem(j+error_sum, STATUS, new QTableWidgetItem(QString("空闲")));
            }
            else if(mon_data.agvinfor(j).agvreport().agvst().status() == 2)
            {
                ui->carInfo_tableWidget->setItem(j+error_sum, STATUS, new QTableWidgetItem(QString("充电")));
            }
            else if(mon_data.agvinfor(j).agvreport().agvst().status() == 3)
            {
                ui->carInfo_tableWidget->setItem(j+error_sum, STATUS, new QTableWidgetItem(QString("避让")));
            }
            else
            {
                ui->carInfo_tableWidget->setItem(j+error_sum, STATUS, new QTableWidgetItem(QString("未知")));
            }


            int no_find_err_id = 0;
            for(int i = 0; i < vec_ErrorInfo.size(); i++)
            {
                if(vec_ErrorInfo[i]->err_type == mon_data.agvinfor(j).agvreport().agvst().error())
                {
                    if(mon_data.agvinfor(j).agvreport().basic().type() >= 1 && mon_data.agvinfor(j).agvreport().basic().type() <= 20)
                    {
                        ui->carInfo_tableWidget->setItem(j+error_sum, ERROR, new QTableWidgetItem(QString("%1(%2)").arg(vec_ErrorInfo[i]->err_string_fork).arg(vec_ErrorInfo[i]->err_type)));
                        vec_Car[j]->errorStr = QString("%1(%2)").arg(vec_ErrorInfo[i]->err_string_fork).arg(vec_ErrorInfo[i]->err_type);
                        no_find_err_id = 1;
                    }
                    else if(mon_data.agvinfor(j).agvreport().basic().type() >= 21 && mon_data.agvinfor(j).agvreport().basic().type() <= 40)
                    {
                        ui->carInfo_tableWidget->setItem(j+error_sum, ERROR, new QTableWidgetItem(QString("%1(%2)").arg(vec_ErrorInfo[i]->err_string_diff).arg(vec_ErrorInfo[i]->err_type)));
                        vec_Car[j]->errorStr = QString("%1(%2)").arg(vec_ErrorInfo[i]->err_string_diff).arg(vec_ErrorInfo[i]->err_type);
                        no_find_err_id = 1;
                    }
                    else if(mon_data.agvinfor(j).agvreport().basic().type() >= 41 && mon_data.agvinfor(j).agvreport().basic().type() <= 60)
                    {
                        ui->carInfo_tableWidget->setItem(j+error_sum, ERROR, new QTableWidgetItem(QString("%1(%2)").arg(vec_ErrorInfo[i]->err_string_lurk).arg(vec_ErrorInfo[i]->err_type)));
                        vec_Car[j]->errorStr = QString("%1(%2)").arg(vec_ErrorInfo[i]->err_string_lurk).arg(vec_ErrorInfo[i]->err_type);
                        no_find_err_id = 1;
                    }
                    else
                    {
                        ui->carInfo_tableWidget->setItem(j+error_sum, ERROR, new QTableWidgetItem(QString("(%1)").arg(vec_ErrorInfo[i]->err_type)));
                        vec_Car[j]->errorStr = QString("(%1)").arg(vec_ErrorInfo[i]->err_type);
                        no_find_err_id = 1;
                    }
                    break;
                }
            }
            if(no_find_err_id == 0)
            {
                ui->carInfo_tableWidget->setItem(j+error_sum, ERROR, new QTableWidgetItem(mon_data.agvinfor(j).agvreport().agvst().error()));
                vec_Car[j]->errorStr = QString("(%1)").arg(mon_data.agvinfor(j).agvreport().agvst().error());
            }

            ui->carInfo_tableWidget->item(j+error_sum,ERROR)->setBackground(QBrush(Qt::NoBrush));


            ui->carInfo_tableWidget->setItem(j+error_sum, BATTERY, new QTableWidgetItem(QString("%1%2").arg(QString::number(mon_data.agvinfor(j).agvreport().agvbat().battery())).arg("%")));
            if(mon_data.agvinfor(j).agvreport().agvbat().chargingtime() > 0)
            {
                ui->carInfo_tableWidget->setItem(j+error_sum,CHARGING, new QTableWidgetItem(QString("%1").arg(mon_data.agvinfor(j).agvreport().agvbat().chargingtime())));
            }
            else
            {
                ui->carInfo_tableWidget->setItem(j+error_sum, CHARGING, new QTableWidgetItem(QString("-")));
            }

            if(mon_data.agvinfor(j).agvreport().agvload().load() >= 0)
            {
                ui->carInfo_tableWidget->setItem(j+error_sum, LOAD, new QTableWidgetItem(QString("有货")));
            }
            else
            {
                ui->carInfo_tableWidget->setItem(j+error_sum, LOAD, new QTableWidgetItem(QString("无货")));
            }
            ui->carInfo_tableWidget->setItem(j+error_sum, ROUTEC, new QTableWidgetItem(QString::number(mon_data.agvinfor(j).agvreport().agvtask().cur_seg())));
            ui->carInfo_tableWidget->setItem(j+error_sum, ROUTEN, new QTableWidgetItem(QString::number(mon_data.agvinfor(j).agvreport().agvtask().nxt_seg())));
            ui->carInfo_tableWidget->setItem(j+error_sum, POINTC, new QTableWidgetItem(QString::number(mon_data.agvinfor(j).agvreport().agvloc().cur_sta())));
            ui->carInfo_tableWidget->setItem(j+error_sum, POINTN, new QTableWidgetItem(QString::number(mon_data.agvinfor(j).agvreport().agvtask().nxt_sta())));

            ui->carInfo_tableWidget->setItem(j+error_sum, IP, new QTableWidgetItem(IPV4IntegerToString((quint32)mon_data.agvinfor(j).ip())));
            ui->carInfo_tableWidget->setItem(j+error_sum, TIME, new QTableWidgetItem(QDateTime::fromSecsSinceEpoch((long long)mon_data.agvinfor(j).ts()).toString("yyyy-MM-dd hh:mm:ss")
                                                                                   ));

            if(mon_data.agvinfor(j).ol() == 1)
            {
                ui->carInfo_tableWidget->setItem(j+error_sum, ONLINE, new QTableWidgetItem(QString("在线")));
            }
            else if(mon_data.agvinfor(j).ol() == 0)
            {
                ui->carInfo_tableWidget->setItem(j+error_sum, ONLINE, new QTableWidgetItem(QString("离线")));
            }
            else
            {
                ui->carInfo_tableWidget->setItem(j+error_sum, ONLINE, new QTableWidgetItem(QString("无")));
            }
            ui->carInfo_tableWidget->setItem(j+error_sum, START, new QTableWidgetItem(QString::number(mon_data.agvinfor(j).agvreport().agvtask().start_id())));
            ui->carInfo_tableWidget->setItem(j+error_sum, END, new QTableWidgetItem(QString::number(mon_data.agvinfor(j).agvreport().agvtask().end_id())));
            ui->carInfo_tableWidget->setItem(j+error_sum, HEIGHT, new QTableWidgetItem(QString::number(mon_data.agvinfor(j).agvreport().agvload().hyd_height())));

            if(mon_data.agvinfor(j).agvreport().protection_size() > 0)
            {
                ui->carInfo_tableWidget->setItem(j+error_sum, SAFEF, new QTableWidgetItem());
                ui->carInfo_tableWidget->setItem(j+error_sum, SAFEB, new QTableWidgetItem());
                ui->carInfo_tableWidget->setItem(j+error_sum, FORKLIGHT, new QTableWidgetItem());
                for(int i = 0 ;i < mon_data.agvinfor(j).agvreport().protection_size(); i++)
                {
                    int key = mon_data.agvinfor(j).agvreport().protection(i).key();
                    if(key == 1)
                    {
                        ui->carInfo_tableWidget->setItem(j+error_sum, SAFEF, new QTableWidgetItem(QString("模式(%1)  区域(%2)").arg((mon_data.agvinfor(j).agvreport().protection(i).value() & 0x0f)).arg(((mon_data.agvinfor(j).agvreport().protection(i).value() & 0xf0)>>4))));
                    }else if(key == 2)
                    {

                        ui->carInfo_tableWidget->setItem(j+error_sum, SAFEB, new QTableWidgetItem(QString("模式(%1)  区域(%2)").arg((mon_data.agvinfor(j).agvreport().protection(i).value() & 0x0f)).arg(((mon_data.agvinfor(j).agvreport().protection(i).value() & 0xf0)>>4))));
                    }else if(key == 3)
                    {

                        if((mon_data.agvinfor(j).agvreport().protection(i).value() & 0x01) == 0)
                        {
                            left_string = QString("左货叉光电正常");
                        }
                        else if((mon_data.agvinfor(j).agvreport().protection(i).value() & 0x01) == 1)
                        {
                            left_string = QString("左货叉光电有障碍物");
                        }
                        else
                        {
                            left_string = QString("数据异常");
                        }
                        if(((mon_data.agvinfor(j).agvreport().protection(i).value() & 0x02)>>1) == 0)
                        {
                            right_string = QString("右货叉光电正常");
                        }
                        else if(((mon_data.agvinfor(j).agvreport().protection(i).value() & 0x02)>>1) == 1)
                        {
                            right_string = QString("右货叉光电有障碍物");
                        }
                        else
                        {
                            right_string = QString("数据异常");
                        }
                        ui->carInfo_tableWidget->setItem(j+error_sum, FORKLIGHT, new QTableWidgetItem(QString("%1  %2").arg(left_string).arg(right_string)));
                    }
                }

            }
            else
            {
                ui->carInfo_tableWidget->setItem(j+error_sum, SAFEF, new QTableWidgetItem());
                ui->carInfo_tableWidget->setItem(j+error_sum, SAFEB, new QTableWidgetItem());
                ui->carInfo_tableWidget->setItem(j+error_sum, FORKLIGHT, new QTableWidgetItem());
            }

            if(mon_data.agvinfor(j).agvreport().dev_status_size() > 0)
            {
                ui->carInfo_tableWidget->setItem(j+error_sum, BATTERYHOLDER, new QTableWidgetItem());
                ui->carInfo_tableWidget->setItem(j+error_sum, HEART1MIN, new QTableWidgetItem());
                ui->carInfo_tableWidget->setItem(j+error_sum, HEART5MIN, new QTableWidgetItem());
                ui->carInfo_tableWidget->setItem(j+error_sum, HEART15MIN, new QTableWidgetItem());
                for(int i = 0 ;i < mon_data.agvinfor(j).agvreport().dev_status_size(); i++)
                {
                    int key = mon_data.agvinfor(j).agvreport().dev_status(i).key();
                    if(key == 1)
                    {
                        ui->carInfo_tableWidget->item(j+error_sum, BATTERYHOLDER)->setText(QString("%1").arg(mon_data.agvinfor(j).agvreport().dev_status(i).value()));
                    }
                    else if(key == 2)
                    {
                        ui->carInfo_tableWidget->item(j+error_sum, HEART1MIN)->setText(QString("%1").arg(mon_data.agvinfor(j).agvreport().dev_status(i).value()));
                    }
                    else if(key == 3)
                    {
                        ui->carInfo_tableWidget->item(j+error_sum, HEART5MIN)->setText(QString("%1").arg(mon_data.agvinfor(j).agvreport().dev_status(i).value()));
                    }
                    else if(key == 4)
                    {
                        ui->carInfo_tableWidget->item(j+error_sum, HEART15MIN)->setText(QString("%1").arg(mon_data.agvinfor(j).agvreport().dev_status(i).value()));
                    }
                }
            }
            else
            {
                ui->carInfo_tableWidget->setItem(j+error_sum, BATTERYHOLDER, new QTableWidgetItem());
                ui->carInfo_tableWidget->setItem(j+error_sum, HEART1MIN, new QTableWidgetItem());
                ui->carInfo_tableWidget->setItem(j+error_sum, HEART5MIN, new QTableWidgetItem());
                ui->carInfo_tableWidget->setItem(j+error_sum, HEART15MIN, new QTableWidgetItem());
            }

            ui->carInfo_tableWidget->setItem(j+error_sum, VERSION, new QTableWidgetItem(QString::fromStdString(mon_data.agvinfor(j).agvreport().basic().version())));
            ui->carInfo_tableWidget->setItem(j+error_sum, VERSION_TS, new QTableWidgetItem(QDateTime::fromSecsSinceEpoch((long long)mon_data.agvinfor(j).agvreport().basic().version_ts()).toString("yyyy-MM-dd hh:mm:ss")));

            if(mon_data.agvinfor(j).agvreport().basic().beta() == 1)
            {
                ui->carInfo_tableWidget->setItem(j+error_sum, BETA, new QTableWidgetItem(QString("正式版")));
            }
            else if(mon_data.agvinfor(j).agvreport().basic().beta() == 0)
            {
                ui->carInfo_tableWidget->setItem(j+error_sum, BETA, new QTableWidgetItem(QString("测试版")));
            }
            else
            {
                ui->carInfo_tableWidget->setItem(j+error_sum, BETA, new QTableWidgetItem(QString("")));
            }

            ui->carInfo_tableWidget->setItem(j+error_sum, BINDAREA, new QTableWidgetItem(QString::number(mon_data.agvinfor(j).agvreport().agvloc().bind_area())));
            ui->carInfo_tableWidget->setItem(j+error_sum, STALIMIT, new QTableWidgetItem(QString::number(mon_data.agvinfor(j).agvreport().agvloc().sta_limit())));

            if(mon_data.agvinfor(j).agvreport().agvst().softstop() == 1)
            {
                ui->carInfo_tableWidget->setItem(j+error_sum, SOFTSTOP, new QTableWidgetItem(QString("软停")));
            }
            else if(mon_data.agvinfor(j).agvreport().agvst().softstop() == 0)
            {
                ui->carInfo_tableWidget->setItem(j+error_sum, SOFTSTOP, new QTableWidgetItem(QString("非软停")));
            }
            else
            {
                ui->carInfo_tableWidget->setItem(j+error_sum, SOFTSTOP, new QTableWidgetItem(QString("无")));
            }

            for(int i = 0; i <  ui->carInfo_tableWidget->columnCount(); i++)
            {
                if(ui->carInfo_tableWidget->item(j+error_sum,i) != nullptr)
                {
                    ui->carInfo_tableWidget->item(j+error_sum,i)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
                }
                else
                {
                    ui->carInfo_tableWidget->setItem(j+error_sum, i, new QTableWidgetItem(QString("")));
                }
            }

        }
        else
        {
            ui->carInfo_tableWidget->item(j+error_sum,CARID)->setText(QString::number(mon_data.agvinfor(j).agvreport().basic().id()));
            if(mon_data.agvinfor(j).agvreport().basic().type() > 0 && mon_data.agvinfor(j).agvreport().basic().type() < 21)
            {
                ui->carInfo_tableWidget->item(j+error_sum, TYPEC)->setText(QString("单舵轮叉车(%1)").arg(mon_data.agvinfor(j).agvreport().basic().type()));
            }
            else if(mon_data.agvinfor(j).agvreport().basic().type() > 20 && mon_data.agvinfor(j).agvreport().basic().type() < 41)
            {
                ui->carInfo_tableWidget->item(j+error_sum, TYPEC)->setText(QString("双轮差速(%1)").arg(mon_data.agvinfor(j).agvreport().basic().type()));
            }
            else if(mon_data.agvinfor(j).agvreport().basic().type() > 40 && mon_data.agvinfor(j).agvreport().basic().type() < 61)
            {
                ui->carInfo_tableWidget->item(j+error_sum, TYPEC)->setText(QString("双舵轮潜入(%1)").arg(mon_data.agvinfor(j).agvreport().basic().type()));
            }
            else
            {
                ui->carInfo_tableWidget->item(j+error_sum, TYPEC)->setText(QString("未知"));
            }
            ui->carInfo_tableWidget->item(j+error_sum,1)->setText(QString::number(mon_data.agvinfor(j).agvreport().agvtask().task_id()));
            if(mon_data.agvinfor(j).agvreport().agvtask().task_type() == 0)
            {
                ui->carInfo_tableWidget->item(j+error_sum,TYPET)->setText(QString("无货"));
            }
            else if(mon_data.agvinfor(j).agvreport().agvtask().task_type() == 1)
            {
                ui->carInfo_tableWidget->item(j+error_sum,TYPET)->setText(QString("取货"));
            }
            else if(mon_data.agvinfor(j).agvreport().agvtask().task_type() == 2)
            {
                ui->carInfo_tableWidget->item(j+error_sum,TYPET)->setText(QString("放货"));
            }
            else if(mon_data.agvinfor(j).agvreport().agvtask().task_type() == -1)
            {
                ui->carInfo_tableWidget->item(j+error_sum,TYPET)->setText(QString("无"));
            }
            else
            {
                ui->carInfo_tableWidget->item(j+error_sum,TYPET)->setText(QString("其他"));
            }
            ui->carInfo_tableWidget->item(j+error_sum, X)->setText(QString::number(mon_data.agvinfor(j).agvreport().agvloc().pos().x(),'f',2));
            ui->carInfo_tableWidget->item(j+error_sum, Y)->setText(QString::number(mon_data.agvinfor(j).agvreport().agvloc().pos().y(),'f',2));
            ui->carInfo_tableWidget->item(j+error_sum, ANGLE)->setText(QString("%1°").arg(QString::number(mon_data.agvinfor(j).agvreport().agvloc().pos_ang(),'f',2)));
            if(mon_data.agvinfor(j).agvreport().agvst().dir() == 1)
            {
                ui->carInfo_tableWidget->item(j+error_sum, DIRECTION)->setText(QString("前行"));
            }
            else if(mon_data.agvinfor(j).agvreport().agvst().dir() == -1)
            {
                ui->carInfo_tableWidget->item(j+error_sum, DIRECTION)->setText(QString("后退"));
            }
            else
            {
                ui->carInfo_tableWidget->item(j+error_sum, DIRECTION)->setText(QString("无"));
            }
            if(speedDisplayMode == 0)
            {
                ui->carInfo_tableWidget->item(j+error_sum, SPEED)->setText(QString("%1").arg(QString::number(mon_data.agvinfor(j).agvreport().agvst().speed(),'f',2)));
            }
            else if(speedDisplayMode == 1)
            {
                ui->carInfo_tableWidget->item(j+error_sum, SPEED)->setText(QString("%1").arg(QString::number(mon_data.agvinfor(j).agvreport().agvst().speed()*60/1000,'f',2)));
            }
            else
            {
                ui->carInfo_tableWidget->item(j+error_sum, SPEED)->setText(QString("%1 ?").arg(QString::number(mon_data.agvinfor(j).agvreport().agvst().speed(),'f',2)));
            }
            if(mon_data.agvinfor(j).agvreport().agvst().state() == 1)
            {
                ui->carInfo_tableWidget->item(j+error_sum,STATE)->setText(QString("自动"));
            }
            else if(mon_data.agvinfor(j).agvreport().agvst().state() == 2)
            {
                ui->carInfo_tableWidget->item(j+error_sum,STATE)->setText(QString("报警"));
            }
            else if(mon_data.agvinfor(j).agvreport().agvst().state() == 3)
            {
                ui->carInfo_tableWidget->item(j+error_sum,STATE)->setText(QString("错误"));
            }
            else if(mon_data.agvinfor(j).agvreport().agvst().state() == 4)
            {
                ui->carInfo_tableWidget->item(j+error_sum,STATE)->setText(QString("手动"));
            }
            else if(mon_data.agvinfor(j).agvreport().agvst().state() == 5)
            {
                ui->carInfo_tableWidget->item(j+error_sum,STATE)->setText(QString("软停"));
            }
            else
            {
                ui->carInfo_tableWidget->item(j+error_sum,STATE)->setText(QString("未知"));
            }


            if(mon_data.agvinfor(j).agvreport().agvst().status() == 0)
            {
                ui->carInfo_tableWidget->item(j+error_sum,STATUS)->setText(QString("运行"));
            }
            else if(mon_data.agvinfor(j).agvreport().agvst().status() == 1)
            {
                ui->carInfo_tableWidget->item(j+error_sum,STATUS)->setText(QString("空闲"));
            }
            else if(mon_data.agvinfor(j).agvreport().agvst().status() == 2)
            {
                ui->carInfo_tableWidget->item(j+error_sum,STATUS)->setText(QString("充电"));
            }
            else if(mon_data.agvinfor(j).agvreport().agvst().status() == 3)
            {
                ui->carInfo_tableWidget->item(j+error_sum,STATUS)->setText(QString("避让"));
            }
            else
            {
                ui->carInfo_tableWidget->item(j+error_sum,STATUS)->setText(QString("未知"));
            }

            int no_find_err_id = 0;
            for(int i = 0; i < vec_ErrorInfo.size(); i++)
            {
                if(vec_ErrorInfo[i]->err_type == mon_data.agvinfor(j).agvreport().agvst().error())
                {
                    if(mon_data.agvinfor(j).agvreport().basic().type() >= 1 && mon_data.agvinfor(j).agvreport().basic().type() <= 20)
                    {
                        ui->carInfo_tableWidget->item(j+error_sum,ERROR)->setText(QString("%1(%2)").arg(vec_ErrorInfo[i]->err_string_fork).arg(vec_ErrorInfo[i]->err_type));
                        vec_Car[j]->errorStr = QString("%1(%2)").arg(vec_ErrorInfo[i]->err_string_fork).arg(vec_ErrorInfo[i]->err_type);
                        no_find_err_id = 1;
                    }
                    else if(mon_data.agvinfor(j).agvreport().basic().type() >= 21 && mon_data.agvinfor(j).agvreport().basic().type() <= 40)
                    {
                        ui->carInfo_tableWidget->item(j+error_sum,ERROR)->setText(QString("%1(%2)").arg(vec_ErrorInfo[i]->err_string_diff).arg(vec_ErrorInfo[i]->err_type));
                        vec_Car[j]->errorStr = QString("%1(%2)").arg(vec_ErrorInfo[i]->err_string_diff).arg(vec_ErrorInfo[i]->err_type);
                        no_find_err_id = 1;
                    }
                    else if(mon_data.agvinfor(j).agvreport().basic().type() >= 41 && mon_data.agvinfor(j).agvreport().basic().type() <= 60)
                    {
                        ui->carInfo_tableWidget->item(j+error_sum,ERROR)->setText(QString("%1(%2)").arg(vec_ErrorInfo[i]->err_string_lurk).arg(vec_ErrorInfo[i]->err_type));
                        vec_Car[j]->errorStr = QString("%1(%2)").arg(vec_ErrorInfo[i]->err_string_lurk).arg(vec_ErrorInfo[i]->err_type);

                        no_find_err_id = 1;
                    }
                    else
                    {
                        ui->carInfo_tableWidget->item(j+error_sum,ERROR)->setText(QString("(%1)").arg(vec_ErrorInfo[i]->err_type));
                        vec_Car[j]->errorStr = QString("(%1)").arg(vec_ErrorInfo[i]->err_type);

                        no_find_err_id = 1;
                    }
                    break;
                }
            }
            if(no_find_err_id == 0)
            {
                ui->carInfo_tableWidget->item(j+error_sum,ERROR)->setText(QString("(%1)").arg(mon_data.agvinfor(j).agvreport().agvst().error()));
                vec_Car[j]->errorStr = QString("(%1)").arg(mon_data.agvinfor(j).agvreport().agvst().error());
            }


            ui->carInfo_tableWidget->item(j+error_sum,ERROR)->setBackground(QBrush(Qt::NoBrush));

            ui->carInfo_tableWidget->item(j+error_sum,BATTERY)->setText(QString("%1%2").arg(QString::number(mon_data.agvinfor(j).agvreport().agvbat().battery())).arg("%"));
            if(mon_data.agvinfor(j).agvreport().agvbat().chargingtime() > 0)
            {
                ui->carInfo_tableWidget->item(j+error_sum, CHARGING)->setText(QString("%1").arg(mon_data.agvinfor(j).agvreport().agvbat().chargingtime()));
            }
            else
            {
                ui->carInfo_tableWidget->item(j+error_sum, CHARGING)->setText(QString("-"));
            }

            if(mon_data.agvinfor(j).agvreport().agvload().load() >= 0)
            {
                ui->carInfo_tableWidget->item(j+error_sum, LOAD)->setText(QString("有货"));
            }
            else
            {
                ui->carInfo_tableWidget->item(j+error_sum, LOAD)->setText(QString("无货"));
            }
            ui->carInfo_tableWidget->item(j+error_sum, ROUTEC)->setText(QString::number(mon_data.agvinfor(j).agvreport().agvtask().cur_seg()));
            ui->carInfo_tableWidget->item(j+error_sum, ROUTEN)->setText(QString::number(mon_data.agvinfor(j).agvreport().agvtask().nxt_seg()));
            ui->carInfo_tableWidget->item(j+error_sum, POINTC)->setText(QString::number(mon_data.agvinfor(j).agvreport().agvloc().cur_sta()));
            ui->carInfo_tableWidget->item(j+error_sum, POINTN)->setText(QString::number(mon_data.agvinfor(j).agvreport().agvtask().nxt_sta()));

            ui->carInfo_tableWidget->item(j+error_sum, IP)->setText(IPV4IntegerToString((quint32)mon_data.agvinfor(j).ip()));

            ui->carInfo_tableWidget->item(j+error_sum,TIME)->setText(QDateTime::fromSecsSinceEpoch((long long)mon_data.agvinfor(j).ts()).toString("yyyy-MM-dd hh:mm:ss"));
            if(mon_data.agvinfor(j).ol() == 1)
            {
                ui->carInfo_tableWidget->item(j+error_sum,ONLINE)->setText(QString("在线"));
            }
            else if(mon_data.agvinfor(j).ol() == 0)
            {
                ui->carInfo_tableWidget->item(j+error_sum,ONLINE)->setText(QString("离线"));
            }
            else
            {
                ui->carInfo_tableWidget->item(j+error_sum,ONLINE)->setText(QString("未知"));
            }
            ui->carInfo_tableWidget->item(j+error_sum, START)->setText(QString::number(mon_data.agvinfor(j).agvreport().agvtask().start_id()));
            ui->carInfo_tableWidget->item(j+error_sum, END)->setText(QString::number(mon_data.agvinfor(j).agvreport().agvtask().end_id()));
            ui->carInfo_tableWidget->item(j+error_sum, HEIGHT)->setText(QString::number(mon_data.agvinfor(j).agvreport().agvload().hyd_height()));

            if(mon_data.agvinfor(j).agvreport().protection_size() > 0)
            {
                ui->carInfo_tableWidget->item(j+error_sum, SAFEF)->setText("");
                ui->carInfo_tableWidget->item(j+error_sum, SAFEB)->setText("");
                ui->carInfo_tableWidget->item(j+error_sum, FORKLIGHT)->setText("");
                for(int i = 0 ;i < mon_data.agvinfor(j).agvreport().protection_size(); i++)
                {
                    int key = mon_data.agvinfor(j).agvreport().protection(i).key();
                    if(key == 1)
                    {
                        ui->carInfo_tableWidget->item(j+error_sum, SAFEF)->setText(QString("模式(%1)  区域(%2)").arg((mon_data.agvinfor(j).agvreport().protection(i).value() & 0x0f)).arg(((mon_data.agvinfor(j).agvreport().protection(i).value() & 0xf0)>>4)));
                    }else if(key == 2)
                    {
                        ui->carInfo_tableWidget->item(j+error_sum, SAFEB)->setText(QString("模式(%1)  区域(%2)").arg((mon_data.agvinfor(j).agvreport().protection(i).value() & 0x0f)).arg(((mon_data.agvinfor(j).agvreport().protection(i).value() & 0xf0)>>4)));
                    }else if(key == 3)
                    {
                        if((mon_data.agvinfor(j).agvreport().protection(i).value() & 0x01) == 0)
                        {
                            left_string = QString("左货叉光电正常");
                        }
                        else if((mon_data.agvinfor(j).agvreport().protection(i).value() & 0x01) == 1)
                        {
                            left_string = QString("左货叉光电有障碍物");
                        }
                        else
                        {
                            left_string = QString("数据异常");
                        }
                        if(((mon_data.agvinfor(j).agvreport().protection(i).value() & 0x02)>>1) == 0)
                        {
                            right_string = QString("右货叉光电正常");
                        }
                        else if(((mon_data.agvinfor(j).agvreport().protection(i).value() & 0x02)>>1) == 1)
                        {
                            right_string = QString("右货叉光电有障碍物");
                        }
                        else
                        {
                            right_string = QString("数据异常");
                        }
                        ui->carInfo_tableWidget->item(j+error_sum, FORKLIGHT)->setText(QString("%1  %2").arg(left_string).arg(right_string));
                    }
                }
            }
            else
            {
                ui->carInfo_tableWidget->item(j+error_sum, SAFEF)->setText("无数据");
                ui->carInfo_tableWidget->item(j+error_sum, SAFEB)->setText("无数据");
                ui->carInfo_tableWidget->item(j+error_sum, FORKLIGHT)->setText("无数据");
            }

            if(mon_data.agvinfor(j).agvreport().dev_status_size() > 0)
            {
                ui->carInfo_tableWidget->item(j+error_sum, BATTERYHOLDER)->setText("");
                ui->carInfo_tableWidget->item(j+error_sum, HEART1MIN)->setText("");
                ui->carInfo_tableWidget->item(j+error_sum, HEART5MIN)->setText("");
                ui->carInfo_tableWidget->item(j+error_sum, HEART15MIN)->setText("");
                for(int i = 0 ;i < mon_data.agvinfor(j).agvreport().dev_status_size(); i++)
                {
                    int key = mon_data.agvinfor(j).agvreport().dev_status(i).key();
                    if(key == 1)
                    {
                        ui->carInfo_tableWidget->item(j+error_sum, BATTERYHOLDER)->setText(QString("%1").arg(mon_data.agvinfor(j).agvreport().dev_status(i).value()));
                    }
                    else if(key == 2)
                    {
                        ui->carInfo_tableWidget->item(j+error_sum, HEART1MIN)->setText(QString("%1").arg(mon_data.agvinfor(j).agvreport().dev_status(i).value()));
                    }
                    else if(key == 3)
                    {
                        ui->carInfo_tableWidget->item(j+error_sum, HEART5MIN)->setText(QString("%1").arg(mon_data.agvinfor(j).agvreport().dev_status(i).value()));
                    }
                    else if(key == 4)
                    {
                        ui->carInfo_tableWidget->item(j+error_sum, HEART15MIN)->setText(QString("%1").arg(mon_data.agvinfor(j).agvreport().dev_status(i).value()));
                    }
                }
            }
            else
            {
                ui->carInfo_tableWidget->item(j+error_sum, BATTERYHOLDER)->setText("无数据");
                ui->carInfo_tableWidget->item(j+error_sum, HEART1MIN)->setText("无数据");
                ui->carInfo_tableWidget->item(j+error_sum, HEART5MIN)->setText("无数据");
                ui->carInfo_tableWidget->item(j+error_sum, HEART15MIN)->setText("无数据");
            }


            ui->carInfo_tableWidget->item(j+error_sum, VERSION)->setText(QString::fromStdString(mon_data.agvinfor(j).agvreport().basic().version()));
            ui->carInfo_tableWidget->item(j+error_sum, VERSION_TS)->setText(QDateTime::fromSecsSinceEpoch((long long)mon_data.agvinfor(j).agvreport().basic().version_ts()).toString("yyyy-MM-dd hh:mm:ss"));
            if(mon_data.agvinfor(j).agvreport().basic().beta() == 1)
            {
                ui->carInfo_tableWidget->item(j+error_sum, BETA)->setText(QString("正式版"));
            }
            else if(mon_data.agvinfor(j).agvreport().basic().beta() == 0)
            {
                ui->carInfo_tableWidget->item(j+error_sum, BETA)->setText(QString("测试版"));
            }
            else
            {
                ui->carInfo_tableWidget->item(j+error_sum, BETA)->setText(QString(""));
            }

            ui->carInfo_tableWidget->item(j+error_sum, START)->setText(QString::number(mon_data.agvinfor(j).agvreport().agvtask().start_id()));
            ui->carInfo_tableWidget->item(j+error_sum, BINDAREA)->setText(QString::number(mon_data.agvinfor(j).agvreport().agvloc().bind_area()));
            ui->carInfo_tableWidget->item(j+error_sum, STALIMIT)->setText(QString::number(mon_data.agvinfor(j).agvreport().agvloc().sta_limit()));

            if(mon_data.agvinfor(j).agvreport().agvst().softstop() == 1)
            {
                ui->carInfo_tableWidget->item(j+error_sum,SOFTSTOP)->setText(QString("软停"));
            }
            else if(mon_data.agvinfor(j).agvreport().agvst().softstop() == 0)
            {
                ui->carInfo_tableWidget->item(j+error_sum,SOFTSTOP)->setText(QString("非软停"));
            }
            else
            {
                ui->carInfo_tableWidget->item(j+error_sum,SOFTSTOP)->setText(QString("无"));
            }

        }
        if(mon_data.agvinfor(j).agvreport().agvst().error() > 0 && mon_data.agvinfor(j).ol() == 1 )
        {
            ui->carInfo_tableWidget->item(j+error_sum,ERROR)->setBackground(Qt::red);
        }

        if(mon_data.agvinfor(j).agvreport().basic().id() == floatWindowCarId)
        {
            unsigned int dev_id = mon_data.agvinfor(j).agvreport().basic().id();
            unsigned char dev_type = mon_data.agvinfor(j).agvreport().basic().type();
            int mission_id = mon_data.agvinfor(j).agvreport().agvtask().task_id();
            char mission_type = mon_data.agvinfor(j).agvreport().agvtask().task_type();
            float pos_x = mon_data.agvinfor(j).agvreport().agvloc().pos().x();
            float pos_y = mon_data.agvinfor(j).agvreport().agvloc().pos().y();
            float pos_ang = mon_data.agvinfor(j).agvreport().agvloc().pos_ang();
            char dir = mon_data.agvinfor(j).agvreport().agvst().dir();
            float speed = mon_data.agvinfor(j).agvreport().agvst().speed();
            char state = mon_data.agvinfor(j).agvreport().agvst().state();
            char status = mon_data.agvinfor(j).agvreport().agvst().status();
            int err_type = mon_data.agvinfor(j).agvreport().agvst().error();
            float battery = mon_data.agvinfor(j).agvreport().agvbat().battery();
            int loadnum = mon_data.agvinfor(j).agvreport().agvload().load();
            int cur_segment = mon_data.agvinfor(j).agvreport().agvtask().cur_seg();
            int nxt_segment = mon_data.agvinfor(j).agvreport().agvtask().nxt_seg();
            unsigned int ip = mon_data.agvinfor(j).ip();
            long long timestamp = mon_data.agvinfor(j).ts();
            int online = mon_data.agvinfor(j).ol();
            int start_id = mon_data.agvinfor(j).agvreport().agvtask().start_id();
            int end_id = mon_data.agvinfor(j).agvreport().agvtask().end_id();
            float hyd_height = mon_data.agvinfor(j).agvreport().agvload().hyd_height();
            char protection1;
            char protection2;
            char protection3;
            char softstop = mon_data.agvinfor(j).agvreport().agvst().softstop();
            int idle_wait_ts = mon_data.agvinfor(j).timeinfo().idle_wait_ts();
            int avoid_set_ts = mon_data.agvinfor(j).timeinfo().avoid_set_ts();
            int manual_on_ts = mon_data.agvinfor(j).timeinfo().manual_on_ts();
            int softstop_on_ts = mon_data.agvinfor(j).timeinfo().softstop_on_ts();
            int warning_on_ts = mon_data.agvinfor(j).timeinfo().warning_on_ts();
            int stopStatus = mon_data.agvinfor(j).avoidinfo().stopstatus();
            int stopLevel = mon_data.agvinfor(j).avoidinfo().stoplevel();
            int stopByAGVId = mon_data.agvinfor(j).avoidinfo().stopbyagvid();
            int stopReason = mon_data.agvinfor(j).avoidinfo().stopreason();
            int speedLevel = mon_data.agvinfor(j).avoidinfo().speedlevel();
            int speedByAGVId = mon_data.agvinfor(j).avoidinfo().speedbyagvid();
            int confPoint = mon_data.agvinfor(j).avoidinfo().confpoint();
            QString errorStr = vec_Car[j]->errorStr;
            QString mission_type_temp;

            floatingwindow->carId = dev_id;
            floatingwindow->mission_id = mission_id;
            floatingwindow->ui->carId_label->setText(QString("车号: %1").arg(dev_id));
            if(mission_id <= 0)
                floatingwindow->ui->taskId_label->setText(QString("任务: 无"));
            else
                floatingwindow->ui->taskId_label->setText(QString("任务: %1").arg(mission_id));
            floatingwindow->ui->errorInfo_label->setText(QString("报警: %1").arg(errorStr));

            if(stopReason == 2)
            {
                floatingwindow->ui->confPoint_label->setText(QString("避让冲突: %1").arg(confPoint));
                floatingwindow->ui->confPoint_label->setVisible(true);
            }
            else
            {
                floatingwindow->ui->confPoint_label->setVisible(false);
            }
            if(speedByAGVId != -1)
            {
                floatingwindow->ui->speedLevel_label->setText(QString("限速: %1(AGV: %2)").arg(speedLevel).arg(speedByAGVId));
                floatingwindow->ui->speedLevel_label->setVisible(true);
            }
            else
            {
                floatingwindow->ui->speedLevel_label->setVisible(false);
            }


            if(QString::number(state) == "4")
            {
                floatingwindow->ui->time_label->setText(QString("手动: %1").arg(sec_to_time(manual_on_ts)));
            }
            else if(err_type > 0)
            {
                floatingwindow->ui->time_label->setText(QString("报警: %1").arg(sec_to_time(warning_on_ts)));
            }
            else if(QString::number(softstop) == "1")
            {
                floatingwindow->ui->time_label->setText(QString("软停: %1").arg(sec_to_time(softstop_on_ts)));
            }
            else  if(QString::number(status) == "3")
            {
                floatingwindow->ui->time_label->setText(QString("避让: %1").arg(sec_to_time(avoid_set_ts)));
            }
            else if(QString::number(status) == "1")
            {
                floatingwindow->ui->time_label->setText(QString("空闲: %1").arg(sec_to_time(idle_wait_ts)));
            }
            else
            {
                floatingwindow->ui->time_label->setText("-");
            }

            if(mission_type == 1)
            {
                 mission_type_temp = QString("取货");
            }
            else if(mission_type == 2)
            {
                 mission_type_temp = QString("放货");
            }
            else if(mission_type == 0)
            {
                 mission_type_temp = QString("无货");
            }
            else if(mission_type == 3)
            {
                mission_type_temp = QString("充电");
            }
            else
            {
                 mission_type_temp = QString("-"); //任务类型错误
            }

            if(status == 3)
            {
                if(stopStatus == 0)
                {
                    floatingwindow->ui->stopStatus_label->setText(QString("避让设置: 不避让"));
                }
                else if(stopStatus == 1)
                {
                    floatingwindow->ui->stopStatus_label->setText(QString("避让设置: 避让"));
                }
                else
                {
                    floatingwindow->ui->stopStatus_label->setText(QString("避让不避让"));
                }

                if(stopLevel == 0)
                {
                    floatingwindow->ui->stopLevel_label->setText(QString("避让级别: 普通避让"));
                }
                else if(stopLevel == 10)
                {
                    floatingwindow->ui->stopLevel_label->setText(QString("避让级别: 紧急避让"));
                }
                else
                {
                    floatingwindow->ui->stopLevel_label->setText(QString("避让级别: "));
                }
                floatingwindow->ui->stopByAGVId_label->setText(QString("避让AGV: %1 ").arg(stopByAGVId));
                switch (stopReason) {
                case 1:
                    floatingwindow->ui->stopReason_label->setText(QString("避让原因: 数据异常(%1)").arg(stopReason));
                    break;
                case 2:
                    floatingwindow->ui->stopReason_label->setText(QString("避让原因: 点扫描被占用(%1)").arg(stopReason));
                    break;
                case 3:
                    floatingwindow->ui->stopReason_label->setText(QString("避让原因: 进站检测(%1)").arg(stopReason));
                    break;
                case 4:
                    floatingwindow->ui->stopReason_label->setText(QString("避让原因: 流控区域限流(%1)").arg(stopReason));
                    break;
                case 5:
                    floatingwindow->ui->stopReason_label->setText(QString("避让原因: 反向路径(%1)").arg(stopReason));
                    break;
                case 6:
                    floatingwindow->ui->stopReason_label->setText(QString("避让原因: 卷帘门关闭(%1)").arg(stopReason));
                    break;
                case 7:
                    floatingwindow->ui->stopReason_label->setText(QString("避让原因: 级别区限制(%1)").arg(stopReason));
                    break;
                case 8:
                    floatingwindow->ui->stopReason_label->setText(QString("避让原因: 前向框(%1)").arg(stopReason));
                    break;
                case 9:
                    floatingwindow->ui->stopReason_label->setText(QString("避让原因: 交叉不连接路径(%1)").arg(stopReason));
                    break;
                case 10:
                    floatingwindow->ui->stopReason_label->setText(QString("避让原因: 同向路径(%1)").arg(stopReason));
                    break;
                case 11:
                    floatingwindow->ui->stopReason_label->setText(QString("避让原因: 停止路径(%1)").arg(stopReason));
                    break;
                case 12:
                    floatingwindow->ui->stopReason_label->setText(QString("避让原因: 网络异常(%1)").arg(stopReason));
                    break;
                case 21:
                    floatingwindow->ui->stopReason_label->setText(QString("避让原因: 防碰撞避让(%1)").arg(stopReason));
                    break;
                default:
                    floatingwindow->ui->stopReason_label->setText(QString("避让原因: (%1)").arg(stopReason));
                    break;
                }

                floatingwindow->ui->avoidFrame->setVisible(true);
            }
            else
            {
                floatingwindow->ui->avoidFrame->setVisible(false);
            }
            if(mission_type == -1)
            {
                floatingwindow->ui->goodsInfo_label->setText(QString("类型: 无"));
            }
            else
            {
                floatingwindow->ui->goodsInfo_label->setText(QString("类型: %1(终点:%2)").arg(mission_type_temp).arg(end_id));
            }
        }
    }
}

void SubWidget::protoTcp_has_connected()
{
    save_ip_to_config(ui->tcp_lineEdit->currentText());
    heartArray.clear();
    protoUdpSocket->bind(myport);
    connect(protoUdpSocket,&QUdpSocket::readyRead,this,&SubWidget::proto_recv_data);
    if(abort_protoTcp_timer->isActive())
    {
        //qDebug()<<"protoTcp connected and stop tcp abort timer";
        abort_protoTcp_timer->stop();
    }
    char STX1 = 0x6E;
    char STX2 = 0x6D;
    int len;
    int agvInfo = 0;
    mon_ask.set_data(myport);
    mon_ask.set_area(rgv_area_id);
    mon_ask.set_type(agvInfo);
    len = mon_ask.ByteSizeLong() + 1 + 1 + 4 + 1 + 1;
    QByteArray tempArray;
    tempArray.resize(mon_ask.ByteSizeLong());
    mon_ask.SerializeToArray(tempArray.data(),mon_ask.ByteSizeLong());
    heartArray.append(STX1);
    heartArray.append(STX2);
    QByteArray lenArray;
    lenArray.resize(sizeof(len));
    memcpy(lenArray.data(),&len,sizeof(len));
    heartArray.append(lenArray);
    heartArray.append(tempArray);

    uint16_t crc16 = crc_calculate((uint8_t*)tempArray.data(),tempArray.size());
    char CKA = (crc16 & 0x00FF);
    char CKB = ((crc16 >> 8) & 0x00FF);
    heartArray.append(CKA);
    heartArray.append(CKB);
    qint64 temp = protoTcpSocket->write(heartArray);
    //qDebug()<<"protoTcp send:"<<temp;
    disconnect(protoTcpSocket,&QTcpSocket::connected,this,&SubWidget::protoTcp_has_connected);

    if(!send_protoTcp_timer->isActive()){
        send_protoTcp_timer->start(2000);
    }
    ui->tcp_disconnect_Button->setVisible(true);
    ui->proto_tcp_connect_Button->setVisible(false);
    ui->proto_tcp_connect_Button->setEnabled(false);
    ui->tcp_disconnect_Button->setEnabled(true);
    vec_LineItem.clear();
    vec_ArcItem.clear();
    save_items_of_scene();
    //qDebug()<<"protoTcp protoTcp_has_connected finish";
}



void SubWidget::use_timer_send_tcp_data()
{
    send_mon_tcp.LOGO1 = 'M';
    send_mon_tcp.LOGO2 = 'E';
    send_mon_tcp.LOGO3 = 'S';
    send_mon_tcp.LOGO4 = 'A';
    send_mon_tcp.LOGO5 = 'G';
    send_mon_tcp.LOGO6 = 'V';
    send_mon_tcp.FLAG1 = 'M';
    send_mon_tcp.FLAG2 = 'O';
    send_mon_tcp.FLAG3 = 'N';
    send_mon_tcp.port = myport;
    send_mon_tcp.area = rgv_area_id;
    if(ui->tcp_lineEdit->currentText() == "127.0.0.1")
    {
        qint64 temp = tcpSocket->write((char*)&send_mon_tcp,sizeof(send_mon_tcp));
    }
    else
    {
        tcpSocket->connectToHost(QHostAddress(ui->tcp_lineEdit->currentText()),9104);
        qint64 temp = tcpSocket->write((char*)&send_mon_tcp,sizeof(send_mon_tcp));
    }
}

void SubWidget::use_timer_send_protoTcp_data()
{
    heartArray.clear();
    char STX1 = 0x6E;
    char STX2 = 0x6D;
    int len;

    int agvInfo = 0;
    mon_ask.set_data(myport);
    mon_ask.set_area(rgv_area_id);
    mon_ask.set_type(agvInfo);
    len = mon_ask.ByteSizeLong() + 1 + 1 + 4 + 1 + 1;
    QByteArray tempArray;
    tempArray.resize(mon_ask.ByteSizeLong());
    mon_ask.SerializeToArray(tempArray.data(),mon_ask.ByteSizeLong());
    heartArray.append(STX1);
    heartArray.append(STX2);
    QByteArray lenArray;
    lenArray.resize(sizeof(len));
    memcpy(lenArray.data(),&len,sizeof(len));
    heartArray.append(lenArray);
    heartArray.append(tempArray);
    uint16_t crc16 = crc_calculate((uint8_t*)tempArray.data(),tempArray.size());
    char CKA = (crc16 & 0x00FF);
    char CKB = ((crc16 >> 8) & 0x00FF);
    heartArray.append(CKA);
    heartArray.append(CKB);
    if(ui->tcp_lineEdit->currentText() == "127.0.0.1")
    {
        qint64 temp = tcpSocket->write((char*)&send_mon_tcp,sizeof(send_mon_tcp));
    }
    else
    {
        protoTcpSocket->connectToHost(QHostAddress(ui->tcp_lineEdit->currentText()),9104);
        qint64 temp = protoTcpSocket->write(heartArray);
    }

}

void SubWidget::abort_tcp()
{
    tcpSocket->abort();
    if(abort_tcp_timer->isActive()){
        //qDebug()<<"tcp abort and stop tcp abort timer";
        abort_tcp_timer->stop();
    }
}

void SubWidget::abort_protoTcp()
{
    protoTcpSocket->abort();
    if(abort_protoTcp_timer->isActive()){
        //qDebug()<<"protoTcp abort and stop tcp abort timer";
        abort_protoTcp_timer->stop();
    }
}

void SubWidget::getTaskDataButtonSlot()
{
    taskInfoDialog->ui->getTaskButton->setEnabled(true);
    getTaskData_timer->stop();
}


void SubWidget::playback_play_send_udp()
{
    QSqlQuery query2(playback_db);
    QString tempstr = QString("select id,monitordata,route_data,uploaddate from %1 where id=%2 limit 0,1;").arg(playback_tool->ui->playback_table_lineEdit->text()).arg(plagback_start_id);
    query2.exec(tempstr);

    while(query2.next())
    {
        playback_tcp_array.clear();
        playback_tcp_array.append(query2.value(2).toByteArray());


        playback_tool->ui->playback_curtime_label->setText("Time:"+query2.value(3).toDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        QByteArray temparr(query2.value(1).toByteArray());
        qint64 temp = protoUdpSocket->writeDatagram(temparr,temparr.size(),QHostAddress("127.0.0.1"),myport);
        slider_id = plagback_start_id;
        if(playback_tool->ui->playSpeed_comboBox->currentText().toInt() > 0)
        {
            plagback_start_id = plagback_start_id + playback_tool->ui->playSpeed_comboBox->currentText().toInt();
        }
        else
        {
            plagback_start_id++;
        }

    }
}

void SubWidget::getTaskDataSlot()
{
    taskInfoDialog->ui->tasktWidget->setRowCount(0);
    task_heartArray.clear();
    char STX1 = 0x6E;
    char STX2 = 0x6D;
    int len;
    int taskInfo = 1;
    mon_ask.set_data(myport);
    mon_ask.set_area(rgv_area_id);
    mon_ask.set_type(taskInfo);
    len = mon_ask.ByteSizeLong() + 1 + 1 + 4 + 1 + 1;
    QByteArray tempArray;
    tempArray.resize(mon_ask.ByteSizeLong());
    mon_ask.SerializeToArray(tempArray.data(),mon_ask.ByteSizeLong());
    task_heartArray.append(STX1);
    task_heartArray.append(STX2);
    QByteArray lenArray;
    lenArray.resize(sizeof(len));
    memcpy(lenArray.data(),&len,sizeof(len));
    task_heartArray.append(lenArray);
    task_heartArray.append(tempArray);
    uint16_t crc16 = crc_calculate((uint8_t*)tempArray.data(),tempArray.size());
    char CKA = (crc16 & 0x00FF);
    char CKB = ((crc16 >> 8) & 0x00FF);
    task_heartArray.append(CKA);
    task_heartArray.append(CKB);
    if(ui->tcp_lineEdit->currentText() == "127.0.0.1")
    {

    }
    else
    {
        protoTcpSocketTask->connectToHost(QHostAddress(ui->tcp_lineEdit->currentText()),9106);
        qint64 temp = protoTcpSocketTask->write(task_heartArray);

    }
    taskInfoDialog->ui->getTaskButton->setEnabled(false);
    getTaskData_timer->start(5000);
}

void SubWidget::getCxjDataSlot()
{
    cxjDataInfoDialog->ui->cxjDatatWidget->setRowCount(0);
    cxjData_heartArray.clear();
    char STX1 = 0x6E;
    char STX2 = 0x6D;
    int len;
    int cxjDataInfo = 3;
    mon_ask.set_data(myport);
    mon_ask.set_area(rgv_area_id);
    mon_ask.set_type(cxjDataInfo);
    len = mon_ask.ByteSizeLong() + 1 + 1 + 4 + 1 + 1;
    QByteArray tempArray;
    tempArray.resize(mon_ask.ByteSizeLong());
    mon_ask.SerializeToArray(tempArray.data(),mon_ask.ByteSizeLong());
    cxjData_heartArray.append(STX1);
    cxjData_heartArray.append(STX2);
    QByteArray lenArray;
    lenArray.resize(sizeof(len));
    memcpy(lenArray.data(),&len,sizeof(len));
    cxjData_heartArray.append(lenArray);
    cxjData_heartArray.append(tempArray);
    uint16_t crc16 = crc_calculate((uint8_t*)tempArray.data(),tempArray.size());
    char CKA = (crc16 & 0x00FF);
    char CKB = ((crc16 >> 8) & 0x00FF);
    cxjData_heartArray.append(CKA);
    cxjData_heartArray.append(CKB);
    if(ui->tcp_lineEdit->currentText() == "127.0.0.1")
    {
    }
    else
    {
        protoTcpSocketCxjData->connectToHost(QHostAddress(ui->tcp_lineEdit->currentText()),9106);
        qint64 temp = protoTcpSocketCxjData->write(cxjData_heartArray);
    }
}

void SubWidget::getCxjBackSlot()
{
    cxjBackInfoDialog->ui->cxjBacktWidget->setRowCount(0);
    cxjBack_heartArray.clear();
    char STX1 = 0x6E;
    char STX2 = 0x6D;
    int len;
    int cxjBackInfo = 4;
    mon_ask.set_data(myport);
    mon_ask.set_area(rgv_area_id);
    mon_ask.set_type(cxjBackInfo);
    len = mon_ask.ByteSizeLong() + 1 + 1 + 4 + 1 + 1;
    QByteArray tempArray;
    tempArray.resize(mon_ask.ByteSizeLong());
    mon_ask.SerializeToArray(tempArray.data(),mon_ask.ByteSizeLong());
    cxjBack_heartArray.append(STX1);
    cxjBack_heartArray.append(STX2);
    QByteArray lenArray;
    lenArray.resize(sizeof(len));
    memcpy(lenArray.data(),&len,sizeof(len));
    cxjBack_heartArray.append(lenArray);
    cxjBack_heartArray.append(tempArray);
    uint16_t crc16 = crc_calculate((uint8_t*)tempArray.data(),tempArray.size());
    char CKA = (crc16 & 0x00FF);
    char CKB = ((crc16 >> 8) & 0x00FF);
    cxjBack_heartArray.append(CKA);
    cxjBack_heartArray.append(CKB);
    if(ui->tcp_lineEdit->currentText() == "127.0.0.1")
    {

    }
    else
    {
        protoTcpSocketCxjBack->connectToHost(QHostAddress(ui->tcp_lineEdit->currentText()),9106);
        qint64 temp = protoTcpSocketCxjBack->write(cxjBack_heartArray);

    }
}

void SubWidget::getBatteryDataSlot()
{
    batteryInfo_widget->ui->batteryTableWidget->setRowCount(0);
    battery_heartArray.clear();
    char STX1 = 0x7A;
    char STX2 = 0x7B;
    int len;

    batterydata.set_info(batteryMonitor::BatteryData::PROFILE);

    len = batterydata.ByteSizeLong() + 1 + 1 + 4 + 1 + 1;
    QByteArray tempArray;
    tempArray.resize(batterydata.ByteSizeLong());
    batterydata.SerializeToArray(tempArray.data(),batterydata.ByteSizeLong());
    battery_heartArray.append(STX1);
    battery_heartArray.append(STX2);
    QByteArray lenArray;
    lenArray.resize(sizeof(len));
    memcpy(lenArray.data(),&len,sizeof(len));
    battery_heartArray.append(lenArray);
    battery_heartArray.append(tempArray);
    uint16_t crc16 = crc_calculate((uint8_t*)tempArray.data(),tempArray.size());
    char CKA = (crc16 & 0x00FF);
    char CKB = ((crc16 >> 8) & 0x00FF);
    battery_heartArray.append(CKA);
    battery_heartArray.append(CKB);


    protoTcpSocketBattery->connectToHost(QHostAddress(ui->tcp_lineEdit->currentText()),9203);
    qint64 temp = protoTcpSocketBattery->write(battery_heartArray);
}

void SubWidget::getBatteryDetailDataSlot(int batteryId)
{
    batteryInfo_widget->ui->batteryDetailWidget->setRowCount(0);
    battery_heartArray.clear();
    char STX1 = 0x7A;
    char STX2 = 0x7B;
    int len;

    batterydata.set_info(batteryMonitor::BatteryData::DETAIL);
    batterydata.set_btytag(batteryId);
    len = batterydata.ByteSizeLong() + 1 + 1 + 4 + 1 + 1;
    QByteArray tempArray;
    tempArray.resize(batterydata.ByteSizeLong());
    batterydata.SerializeToArray(tempArray.data(),batterydata.ByteSizeLong());
    battery_heartArray.append(STX1);
    battery_heartArray.append(STX2);
    QByteArray lenArray;
    lenArray.resize(sizeof(len));
    memcpy(lenArray.data(),&len,sizeof(len));
    battery_heartArray.append(lenArray);
    battery_heartArray.append(tempArray);
    uint16_t crc16 = crc_calculate((uint8_t*)tempArray.data(),tempArray.size());
    char CKA = (crc16 & 0x00FF);
    char CKB = ((crc16 >> 8) & 0x00FF);
    battery_heartArray.append(CKA);
    battery_heartArray.append(CKB);


    protoTcpSocketBatteryDetail->connectToHost(QHostAddress(ui->tcp_lineEdit->currentText()),9203);
    qint64 temp = protoTcpSocketBatteryDetail->write(battery_heartArray);
}

void SubWidget::sleep(unsigned int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}


void SubWidget::ParameterInit()
{
    QSettings settings("Config.ini", QSettings::IniFormat);

    fileName = settings.value("map").toString();

    colorName = settings.value("color").toString();

    routeColorName = settings.value("rcolor").toString();

    ip_text = settings.value("ip").toString();

    area_status = settings.value("area_status").toString();

    font = settings.value("font").toString();

    font_S = settings.value("font_S").toString();

    seg_w = settings.value("seg_w").toString();

    fw_status = settings.value("fw_status").toString();

    arrow = settings.value("arrow").toString();

    sa_bg = settings.value("sa_bg").toString();

    SDM = settings.value("SDM").toString();

    arrow2 = settings.value("arrow2").toString();

    X_MIN = settings.value("X_MIN").toString();

    X_MAX = settings.value("X_MAX").toString();

    Y_MIN = settings.value("Y_MIN").toString();

    Y_MAX = settings.value("Y_MAX").toString();

    MAP_ID = settings.value("MAP_ID").toString();

    START_ANGLE = settings.value("START_ANGLE").toString();

    LOGISTICS_MAX = settings.value("LOGISTICS_MAX").toString();

    Station_B_F = settings.value("Station_B_F").toString();

    oL = settings.value("CARPARAM/OL").toInt();
    bATTERYHOLDER = settings.value("CARPARAM/BATTERYHOLDER").toInt();
    bETA = settings.value("CARPARAM/BETA").toInt();
    bTY = settings.value("CARPARAM/BTY").toInt();
    cHARGING = settings.value("CARPARAM/CHARGING").toInt();
    dIRECTION = settings.value("CARPARAM/DIRECTION").toInt();
    eND = settings.value("CARPARAM/END").toInt();
    fORKLIGHT = settings.value("CARPARAM/FORKLIGHT").toInt();
    hEART15MIN = settings.value("CARPARAM/HEART15MIN").toInt();
    hEART1MIN = settings.value("CARPARAM/HEART1MIN").toInt();
    hEART5MIN = settings.value("CARPARAM/HEART5MIN").toInt();
    hEIGHT = settings.value("CARPARAM/HEIGHT").toInt();
    iP = settings.value("CARPARAM/IP").toInt();
    pOINTC = settings.value("CARPARAM/POINTC").toInt();
    pOINTN = settings.value("CARPARAM/POINTN").toInt();
    sAFEB = settings.value("CARPARAM/SAFEB").toInt();
    sAFEF = settings.value("CARPARAM/SAFEF").toInt();
    sPEED = settings.value("CARPARAM/SPEED").toInt();
    sTART = settings.value("CARPARAM/START").toInt();
    tYPEC = settings.value("CARPARAM/TYPEC").toInt();
    vERSION = settings.value("CARPARAM/VERSION").toInt();
    vERSION_TS = settings.value("CARPARAM/VERSION_TS").toInt();
    bINDAREA = settings.value("CARPARAM/BINDAREA").toInt();
    sTALIMIT = settings.value("CARPARAM/STALIMIT").toInt();
    xCOORD = settings.value("CARPARAM/XCOORD").toInt();
    yCOORD = settings.value("CARPARAM/YCOORD").toInt();
    aNGLE = settings.value("CARPARAM/ANGLE").toInt();

    ui->checkBox_OL->setCheckState(Qt::CheckState(oL));
    ui->checkBox_battery->setCheckState(Qt::CheckState(bTY));
    ui->checkBox_cTime->setCheckState(Qt::CheckState(cHARGING));
    ui->checkBox_startId->setCheckState(Qt::CheckState(sTART));
    ui->checkBox_endId->setCheckState(Qt::CheckState(eND));
    ui->checkBox_hight->setCheckState(Qt::CheckState(hEIGHT));
    ui->checkBox_dir->setCheckState(Qt::CheckState(dIRECTION));
    ui->checkBox_speed->setCheckState(Qt::CheckState(sPEED));
    ui->checkBox_curpoint->setCheckState(Qt::CheckState(pOINTC));
    ui->checkBox_nxtpoint->setCheckState(Qt::CheckState(pOINTN));
    ui->checkBox_fSafe->setCheckState(Qt::CheckState(sAFEF));
    ui->checkBox_bSafe->setCheckState(Qt::CheckState(sAFEB));
    ui->checkBox_forkLight->setCheckState(Qt::CheckState(fORKLIGHT));
    ui->checkBox_IPaddr->setCheckState(Qt::CheckState(iP));
    ui->checkBox_typeA->setCheckState(Qt::CheckState(tYPEC));
    ui->checkBox_batteryZD->setCheckState(Qt::CheckState(bATTERYHOLDER));
    ui->checkBox_1min->setCheckState(Qt::CheckState(hEART1MIN));
    ui->checkBox_5min->setCheckState(Qt::CheckState(hEART5MIN));
    ui->checkBox_15min->setCheckState(Qt::CheckState(hEART15MIN));
    ui->checkBox_carVersion->setCheckState(Qt::CheckState(vERSION));
    ui->checkBox_versionType->setCheckState(Qt::CheckState(bETA));
    ui->checkBox_carCT->setCheckState(Qt::CheckState(vERSION_TS));
    ui->checkBox_xcoord->setCheckState(Qt::CheckState(xCOORD));
    ui->checkBox_ycoord->setCheckState(Qt::CheckState(yCOORD));
    ui->checkBox_angle->setCheckState(Qt::CheckState(aNGLE));
    ui->checkBox_bindarea->setCheckState(Qt::CheckState(bINDAREA));
    ui->checkBox_stalimitf->setCheckState(Qt::CheckState(sTALIMIT));
}


void SubWidget::ParameterSave(QString Type, QString p)
{
        QSettings settings("Config.ini", QSettings::IniFormat);
        settings.setValue(Type,p);
}


void SubWidget::connect_playback_DB()
{
    if(pingOk(playback_tool->ui->playback_ip_lineEdit->curText()) == true)
    {
        on_tcp_disconnect_Button_clicked();
        playbacking = 1;//回放中
        ui->tcp_connect_Button->setVisible(false);
        ui->proto_tcp_connect_Button->setVisible(false);
        ui->tcp_disconnect_Button->setVisible(false);

        ui->stop_playback_Button->setVisible(true);

        playback_db =QSqlDatabase::addDatabase("QMYSQL","playback_connection");
        playback_db.setHostName(playback_tool->ui->playback_ip_lineEdit->curText());
        playback_db.setUserName("manager");
        playback_db.setPassword("AgvManager");
        playback_db.setDatabaseName("agvtransfer_log");
        if(!playback_db.open())
        {
            QMessageBox::warning(this,"数据库打开失败",playback_db.lastError().text());
            return ;
        }
        else
        {
            playback_tool->ui->playback_confirm_Button->setEnabled(true);
            protoUdpSocket->bind(myport);
            on_proto_tcp_connect_Button_clicked();

            QSqlQuery query1(playback_db);

            int count = 0;

            playback_tool->ui->playback_status_label->setText(QString("%1 %2 rows.").arg(playback_tool->ui->playback_table_lineEdit->text()).arg(count));



            QDateTime current_sys_time = QDateTime::currentDateTime().addSecs(-300); ;
            QString tempstr = QString("select id,monitordata,route_data,uploaddate from %1 where uploaddate=\"%2\" limit 0,1;").arg(playback_tool->ui->playback_table_lineEdit->text()).arg(current_sys_time.toString("yyyy-MM-dd hh:mm:ss"));

            query1.exec(tempstr);

            if(query1.next() == true)
            {
                emit sendMessage(QString("进入回放模式."));
                playback_tcp_array.clear();
                playback_tcp_array.append(query1.value(2).toByteArray());

                plagback_start_id = query1.value(0).toLongLong();

                playback_tool->ui->playback_curtime_label->setText("Time:"+ query1.value(3).toDateTime().toString("yyyy-MM-dd hh:mm:ss"));
                QByteArray temparr(query1.value(1).toByteArray());
                qint64 temp = protoUdpSocket->writeDatagram(temparr,temparr.size(),QHostAddress("127.0.0.1"),myport);
                qint64 temp2 = protoUdpSocket->writeDatagram(temparr,temparr.size(),QHostAddress("127.0.0.1"),myport);
                qint64 temp3 = protoUdpSocket->writeDatagram(temparr,temparr.size(),QHostAddress("127.0.0.1"),myport);
                qint64 temp4 = protoUdpSocket->writeDatagram(temparr,temparr.size(),QHostAddress("127.0.0.1"),myport);
                playback_tool->ui->playback_textEdit->append(QString("调度: %1 第%2条数据(%3)发送完毕.").arg(playback_tool->ui->playback_ip_lineEdit->curText()).arg(plagback_start_id).arg(query1.value(3).toDateTime().toString("yyyy-MM-dd hh:mm:ss")));
                slider_id = plagback_start_id;
                if(playback_tool->ui->playSpeed_comboBox->currentText().toInt() > 0)
                {
                    plagback_start_id = plagback_start_id + playback_tool->ui->playSpeed_comboBox->currentText().toInt();
                }
                else
                {
                    plagback_start_id++;
                }
                playback_tool->ui->playback_play_Button->setEnabled(true);
                playback_tool->ui->playback_Slider->setEnabled(true);
            }
            else
            {
                playback_tool->ui->playback_curtime_label->setText(current_sys_time.toString("yyyy-MM-dd hh:mm:ss"));
                playback_tool->ui->playback_textEdit->append(QString("调度: %1 (%2) 无数据.").arg(playback_tool->ui->playback_ip_lineEdit->curText()).arg(current_sys_time.toString("yyyy-MM-dd hh:mm:ss")));
            }

            QSqlQuery querytime(playback_db);
            QString dbtime = QString("select now();");

            querytime.exec(dbtime);
            if(querytime.next() == true)
            {
                playback_tool->ui->playback_datetime_lineEdit->setDateTime(querytime.value(0).toDateTime());
            }
            else
            {
            }
            return ;
        }
    }
    else
    {
        playback_tool->ui->playback_textEdit->append(QString("ping %1 失败!").arg(playback_tool->ui->playback_ip_lineEdit->curText()));
    }



}



void SubWidget::playback_play_Button_clicked()
{
    playback_db =QSqlDatabase::addDatabase("QMYSQL","playback_connection");
    playback_db.setHostName(playback_tool->ui->playback_ip_lineEdit->curText());
    playback_db.setUserName("manager");
    playback_db.setPassword("AgvManager");
    playback_db.setDatabaseName("agvtransfer_log");
    if(!playback_db.isOpen())
    {
        if(!playback_db.open())
        {
            QMessageBox::warning(this,"数据库打开失败",playback_db.lastError().text());
            return ;
        }
        else
        {
        }
    }
        disconnect(playback_tool,&PlayBack_Tool::playback_Slider_valueChanged,this,&SubWidget::playback_Slider_valueChanged);
        playback_tool->ui->playback_play_Button->setVisible(false);
        playback_tool->ui->playback_stop_Button->setVisible(true);
        playback_tool->ui->playback_Slider->setValue(0);
        playback_tool->ui->playback_Slider->setEnabled(false);

        QSqlQuery query2(playback_db);

        QString tempstr = QString("select id,monitordata,route_data,uploaddate from %1 where id=%2 limit 0,1;").arg(playback_tool->ui->playback_table_lineEdit->text()).arg(plagback_start_id);
        query2.exec(tempstr);

        while(query2.next())
        {
            playback_tcp_array = query2.value(2).toByteArray();

            playback_tool->ui->playback_curtime_label->setText("Time:"+query2.value(3).toDateTime().toString("yyyy-MM-dd hh:mm:ss"));

            QByteArray temparr(query2.value(1).toByteArray());
            qint64 temp = protoUdpSocket->writeDatagram(temparr,temparr.size(),QHostAddress("127.0.0.1"),myport);
            slider_id = plagback_start_id;
            if(playback_tool->ui->playSpeed_comboBox->currentText().toInt() > 0)
            {
                plagback_start_id = plagback_start_id + playback_tool->ui->playSpeed_comboBox->currentText().toInt();
            }
            else
            {
                plagback_start_id++;
            }

        }

        if(!playback_play_timer->isActive())
        {
            playback_play_timer->start(playback_tool->ui->playback_delay_lineEdit->curText().toInt());
        }


}


void SubWidget::playback_stop_Button_clicked()
{
    connect(playback_tool,&PlayBack_Tool::playback_Slider_valueChanged,this,&SubWidget::playback_Slider_valueChanged);
    playback_tool->ui->playback_play_Button->setVisible(true);
    playback_tool->ui->playback_stop_Button->setVisible(false);
    playback_tool->ui->playback_Slider->setEnabled(true);
    if(playback_play_timer->isActive())
    {
        playback_play_timer->stop();
    }
}


void SubWidget::playback_Slider_valueChanged(int value)
{
    QSqlQuery query3(playback_db);
    QString tempstr = QString("select id,monitordata,route_data,uploaddate from %1 where id=%2 limit 0,1;").arg(playback_tool->ui->playback_table_lineEdit->text()).arg(slider_id + value);
    query3.exec(tempstr);

    while(query3.next())
    {
        playback_tcp_array = query3.value(2).toByteArray();

        playback_tool->ui->playback_curtime_label->setText("Time:"+query3.value(3).toDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        QByteArray temparr(query3.value(1).toByteArray());
        qint64 temp = protoUdpSocket->writeDatagram(temparr,temparr.size(),QHostAddress("127.0.0.1"),myport);
        plagback_start_id = slider_id + value + 1;
    }
}


void SubWidget::playback_confim_Button_clicked()
{
    playback_db =QSqlDatabase::addDatabase("QMYSQL","playback_connection");
    playback_db.setHostName(playback_tool->ui->playback_ip_lineEdit->curText());
    playback_db.setUserName("manager");
    playback_db.setPassword("AgvManager");
    playback_db.setDatabaseName("agvtransfer_log");
    if(!playback_db.isOpen())
    {
        if(!playback_db.open())
        {
            QMessageBox::warning(this,"数据库打开失败",playback_db.lastError().text());
            return ;
        }
        else
        {
        }
    }


    QSqlQuery query1(playback_db);


    QString tempstr = QString("select id,monitordata,route_data,uploaddate from %1 where uploaddate=\"%2\" limit 0,1;").arg(playback_tool->ui->playback_table_lineEdit->text()).arg(playback_tool->ui->playback_datetime_lineEdit->dateTime().toString("yyyy-MM-dd hh:mm:ss"));
    query1.exec(tempstr);

    if(query1.next() == true)
    {

            playback_tcp_array = query1.value(2).toByteArray();
            plagback_start_id = query1.value(0).toLongLong();
            playback_tool->ui->playback_curtime_label->setText("Time:"+ query1.value(3).toDateTime().toString("yyyy-MM-dd hh:mm:ss"));
            QByteArray temparr(query1.value(1).toByteArray());
            qint64 temp = protoUdpSocket->writeDatagram(temparr,temparr.size(),QHostAddress("127.0.0.1"),myport);
            qint64 temp2 = protoUdpSocket->writeDatagram(temparr,temparr.size(),QHostAddress("127.0.0.1"),myport);
            qint64 temp3 = protoUdpSocket->writeDatagram(temparr,temparr.size(),QHostAddress("127.0.0.1"),myport);
            qint64 temp4 = protoUdpSocket->writeDatagram(temparr,temparr.size(),QHostAddress("127.0.0.1"),myport);
            playback_tool->ui->playback_textEdit->append(QString("调度: %1 第%2条数据(%3)发送完毕.").arg(playback_tool->ui->playback_ip_lineEdit->curText()).arg(plagback_start_id).arg(query1.value(3).toDateTime().toString("yyyy-MM-dd hh:mm:ss")));
            slider_id = plagback_start_id;
            if(playback_tool->ui->playSpeed_comboBox->currentText().toInt() > 0)
            {
                plagback_start_id = plagback_start_id + playback_tool->ui->playSpeed_comboBox->currentText().toInt();
            }
            else
            {
                plagback_start_id++;
            }
            playback_tool->ui->playback_play_Button->setEnabled(true);
            playback_tool->ui->playback_Slider->setEnabled(true);
//        }
    }
    else
    {
        playback_tool->ui->playback_curtime_label->setText("Time:"+ playback_tool->ui->playback_datetime_lineEdit->dateTime().toString("yyyy-MM-dd hh:mm:ss"));
        playback_tool->ui->playback_textEdit->append(QString("调度: %1 (%2) 无数据.").arg(playback_tool->ui->playback_ip_lineEdit->curText()).arg(playback_tool->ui->playback_datetime_lineEdit->dateTime().toString("yyyy-MM-dd hh:mm:ss")));
    }


}

void SubWidget::floatingwindow_show_slot(unsigned int dev_id, unsigned char dev_type, int mission_id, char mission_type, float pos_x, float pos_y, float pos_ang, char dir, float speed, char state, char status, int err_type, float battery, int loadnum, int cur_segment, int nxt_segment, unsigned int ip, long long timestamp, int online, int start_id, int end_id, float hyd_height, char protection1, char protection2, char protection3, char softstop, int idle_wait_ts, int avoid_set_ts, int manual_on_ts, int softstop_on_ts, int warning_on_ts, int stopStatus, int stopLevel, int stopByAGVId, int stopReason, int speedLevel, int speedByAGVId, QString errorStr, int confPoint, int mouse_x, int mouse_y)
{
    if(floatwindow_flag == 0)
    {
        return;
    }
    QString mission_type_temp;
    floatingwindow->carId = dev_id;
    floatingwindow->mission_id = mission_id;
    floatingwindow->ui->carId_label->setText(QString("车号: %1").arg(dev_id));
    if(mission_id <= 0)
        floatingwindow->ui->taskId_label->setText(QString("任务: 无"));
    else
        floatingwindow->ui->taskId_label->setText(QString("任务: %1").arg(mission_id));
    floatingwindow->ui->errorInfo_label->setText(QString("报警: %1").arg(errorStr));

    if(stopReason == 2)
    {
        floatingwindow->ui->confPoint_label->setText(QString("避让冲突: %1").arg(confPoint));
        floatingwindow->ui->confPoint_label->setVisible(true);
    }
    else
    {
        floatingwindow->ui->confPoint_label->setVisible(false);
    }
    if(speedByAGVId != -1)
    {
        floatingwindow->ui->speedLevel_label->setText(QString("限速: %1(AGV: %2)").arg(speedLevel).arg(speedByAGVId));
        floatingwindow->ui->speedLevel_label->setVisible(true);
    }
    else
    {
        floatingwindow->ui->speedLevel_label->setVisible(false);
    }


    if(QString::number(state) == "4")
    {
        floatingwindow->ui->time_label->setText(QString("手动: %1").arg(sec_to_time(manual_on_ts)));
    }
    else if(err_type > 0)
    {
        floatingwindow->ui->time_label->setText(QString("报警: %1").arg(sec_to_time(warning_on_ts)));
    }
    else if(QString::number(softstop) == "1")
    {
        floatingwindow->ui->time_label->setText(QString("软停: %1").arg(sec_to_time(softstop_on_ts)));
    }
    else  if(QString::number(status) == "3")
    {
        floatingwindow->ui->time_label->setText(QString("避让: %1").arg(sec_to_time(avoid_set_ts)));
    }
    else if(QString::number(status) == "1")
    {
        floatingwindow->ui->time_label->setText(QString("空闲: %1").arg(sec_to_time(idle_wait_ts)));
    }
    else
    {
        floatingwindow->ui->time_label->setText("-");
    }

    if(mission_type == 1)
    {
         mission_type_temp = QString("取货");
    }
    else if(mission_type == 2)
    {
         mission_type_temp = QString("放货");
    }
    else if(mission_type == 0)
    {
         mission_type_temp = QString("无货");
    }
    else if(mission_type == 3)
    {
        mission_type_temp = QString("充电");
    }
    else
    {
         mission_type_temp = QString("-"); //任务类型错误
    }

    if(status == 3)
    {
        if(stopStatus == 0)
        {
            floatingwindow->ui->stopStatus_label->setText(QString("避让设置: 不避让"));
        }
        else if(stopStatus == 1)
        {
            floatingwindow->ui->stopStatus_label->setText(QString("避让设置: 避让"));
        }
        else
        {
            floatingwindow->ui->stopStatus_label->setText(QString("避让不避让"));
        }

        if(stopLevel == 0)
        {
            floatingwindow->ui->stopLevel_label->setText(QString("避让级别: 普通避让"));
        }
        else if(stopLevel == 10)
        {
            floatingwindow->ui->stopLevel_label->setText(QString("避让级别: 紧急避让"));
        }
        else
        {
            floatingwindow->ui->stopLevel_label->setText(QString("避让级别: "));
        }
        floatingwindow->ui->stopByAGVId_label->setText(QString("避让AGV: %1 ").arg(stopByAGVId));
        switch (stopReason) {
        case 1:
            floatingwindow->ui->stopReason_label->setText(QString("避让原因: 数据异常(%1)").arg(stopReason));
            break;
        case 2:
            floatingwindow->ui->stopReason_label->setText(QString("避让原因: 点扫描被占用(%1)").arg(stopReason));
            break;
        case 3:
            floatingwindow->ui->stopReason_label->setText(QString("避让原因: 进站检测(%1)").arg(stopReason));
            break;
        case 4:
            floatingwindow->ui->stopReason_label->setText(QString("避让原因: 流控区域限流(%1)").arg(stopReason));
            break;
        case 5:
            floatingwindow->ui->stopReason_label->setText(QString("避让原因: 反向路径(%1)").arg(stopReason));
            break;
        case 6:
            floatingwindow->ui->stopReason_label->setText(QString("避让原因: 卷帘门关闭(%1)").arg(stopReason));
            break;
        case 7:
            floatingwindow->ui->stopReason_label->setText(QString("避让原因: 级别区限制(%1)").arg(stopReason));
            break;
        case 8:
            floatingwindow->ui->stopReason_label->setText(QString("避让原因: 前向框(%1)").arg(stopReason));
            break;
        case 9:
            floatingwindow->ui->stopReason_label->setText(QString("避让原因: 交叉不连接路径(%1)").arg(stopReason));
            break;
        case 10:
            floatingwindow->ui->stopReason_label->setText(QString("避让原因: 同向路径(%1)").arg(stopReason));
            break;
        case 11:
            floatingwindow->ui->stopReason_label->setText(QString("避让原因: 停止路径(%1)").arg(stopReason));
            break;
        case 12:
            floatingwindow->ui->stopReason_label->setText(QString("避让原因: 网络异常(%1)").arg(stopReason));
            break;
        case 21:
            floatingwindow->ui->stopReason_label->setText(QString("避让原因: 防碰撞避让(%1)").arg(stopReason));
            break;
        default:
            floatingwindow->ui->stopReason_label->setText(QString("避让原因: (%1)").arg(stopReason));
            break;
        }

        floatingwindow->ui->avoidFrame->setVisible(true);
    }
    else
    {
        floatingwindow->ui->avoidFrame->setVisible(false);
    }
    if(mission_type == -1)
    {
        floatingwindow->ui->goodsInfo_label->setText(QString("类型: 无"));
    }
    else
    {
        floatingwindow->ui->goodsInfo_label->setText(QString("类型: %1(终点:%2)").arg(mission_type_temp).arg(end_id));
    }

    floatItem->setPos(mouse_x+500,mouse_y);
    floatItem->show();
}

void SubWidget::changeFloatWindowCarId(unsigned int dev_id)
{
    floatWindowCarId = dev_id;
}

void SubWidget::floatingwindow_hide_slot()
{
    floatItem->hide();
}

void SubWidget::showRoute(int carId)
{
    for(int i = 0 ; i < carNum; i++)
    {
        if(mon_data.agvinfor(i).agvreport().basic().id() == carId)
        {
            int routeNum = mon_data.agvinfor(i).route_id_size();


            int route_list[routeNum];
            int cur_id;
            int nxt_id;
            //qDebug()<<routeNum<< "1";
            for(int j = 0 ; j < routeNum ; j++)
            {
                //qDebug()<<mon_data.agvinfor(i).route_id(j)<<"_"<<j<< "1";
                route_list[j] = mon_data.agvinfor(i).route_id(j);
            }

            //qDebug()<<routeNum<< "3";
            cur_id = mon_data.agvinfor(i).agvreport().agvtask().cur_seg();
            nxt_id = mon_data.agvinfor(i).agvreport().agvtask().nxt_seg();

            for(int j = 0 ; j < vec_LineItem.size(); j++)
            {
                if(vec_LineItem[j] != nullptr)
                {
                    vec_LineItem[j]->route_flag = 0;
                }
            }
            for(int j = 0 ; j < vec_ArcItem.size(); j++)
            {
                if(vec_ArcItem[j] != nullptr)
                {
                    vec_ArcItem[j]->route_flag = 0;
                }
            }

            for(int i = 0; i < routeNum; i++)
            {
                if(vec_LineItem[route_list[i]] != nullptr)
                {
                    if(route_list[i] == cur_id)
                    {
                        vec_LineItem[route_list[i]]->route_flag = 2;
                    }
                    else if(route_list[i] == nxt_id)
                    {
                        vec_LineItem[route_list[i]]->route_flag = 3;
                    }
                    else
                    {
                        vec_LineItem[route_list[i]]->route_flag = 1;
                    }
                }
                else if(vec_ArcItem[route_list[i]] != nullptr)
                {
                    if(route_list[i] == cur_id)
                    {
                        vec_ArcItem[route_list[i]]->route_flag = 2;
                    }
                    else if(route_list[i] == nxt_id)
                    {
                        vec_ArcItem[route_list[i]]->route_flag = 3;
                    }
                    else
                    {
                        vec_ArcItem[route_list[i]]->route_flag = 1;
                    }
                }
                else
                {
                }
            }
            ui->graphicsView->scene()->update();
            return;
        }
    }
}

void SubWidget::init_config()
{
    ParameterInit();
    if(colorName.isEmpty())
    {
        scene->scene_color = Qt::white;
        scene->setBackgroundBrush(Qt::white);
    }
    else
    {
        QColor s_color(colorName.toUInt(NULL,16));
        scene->scene_color = s_color;
        scene->setBackgroundBrush(s_color);
    }

    if(ip_text.isEmpty())
    {
        ui->tcp_lineEdit->setCurrentText("请选择IP地址");
        playback_tool->ui->playback_ip_lineEdit->setCurText("请选择IP地址");
        agvctrl_tool->ui->agvctrl_tab_ip_lineEdit->setCurText("请选择IP地址");
        agvctrl_tool->ui->agvctrl_tab_ip_lineEdit_2->setCurText("请选择IP地址");

    }
    else
    {
        ui->tcp_lineEdit->setCurrentText(ip_text);
        playback_tool->ui->playback_ip_lineEdit->setCurText(ip_text);
        agvctrl_tool->ui->agvctrl_tab_ip_lineEdit->setCurText(ip_text);
        agvctrl_tool->ui->agvctrl_tab_ip_lineEdit_2->setCurText(ip_text);

    }

    if(area_status.toInt() == 1)
    {
        avoid_Area_flag = 1;
        ui->graphicsView->area_flag = 1;
    }

    if(fw_status.isEmpty())
    {
        floatwindow_flag = 0;
    }
    else if(fw_status.toInt() == 1)
    {
        floatwindow_flag = 1;
    }
    else
    {
        floatwindow_flag = 0;
    }

    if(sa_bg.isEmpty())
    {
        saBackground_flag = 0;
    }
    else if(sa_bg.toInt() == 1)
    {
        saBackground_flag = 1;
        scene->addItem(sa_backgrounditem);
        sa_backgrounditem->setPos(9143,25066);
    }
    else
    {
        saBackground_flag = 0;
    }

    if(SDM.isEmpty())
    {
        speedDisplayMode = 0;
    }
    else
    {
        speedDisplayMode = SDM.toInt();
    }

    if(X_MIN.isEmpty())
    {

    }
    else
    {
        ui->graphicsView->mapConfig[ ui->graphicsView->cur_map_index].x_min = X_MIN.toInt();
        ui->graphicsView->readsizefromSQL = false;

    }
    if(X_MAX.isEmpty())
    {

    }
    else
    {
        ui->graphicsView->mapConfig[ ui->graphicsView->cur_map_index].x_max = X_MAX.toInt();
        ui->graphicsView->readsizefromSQL = false;

    }
    if(Y_MIN.isEmpty())
    {

    }
    else
    {
        ui->graphicsView->mapConfig[ ui->graphicsView->cur_map_index].y_min = Y_MIN.toInt();
        ui->graphicsView->readsizefromSQL = false;

    }
    if(Y_MAX.isEmpty())
    {

    }
    else
    {
        ui->graphicsView->mapConfig[ ui->graphicsView->cur_map_index].y_max = Y_MAX.toInt();

        ui->graphicsView->readsizefromSQL = false;

    }
    if(MAP_ID.isEmpty())
    {

    }
    else
    {
        ui->graphicsView->mapConfig[ ui->graphicsView->cur_map_index].mapId = MAP_ID.toInt();
        ui->graphicsView->readsizefromSQL = false;

    }
    if(START_ANGLE.isEmpty())
    {

    }
    else
    {
        ui->graphicsView->readsizefromSQL = false;

    }
    if(LOGISTICS_MAX.isEmpty())
    {

    }
    else
    {
        ui->graphicsView->readsizefromSQL = false;

    }

    if(Station_B_F.isEmpty())
    {
        station_background_flag = 0;
    }
    else
    {
        station_background_flag = Station_B_F.toInt();
    }

    scene->setSceneRect(
                ui->graphicsView->mapConfig[ ui->graphicsView->cur_map_index].x_min,
            ui->graphicsView->mapConfig[ ui->graphicsView->cur_map_index].y_min,
            ui->graphicsView->mapConfig[ ui->graphicsView->cur_map_index].x_max - ui->graphicsView->mapConfig[ ui->graphicsView->cur_map_index].x_min,
            ui->graphicsView->mapConfig[ ui->graphicsView->cur_map_index].y_max - ui->graphicsView->mapConfig[ ui->graphicsView->cur_map_index].y_min);

}

void SubWidget::init_tcplineEdit()
{
    QStringList list;
    list<<QString("192.168.66.234")<<QString("172.16.115.9")<<QString("172.16.115.6")<<QString("192.168.227.140")
       <<QString("172.16.251.234")<<QString("10.217.204.88")<<QString("192.168.1.220")<<QString("192.168.1.223")
      <<QString("192.168.1.234");
    ui->tcp_lineEdit->addItems(list);


}

void SubWidget::init_tableWidget()
{
    batteryInfo_widget = new BatteryInfo_Widget;
    connect(batteryInfo_widget,&BatteryInfo_Widget::getBatteryData,this,&SubWidget::getBatteryDataSlot);
    connect(batteryInfo_widget,&BatteryInfo_Widget::getBatteryDetailData,this,&SubWidget::getBatteryDetailDataSlot);

    batteryInfo_widget->ui->batteryTableWidget->setColumnCount(12);
    QStringList batteryInfo_widgetHeaders;
    batteryInfo_widgetHeaders<<"电池ID号"<<"电池RFID"<<"所在AGV ID"<<"所在STATION ID"<<"所在充电站仓位"<<"电量"
                            <<"平均使用时长(min)"<<"平均充电时长(min)"<<"平均闲置时长(min)"<<"平均加水间隔(D)"<<"上次加水时间"<<"启用时间";
    batteryInfo_widget->ui->batteryTableWidget->setHorizontalHeaderLabels(batteryInfo_widgetHeaders);
    for(int i = 0; i < batteryInfo_widgetHeaders.size(); i++)
    {
        batteryInfo_widget->ui->batteryTableWidget->horizontalHeader()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
    }
    batteryInfo_widget->ui->batteryTableWidget->horizontalHeader()->setSortIndicatorShown(true);
    batteryInfo_widget->ui->batteryTableWidget->horizontalHeader()->setSortIndicator(0,Qt::SortOrder::AscendingOrder);
    connect(batteryInfo_widget->ui->batteryTableWidget->horizontalHeader(),SIGNAL(sectionClicked(int )),this, SLOT(sortTableByColBattery(int)));

    batteryInfo_widget->ui->batteryDetailWidget->setColumnCount(7);
    QStringList batteryDetailInfo_widgetHeaders;
    batteryDetailInfo_widgetHeaders<<"电池标签"<<"电池RFID"<<"事件项"<<"相关AGV"<<"事件开始时间"<<"事件结束时间";
    batteryInfo_widget->ui->batteryDetailWidget->setHorizontalHeaderLabels(batteryDetailInfo_widgetHeaders);
    for(int i = 0; i < batteryDetailInfo_widgetHeaders.size(); i++)
    {
        batteryInfo_widget->ui->batteryDetailWidget->horizontalHeader()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
    }
    batteryInfo_widget->ui->batteryDetailWidget->horizontalHeader()->setSortIndicatorShown(true);
    batteryInfo_widget->ui->batteryDetailWidget->horizontalHeader()->setSortIndicator(0,Qt::SortOrder::AscendingOrder);
    connect(batteryInfo_widget->ui->batteryDetailWidget->horizontalHeader(),SIGNAL(sectionClicked(int )),this, SLOT(sortDetailByColBattery(int)));


    taskInfoDialog = new taskDialog;
    connect(taskInfoDialog,&taskDialog::getTaskData,this,&SubWidget::getTaskDataSlot);
    taskInfoDialog->ui->tasktWidget->setColumnCount(16);
    QStringList taskInfoDialogheaders;
    taskInfoDialogheaders  << "WMS" << "MES"<< "区域"<<"状态"<<"AGV ID"<<"起点"<<"起点位置"<<"终点"<<"终点位置"<<"托盘号"<<"MES创建时间"
                           <<"调度接收时间"<<"取货开始时间"<<"优先级"<<"预期的AGV"<< "任务延迟类型";
    taskInfoDialog->ui->tasktWidget->setHorizontalHeaderLabels(taskInfoDialogheaders);
    for(int i = 0; i < taskInfoDialogheaders.size(); i++)
    {
        taskInfoDialog->ui->tasktWidget->horizontalHeader()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
    }
    taskInfoDialog->ui->tasktWidget->horizontalHeader()->setSortIndicatorShown(true);
    taskInfoDialog->ui->tasktWidget->horizontalHeader()->setSortIndicator(0,Qt::SortOrder::AscendingOrder);
    connect(taskInfoDialog->ui->tasktWidget->horizontalHeader(),SIGNAL(sectionClicked(int )),this, SLOT(sortTableByColTask(int)));



    cxjDataInfoDialog = new cxjDataDialog;
    connect(cxjDataInfoDialog,&cxjDataDialog::getCxjData,this,&SubWidget::getCxjDataSlot);
    cxjDataInfoDialog->ui->cxjDatatWidget->setColumnCount(10);
    QStringList cxjDataInfoDialogheaders;
    cxjDataInfoDialogheaders  << "设备" << "类型"<< "库位ID"<<"库位状态"<<"移动平台"<<"光电数据"<<"AGV占用"<<"设备心跳"<<"AGV ID"<<"任务号";
    cxjDataInfoDialog->ui->cxjDatatWidget->setHorizontalHeaderLabels(cxjDataInfoDialogheaders);
    for(int i = 0; i < cxjDataInfoDialogheaders.size(); i++)
    {
        cxjDataInfoDialog->ui->cxjDatatWidget->horizontalHeader()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
    }


    cxjBackInfoDialog = new cxjBackDialog;
    connect(cxjBackInfoDialog,&cxjBackDialog::getCxjBack,this,&SubWidget::getCxjBackSlot);
    cxjBackInfoDialog->ui->cxjBacktWidget->setColumnCount(7);
    QStringList cxjBackInfoDialogheaders;
    cxjBackInfoDialogheaders  << "库位ID" << "站点区域"<< "站点类型"<<"任务号"<<"工装号"<<"设备IP"<<"时间";
    cxjBackInfoDialog->ui->cxjBacktWidget->setHorizontalHeaderLabels(cxjBackInfoDialogheaders);
    for(int i = 0; i < cxjBackInfoDialogheaders.size(); i++)
    {
        cxjBackInfoDialog->ui->cxjBacktWidget->horizontalHeader()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
    }


    taskInfoSpecialDialog = new taskSpecialDialog;
    taskInfoSpecialDialog->ui->taskSpecialTable->setColumnCount(11);
    QStringList taskInfoSpecialDialogheaders;
    taskInfoSpecialDialogheaders  << "任务ID" << "MES"<< "起始点"<<"终止点"<<"开始日期"<<"完成日期"<<"AGV ID"<<"取货ERROR ED"<<"取货ERROR ANGLE"<<"放货ERROR ED"<<"放货ERROR ANGLE";
    taskInfoSpecialDialog->ui->taskSpecialTable->setHorizontalHeaderLabels(taskInfoSpecialDialogheaders);
    for(int i = 0; i < taskInfoSpecialDialogheaders.size(); i++)
    {
        taskInfoSpecialDialog->ui->taskSpecialTable->horizontalHeader()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
    }
    taskInfoSpecialDialog->ui->taskSpecialTable->horizontalHeader()->setSortIndicatorShown(true);
    taskInfoSpecialDialog->ui->taskSpecialTable->horizontalHeader()->setSortIndicator(0,Qt::SortOrder::AscendingOrder);
    connect(taskInfoSpecialDialog->ui->taskSpecialTable->horizontalHeader(),SIGNAL(sectionClicked(int )),this, SLOT(sortTableByColSpecialTask(int)));
}

bool SubWidget::pingOk(QString sIp)
{
       if (sIp.isEmpty()) return true;

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

void SubWidget::recover_route_slot()
{
    routeCarId = -1;
    for(int j = 0 ; j < vec_LineItem.size(); j++)
    {
        if(vec_LineItem[j] != nullptr)
        {
            vec_LineItem[j]->route_flag = 0;
        }
    }
    for(int j = 0 ; j < vec_ArcItem.size(); j++)
    {
        if(vec_ArcItem[j] != nullptr)
        {
            vec_ArcItem[j]->route_flag = 0;
        }
    }
    foreach (GraphicsPointItem *item, vec_PointItem) {
        if(item != nullptr)
        {
            item->setZValue(0.1);
        }
    }

    ui->graphicsView->scene()->update();
}

void SubWidget::recover_area_slot()
{
    for(int j = 0 ; j < vec_AreaItem.size(); j++)
    {
        if(vec_AreaItem[j] != nullptr)
        {
            vec_AreaItem[j]->setSelected(false);
        }
    }
}

void SubWidget::control_SAItem(int flag)
{
    if(flag == 1)
    {
        scene->addItem(sa_backgrounditem);
        sa_backgrounditem->setPos(9143,25066);
    }
    else if(flag == 0)
    {
        scene->removeItem(sa_backgrounditem);
    }
}

void SubWidget::carSizeInfoSlot(QString carType, QString customCarL, QString customCarW)
{
    int dev_type_min;
    int dev_type_max;
    if(carType == QString("短腿叉车"))
    {
        dev_type_min = 1;
        dev_type_max = 20;
    }
    else if(carType == QString("长腿叉车"))
    {
        dev_type_min = 1;
        dev_type_max = 20;
    }
    else if(carType == QString("潜入式车"))
    {
        dev_type_min = 41;
        dev_type_max = 9999;
    }
    else if(carType == QString("差速车"))
    {
        dev_type_min = 21;
        dev_type_max = 40;
    }
    for(int i = 0; i < vec_Car.size(); i++)
    {
        if(vec_Car[i]->get_dev_type() >= dev_type_min && vec_Car[i]->get_dev_type() <= dev_type_max)
        {
            vec_Car[i]->customCarL = customCarL.toInt();
            vec_Car[i]->customCarW = customCarW.toInt();
        }
    }
    scene->update();
}





void SubWidget::createPropertyEditor()
{
    variantManager = new QtVariantPropertyManager();
    enumManager = new QtEnumPropertyManager();
    connect(variantManager, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
                this, SLOT(valueChanged(QtProperty *, const QVariant &)));
    connect(enumManager, SIGNAL(valueChanged(QtProperty *, int)),
                this, SLOT(valueChangedEnum(QtProperty *, int)));

    ui->propertyEditor->setFactoryForManager(variantManager,new QtVariantEditorFactory());
    ui->propertyEditor->setFactoryForManager(enumManager,new QtEnumEditorFactory());

    ui->propertyEditor->setResizeMode(ui->propertyEditor->Interactive);
    ui->propertyEditor->setIndentation(10);
    ui->propertyEditor->setSplitterPosition(160);

}

void SubWidget::save_ip_to_config(QString ip_text)
{
    ParameterSave("ip",ip_text);
}

void SubWidget::playback_delay_changed(QString delay_text)
{
    if(playback_play_timer->isActive())
    {
        playback_play_timer->stop();
        playback_play_timer->start(delay_text.toInt());
    }
}



void SubWidget::set_temparea(double temp)
{
    temparea = temp;
}

void SubWidget::deleteItem()
{
    QGraphicsScene * scene = ui->graphicsView->scene();
    ui->graphicsView->setModified(true);

    if (scene->selectedItems().isEmpty())
        return;

    QUndoCommand *deleteCommand = new RemoveShapeCommand(scene);
    undoStack->push(deleteCommand);
}


void SubWidget::itemSelected()
{

    QGraphicsScene * scene = ui->graphicsView->scene();

    foreach (QGraphicsItem *item , scene->items()) {
        if ( item->type() == GraphicsPointItem::Type)
        {
            GraphicsPointItem * my_point_item = dynamic_cast<GraphicsPointItem*>(item);
            my_point_item->isEndpoint = false;
        }
    }

    QList<QGraphicsItem *> items = scene->selectedItems();

    if(items.size() > 0)
    {
        for(int i = 0; i < items.size(); i++)
        {
            if(items[i]->type() == GraphicsLineItem::Type)
            {
                GraphicsLineItem *lineItem = dynamic_cast<GraphicsLineItem *>(items[i]);
                QList<QGraphicsItem *> itemList = scene->items(QPointF(lineItem->map_segment_pb.basic().start_x(),lineItem->map_segment_pb.basic().start_y()));
                for(int i = 0; i < itemList.size(); i++)
                {
                    if(itemList[i]->type() == GraphicsPointItem::Type)
                    {
                         GraphicsPointItem *pointItem = dynamic_cast<GraphicsPointItem *>(itemList[i]);
                         if(lineItem->map_segment_pb.basic().start_x() == pointItem->map_point_pb.basic().x() && lineItem->map_segment_pb.basic().start_y() == pointItem->map_point_pb.basic().y())
                         {
                             pointItem->isEndpoint = true;
                         }
                    }
                }

                itemList = scene->items(QPointF(lineItem->map_segment_pb.basic().end_x(),lineItem->map_segment_pb.basic().end_y()));
                for(int i = 0; i < itemList.size(); i++)
                {
                    if(itemList[i]->type() == GraphicsPointItem::Type)
                    {
                         GraphicsPointItem *pointItem = dynamic_cast<GraphicsPointItem *>(itemList[i]);
                         if(lineItem->map_segment_pb.basic().end_x() == pointItem->map_point_pb.basic().x() && lineItem->map_segment_pb.basic().end_y() == pointItem->map_point_pb.basic().y())
                         {
                             pointItem->isEndpoint = true;
                         }
                    }
                }
            }
            else if(items[i]->type() == GraphicsArcItem::Type)
            {
                GraphicsArcItem *arcItem = dynamic_cast<GraphicsArcItem *>(items[i]);
                QList<QGraphicsItem *> itemList = scene->items(QPointF(arcItem->map_segment_pb.basic().start_x(),arcItem->map_segment_pb.basic().start_y()));
                for(int i = 0; i < itemList.size(); i++)
                {
                    if(itemList[i]->type() == GraphicsPointItem::Type)
                    {
                         GraphicsPointItem *pointItem = dynamic_cast<GraphicsPointItem *>(itemList[i]);
                         if(arcItem->map_segment_pb.basic().start_x() == pointItem->map_point_pb.basic().x() && arcItem->map_segment_pb.basic().start_y() == pointItem->map_point_pb.basic().y())
                         {
                             pointItem->isEndpoint = true;
                         }
                    }
                }

                itemList = scene->items(QPointF(arcItem->map_segment_pb.basic().end_x(),arcItem->map_segment_pb.basic().end_y()));
                for(int i = 0; i < itemList.size(); i++)
                {
                    if(itemList[i]->type() == GraphicsPointItem::Type)
                    {
                         GraphicsPointItem *pointItem = dynamic_cast<GraphicsPointItem *>(itemList[i]);
                         if(arcItem->map_segment_pb.basic().end_x() == pointItem->map_point_pb.basic().x() && arcItem->map_segment_pb.basic().end_y() == pointItem->map_point_pb.basic().y())
                         {
                             pointItem->isEndpoint = true;
                         }
                    }
                }
            }
            else if(items[i]->type() == GraphicsAreaItem::Type)   // 选中避让区域后，出入点高亮
            {
                GraphicsAreaItem *areaItem = dynamic_cast<GraphicsAreaItem *>(items[i]);
                if(areaItem->map_area_pb.basic().type() == 0 || areaItem->map_area_pb.basic().type() == 10)
                {
                    for(int i = 1; i < 10; i++)
                    {
                        foreach (QGraphicsItem *item , scene->items())
                        {
                            if ( item->type() == GraphicsPointItem::Type)
                            {
                                GraphicsPointItem * my_point_item = dynamic_cast<GraphicsPointItem*>(item);
                                if(areaItem->map_area.gateway[i] == my_point_item->map_point_pb.basic().id() )
                                {
                                    my_point_item->isEndpoint = true;
                                }

                            }
                        }

                    }
                }
            }
        }
    }

    if( scene->selectedItems().count() == 1  && scene->selectedItems().first()->isSelected())
    {
        QGraphicsItem *item = scene->selectedItems().first();

        updateExpandState();
        QMap<QtProperty *, QString>::ConstIterator itProp = propertyToId.constBegin();
        while (itProp != propertyToId.constEnd()) {
            delete itProp.key();
            itProp++;
        }
        propertyToId.clear();
        idToProperty.clear();

        currentItem = item;


        QtProperty *groupBaseItem = variantManager->addProperty(QtVariantPropertyManager::groupTypeId(),QStringLiteral("基本属性"));
        QtProperty *groupAgvTypeItem = variantManager->addProperty(QtVariantPropertyManager::groupTypeId(),QStringLiteral("车类型"));
        QtProperty *groupAgvParmItem = variantManager->addProperty(QtVariantPropertyManager::groupTypeId(),QStringLiteral("AGV"));
        QtProperty *groupDispatchParmItem = variantManager->addProperty(QtVariantPropertyManager::groupTypeId(),QStringLiteral("AGVCTRL"));
        QtProperty *groupAvoidParmItem = variantManager->addProperty(QtVariantPropertyManager::groupTypeId(),QStringLiteral("AGVAVOID"));

        ui->propertyEditor->setSplitterPosition(198);
        if(item->type() == GraphicsPointItem::Type)
        {
            ui->propertyEditor->setSplitterPosition(160);
            GraphicsPointItem *pointItem = dynamic_cast<GraphicsPointItem *>(item);

            // 点的基本属性

            addVariantProperty(QVariant::Int, pointItem->map_point_pb.basic().id(), tr("站点Id"), QString("Id"), groupBaseItem);
            addVariantProperty(QVariant::String, QString::fromStdString(pointItem->map_point_pb.basic().name()).mid(0,5), tr("库位名称"), QString("Name"), groupBaseItem);
            addVariantProperty(QVariant::Int, pointItem->map_point_pb.basic().type(), tr("Type库位属性"), QString("Type"), groupBaseItem);
            addVariantProperty(QVariant::Int, pointItem->map_point_pb.basic().x(), tr("X坐标"), QString("X"), groupBaseItem);
            addVariantProperty(QVariant::Int, pointItem->map_point_pb.basic().y(), tr("Y坐标"), QString("Y"), groupBaseItem);
            addVariantProperty(QVariant::Int, pointItem->map_point_pb.basic().get_height(), tr("取货高度"), QString("getHeight"), groupBaseItem);
            addVariantProperty(QVariant::Int, pointItem->map_point_pb.basic().put_height(), tr("放货高度"), QString("putHeight"), groupBaseItem);
            addVariantProperty(QVariant::Int, pointItem->map_point_pb.basic().run_height(), tr("行走高度"), QString("runHeight"), groupBaseItem);
            addVariantProperty(QVariant::Int, pointItem->map_point_pb.basic().area_id(), tr("区域Id"), QString("areaId"), groupBaseItem);
            addVariantProperty(QVariant::String, QString::fromStdString(pointItem->map_point_pb.basic().area_name()), tr("区域名称"), QString("areaName"), groupBaseItem);
            addVariantProperty(QVariant::Int, pointItem->map_point_pb.basic().qrcode(), tr("二维码属性"), QString("QRcode"), groupBaseItem);
            addVariantProperty(QVariant::Int, pointItem->map_point_pb.basic().stype(), tr("Stype库位进出属性"), QString("Store"), groupBaseItem);
            addVariantProperty(QVariant::Int, pointItem->map_point_pb.basic().atype(), tr("Atype库位允许AGV属性"), QString("Agv"), groupBaseItem);
            addVariantProperty(QVariant::Bool, pointItem->map_point_pb.basic().disable(), tr("禁用站点"), QString("DisablePoint"), groupBaseItem);
            addVariantProperty(QVariant::Bool, pointItem->map_point_pb.basic().lock(), tr("锁定站点"), QString("LockPoint"), groupBaseItem);
            addVariantProperty(QVariant::Int, pointItem->map_point_pb.basic().floor(), tr("所属楼层"), QString("floor_point"), groupBaseItem);

            // Point Agv Param
            QMap<int, QString> map_describe;
            QMap<int, int>     map_value;
            for(auto it = pointItem->map_point_pb.agv_param().cbegin(); it != pointItem->map_point_pb.agv_param().cend(); it++)
            {
                if(!ui->graphicsView->mapArr_pb.describe().point_agv_desc().contains(it->first))
                {
                    QMessageBox::warning(this,"Warning", tr("    请先在参数配置中添加Point组Agv参数%1的描述！   ").arg(it->first));
                    continue;
                }
                QString desc = QString::fromStdString(ui->graphicsView->mapArr_pb.describe().point_agv_desc().at(it->first));
                //addVariantProperty(QVariant::Int, it->second, tr("%1.").arg(it->first) + desc, QString("point_agv_%1").arg(it->first), groupAgvParmItem);
                map_describe.insert(it->first, desc);
                map_value.insert(it->first, it->second);
            }
            for(int i = 0; i < map_value.size(); i++)
            {
                addVariantProperty(QVariant::Int, map_value[map_value.keys()[i]], tr("%1.").arg(map_value.keys()[i]) + map_describe[map_describe.keys()[i]], QString("point_agv_%1").arg(map_value.keys()[i]), groupAgvParmItem);
            }

            // Point Ctl Param
            map_describe.clear();
            map_value.clear();
            for(auto it = pointItem->map_point_pb.ctl_param().cbegin(); it != pointItem->map_point_pb.ctl_param().cend(); it++)
            {
                if(!ui->graphicsView->mapArr_pb.describe().point_ctl_desc().contains(it->first))
                {
                    QMessageBox::warning(this,"Warning", tr("    请先在参数配置中添加Point组Ctl参数%1的描述！   ").arg(it->first));
                    continue;
                }
                QString desc = QString::fromStdString(ui->graphicsView->mapArr_pb.describe().point_ctl_desc().at(it->first));
                map_describe.insert(it->first, desc);
                map_value.insert(it->first, it->second);
            }
            for(int i = 0; i < map_value.size(); i++)
            {
                addVariantProperty(QVariant::Int, map_value[map_value.keys()[i]], tr("%1.").arg(map_value.keys()[i]) + map_describe[map_describe.keys()[i]], QString("point_ctl_%1").arg(map_value.keys()[i]), groupDispatchParmItem);
            }

            // Point Avo Param
            map_describe.clear();
            map_value.clear();
            for(auto it = pointItem->map_point_pb.avo_param().cbegin(); it != pointItem->map_point_pb.avo_param().cend(); it++)
            {
                if(!ui->graphicsView->mapArr_pb.describe().point_avo_desc().contains(it->first))
                {
                    QMessageBox::warning(this,"Warning", tr("    请先在参数配置中添加Point组Avo参数%1的描述！   ").arg(it->first));
                    continue;
                }
                QString desc = QString::fromStdString(ui->graphicsView->mapArr_pb.describe().point_avo_desc().at(it->first));
                map_describe.insert(it->first, desc);
                map_value.insert(it->first, it->second);
            }
            for(int i = 0; i < map_value.size(); i++)
            {
                addVariantProperty(QVariant::Int, map_value[map_value.keys()[i]], tr("%1.").arg(map_value.keys()[i]) + map_describe[map_describe.keys()[i]], QString("point_avo_%1").arg(map_value.keys()[i]), groupAvoidParmItem);
            }

            ui->propertyEditor->addProperty(groupBaseItem);
            ui->propertyEditor->addProperty(groupAgvParmItem);
            ui->propertyEditor->addProperty(groupDispatchParmItem);
            ui->propertyEditor->addProperty(groupAvoidParmItem);

            // Point Sta Param
            QMap<int, agv_map::Point_Sparam> map_Sparam;
            for(auto it = pointItem->map_point_pb.sta_param().cbegin(); it != pointItem->map_point_pb.sta_param().cend(); it++)
            {
                map_Sparam.insert(it->first, it->second);
            }
            for(int i = 0; i < map_Sparam.size(); i++)
            {
                QtProperty *groupStationParmItem = variantManager->addProperty(QtVariantPropertyManager::groupTypeId(),QStringLiteral("STATION_%1").arg(map_Sparam.keys()[i]));
                addVariantProperty(QVariant::Int, map_Sparam[map_Sparam.keys()[i]].layers(), tr("层"), QString("sta_floor_%1").arg(map_Sparam.keys()[i]), groupStationParmItem);
                addVariantProperty(QVariant::Int, map_Sparam[map_Sparam.keys()[i]].horizon_offset(), tr("x"), QString("sta_x_%1").arg(map_Sparam.keys()[i]), groupStationParmItem);
                addVariantProperty(QVariant::Int, map_Sparam[map_Sparam.keys()[i]].vertical_offset(), tr("y"), QString("sta_y_%1").arg(map_Sparam.keys()[i]), groupStationParmItem);
                addVariantProperty(QVariant::Int, map_Sparam[map_Sparam.keys()[i]].get_height(), tr("取货高度"), QString("sta_getHeight_%1").arg(map_Sparam.keys()[i]), groupStationParmItem);
                addVariantProperty(QVariant::Int, map_Sparam[map_Sparam.keys()[i]].put_height(), tr("放货高度"), QString("sta_putHeight_%1").arg(map_Sparam.keys()[i]), groupStationParmItem);
                addVariantProperty(QVariant::Int, map_Sparam[map_Sparam.keys()[i]].run_height(), tr("行走高度"), QString("sta_runHeight_%1").arg(map_Sparam.keys()[i]), groupStationParmItem);
                ui->propertyEditor->addProperty(groupStationParmItem);
            }

        }
        else if(item->type() == GraphicsLineItem::Type)
        {
            ui->propertyEditor->setSplitterPosition(160);
            GraphicsLineItem *lineItem = dynamic_cast<GraphicsLineItem *>(item);

            addVariantProperty(QVariant::Int, lineItem->map_segment_pb.basic().id(), tr("片段Id"), QString("Id"), groupBaseItem);
            addVariantProperty(QVariant::Int, lineItem->map_segment_pb.basic().start_id(), tr("起点id"), QString("StartId"), groupBaseItem);
            addVariantProperty(QVariant::Int, lineItem->map_segment_pb.basic().start_x(), tr("起点X坐标"), QString("StartX"), groupBaseItem);
            addVariantProperty(QVariant::Int, lineItem->map_segment_pb.basic().start_y(), tr("起点Y坐标"), QString("StartY"), groupBaseItem);
            addVariantProperty(QVariant::Int, lineItem->map_segment_pb.basic().end_id(), tr("终点id"), QString("EndId"), groupBaseItem);
            addVariantProperty(QVariant::Int, lineItem->map_segment_pb.basic().end_x(), tr("终点X坐标"), QString("EndX"), groupBaseItem);
            addVariantProperty(QVariant::Int, lineItem->map_segment_pb.basic().end_y(), tr("终点Y坐标"), QString("EndY"), groupBaseItem);
            addVariantProperty(QVariant::Int, lineItem->map_segment_pb.basic().cen_x(), tr("弧中心X坐标"), QString("CenterX"), groupBaseItem);
            addVariantProperty(QVariant::Int, lineItem->map_segment_pb.basic().cen_y(), tr("弧中心Y坐标"), QString("CenterY"), groupBaseItem);
            addVariantProperty(QVariant::Int, lineItem->map_segment_pb.basic().radius(), tr("弧线半径"), QString("Radius"), groupBaseItem);
            addVariantProperty(QVariant::Int, lineItem->map_segment_pb.basic().seglen(), tr("片段长度"), QString("Length"), groupBaseItem);
            addVariantProperty(QVariant::Int, lineItem->map_segment_pb.basic().type(), tr("片段类型"), QString("Type"), groupBaseItem);
            addVariantProperty(QVariant::Int, lineItem->map_segment_pb.basic().dir_mode(), tr("片段方向"), QString("Direction"), groupBaseItem);
            addVariantProperty(QVariant::Int, lineItem->map_segment_pb.basic().floor(), tr("所属楼层"), QString("floor_segment"), groupBaseItem);

            float start_angle,end_angle;
            start_angle  = lineItem->map_segment_pb.basic().start_angle();
            end_angle  = lineItem->map_segment_pb.basic().end_angle();

            //坐标系角度 y轴正方向为0°时，转换正常坐标系角度±180
//            if((start_angle >= 0) && (start_angle < 90))
//            {
//                start_angle += 90;
//            }
//            else{
//                start_angle -= 270;
//            }
//            if((end_angle >= 0) && (end_angle < 90))
//            {
//                end_angle += 90;
//            }
//            else{
//                end_angle -= 270;
//            }
            addVariantProperty(QVariant::Double, start_angle, tr("起点角度"), QString("StartAngle"), groupBaseItem);
            addVariantProperty(QVariant::Double, end_angle, tr("终点角度"), QString("EndAngle"), groupBaseItem);

            addVariantProperty(QVariant::Bool, lineItem->map_segment_pb.basic().disable(), tr("禁用片段"), QString("DisableSeg"), groupBaseItem);
            addVariantProperty(QVariant::Bool, lineItem->map_segment_pb.basic().lock(), tr("锁定片段"), QString("LockSeg"), groupBaseItem);

            int64_t agv_type = lineItem->map_segment_pb.basic().agv_type();

            for(int i = 0; i < 64; i++)
            {
                addVariantProperty(QVariant::Bool, agv_type & ((int64_t)1 << i), tr("AGV类型(%1)").arg(i+1), QString("AgvType_%1").arg(i+1), groupAgvTypeItem);
            }

            // Segment Agv Param
            QMap<int, QString> map_describe;
            QMap<int, int>     map_value;
            for(auto it = lineItem->map_segment_pb.agv_param().cbegin(); it != lineItem->map_segment_pb.agv_param().cend(); it++)
            {
                if(!ui->graphicsView->mapArr_pb.describe().seg_agv_desc().contains(it->first))
                {
                    QMessageBox::warning(this,"Warning", tr("    请先在参数配置中添加Segment组Agv参数%1的描述！   ").arg(it->first));
                    continue;
                }
                QString desc = QString::fromStdString(ui->graphicsView->mapArr_pb.describe().seg_agv_desc().at(it->first));
                map_describe.insert(it->first, desc);
                map_value.insert(it->first, it->second);
            }
            for(int i = 0; i < map_value.size(); i++)
            {
                addVariantProperty(QVariant::Int, map_value[map_value.keys()[i]], tr("%1.").arg(map_value.keys()[i]) + map_describe[map_describe.keys()[i]], QString("seg_agv_%1").arg(map_value.keys()[i]), groupAgvParmItem);
            }

            // Segment Ctl Param
            map_describe.clear();
            map_value.clear();
            for(auto it = lineItem->map_segment_pb.ctl_param().cbegin(); it != lineItem->map_segment_pb.ctl_param().cend(); it++)
            {
                if(!ui->graphicsView->mapArr_pb.describe().seg_ctl_desc().contains(it->first))
                {
                    QMessageBox::warning(this,"Warning", tr("    请先在参数配置中添加Segment组Ctl参数%1的描述！   ").arg(it->first));
                    continue;
                }
                QString desc = QString::fromStdString(ui->graphicsView->mapArr_pb.describe().seg_ctl_desc().at(it->first));
                map_describe.insert(it->first, desc);
                map_value.insert(it->first, it->second);
            }
            for(int i = 0; i < map_value.size(); i++)
            {
                addVariantProperty(QVariant::Int, map_value[map_value.keys()[i]], tr("%1.").arg(map_value.keys()[i]) + map_describe[map_describe.keys()[i]], QString("seg_ctl_%1").arg(map_value.keys()[i]), groupDispatchParmItem);
            }

            // Segment Avo Param
            map_describe.clear();
            map_value.clear();
            for(auto it = lineItem->map_segment_pb.avo_param().cbegin(); it != lineItem->map_segment_pb.avo_param().cend(); it++)
            {
                if(!ui->graphicsView->mapArr_pb.describe().seg_avo_desc().contains(it->first))
                {
                    QMessageBox::warning(this,"Warning", tr("    请先在参数配置中添加Segment组Avo参数%1的描述！   ").arg(it->first));
                    continue;
                }
                QString desc = QString::fromStdString(ui->graphicsView->mapArr_pb.describe().seg_avo_desc().at(it->first));
                map_describe.insert(it->first, desc);
                map_value.insert(it->first, it->second);
            }
            for(int i = 0; i < map_value.size(); i++)
            {
                addVariantProperty(QVariant::Int, map_value[map_value.keys()[i]], tr("%1.").arg(map_value.keys()[i]) + map_describe[map_describe.keys()[i]], QString("seg_avo_%1").arg(map_value.keys()[i]), groupAvoidParmItem);
            }

            ui->propertyEditor->addProperty(groupBaseItem);
            ui->propertyEditor->addProperty(groupAgvTypeItem);
            ui->propertyEditor->addProperty(groupAgvParmItem);
            ui->propertyEditor->addProperty(groupDispatchParmItem);
            ui->propertyEditor->addProperty(groupAvoidParmItem);
            ui->propertyEditor->setExpanded(ui->propertyEditor->items(groupAgvTypeItem).at(0), false);
        }
        else if(item->type() == GraphicsArcItem::Type)
        {
            ui->propertyEditor->setSplitterPosition(160);
            GraphicsArcItem *arcItem = dynamic_cast<GraphicsArcItem *>(item);

            addVariantProperty(QVariant::Int, arcItem->map_segment_pb.basic().id(), tr("片段Id"), QString("Id"), groupBaseItem);
            addVariantProperty(QVariant::Int, arcItem->map_segment_pb.basic().start_id(), tr("起点id"), QString("StartId"), groupBaseItem);
            addVariantProperty(QVariant::Int, arcItem->map_segment_pb.basic().start_x(), tr("起点X坐标"), QString("StartX"), groupBaseItem);
            addVariantProperty(QVariant::Int, arcItem->map_segment_pb.basic().start_y(), tr("起点Y坐标"), QString("StartY"), groupBaseItem);
            addVariantProperty(QVariant::Int, arcItem->map_segment_pb.basic().end_id(), tr("终点id"), QString("EndId"), groupBaseItem);
            addVariantProperty(QVariant::Int, arcItem->map_segment_pb.basic().end_x(), tr("终点X坐标"), QString("EndX"), groupBaseItem);
            addVariantProperty(QVariant::Int, arcItem->map_segment_pb.basic().end_y(), tr("终点Y坐标"), QString("EndY"), groupBaseItem);
            addVariantProperty(QVariant::Int, arcItem->map_segment_pb.basic().cen_x(), tr("弧中心X坐标"), QString("CenterX"), groupBaseItem);
            addVariantProperty(QVariant::Int, arcItem->map_segment_pb.basic().cen_y(), tr("弧中心Y坐标"), QString("CenterY"), groupBaseItem);
            addVariantProperty(QVariant::Int, arcItem->map_segment_pb.basic().radius(), tr("弧线半径"), QString("Radius"), groupBaseItem);
            addVariantProperty(QVariant::Int, arcItem->map_segment_pb.basic().seglen(), tr("片段长度"), QString("Length"), groupBaseItem);
            addVariantProperty(QVariant::Int, arcItem->map_segment_pb.basic().type(), tr("片段类型"), QString("Type"), groupBaseItem);
            addVariantProperty(QVariant::Int, arcItem->map_segment_pb.basic().dir_mode(), tr("片段方向"), QString("Direction"), groupBaseItem);
            addVariantProperty(QVariant::Int, arcItem->map_segment_pb.basic().floor(), tr("所属楼层"), QString("floor_segment"), groupBaseItem);

            float start_angle,end_angle;
            start_angle  = arcItem->map_segment_pb.basic().start_angle();
            end_angle  = arcItem->map_segment_pb.basic().end_angle();

            //坐标系角度 y轴正方向为0°时，转换正常坐标系角度±180
//            if((start_angle >= 0) && (start_angle < 90))
//            {
//                start_angle += 90;
//            }
//            else{
//                start_angle -= 270;
//            }
//            if((end_angle >= 0) && (end_angle < 90))
//            {
//                end_angle += 90;
//            }
//            else{
//                end_angle -= 270;
//            }
            addVariantProperty(QVariant::Double, start_angle, tr("起点角度"), QString("StartAngle"), groupBaseItem);
            addVariantProperty(QVariant::Double, end_angle, tr("终点角度"), QString("EndAngle"), groupBaseItem);

            addVariantProperty(QVariant::Bool, arcItem->map_segment_pb.basic().disable(), tr("禁用片段"), QString("DisableSeg"), groupBaseItem);
            addVariantProperty(QVariant::Bool, arcItem->map_segment_pb.basic().lock(), tr("锁定片段"), QString("LockSeg"), groupBaseItem);

            int64_t agv_type = arcItem->map_segment_pb.basic().agv_type();

            for(int i = 0; i < 64; i++)
            {
                addVariantProperty(QVariant::Bool, agv_type & ((int64_t)1 << i), tr("AGV类型(%1)").arg(i+1), QString("AgvType_%1").arg(i+1), groupAgvTypeItem);
            }

            // Segment Agv Param
            QMap<int, QString> map_describe;
            QMap<int, int>     map_value;
            for(auto it = arcItem->map_segment_pb.agv_param().cbegin(); it != arcItem->map_segment_pb.agv_param().cend(); it++)
            {
                if(!ui->graphicsView->mapArr_pb.describe().seg_agv_desc().contains(it->first))
                {
                    QMessageBox::warning(this,"Warning", tr("    请先在参数配置中添加Segment组Agv参数%1的描述！   ").arg(it->first));
                    continue;
                }
                QString desc = QString::fromStdString(ui->graphicsView->mapArr_pb.describe().seg_agv_desc().at(it->first));
                map_describe.insert(it->first, desc);
                map_value.insert(it->first, it->second);
            }
            for(int i = 0; i < map_value.size(); i++)
            {
                addVariantProperty(QVariant::Int, map_value[map_value.keys()[i]], tr("%1.").arg(map_value.keys()[i]) + map_describe[map_describe.keys()[i]], QString("seg_agv_%1").arg(map_value.keys()[i]), groupAgvParmItem);
            }

            // Segment Ctl Param
            map_describe.clear();
            map_value.clear();
            for(auto it = arcItem->map_segment_pb.ctl_param().cbegin(); it != arcItem->map_segment_pb.ctl_param().cend(); it++)
            {
                if(!ui->graphicsView->mapArr_pb.describe().seg_ctl_desc().contains(it->first))
                {
                    QMessageBox::warning(this,"Warning", tr("    请先在参数配置中添加Segment组Ctl参数%1的描述！   ").arg(it->first));
                    continue;
                }
                QString desc = QString::fromStdString(ui->graphicsView->mapArr_pb.describe().seg_ctl_desc().at(it->first));
                map_describe.insert(it->first, desc);
                map_value.insert(it->first, it->second);
            }
            for(int i = 0; i < map_value.size(); i++)
            {
                addVariantProperty(QVariant::Int, map_value[map_value.keys()[i]], tr("%1.").arg(map_value.keys()[i]) + map_describe[map_describe.keys()[i]], QString("seg_ctl_%1").arg(map_value.keys()[i]), groupDispatchParmItem);
            }

            // Segment Avo Param
            map_describe.clear();
            map_value.clear();
            for(auto it = arcItem->map_segment_pb.avo_param().cbegin(); it != arcItem->map_segment_pb.avo_param().cend(); it++)
            {
                if(!ui->graphicsView->mapArr_pb.describe().seg_avo_desc().contains(it->first))
                {
                    QMessageBox::warning(this,"Warning", tr("    请先在参数配置中添加Segment组Avo参数%1的描述！   ").arg(it->first));
                    continue;
                }
                QString desc = QString::fromStdString(ui->graphicsView->mapArr_pb.describe().seg_avo_desc().at(it->first));
                map_describe.insert(it->first, desc);
                map_value.insert(it->first, it->second);
            }
            for(int i = 0; i < map_value.size(); i++)
            {
                addVariantProperty(QVariant::Int, map_value[map_value.keys()[i]], tr("%1.").arg(map_value.keys()[i]) + map_describe[map_describe.keys()[i]], QString("seg_avo_%1").arg(map_value.keys()[i]), groupAvoidParmItem);
            }


            ui->propertyEditor->addProperty(groupBaseItem);
            ui->propertyEditor->addProperty(groupAgvTypeItem);
            ui->propertyEditor->addProperty(groupAgvParmItem);
            ui->propertyEditor->addProperty(groupDispatchParmItem);
            ui->propertyEditor->addProperty(groupAvoidParmItem);
            ui->propertyEditor->setExpanded(ui->propertyEditor->items(groupAgvTypeItem).at(0), false);
        }
        else if(item->type() == GraphicsAreaItem::Type)
        {
            ui->propertyEditor->setSplitterPosition(160);
            GraphicsAreaItem *areaItem = dynamic_cast<GraphicsAreaItem *>(item);
            ui->propertyEditor->setSplitterPosition(95);
            addVariantProperty(QVariant::Int, areaItem->map_area_pb.basic().id(), tr("区域Id"), QString("Id"), groupBaseItem);
            QStringList valueList;

            QMap<int, QString> map_typeDesc;
            areaKeyList.clear();
            for(auto it = ui->graphicsView->mapArr_pb.describe().area_desc().cbegin(); it != ui->graphicsView->mapArr_pb.describe().area_desc().cend(); it++)
            {
                QString desc = QString::fromStdString(ui->graphicsView->mapArr_pb.describe().area_desc().at(it->first).type());
                map_typeDesc.insert(it->first, desc);
            }
            for(int i = 0; i < map_typeDesc.size(); i++)
            {
                valueList.append(QString("%1.").arg(map_typeDesc.keys()[i]) + map_typeDesc[map_typeDesc.keys()[i]]);
                areaKeyList.append(map_typeDesc.keys()[i]);
            }

            addEnumProperty(valueList, areaKeyList.indexOf(areaItem->map_area_pb.basic().type()), tr("区域类型"), QString("Type"), groupBaseItem);

            QMap<int, QString> map_describe;
            QMap<int, int>     map_value;
            for(auto it = areaItem->map_area_pb.param().cbegin(); it != areaItem->map_area_pb.param().cend(); it++)
            {
                if(!ui->graphicsView->mapArr_pb.describe().area_desc().contains(areaItem->map_area_pb.basic().type()))
                {
                    QMessageBox::warning(this,"Warning", tr("    请先在参数配置中添加Area组类型为%1的描述！   ").arg(areaItem->map_area_pb.basic().type()));
                    continue;
                }
                if(!ui->graphicsView->mapArr_pb.describe().area_desc().at(areaItem->map_area_pb.basic().type()).desc().contains(it->first))
                {
                    QMessageBox::warning(this,"Warning", tr("    请先在参数配置中添加Area组类型为%1, 参数%2的描述！   ").arg(areaItem->map_area_pb.basic().type()).arg(it->first));
                    continue;
                }
                QString desc = QString::fromStdString(ui->graphicsView->mapArr_pb.describe().area_desc().at(areaItem->map_area_pb.basic().type()).desc().at(it->first));
                map_describe.insert(it->first, desc);
                map_value.insert(it->first, it->second);
            }
            for(int i = 0; i < map_value.size(); i++)
            {
                addVariantProperty(QVariant::Int, map_value[map_value.keys()[i]], tr("%1.").arg(map_value.keys()[i]) + map_describe[map_describe.keys()[i]], QString("area_param_%1").arg(map_value.keys()[i]), groupBaseItem);
            }

            ui->propertyEditor->addProperty(groupBaseItem);
        }
    }
}



void SubWidget::itemMoved(QGraphicsItem *item, const QPointF &oldPosition)
{
    Q_UNUSED(item);
    ui->graphicsView->setModified(true);

    if ( item ){
        QUndoCommand *moveCommand = new MoveShapeCommand(item, oldPosition);
        undoStack->push(moveCommand);
    }else{
        QUndoCommand *moveCommand = new MoveShapeCommand(ui->graphicsView->scene(), oldPosition);
        undoStack->push(moveCommand);
    }

    if (item != currentItem)
        return;

    if(item->type() == GraphicsPointItem::Type)
    {
        variantManager->setValue(idToProperty[QLatin1String("xpos")], item->x());
        variantManager->setValue(idToProperty[QLatin1String("ypos")], item->y());
    }
}

void SubWidget::itemAdded(QList<QGraphicsItem *> myItems)
{
    ui->graphicsView->setModified(true);
    QUndoCommand *addCommand = new AddShapeCommand(myItems, ui->graphicsView->scene());
    undoStack->push(addCommand);
}

void SubWidget::deleteAdded(QList<QGraphicsItem *> items)
{
    QGraphicsScene * scene = ui->graphicsView->scene();
    ui->graphicsView->setModified(true);

    if (scene->selectedItems().isEmpty())
    {
        //return;
    }

    QUndoCommand *deleteCommand = new RemoveShapeCommand(scene,items);
    undoStack->push(deleteCommand);
}

void SubWidget::itemRotate(QGraphicsItem *item, const qreal oldAngle, QPoint center)
{
    ui->graphicsView->setModified(true);
    QUndoCommand *rotateCommand = new RotateShapeCommand(ui->graphicsView->scene() , oldAngle, center);
    undoStack->push(rotateCommand);
}

void SubWidget::itemResize(QGraphicsItem *item, int handle, const QPointF &scale)
{
    ui->graphicsView->setModified(true);
    QUndoCommand *resizeCommand = new ResizeShapeCommand(item ,handle, scale );
    undoStack->push(resizeCommand);
}

void SubWidget::itemControl(QGraphicsItem *item, int handle, const QPointF &newPos, const QPointF &lastPos_)
{
    ui->graphicsView->setModified(true);
    QUndoCommand *controlCommand = new ControlShapeCommand(item ,handle, newPos, lastPos_ );
    undoStack->push(controlCommand);
}

void SubWidget::valueChanged(QtProperty *property, const QVariant &value)
{
    QGraphicsScene * scene = ui->graphicsView->scene();

    if (!propertyToId.contains(property))
        return;
    if (!currentItem)
        return;

    QString id = propertyToId[property];

    if(currentItem->type() == GraphicsPointItem::Type)
    {
        GraphicsPointItem *cur_point_item = dynamic_cast<GraphicsPointItem *>(currentItem);
        if(id != QLatin1String("LockPoint") && cur_point_item->map_point.lock == 1)
        {
            scene->clearSelection();
            currentItem->setSelected(true);
            QMessageBox::warning(this, tr("警告"), tr("    属性已被锁定，该修改无效！   "));
            return;
        }

        if(id == QLatin1String("Name"))
        {
            memcpy(cur_point_item->map_point.name, value.toString().toLatin1().data(), 5);
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("X")) {
            cur_point_item->setX(value.toDouble());
            cur_point_item->map_point.x = value.toDouble();
            ui->graphicsView->setModified(true);
        } else if (id == QLatin1String("Y")) {
            cur_point_item->setY(value.toDouble());
            cur_point_item->map_point.y = value.toDouble();
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("QRcode")) {
            cur_point_item->map_point.QRcode = value.toInt();
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("Type")) {
            cur_point_item->map_point.type = value.toInt();
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("Store")) {
            cur_point_item->map_point.stype = value.toInt();
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("Agv1")) {
            cur_point_item->map_point.atype &= ~(1 << 0);
            cur_point_item->map_point.atype |= (value.toBool() << 0);
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("Agv2")) {
            cur_point_item->map_point.atype &= ~(1 << 1);
            cur_point_item->map_point.atype |= (value.toBool() << 1);
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("Agv3")) {
            cur_point_item->map_point.atype &= ~(1 << 2);
            cur_point_item->map_point.atype |= (value.toBool() << 2);
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("Agv4")) {
            cur_point_item->map_point.atype &= ~(1 << 3);
            cur_point_item->map_point.atype |= (value.toBool() << 3);
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("Agv5")) {
            cur_point_item->map_point.atype &= ~(1 << 4);
            cur_point_item->map_point.atype |= (value.toBool() << 4);
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("Agv6")) {
            cur_point_item->map_point.atype &= ~(1 << 5);
            cur_point_item->map_point.atype |= (value.toBool() << 5);
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("Agv7")) {
            cur_point_item->map_point.atype &= ~(1 << 6);
            cur_point_item->map_point.atype |= (value.toBool() << 6);
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("Agv8")) {
            cur_point_item->map_point.atype &= ~(1 << 7);
            cur_point_item->map_point.atype |= (value.toBool() << 7);
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("Dist")) {
            cur_point_item->map_point.dist = value.toInt();
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("ChargeDir")) {
            cur_point_item->map_point.chargeDir = value.toInt();
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("Infrared")) {
            cur_point_item->map_point.Infrared = value.toFloat();
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("Angle")) {
            cur_point_item->map_point.angle = value.toInt();
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("DisablePoint")) {
            cur_point_item->map_point.disable = value.toInt();
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("LockPoint")) {
            cur_point_item->map_point.lock = value.toInt();
            ui->graphicsView->setModified(true);
        }
        else{
            for(unsigned i = 0; i < sizeof(cur_point_item->map_point.parm)/sizeof(cur_point_item->map_point.parm[0]); i++)
            {
                if(id == QString("Parm%1").arg(i))
                {
                    cur_point_item->map_point.parm[i] = value.toDouble();
                    ui->graphicsView->setModified(true);
                }
            }
        }
//        GraphicsPointItem *cur_point_item = dynamic_cast<GraphicsPointItem *>(currentItem);
//        if(id == QLatin1String("Name"))
//        {
//            memcpy(cur_point_item->map_point.name, value.toString().toLatin1().data(), 5);
//        }
//        else if (id == QLatin1String("X")) {
//            cur_point_item->setX(value.toDouble());
//            cur_point_item->map_point.x = value.toDouble();
//        } else if (id == QLatin1String("Y")) {
//            cur_point_item->setY(value.toDouble());
//            cur_point_item->map_point.y = value.toDouble();
//        }
//        else if (id == QLatin1String("QRcode")) {
//            cur_point_item->map_point.QRcode = value.toInt();
//        }
//        else if (id == QLatin1String("Type")) {
//            cur_point_item->map_point.type = value.toInt();
//        }
//        else if (id == QLatin1String("Store")) {
//            cur_point_item->map_point.stype = value.toInt();
//        }
//        else if (id == QLatin1String("Agv")) {
//            cur_point_item->map_point.atype = value.toInt();
//        }
//        else if (id == QLatin1String("Dist")) {
//            cur_point_item->map_point.dist = value.toInt();
//        }
//        else if (id == QLatin1String("ChargeDir")) {
//            cur_point_item->map_point.chargeDir = value.toInt();
//        }
//        else if (id == QLatin1String("Infrared")) {
//            cur_point_item->map_point.Infrared = value.toFloat();
//        }
//        else if (id == QLatin1String("Angle")) {
//            cur_point_item->map_point.angle = value.toInt();
//        }
//        else{
//            for(unsigned i = 0; i < sizeof(cur_point_item->map_point.parm)/sizeof(cur_point_item->map_point.parm[0]); i++)
//            {
//                if(id == QString("Parm%1").arg(i))
//                {
//                    cur_point_item->map_point.parm[i] = value.toDouble();
//                }
//            }
//        }

    }
    else if(currentItem->type() == GraphicsLineItem::Type)
    {
        GraphicsLineItem *cur_line_item = dynamic_cast<GraphicsLineItem *>(currentItem);        
        if(id != QLatin1String("LockSeg") && cur_line_item->map_segment.lock == 1)
        {
            scene->clearSelection();
            currentItem->setSelected(true);
            QMessageBox::warning(this, tr("警告"), tr("    属性已被锁定，该修改无效！   "));
            return;
        }
        if (id == QLatin1String("StartX")) {
            cur_line_item->map_segment.start_x = value.toDouble();
            QPointF temp(value.toDouble(),cur_line_item->map_segment.start_y);
            cur_line_item->m_points.begin()[0] = cur_line_item->mapFromScene(temp);
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("StartY")) {
            cur_line_item->map_segment.start_y = value.toDouble();
            QPointF temp(cur_line_item->map_segment.start_x,value.toDouble());
            cur_line_item->m_points.begin()[0] = cur_line_item->mapFromScene(temp);
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("EndX")) {
            cur_line_item->map_segment.end_x = value.toDouble();
            QPointF temp(value.toDouble(),cur_line_item->map_segment.end_y);
            cur_line_item->m_points.begin()[1] = cur_line_item->mapFromScene(temp);
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("EndY")) {
            cur_line_item->map_segment.end_y = value.toDouble();
            QPointF temp(cur_line_item->map_segment.end_x,value.toDouble());
            cur_line_item->m_points.begin()[1] = cur_line_item->mapFromScene(temp);
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("Radius")) {
            cur_line_item->map_segment.radius = value.toFloat();
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("Direction")) {
            cur_line_item->map_segment.dirmode = value.toInt();
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("TraceMode")) {
            cur_line_item->map_segment.Tmode = value.toInt();
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("AgvType1")) {
            cur_line_item->map_segment.AgvType &= ~(1 << 0);
            cur_line_item->map_segment.AgvType |= (value.toBool() << 0);
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("AgvType2")) {
            cur_line_item->map_segment.AgvType &= ~(1 << 1);
            cur_line_item->map_segment.AgvType |= (value.toBool() << 1);
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("AgvType3")) {
            cur_line_item->map_segment.AgvType &= ~(1 << 2);
            cur_line_item->map_segment.AgvType |= (value.toBool() << 2);
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("AgvType4")) {
            cur_line_item->map_segment.AgvType &= ~(1 << 3);
            cur_line_item->map_segment.AgvType |= (value.toBool() << 3);
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("AgvType5")) {
            cur_line_item->map_segment.AgvType &= ~(1 << 4);
            cur_line_item->map_segment.AgvType |= (value.toBool() << 4);
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("AgvType6")) {
            cur_line_item->map_segment.AgvType &= ~(1 << 5);
            cur_line_item->map_segment.AgvType |= (value.toBool() << 5);
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("AgvType7")) {
            cur_line_item->map_segment.AgvType &= ~(1 << 6);
            cur_line_item->map_segment.AgvType |= (value.toBool() << 6);
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("AgvType8")) {
            cur_line_item->map_segment.AgvType &= ~(1 << 7);
            cur_line_item->map_segment.AgvType |= (value.toBool() << 7);
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("AllowPoint")) {
            cur_line_item->map_segment.allowId = value.toInt();
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("BlockPoint")) {
            cur_line_item->map_segment.blockId = value.toInt();
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("ScanSwitch")) {
            cur_line_item->map_segment.detection = value.toInt();
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("SpeedLimit")) {
            cur_line_item->map_segment.speed_limit = value.toInt();
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("DisableSeg")) {
            cur_line_item->map_segment.disable = value.toInt();
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("LockSeg")) {
            cur_line_item->map_segment.lock = value.toInt();
            ui->graphicsView->setModified(true);
        }
        else{
            for(unsigned i = 0; i < sizeof(cur_line_item->map_segment.reserved)/sizeof(cur_line_item->map_segment.reserved[0]); i++)
            {
                if(id == QString("Reserved%1").arg(i))
                {
                    cur_line_item->map_segment.reserved[i] = value.toDouble();
                    ui->graphicsView->setModified(true);
                }
            }
        }
//        if (id == QLatin1String("StartX")) {
//            cur_line_item->map_segment.start_x = value.toDouble();
//            QPointF temp(value.toDouble(),cur_line_item->map_segment.start_y);
//            cur_line_item->m_points.begin()[0] = cur_line_item->mapFromScene(temp);
//        }
//        else if (id == QLatin1String("StartY")) {
//            cur_line_item->map_segment.start_y = value.toDouble();
//            QPointF temp(cur_line_item->map_segment.start_x,value.toDouble());
//            cur_line_item->m_points.begin()[0] = cur_line_item->mapFromScene(temp);
//        }
//        else if (id == QLatin1String("EndX")) {
//            cur_line_item->map_segment.end_x = value.toDouble();
//            QPointF temp(value.toDouble(),cur_line_item->map_segment.end_y);
//            cur_line_item->m_points.begin()[1] = cur_line_item->mapFromScene(temp);
//        }
//        else if (id == QLatin1String("EndY")) {
//            cur_line_item->map_segment.end_y = value.toDouble();
//            QPointF temp(cur_line_item->map_segment.end_x,value.toDouble());
//            cur_line_item->m_points.begin()[1] = cur_line_item->mapFromScene(temp);
//        }
//        else if (id == QLatin1String("Radius")) {
//            cur_line_item->map_segment.radius = value.toFloat();
//        }
//        else if (id == QLatin1String("Direction")) {
//            cur_line_item->map_segment.dirmode = value.toInt();
//        }
//        else if (id == QLatin1String("TraceMode")) {
//            cur_line_item->map_segment.Tmode = value.toInt();
//        }
//        else if (id == QLatin1String("LN-NF001")) {
//            cur_line_item->map_segment.AgvType1 = value.toInt();
//        }
//        else if (id == QLatin1String("LN-NF002")) {
//            cur_line_item->map_segment.AgvType2 = value.toInt();
//        }
//        else if (id == QLatin1String("LN-DJ041")) {
//            cur_line_item->map_segment.AgvType3 = value.toInt();
//        }
//        else if (id == QLatin1String("AllowPoint")) {
//            cur_line_item->map_segment.allowId = value.toInt();
//        }
//        else if (id == QLatin1String("BlockPoint")) {
//            cur_line_item->map_segment.blockId = value.toInt();
//        }
//        else if (id == QLatin1String("ScanSwitch")) {
//            cur_line_item->map_segment.detection = value.toInt();
//        }
//        else if (id == QLatin1String("SpeedLimit")) {
//            cur_line_item->map_segment.speed_limit = value.toInt();
//        }
//        else{
//            for(unsigned i = 0; i < sizeof(cur_line_item->map_segment.reserved)/sizeof(cur_line_item->map_segment.reserved[0]); i++)
//            {
//                if(id == QString("Reserved%1").arg(i))
//                {
//                    cur_line_item->map_segment.reserved[i] = value.toDouble();
//                }
//            }
//        }


        //cur_line_item->update();
    }
    else if(currentItem->type() == GraphicsArcItem::Type)
    {
        GraphicsArcItem *cur_arc_item = dynamic_cast<GraphicsArcItem *>(currentItem);
        if(id != QLatin1String("LockSeg") && cur_arc_item->map_segment.lock == 1)
        {
            scene->clearSelection();
            currentItem->setSelected(true);
            QMessageBox::warning(this, tr("警告"), tr("    属性已被锁定，该修改无效！   "));
            return;
        }
        if (id == QLatin1String("StartX")) {
            cur_arc_item->map_segment.start_x = value.toDouble();
            QPointF temp(value.toDouble(),cur_arc_item->map_segment.start_y);
            cur_arc_item->m_points.begin()[0] = cur_arc_item->mapFromScene(temp);
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("StartY")) {
            cur_arc_item->map_segment.start_y = value.toDouble();
            QPointF temp(cur_arc_item->map_segment.start_x,value.toDouble());
            cur_arc_item->m_points.begin()[0] = cur_arc_item->mapFromScene(temp);
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("EndX")) {
            cur_arc_item->map_segment.end_x = value.toDouble();
            QPointF temp(value.toDouble(),cur_arc_item->map_segment.end_y);
            cur_arc_item->m_points.begin()[1] = cur_arc_item->mapFromScene(temp);
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("EndY")) {
            cur_arc_item->map_segment.end_y = value.toDouble();
            QPointF temp(cur_arc_item->map_segment.end_x,value.toDouble());
            cur_arc_item->m_points.begin()[1] = cur_arc_item->mapFromScene(temp);
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("Radius")) {
            cur_arc_item->map_segment.radius = value.toFloat();
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("Direction")) {
            cur_arc_item->map_segment.dirmode = value.toInt();
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("TraceMode")) {
            cur_arc_item->map_segment.Tmode = value.toInt();
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("AgvType1")) {
            cur_arc_item->map_segment.AgvType &= ~(1 << 0);
            cur_arc_item->map_segment.AgvType |= (value.toBool() << 0);
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("AgvType2")) {
            cur_arc_item->map_segment.AgvType &= ~(1 << 1);
            cur_arc_item->map_segment.AgvType |= (value.toBool() << 1);
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("AgvType3")) {
            cur_arc_item->map_segment.AgvType &= ~(1 << 2);
            cur_arc_item->map_segment.AgvType |= (value.toBool() << 2);
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("AgvType4")) {
            cur_arc_item->map_segment.AgvType &= ~(1 << 3);
            cur_arc_item->map_segment.AgvType |= (value.toBool() << 3);
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("AgvType5")) {
            cur_arc_item->map_segment.AgvType &= ~(1 << 4);
            cur_arc_item->map_segment.AgvType |= (value.toBool() << 4);
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("AgvType6")) {
            cur_arc_item->map_segment.AgvType &= ~(1 << 5);
            cur_arc_item->map_segment.AgvType |= (value.toBool() << 5);
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("AgvType7")) {
            cur_arc_item->map_segment.AgvType &= ~(1 << 6);
            cur_arc_item->map_segment.AgvType |= (value.toBool() << 6);
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("AgvType8")) {
            cur_arc_item->map_segment.AgvType &= ~(1 << 7);
            cur_arc_item->map_segment.AgvType |= (value.toBool() << 7);
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("AllowPoint")) {
            cur_arc_item->map_segment.allowId = value.toInt();
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("BlockPoint")) {
            cur_arc_item->map_segment.blockId = value.toInt();
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("ScanSwitch")) {
            cur_arc_item->map_segment.detection = value.toInt();
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("SpeedLimit")) {
            cur_arc_item->map_segment.speed_limit = value.toInt();
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("DisableSeg")) {
            cur_arc_item->map_segment.disable = value.toInt();
            ui->graphicsView->setModified(true);
        }
        else if (id == QLatin1String("LockSeg")) {
            cur_arc_item->map_segment.lock = value.toInt();
            ui->graphicsView->setModified(true);
        }
        else{
            for(unsigned i = 0; i < sizeof(cur_arc_item->map_segment.reserved)/sizeof(cur_arc_item->map_segment.reserved[0]); i++)
            {
                if(id == QString("Reserved%1").arg(i))
                {
                    cur_arc_item->map_segment.reserved[i] = value.toDouble();
                    ui->graphicsView->setModified(true);
                }
            }
        }
//        if (id == QLatin1String("StartX")) {
//            cur_arc_item->map_segment.start_x = value.toDouble();
//            QPointF temp(value.toDouble(),cur_arc_item->map_segment.start_y);
//            cur_arc_item->m_points.begin()[0] = cur_arc_item->mapFromScene(temp);
//        }
//        else if (id == QLatin1String("StartY")) {
//            cur_arc_item->map_segment.start_y = value.toDouble();
//            QPointF temp(cur_arc_item->map_segment.start_x,value.toDouble());
//            cur_arc_item->m_points.begin()[0] = cur_arc_item->mapFromScene(temp);
//        }
//        else if (id == QLatin1String("EndX")) {
//            cur_arc_item->map_segment.end_x = value.toDouble();
//            QPointF temp(value.toDouble(),cur_arc_item->map_segment.end_y);
//            cur_arc_item->m_points.begin()[1] = cur_arc_item->mapFromScene(temp);
//        }
//        else if (id == QLatin1String("EndY")) {
//            cur_arc_item->map_segment.end_y = value.toDouble();
//            QPointF temp(cur_arc_item->map_segment.end_x,value.toDouble());
//            cur_arc_item->m_points.begin()[1] = cur_arc_item->mapFromScene(temp);
//        }
//        else if (id == QLatin1String("Radius")) {
//            cur_arc_item->map_segment.radius = value.toFloat();
//        }
//        else if (id == QLatin1String("Direction")) {
//            cur_arc_item->map_segment.dirmode = value.toInt();
//        }
//        else if (id == QLatin1String("TraceMode")) {
//            cur_arc_item->map_segment.Tmode = value.toInt();
//        }
//        else if (id == QLatin1String("LN-NF001")) {
//            cur_arc_item->map_segment.AgvType1 = value.toInt();
//        }
//        else if (id == QLatin1String("LN-NF002")) {
//            cur_arc_item->map_segment.AgvType2 = value.toInt();
//        }
//        else if (id == QLatin1String("LN-DJ041")) {
//            cur_arc_item->map_segment.AgvType3 = value.toInt();
//        }
//        else if (id == QLatin1String("AllowPoint")) {
//            cur_arc_item->map_segment.allowId = value.toInt();
//        }
//        else if (id == QLatin1String("BlockPoint")) {
//            cur_arc_item->map_segment.blockId = value.toInt();
//        }
//        else if (id == QLatin1String("ScanSwitch")) {
//            cur_arc_item->map_segment.detection = value.toInt();
//        }
//        else if (id == QLatin1String("SpeedLimit")) {
//            cur_arc_item->map_segment.speed_limit = value.toInt();
//        }
//        else{
//            for(unsigned i = 0; i < sizeof(cur_arc_item->map_segment.reserved)/sizeof(cur_arc_item->map_segment.reserved[0]); i++)
//            {
//                if(id == QString("Reserved%1").arg(i))
//                {
//                    cur_arc_item->map_segment.reserved[i] = value.toDouble();
//                }
//            }
//        }
    }
    else if(currentItem->type() == GraphicsAreaItem::Type)
    {
        GraphicsAreaItem *cur_area_item = dynamic_cast<GraphicsAreaItem *>(currentItem);

        if (id == QLatin1String("Type"))
        {
            cur_area_item->map_area.type = value.toInt();
            ui->graphicsView->setModified(true);
        }
        else
        {
            for(unsigned i = 0; i < sizeof(cur_area_item->map_area.gateway)/sizeof(cur_area_item->map_area.gateway[0]); i++ )
            {
                if (id == QString("Gateway%1").arg(i))
                {
                    cur_area_item->map_area.gateway[i] = value.toInt();
                    ui->graphicsView->setModified(true);
                }
            }
        }
    }
    scene->update();
}

void SubWidget::valueChangedEnum(QtProperty *property, int value)
{
    QGraphicsScene * scene = ui->graphicsView->scene();

    if (!propertyToId.contains(property))
        return;
    if (!currentItem)
        return;

    QString id = propertyToId[property];

    if(currentItem->type() == GraphicsAreaItem::Type)
    {
        GraphicsAreaItem *cur_area_item = dynamic_cast<GraphicsAreaItem *>(currentItem);
        scene->clearSelection();
        if(id == QLatin1String("Type"))
        {
            if(value == 13)
            {
                cur_area_item->map_area.type = 111;
            }
            else
            {
                cur_area_item->map_area.type =  value;
            }
            //qDebug() << cur_point_item->map_point.area ;

            ui->graphicsView->setModified(true);
        }
        cur_area_item->setSelected(true);
    }

    scene->update();
}

void SubWidget::addProperty(QtVariantProperty *property, const QString &id)
{
    propertyToId[property] = id;
    idToProperty[id] = property;
    QtBrowserItem *item = ui->propertyEditor->addProperty(property);
    if (idToExpanded.contains(id))
        ui->propertyEditor->setExpanded(item, idToExpanded[id]);
}

void SubWidget::addVariantProperty(int type, QVariant value, const QString &name, const QString &id, QtProperty *groupItem)
{
    QtVariantProperty *property;
    property = variantManager->addProperty(type, name);
    property->setValue(value);
    propertyToId[property] = id;
    idToProperty[id] = property;
    groupItem->addSubProperty(property);
}

void SubWidget::addEnumProperty(QStringList styleNames, int value, const QString &name, const QString &id, QtProperty *groupItem)
{
    QtProperty *enum_property;
    enum_property = enumManager->addProperty(name);
    enumManager->setEnumNames(enum_property,styleNames);
    enumManager->setValue(enum_property,value);
    propertyToId[enum_property] = id;
    groupItem->addSubProperty(enum_property);
}

void SubWidget::updateExpandState()
{
    QList<QtBrowserItem *> list = ui->propertyEditor->topLevelItems();
    QListIterator<QtBrowserItem *> it(list);
    while (it.hasNext()) {
        QtBrowserItem *item = it.next();
        QtProperty *prop = item->property();
        idToExpanded[propertyToId[prop]] = ui->propertyEditor->isExpanded(item);
    }
}

void SubWidget::actionList_TableViewUpdate(QVector<MapAction> vecAction)
{
//    action_item_model->removeRows(0,action_item_model->rowCount());
    QList<QStandardItem *> lsi;
    QStandardItem *item;

    for(int i = 0; i < vecAction.size(); i++)
    {
        lsi.clear();
        item = new QStandardItem(QString("%1").arg((int)vecAction[i].id));
        item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        lsi.append(item);
        item = new QStandardItem(QString("%1").arg((int)vecAction[i].point_id));
        item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        lsi.append(item);

        for(int j = 0; j < 5; j++)
        {
            item = new QStandardItem(QString("%1").arg((int)vecAction[i].action[j].act));
            item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            lsi.append(item);
            item = new QStandardItem(QString("%1").arg((int)vecAction[i].action[j].value));
            item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            lsi.append(item);
        }
//        action_item_model->appendRow(lsi);
    }
}

void SubWidget::findOccupyCar(int pointID)
{
    for(int i = 0; i < occupyPointID_vec.size() ; i++ )
    {
        for(int j = 0 ; j < occupyPointID_vec[i].size() ; j++)
        {
            if(occupyPointID_vec[i][j] == pointID)
            {
                emit sendMessage(QString("点: %1 被车: %2 占用").arg(pointID).arg(-occupyPointID_vec[i][0]));
                return;
            }
        }
    }

    emit sendMessage(QString("点: %1 没有被占用").arg(pointID));

}

void SubWidget::findScanCar(int pointID)
{
    QVector<int> scanCarid_vec;
    scanCarid_vec.clear();
    QString scanstr;
    QString occupystr;
    QString allstr;

    for(int i = 0; i < scanPointID_vec.size() ; i++ )
    {
        for(int j = 0 ; j < scanPointID_vec[i].size() ; j++)
        {
            if(scanPointID_vec[i][j] == pointID)
            {
                scanCarid_vec.append(-scanPointID_vec[i][0]);
            }
        }
    }

    if(scanCarid_vec.size() < 1)
    {
        scanstr = QString("没有被扫描");
    }
    else
    {
        scanstr = QString("被  ");
        for(int i=0;i<scanCarid_vec.size();i++)
        {
            scanstr += QString("车: %1  ").arg(scanCarid_vec[i]);
        }
        scanstr += QString("扫描");
    }


    for(int i = 0; i < occupyPointID_vec.size() ; i++ )
    {
        for(int j = 0 ; j < occupyPointID_vec[i].size() ; j++)
        {
            if(occupyPointID_vec[i][j] == pointID)
            {
                occupystr = QString("被车: %1 占用").arg(-occupyPointID_vec[i][0]);
                allstr = QString("点: %1 ").arg(pointID) + scanstr + "     " + occupystr;
                emit sendMessage(allstr);
                return;
            }
        }
    }

    occupystr = QString("没有被占用");
    allstr = QString("点: %1 ").arg(pointID) + scanstr + "     " + occupystr;
    emit sendMessage(allstr);

}


//void SubWidget::monitor_connected(QHostAddress IpAddress)
//{
//    qDebug() << "SubWidget::monitor_connected()";
//}

void SubWidget::monitor_disconnected()
{
    qDebug() << "SubWidget::monitor_disconnected()";
}


void SubWidget::get_window_xy()
{
    emit return_window_xy(this->x(),this->y());
}



void SubWidget::send_route_tcp_func(int dev_id,int flag,int port)
{
    if(playbacking == 1)
    {
        if(playback_tcp_array.size() <= 4)
        {
            //qDebug()<<"所有车无路径或playback_tcp_array大小异常";
            return;
        }
        else
        {
            int have_seek_byte = 0;

            int *p = (int*)playback_tcp_array.data();
            int max_byte = *(p+0);
            have_seek_byte += 4;
            //qDebug()<<"数据长度为："<< max_byte;
            p = p + 1;
            have_seek_byte += 4;
            while(*p != dev_id  )
            {
                p = p + 1;
                have_seek_byte += 4;
                int temp_route = *p;
                have_seek_byte = have_seek_byte + (temp_route + 1 + 1 + 1)*4;
                if(have_seek_byte > max_byte)
                {
                    //qDebug()<<"----------越界返回,没有该车的路径----------";
                    return;
                }
                p = p + temp_route + 1 + 1 + 1;
            }


            p = p + 1;
            int route_num = *p;
            int cur_id = *(p + route_num + 1);
            int nxt_id = *(p + route_num + 1 + 1);

            for(int j = 0 ; j < vec_LineItem.size(); j++)
            {
                if(vec_LineItem[j] != nullptr)
                {
                    vec_LineItem[j]->route_flag = 0;
                }
            }
            for(int j = 0 ; j < vec_ArcItem.size(); j++)
            {
                if(vec_ArcItem[j] != nullptr)
                {
                    vec_ArcItem[j]->route_flag = 0;
                }
            }

            for(int i = 0; i < route_num; i++)
            {
                if(vec_LineItem[*(p+i+1)] != nullptr)
                {
                    if(*(p+i+1) == cur_id)
                    {
                        vec_LineItem[*(p+i+1)]->route_flag = 2;
                    }
                    else if(*(p+i+1) == nxt_id)
                    {
                        vec_LineItem[*(p+i+1)]->route_flag = 3;
                    }
                    else
                    {
                        vec_LineItem[*(p+i+1)]->route_flag = 1;
                    }
                }
                else if(vec_ArcItem[*(p+i+1)] != nullptr)
                {
                    if(*(p+i+1) == cur_id)
                    {
                        vec_ArcItem[*(p+i+1)]->route_flag = 2;
                    }
                    else if(*(p+i+1) == nxt_id)
                    {
                        vec_ArcItem[*(p+i+1)]->route_flag = 3;
                    }
                    else
                    {
                        vec_ArcItem[*(p+i+1)]->route_flag = 1;
                    }

                }
                else
                {
                }
            }
            ui->graphicsView->scene()->update();
        }
    }
    else
    {
        send_route_tcp.LOGO1 = 'M';
        send_route_tcp.LOGO2 = 'E';
        send_route_tcp.LOGO3 = 'S';
        send_route_tcp.LOGO4 = 'A';
        send_route_tcp.LOGO5 = 'G';
        send_route_tcp.LOGO6 = 'V';
        send_route_tcp.FLAG = flag;
        send_route_tcp.AGVID = dev_id;
        route_tcpSocket->connectToHost(QHostAddress(ui->tcp_lineEdit->currentText()),port);
        qint64 temp = route_tcpSocket->write((char*)&send_route_tcp,sizeof(send_route_tcp));

    }

}

void SubWidget::proto_send_route_tcp_func(int dev_id,int flag,int port)
{
    routeCarId = dev_id;
    if(playbacking == 1)
    {
        //qDebug()<<"回放  准备处理路径";
        route_list.clear();
        int carIndex = -1;
        QByteArray allArray;

        allArray = playback_tcp_array;
        route_data.ParseFromArray(allArray.data()+6,allArray.size()-8);
        if(route_data.agvroute_size() < 1)
        {
            QMessageBox::information(this," ","无路径数据");
            return;
        }
        for(int i = 0 ; i < route_data.agvroute_size(); i++)
        {
            if(route_data.agvroute(i).id() == dev_id)
            {
                carIndex = i;
            }
        }
        if(carIndex == -1)
        {
            QMessageBox::information(this," ","该车无路径");
            return;
        }


        int route_num = route_data.agvroute(carIndex).route_id_size();
        if(route_num < 1)
        {
            return;
        }
        int cur_id;
        int nxt_id;
        for(int i = 0; i < route_num; i++)
        {
            route_list.push_back(route_data.agvroute(carIndex).route_id(i));
        }
        cur_id = route_data.agvroute(carIndex).cur_seg();
        nxt_id = route_data.agvroute(carIndex).nxt_seg();

        debugWidget->ui->textEditDebug->append(QString("%1 号车的所有路径ID:").arg(dev_id));
        for(int i = 0 ; i < route_num; i++)
        {
            debugWidget->ui->textEditDebug->append(QString("%1").arg(route_list[i]));
        }
        debugWidget->ui->textEditDebug->append(QString("当前路径ID: %1 ").arg(cur_id));
        debugWidget->ui->textEditDebug->append(QString("下一路径ID: %1 ").arg(nxt_id));


        for(int j = 0 ; j < vec_LineItem.size(); j++)
        {
            if(vec_LineItem[j] != nullptr)
            {
                vec_LineItem[j]->route_flag = 0;
            }
        }
        for(int j = 0 ; j < vec_ArcItem.size(); j++)
        {
            if(vec_ArcItem[j] != nullptr)
            {
                vec_ArcItem[j]->route_flag = 0;
            }
        }

        for(int i = 0; i < route_num; i++)
        {
            if(route_list[i] >= vec_LineItem.size() || route_list[i] < 1)
            {
                continue;
            }
            if(vec_LineItem[route_list[i]] != nullptr)
            {
                if(route_list[i] == cur_id)
                {
                    vec_LineItem[route_list[i]]->route_flag = 2;
                }
                else if(route_list[i] == nxt_id)
                {
                    vec_LineItem[route_list[i]]->route_flag = 3;
                }
                else
                {
                    vec_LineItem[route_list[i]]->route_flag = 1;
                }
            }
            else if(vec_ArcItem[route_list[i]] != nullptr)
            {
                if(route_list[i] == cur_id)
                {
                    vec_ArcItem[route_list[i]]->route_flag = 2;
                }
                else if(route_list[i] == nxt_id)
                {
                    vec_ArcItem[route_list[i]]->route_flag = 3;
                }
                else
                {
                    vec_ArcItem[route_list[i]]->route_flag = 1;
                }
            }
            else
            {
            }
        }
        ui->graphicsView->scene()->update();
    }
    else
    {
        route_heartArray.clear();
        char STX1 = 0x6E;
        char STX2 = 0x6D;
        int len;
        int agvRoute = 2;
        mon_ask.set_data(dev_id);
        mon_ask.set_area(rgv_area_id);
        mon_ask.set_type(agvRoute);
        len = mon_ask.ByteSizeLong() + 1 + 1 + 4 + 1 + 1;
        QByteArray tempArray;
        tempArray.resize(mon_ask.ByteSizeLong());
        mon_ask.SerializeToArray(tempArray.data(),mon_ask.ByteSizeLong());
        route_heartArray.append(STX1);
        route_heartArray.append(STX2);
        QByteArray lenArray;
        lenArray.resize(sizeof(len));
        memcpy(lenArray.data(),&len,sizeof(len));
        route_heartArray.append(lenArray);
        route_heartArray.append(tempArray);
        uint16_t crc16 = crc_calculate((uint8_t*)tempArray.data(),tempArray.size());
        char CKA = (crc16 & 0x00FF);
        char CKB = ((crc16 >> 8) & 0x00FF);
        route_heartArray.append(CKA);
        route_heartArray.append(CKB);

        protoTcpSocketRoute->connectToHost(QHostAddress(ui->tcp_lineEdit->currentText()),9106);
        qint64 temp = protoTcpSocketRoute->write(route_heartArray);

    }
}

void SubWidget::select_tablewidget_value(int dev_id, int flag, int port)
{
    for(int i = 0; i < ui->carInfo_tableWidget->rowCount(); i++)
    {
        if(ui->carInfo_tableWidget->item(i,0) != nullptr)
        {

            if(dev_id == ui->carInfo_tableWidget->item(i,0)->text().toInt())
            {
                ui->carInfo_tableWidget->selectRow(i);
                break;
            }
        }
    }
    selectCarID = dev_id;
}

void SubWidget::get_area_value(double area)
{
    rgv_area_id = area;
}

void SubWidget::get_seg_maxID(int max)
{
    seg_maxID = max;
}

void SubWidget::get_point_maxID(int max)
{
    point_maxID = max;
}

void SubWidget::get_area_maxID(int max)
{
    area_maxID = max;
}

void SubWidget::tablewidget_sort(int col)
{
    ui->carInfo_tableWidget->sortItems(col,Qt::AscendingOrder);
}

void SubWidget::displayPointObvious(float x, float y)
{
    displayPointObviousRectItem = new QGraphicsRectItem;
    displayPointObviousRectItem->setRect(QRect(-600,-600,1200,1200));
    displayPointObviousRectItem->setBrush(Qt::NoBrush);
    QPen pen;
    pen.setWidth(40);
    pen.setColor(QColor(0,255,32));
    displayPointObviousRectItem->setPen(pen);
    scene->addItem(displayPointObviousRectItem);
    displayPointObviousRectItem->setPos(x,y);
}

void SubWidget::station_background_slot()
{
    if(station_background_flag == 0)
    {
        for(int i=0; i<list_Station_BackgroundItem.size(); i++)
        {
            scene->addItem(list_Station_BackgroundItem[i]);
        }
        station_backgrounditem612->setPos(120118,99334);
        station_backgrounditem613->setPos(142284,99334);
        station_backgrounditem614->setPos(163966,99334);
        station_backgrounditem615->setPos(187439,99334);
        station_backgrounditem616->setPos(209046,99334);
        station_backgrounditem617->setPos(232008,99334);
        station_backgrounditem607->setPos(120588,73520);
        station_backgrounditem608->setPos(147627,73520);
        station_backgrounditem609->setPos(174467,73520);
        station_backgrounditem610->setPos(201202,73520);
        station_background_flag = 1;
        ParameterSave("Station_B_F","1");
    }
    else if(station_background_flag == 1)
    {
        for(int i=0; i<list_Station_BackgroundItem.size(); i++)
        {
            scene->removeItem(list_Station_BackgroundItem[i]);
        }
        station_background_flag = 0;
        ParameterSave("Station_B_F","0");
    }
}



void SubWidget::closeEvent(QCloseEvent *event)
{
    if (DrawTool::c_drawShape == measure)
    {
        ui->graphicsView->clearMeasureLine();
        ui->graphicsView->scene()->update();
        MeasureTool * tool = dynamic_cast<MeasureTool *>(DrawTool::findTool( DrawTool::c_drawShape ));
        tool->resetVariable(dynamic_cast<DrawScene *>(ui->graphicsView->scene()));
    }

    if (ui->graphicsView->maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

void SubWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
    {
        if(isFullScreenState)
        {
            emit addToMidArea(this);
        }
        else
        {
            QWidget::keyPressEvent(event);
        }
    }
    else if(event->key() == Qt::Key_P)
    {
        if(playback_tool->ui->playback_play_Button->isVisible() && playback_tool->ui->playback_play_Button->isEnabled())
        {
            playback_play_Button_clicked();
        }
        else
        {
            playback_stop_Button_clicked();
        }
    }

    QWidget::keyPressEvent(event);

}



void SubWidget::on_tcp_disconnect_Button_clicked()
{
    udpSocket->abort();

    disconnect(protoUdpSocket,&QUdpSocket::readyRead,this,&SubWidget::proto_recv_data);
    protoUdpSocket->abort();

    emit sendMessage(QString("退出回放模式."));
    first_read_udp_flag = 0;
    if(send_tcp_timer->isActive()){
        send_tcp_timer->stop();
    }
    if(send_protoTcp_timer->isActive()){
        send_protoTcp_timer->stop();
    }


    ui->tcp_disconnect_Button->setVisible(false);
    ui->tcp_disconnect_Button->setEnabled(false);
    ui->proto_tcp_connect_Button->setVisible(true);
    ui->proto_tcp_connect_Button->setEnabled(true);
    for(int i = 0 ; i < vec_Car.size(); i ++)
    {
        ui->graphicsView->scene()->removeItem(vec_Car[i]);//待添加delete
    }
    vec_Car.clear();
    ui->graphicsView->scene()->update();
    connect(protoTcpSocket,&QTcpSocket::connected,this,&SubWidget::protoTcp_has_connected);

}


void SubWidget::on_proto_tcp_connect_Button_clicked()
{
        if(playbacking == 1)
        {
            current_car_num = -1;
            connect(protoUdpSocket,&QUdpSocket::readyRead,this,&SubWidget::proto_recv_data);
            ui->tcp_disconnect_Button->setVisible(true);
            ui->proto_tcp_connect_Button->setVisible(false);
            ui->proto_tcp_connect_Button->setEnabled(false);
            ui->tcp_disconnect_Button->setEnabled(true);
            vec_LineItem.clear();
            vec_ArcItem.clear();
            save_items_of_scene();
        }
        else
        {
            current_car_num = -1;
            protoTcpSocket->connectToHost(QHostAddress(ui->tcp_lineEdit->currentText()),9104);
            if(!abort_protoTcp_timer->isActive())
            {
                abort_protoTcp_timer->start(200);
            }
        }
}



void SubWidget::on_carInfo_tableWidget_cellDoubleClicked(int row, int column)
{
    int tempId = ui->carInfo_tableWidget->item(row,column)->text().toInt();
    for(int i = 0; i < vec_Car.size(); i++)
    {
        if(vec_Car[i]->get_id() == tempId)
        {
            ui->graphicsView->fitInView(vec_Car[i],Qt::KeepAspectRatio);
            ui->graphicsView->scale(0.07,0.07);
            break;
        }
    }

}


void SubWidget::on_mysteriousButton_clicked()
{
    if(mysterious_button_flag == 1)
    {
        mysterious_button_flag = 0;
        emit sendMessage(QString("避让显示已关闭."));
    }else
    {
        mysterious_button_flag = 1;
        emit sendMessage(QString("避让显示已开启."));
    }
}

void SubWidget::on_stop_playback_Button_clicked()
{
    playback_tool->ui->playback_confirm_Button->setEnabled(false);
    ui->stop_playback_Button->setVisible(false);
    on_tcp_disconnect_Button_clicked();
    playbacking = 0;
    on_proto_tcp_connect_Button_clicked();
}



void SubWidget::on_checkBox_OL_stateChanged(int arg1)
{
    ui->carInfo_tableWidget->setColumnHidden(ONLINE,!arg1);
    ParameterSave("CARPARAM/OL",QString("%1").arg(arg1));
}


void SubWidget::on_checkBox_battery_stateChanged(int arg1)
{
    ui->carInfo_tableWidget->setColumnHidden(BATTERY,!arg1);
    ParameterSave("CARPARAM/BTY",QString("%1").arg(arg1));
}

void SubWidget::on_checkBox_cTime_stateChanged(int arg1)
{
    ui->carInfo_tableWidget->setColumnHidden(CHARGING,!arg1);
    ParameterSave("CARPARAM/CHARGING",QString("%1").arg(arg1));
}

//void SubWidget::on_checkBox_load_stateChanged(int arg1)
//{
//    ui->carInfo_tableWidget->setColumnHidden(LOAD,!arg1);
//}

void SubWidget::on_checkBox_startId_stateChanged(int arg1)
{
    ui->carInfo_tableWidget->setColumnHidden(START,!arg1);
    ParameterSave("CARPARAM/START",QString("%1").arg(arg1));
}

void SubWidget::on_checkBox_endId_stateChanged(int arg1)
{
    ui->carInfo_tableWidget->setColumnHidden(END,!arg1);
    ParameterSave("CARPARAM/END",QString("%1").arg(arg1));
}

void SubWidget::on_checkBox_hight_stateChanged(int arg1)
{
    ui->carInfo_tableWidget->setColumnHidden(HEIGHT,!arg1);
    ParameterSave("CARPARAM/HEIGHT",QString("%1").arg(arg1));
}

//void SubWidget::on_checkBox_posX_stateChanged(int arg1)
//{
//    ui->carInfo_tableWidget->setColumnHidden(X,!arg1);
//}

//void SubWidget::on_checkBox_posY_stateChanged(int arg1)
//{
//    ui->carInfo_tableWidget->setColumnHidden(Y,!arg1);
//}

//void SubWidget::on_checkBox_angle_stateChanged(int arg1)
//{
//    ui->carInfo_tableWidget->setColumnHidden(ANGLE,!arg1);
//}

void SubWidget::on_checkBox_dir_stateChanged(int arg1)
{
    ui->carInfo_tableWidget->setColumnHidden(DIRECTION,!arg1);
    ParameterSave("CARPARAM/DIRECTION",QString("%1").arg(arg1));
}

void SubWidget::on_checkBox_speed_stateChanged(int arg1)
{
    ui->carInfo_tableWidget->setColumnHidden(SPEED,!arg1);
    ParameterSave("CARPARAM/SPEED",QString("%1").arg(arg1));
}

//void SubWidget::on_checkBox_curseg_stateChanged(int arg1)
//{
//    ui->carInfo_tableWidget->setColumnHidden(ROUTEC,!arg1);
//}

//void SubWidget::on_checkBox_nxtseg_stateChanged(int arg1)
//{
//    ui->carInfo_tableWidget->setColumnHidden(ROUTEN,!arg1);
//}

void SubWidget::on_checkBox_curpoint_stateChanged(int arg1)
{
    ui->carInfo_tableWidget->setColumnHidden(POINTC,!arg1);
    ParameterSave("CARPARAM/POINTC",QString("%1").arg(arg1));
}

void SubWidget::on_checkBox_nxtpoint_stateChanged(int arg1)
{
    ui->carInfo_tableWidget->setColumnHidden(POINTN,!arg1);
    ParameterSave("CARPARAM/POINTN",QString("%1").arg(arg1));
}

void SubWidget::on_checkBox_fSafe_stateChanged(int arg1)
{
    ui->carInfo_tableWidget->setColumnHidden(SAFEF,!arg1);
    ParameterSave("CARPARAM/SAFEF",QString("%1").arg(arg1));
}

void SubWidget::on_checkBox_bSafe_stateChanged(int arg1)
{
    ui->carInfo_tableWidget->setColumnHidden(SAFEB,!arg1);
    ParameterSave("CARPARAM/SAFEB",QString("%1").arg(arg1));
}

void SubWidget::on_checkBox_forkLight_stateChanged(int arg1)
{
    ui->carInfo_tableWidget->setColumnHidden(FORKLIGHT,!arg1);
    ParameterSave("CARPARAM/FORKLIGHT",QString("%1").arg(arg1));
}



void SubWidget::on_checkBox_carCT_stateChanged(int arg1)
{
   ui->carInfo_tableWidget->setColumnHidden(VERSION_TS,!arg1);
   ParameterSave("CARPARAM/VERSION_TS",QString("%1").arg(arg1));
}

void SubWidget::on_checkBox_1min_stateChanged(int arg1)
{
    ui->carInfo_tableWidget->setColumnHidden(HEART1MIN,!arg1);
    ParameterSave("CARPARAM/HEART1MIN",QString("%1").arg(arg1));
}
#if 1

void SubWidget::on_checkBox_IPaddr_stateChanged(int arg1)
{
    ui->carInfo_tableWidget->setColumnHidden(IP,!arg1);
    ParameterSave("CARPARAM/IP",QString("%1").arg(arg1));
}


void SubWidget::on_checkBox_typeA_stateChanged(int arg1)
{
    ui->carInfo_tableWidget->setColumnHidden(TYPEC,!arg1);
    ParameterSave("CARPARAM/TYPEC",QString("%1").arg(arg1));
}

void SubWidget::on_checkBox_batteryZD_stateChanged(int arg1)
{
    ui->carInfo_tableWidget->setColumnHidden(BATTERYHOLDER,!arg1);
    ParameterSave("CARPARAM/BATTERYHOLDER",QString("%1").arg(arg1));
}



void SubWidget::on_checkBox_5min_stateChanged(int arg1)
{
   ui->carInfo_tableWidget->setColumnHidden(HEART5MIN,!arg1);
   ParameterSave("CARPARAM/HEART5MIN",QString("%1").arg(arg1));
}

void SubWidget::on_checkBox_15min_stateChanged(int arg1)
{
    ui->carInfo_tableWidget->setColumnHidden(HEART15MIN,!arg1);
    ParameterSave("CARPARAM/HEART15MIN",QString("%1").arg(arg1));
}

void SubWidget::on_checkBox_carVersion_stateChanged(int arg1)
{
    ui->carInfo_tableWidget->setColumnHidden(VERSION,!arg1);
    ParameterSave("CARPARAM/VERSION",QString("%1").arg(arg1));
}

void SubWidget::on_checkBox_versionType_stateChanged(int arg1)
{
    ui->carInfo_tableWidget->setColumnHidden(BETA,!arg1);
    ParameterSave("CARPARAM/BETA",QString("%1").arg(arg1));
}

void SubWidget::on_checkBox_xcoord_stateChanged(int arg1)
{
    ui->carInfo_tableWidget->setColumnHidden(X,!arg1);
    ParameterSave("CARPARAM/XCOORD",QString("%1").arg(arg1));
}

void SubWidget::on_checkBox_ycoord_stateChanged(int arg1)
{
    ui->carInfo_tableWidget->setColumnHidden(Y,!arg1);
    ParameterSave("CARPARAM/YCOORD",QString("%1").arg(arg1));
}

void SubWidget::on_checkBox_angle_stateChanged(int arg1)
{
    ui->carInfo_tableWidget->setColumnHidden(ANGLE,!arg1);
    ParameterSave("CARPARAM/ANGLE",QString("%1").arg(arg1));
}

void SubWidget::on_checkBox_bindarea_stateChanged(int arg1)
{
    ui->carInfo_tableWidget->setColumnHidden(BINDAREA,!arg1);
    ParameterSave("CARPARAM/BINDAREA",QString("%1").arg(arg1));
}

void SubWidget::on_checkBox_stalimitf_stateChanged(int arg1)
{
    ui->carInfo_tableWidget->setColumnHidden(STALIMIT,!arg1);
    ParameterSave("CARPARAM/STALIMIT",QString("%1").arg(arg1));
}
#endif
