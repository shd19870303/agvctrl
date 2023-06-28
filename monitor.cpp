#include "monitor.h"
#include "ui_monitor.h"

#include "simulationwidget.h"

Monitor::Monitor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Monitor)
{
    ui->setupUi(this);
#if 0
    QRegExp regx("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b");
    QValidator* validator = new QRegExpValidator(regx, ui->lineEdit_IP);
    ui->lineEdit_IP->setValidator(validator);

    timer = new QTimer(this);    
    timer->setInterval(10);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateValue()));

    udpSocket = new QUdpSocket(this);
    bool result = udpSocket->bind(9001);
    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(recvUdpData()));


    tcpSocket = new QTcpSocket(this);

    tcpSocketHeartbeat = new QTcpSocket(this);
    timerHeartbeat = new QTimer(this);
    timerHeartbeat->setInterval(2000);
    connect(timerHeartbeat, SIGNAL(timeout()), this, SLOT(timerSendHeartbeat()));
    this->setVisible(false);
#endif
}

Monitor::~Monitor()
{
    delete ui;
}

void Monitor::setGraphicsView(DrawView *activeView)
{
    this->activeView = activeView;
}

void Monitor::recvUdpData()
{
    while(udpSocket->hasPendingDatagrams())
    {
        recvPackageArray.resize(int(udpSocket->pendingDatagramSize()));
        udpSocket->readDatagram(recvPackageArray.data(), recvPackageArray.size()/*,&IpAddress*/);
        if(0xA0 == (unsigned char)recvPackageArray[0] && 0x02 == (unsigned char)recvPackageArray[1])
        {
            uint16_t crc16 = crc_calculate((uint8_t*)recvPackageArray.data(),recvPackageArray.size() - 2 );
            if((crc16 & 0x00FF) == (unsigned char)recvPackageArray[recvPackageArray.size()-2] && ((crc16 >> 8) & 0x00FF) == (unsigned char)recvPackageArray[recvPackageArray.size()-1])
            {
                if(activeView)
                {
                    mapId.clear();

                    foreach (QGraphicsItem *item , activeView->scene()->items())
                    {
                        if(item->type() == GraphicsRgvItem::Type)
                        {
                            GraphicsRgvItem *rgvItem = dynamic_cast<GraphicsRgvItem *>(item);
                            activeView->scene()->removeItem(rgvItem);
                        }
                    }

                    for(int i = 0; i < (unsigned char)recvPackageArray[4]; i++)
                    {
                        GraphicsRgvItem *rgv = new GraphicsRgvItem();
                        memcpy(&rgv->rgv_info,recvPackageArray.data() + 5 + i * sizeof(rgv->rgv_info),sizeof(rgv->rgv_info));
                        QPointF pos = activeView->path.pointAtPercent(activeView->path.percentAtLength(rgv->rgv_info.mileage));
                        qreal angle = activeView->path.angleAtPercent(activeView->path.percentAtLength(rgv->rgv_info.mileage));
                        rgv->setPos(pos);
                        rgv->setRotation(-angle);
                        activeView->scene()->addItem(rgv);

                        if(rgv->rgv_info.task_main_state == 2 && rgv->rgv_info.rgv_sub_state == TASKRUNNING)
                        {
                            //emit deleteTaskByIdSignal(rgv->rgv_info.task_id);
                        }

                        mapId.insert(rgv->rgv_info.dev_id, rgv->rgv_info.task_id);

                    }



                    for(int i = 0; i < mapId.size(); i ++)
                    {
                        if(mapId.value(mapId.keys()[i]) != 0 && mapId.value(mapId.keys()[i]) != -1 && lastMapId.value(mapId.keys()[i]) != 0 && lastMapId.value(mapId.keys()[i]) != -1)
                        {
                            if(lastMapId.contains(mapId.keys()[i]))
                            {
                                if(lastMapId.value(mapId.keys()[i]) != mapId.value(mapId.keys()[i]))
                                {
                                    mapIdFlash[mapId.keys()[i]].isflash = true;
                                    mapIdFlash[mapId.keys()[i]].count = 0;
                                    mapIdFlash[mapId.keys()[i]].showing = false;
                                    qDebug() << mapId.keys()[i] <<"号车切换任务" << lastMapId.value(mapId.keys()[i]) << "--->" << mapId.value(mapId.keys()[i]);
                                }
                            }
                        }
                    }

                    //qDebug() << mapIdFlash.size() << mapId.size() << mapIdFlash.keys();

                    for(int i = 0; i < mapIdFlash.size(); i++)
                    {
                        if(mapIdFlash[mapIdFlash.keys()[i]].isflash == true)
                        {

                            mapIdFlash[mapIdFlash.keys()[i]].count++;
                            //qDebug() << mapIdFlash.keys()[i] << "count=" << mapIdFlash[mapIdFlash.keys()[i]].count;
                            if(mapIdFlash[mapIdFlash.keys()[i]].count%1 == 0)
                            {
                                mapIdFlash[mapIdFlash.keys()[i]].showing = !mapIdFlash[mapIdFlash.keys()[i]].showing;
                            }

//                            //从scene中删除掉rgv
                            if(mapIdFlash[mapIdFlash.keys()[i]].showing == true)
                            {
                                foreach (QGraphicsItem *item , activeView->scene()->items())
                                {
                                    if(item->type() == GraphicsRgvItem::Type)
                                    {
                                        GraphicsRgvItem *rgvItem = dynamic_cast<GraphicsRgvItem *>(item);
                                        if(rgvItem->rgv_info.dev_id == mapIdFlash.keys()[i])
                                        {
                                            //activeView->scene()->removeItem(rgvItem);
                                            rgvItem->hide();
                                        }
                                    }
                                }
                            }

                            if(mapIdFlash[mapIdFlash.keys()[i]].count > 6)
                            {
                                qDebug() << mapIdFlash[mapId.keys()[i]].count;
                                mapIdFlash[mapIdFlash.keys()[i]].count = 0;
                                mapIdFlash[mapIdFlash.keys()[i]].isflash = false;
                                mapIdFlash[mapIdFlash.keys()[i]].showing = false;

                                foreach (QGraphicsItem *item , activeView->scene()->items())
                                {
                                    if(item->type() == GraphicsRgvItem::Type)
                                    {
                                        GraphicsRgvItem *rgvItem = dynamic_cast<GraphicsRgvItem *>(item);
                                        if(rgvItem->rgv_info.dev_id == mapIdFlash.keys()[i])
                                        {
                                            //activeView->scene()->removeItem(rgvItem);
                                            rgvItem->show();
                                        }
                                    }
                                }
                            }
                        }
                    }

                    lastMapId = mapId;
                    activeView->scene()->update();
                }

            }
            else{
                qDebug() << "crc错误 收到：" << (unsigned char)recvPackageArray[recvPackageArray.size()-2] << (unsigned char)recvPackageArray[recvPackageArray.size()-1]
                         << "计算：" << (crc16 & 0x00FF) << ((crc16 >> 8) & 0x00FF);
            }
        }
        else{
            qDebug() << "收到的头错误" << recvPackageArray.size() << (unsigned char)recvPackageArray[0] << (unsigned char)recvPackageArray[1];
        }
    }
}

