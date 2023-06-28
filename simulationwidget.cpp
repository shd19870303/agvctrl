#include "simulationwidget.h"
#include "ui_simulationwidget.h"
#include <QFileDialog>
#include <QMessageBox>

#include "inputcreatetasksum.h"

bool comTaskCreateTime(simTask::SIMTASK &a, simTask::SIMTASK &b)
{
    if (b.ctimes > a.ctimes)
    {
        return true;
    }
    else
    {
        return false;
    }
}

SimulationWidget::SimulationWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SimulationWidget)
{
    ui->setupUi(this);
    add_task_item_model = new QStandardItemModel(this);
    ui->tableView->setModel(add_task_item_model);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    QStringList column;
    column << "Start_Id" << "End_Id" << "cntDown";
    add_task_item_model->setHorizontalHeaderLabels(column);

    udpSocket = new QUdpSocket(this);
    bool result = udpSocket->bind(9001);

    updateListTimer = new QTimer(this);
    updateListTimer->setInterval(1000);
    connect(updateListTimer, SIGNAL(timeout()), this, SLOT(timerUpdateTaskList()));

    simulation = new simTask();
    sendTaskThread = new SendTaskThread(this);
}

SimulationWidget::~SimulationWidget()
{
    if(simulation->isRunning())
    {
        simulation->quit();
        simulation->terminate();
    }
    if(sendTaskThread->isRunning())
    {
        sendTaskThread->quit();
        sendTaskThread->terminate();
    }

    delete ui;
}

void SimulationWidget::setGraphicsView(DrawView *activeView)
{
    this->activeView = activeView;
}

void SimulationWidget::on_pushBtn_add_clicked()
{
    QList<QStandardItem *> lsi;
    QStandardItem *item;
    item = new QStandardItem(QString("%1").arg(-1));
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    lsi.append(item);
    item = new QStandardItem(QString("%1").arg(-1));
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    lsi.append(item);
    item = new QStandardItem(QString("%1").arg(0));
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    lsi.append(item);
    add_task_item_model->appendRow(lsi);
}

void SimulationWidget::on_pushBtn_addToList_clicked()
{
    //qDebug() << add_task_item_model->rowCount()  << ui->tableView->currentIndex().row();
    int rowCount = add_task_item_model->rowCount();

    if(rowCount > 0)
    {
        for(int i = 0; i < rowCount; i++)
        {
            for(int j = 0; j < add_task_item_model->columnCount(); j++)
            {
 //               qDebug() <<  add_task_item_model->data(add_task_item_model->index(i,j)).toInt();
            }
            emit addTaskManualSignal(add_task_item_model->data(add_task_item_model->index(i,0)).toInt(),
                                     add_task_item_model->data(add_task_item_model->index(i,1)).toInt(),
                                     add_task_item_model->data(add_task_item_model->index(i,2)).toInt());
        }
    }
    //add_task_item_model->removeRows(0,rowCount);
}

void SimulationWidget::on_pushBtn_del_clicked()
{
    int current = ui->tableView->currentIndex().row();
    if(current >= 0)
    {
        add_task_item_model->removeRow(current);
    }
}

void SimulationWidget::on_pushButton_open_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,"打开文件",".","*.txt");
    if (!fileName.isEmpty())
    {
        QFile file(fileName);
        if(!file.open(QFile::ReadOnly | QIODevice::Text))
        {
            QMessageBox::warning(this, tr("Add Task"),
                                         tr("Cannot open file %1:\n%2.")
                                         .arg(fileName)
                                         .arg(file.errorString()));
        }

        int rowCount = add_task_item_model->rowCount();
        add_task_item_model->removeRows(0,rowCount);

        QTextStream txtInput(&file);
        QString lineStr;
        QStringList listStr;
        while(!txtInput.atEnd())
        {
            lineStr = txtInput.readLine();
            listStr = lineStr.split(',');

            //qDebug() << listStr;

            QList<QStandardItem *> lsi;
            QStandardItem *item;
            item = new QStandardItem(QString("%1").arg(listStr[0].trimmed().toInt()));
            item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            lsi.append(item);
            item = new QStandardItem(QString("%1").arg(listStr[1].trimmed().toInt()));
            item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            lsi.append(item);
            item = new QStandardItem(QString("%1").arg(listStr[2].trimmed().toInt()));
            item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            lsi.append(item);
            add_task_item_model->appendRow(lsi);
        }
    }
}

void SimulationWidget::on_pushButton_save_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,"保存文件",".","*.txt");
    if (!fileName.isEmpty())
    {
        QFile file(fileName);
        if(!file.open(QFile::WriteOnly | QIODevice::Text))
        {
            QMessageBox::warning(this, tr("Add Task"),
                                         tr("Cannot open file %1:\n%2.")
                                         .arg(fileName)
                                         .arg(file.errorString()));
        }


        QTextStream txtOutput(&file);

        //txtOutput << "Start_Id    End_Id";

        int rowCount = add_task_item_model->rowCount();
        if(rowCount > 0)
        {
            for(int i = 0; i < rowCount; i++)
            {
                for(int j = 0; j < add_task_item_model->columnCount(); j++)
                {
//                    qDebug() <<  add_task_item_model->data(add_task_item_model->index(i,j)).toInt();
                }
                txtOutput << QString("%1, %2, %3").arg(add_task_item_model->data(add_task_item_model->index(i,0)).toInt())
                             .arg(add_task_item_model->data(add_task_item_model->index(i,1)).toInt())
                             .arg(add_task_item_model->data(add_task_item_model->index(i,2)).toInt()) << endl;
            }
        }
        file.close();
    }
}