void Monitor::updateValue()
{
    if(activeView)
    {
        test_mileage+= 50;
        if(test_mileage > activeView->path.length())
        {
            test_mileage = 0;
        }

        foreach (QGraphicsItem *item , activeView->scene()->items())
        {
            if(item->type() == GraphicsRgvItem::Type)
            {
                GraphicsRgvItem *rgvItem = dynamic_cast<GraphicsRgvItem *>(item);
                activeView->scene()->removeItem(rgvItem);
            }
        }

        GraphicsRgvItem *rgv = new GraphicsRgvItem();
        QPointF pos = activeView->path.pointAtPercent(activeView->path.percentAtLength(test_mileage));
        qreal angle = activeView->path.angleAtPercent(activeView->path.percentAtLength(test_mileage));
        rgv->setPos(pos);
        rgv->setRotation(-angle);
        activeView->scene()->addItem(rgv);
        activeView->scene()->update();
    }
}


void Monitor::timerSendHeartbeat()
{

}


void Monitor::on_pushBtn_connect_clicked()
{
////        qDebug() << "activeView";
////        //qDebug() << activeView->path.pointAtPercent(activeView->path.percentAtLength(3355));
////        GraphicsRgvItem *rgv = new GraphicsRgvItem();
////        QPointF pos = activeView->path.pointAtPercent(activeView->path.percentAtLength(test_mileage));
////        qreal angle = activeView->path.angleAtPercent(activeView->path.percentAtLength(test_mileage));
////        rgv->setPos(pos);
////        rgv->setRotation(angle);
////        activeView->scene()->addItem(rgv);

////        timer->start();

//        IpAddress = QHostAddress(ui->lineEdit_IP->text());

//        emit monitor_connected_signal(IpAddress);

////        tcpSocketHeartbeat->connectToHost(IpAddress,9502);
////        if(tcpSocketHeartbeat->waitForConnected(3000))
////        {
////            MonitorHeartbeat heartPackage;
////            ui->pushBtn_connect->setEnabled(false);
////            timerHeartbeat->start();
////        }

//        QByteArray sendTaskArray;
//        SendTaskPackage header;
//        header.header1 = 0xC0;
//        header.header2 = 0xD3;
//        header.timestamp = QDateTime::currentMSecsSinceEpoch();
//        //header.task_num = monitor->simulation->taskList.size();
//        header.task_num = 0;
//        sendTaskArray.append((char *)&header,sizeof(header));
//        uint16_t crc16 = crc_calculate((uint8_t*)sendTaskArray.data(),sendTaskArray.size());
//        sendTaskArray.append(crc16 & 0x00FF);
//        sendTaskArray.append((crc16 >> 8) & 0x00FF);
//        udpSocket->writeDatagram(sendTaskArray,IpAddress,9003);

}

void Monitor::on_pushBtn_disconnect_clicked()
{

    foreach (QGraphicsItem *item , activeView->scene()->items())
    {
        if(item->type() == GraphicsRgvItem::Type)
        {
            GraphicsRgvItem *rgvItem = dynamic_cast<GraphicsRgvItem *>(item);
            activeView->scene()->removeItem(rgvItem);
        }
    }

    timer->stop();

    emit monitor_disconnected_signal();
}