void SimulationWidget::on_pushBtn_SimulateStart_clicked()
{
    simTask::STATIONSET sim_station;
    simulation->stationList.clear();

    foreach (QGraphicsItem *item , activeView->scene()->items())
    {
        if ( item->type() == GraphicsPointItem::Type)
        {
            GraphicsPointItem * my_point_item = dynamic_cast<GraphicsPointItem*>(item);
            if(my_point_item->map_point.type != 0)
            {
                sim_station.pntID = my_point_item->map_point.id;
                sim_station.mileage = my_point_item->map_point.stationpos;
                sim_station.group = my_point_item->map_point.group;
                sim_station.type = (simTask::STATYPE)my_point_item->map_point.type;
                sim_station.staEnable = my_point_item->map_point.staEnable;
                memcpy(sim_station.tarGroup,my_point_item->map_point.tarGroup,5);
                sim_station.freOffset = my_point_item->map_point.freOffset;
                sim_station.frequency = my_point_item->map_point.frequency;

                simulation->stationList.push_back(sim_station);
            }

        }
    }

    if(activeView->vecSegment.size() > 0 && activeView->vecPoint.size())
    {
        simulation->totalMileage = activeView->vecSegment[(activeView->vecSegment.size()-1)].end_mileage;

        qDebug() << simulation->stationList.size();

        simulation->randomLastTaskTimes();
        simulation->randomTaskExceptTiems();

        simulation->start();
        sendTaskThread->start();

        ui->pushBtn_SimulateStart->setEnabled(false);
        ui->pushBtn_SimulatePause->setEnabled(true);
        ui->pushBtn_SimulateStop->setEnabled(true);
        ui->pushBtn_TaskClear->setEnabled(true);

        updateListTimer->start();
    }
    else{
        QMessageBox::warning(this,"Warning","     地图错误！    ");
    }

}

void SimulationWidget::on_pushBtn_SimulatePause_clicked()
{
    simulation->quit();
    simulation->terminate();
    ui->pushBtn_SimulateStart->setEnabled(true);
    ui->pushBtn_SimulatePause->setEnabled(false);
}

void SimulationWidget::on_pushBtn_SimulateStop_clicked()
{
    simulation->taskList.clear();
    simulation->taskSend.clear();
    simulation->quit();
    simulation->terminate();
    sendTaskThread->quit();
    sendTaskThread->terminate();
    emit sendTaskThread->taskListUpdateSignal();
    ui->pushBtn_SimulateStart->setEnabled(true);
    ui->pushBtn_SimulateStop->setEnabled(false);
    ui->pushBtn_TaskClear->setEnabled(false);
    updateListTimer->stop();
}

void SimulationWidget::on_pushBtn_TaskClear_clicked()
{
    simulation->taskList.clear();
    simulation->taskSend.clear();
    simulation->taskId = 100;
    emit sendTaskThread->taskListUpdateSignal();
}

void SimulationWidget::getIpAddress(QHostAddress IpAddress)
{
    this->IpAddress = IpAddress;
    ui->pushBtn_SimulateStart->setEnabled(true);

    qDebug() << "IpAddress:" << this->IpAddress;
}

void SimulationWidget::timerUpdateTaskList()
{
    emit sendTaskThread->taskListUpdateSignal();

    for(int i = 0; i < simulation->taskList.size(); i++)
    {
        if(simulation->taskList[i].manualCreat == 1)
        {
            if(simulation->taskList[i].countDown == 0)
            {
                simulation->taskList[i].manualCreat = 0;
                simulation->taskList[i].taskId = simulation->taskId++;
                simulation->FilterTaskForSend();
            }
            else{
                simulation->taskList[i].countDown--;
                break;
            }
        }
    }
}

void SimulationWidget::deleteTaskById(int taskID)
{
    simulation->deleteTaskByID(taskID);
}


void SimulationWidget::monitor_disconnected()
{
    simulation->taskList.clear();
    simulation->quit();
    simulation->terminate();
    sendTaskThread->quit();
    sendTaskThread->terminate();
    emit sendTaskThread->taskListUpdateSignal();

    ui->pushBtn_SimulateStart->setEnabled(false);
    ui->pushBtn_SimulatePause->setEnabled(false);
    ui->pushBtn_SimulateStop->setEnabled(false);
    ui->pushBtn_TaskClear->setEnabled(false);
}

SendTaskThread::SendTaskThread(SimulationWidget *parent)
{
    this->simulationWidget = parent;
}

void SendTaskThread::run()
{
    QByteArray sendTaskArray;
    SendTaskPackage header;
    int last_taskListSize = 0;
    while(true)
    {
        sendTaskArray.clear();
        header.header1 = 0xC0;
        header.header2 = 0xD3;
        header.timestamp = QDateTime::currentMSecsSinceEpoch();
        //header.task_num = monitor->simulation->taskList.size();
        header.task_num = simulationWidget->simulation->taskSend.size();
        sendTaskArray.append((char *)&header,sizeof(header));
        //for(int i = 0; i < monitor->simulation->taskList.size(); i++)
        for(int i = 0; i < simulationWidget->simulation->taskSend.size(); i++)
        {
            //sendTaskArray.append((char *)&monitor->simulation->taskList[i],sizeof(simTask::SIMTASK));
            sendTaskArray.append((char *)&simulationWidget->simulation->taskSend[i],sizeof(simTask::SIMTASKSEND));
        }

        uint16_t crc16 = crc_calculate((uint8_t*)sendTaskArray.data(),sendTaskArray.size());

        sendTaskArray.append(crc16 & 0x00FF);
        sendTaskArray.append((crc16 >> 8) & 0x00FF);

        simulationWidget->udpSocket->writeDatagram(sendTaskArray,simulationWidget->IpAddress,9003);

 //       qDebug() << "Send Task in Thread: task size = " << monitor->simulation->taskList.size() << "task send size"<< monitor->simulation->taskSend.size() << monitor->IpAddress;

        if(last_taskListSize != simulationWidget->simulation->taskList.size())
        {
            emit taskListUpdateSignal();
        }

        last_taskListSize = simulationWidget->simulation->taskList.size();

        QThread::usleep(500000);
    }
}

void SimulationWidget::on_pushBtn_CreatTaskToFile_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,"保存文件",".","*.txt");
    if (!fileName.isEmpty())
    {
        QFile file(fileName);
        if(!file.open(QFile::WriteOnly | QIODevice::Text))
        {
            QMessageBox::warning(this, tr("Add Task"),
                                         tr("Cannot open file %1:\n%2.")
                                         .arg(fileName)
                                         .arg(file.errorString()));
        }

        InputCreateTaskSum inputCreateTaskSumDialog(this);
        int result = inputCreateTaskSumDialog.exec();
        int ret = 0;
        if(result == QDialog::Accepted)
        {
            ret = inputCreateTaskSumDialog.getInputNum();
        }
        else if(result == QDialog::Rejected)
        {
            return;
        }

        simTask::STATIONSET sim_station;
        simulation->stationListManualCreate.clear();
        foreach (QGraphicsItem *item , activeView->scene()->items())
        {
            if ( item->type() == GraphicsPointItem::Type)
            {
                GraphicsPointItem * my_point_item = dynamic_cast<GraphicsPointItem*>(item);
                if(my_point_item->map_point.type != 0)
                {
                    sim_station.pntID = my_point_item->map_point.id;
                    sim_station.mileage = my_point_item->map_point.stationpos;
                    sim_station.group = my_point_item->map_point.group;
                    sim_station.type = (simTask::STATYPE)my_point_item->map_point.type;
                    sim_station.staEnable = my_point_item->map_point.staEnable;
                    memcpy(sim_station.tarGroup,my_point_item->map_point.tarGroup,5);
                    sim_station.freOffset = my_point_item->map_point.freOffset;
                    sim_station.frequency = my_point_item->map_point.frequency;

                    simulation->stationListManualCreate.push_back(sim_station);
                }
            }
        }

        if(activeView->vecSegment.size() > 0 && activeView->vecPoint.size())
        {
            simulation->totalMileage = activeView->vecSegment[(activeView->vecSegment.size()-1)].end_mileage;
            qDebug() << simulation->stationListManualCreate.size();
        }
        else{
            QMessageBox::warning(this,"Warning","     地图错误！    ");
            return;
        }

        QTextStream txtOutput(&file);

        simulation->taskListManualCreate.clear();

        simulation->CreateTaskListManual(ret);

        qSort(simulation->taskListManualCreate.begin(),simulation->taskListManualCreate.end(),comTaskCreateTime);

        QVector<simTask::SIMTASK> taskListTemp = simulation->taskListManualCreate;
        for(int i = 0; i < simulation->taskListManualCreate.size(); i++)  // 处理任务生成的时间
        {
            if(i > 0)
            {
                simulation->taskListManualCreate[i].ctimes = simulation->taskListManualCreate[i].ctimes - taskListTemp[i-1].ctimes;
            }
        }

        simulation->taskListManualCreate[0].ctimes = 0;

        for(int i = 0; i < simulation->taskListManualCreate.size(); i++)
        {
            txtOutput << QString("%1, %2, %3").arg(simulation->taskListManualCreate[i].startStaID)
                         .arg(simulation->taskListManualCreate[i].endStaID)
                         .arg(simulation->taskListManualCreate[i].ctimes ) << endl;
        }

        file.close();
    }
}
