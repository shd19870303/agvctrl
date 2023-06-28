#include "taskspecialdialog.h"
#include <QDateTime>

taskSpecialDialog::taskSpecialDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::taskSpecialDialog)
{
    ui->setupUi(this);
    Qt::WindowFlags windowFlag  = Qt::Dialog;
    windowFlag                  |= Qt::WindowMinimizeButtonHint;
    windowFlag                  |= Qt::WindowMaximizeButtonHint;
    windowFlag                  |= Qt::WindowCloseButtonHint;

    setWindowFlags(windowFlag);
    ui->dateEdit->setDateTime(QDateTime::currentDateTime());
}

taskSpecialDialog::~taskSpecialDialog()
{
    delete ui;
}

void taskSpecialDialog::on_queryButton_clicked()
{
    if(taskDBName.isEmpty())
    {
        getTaskDBName_db = QSqlDatabase::addDatabase("QMYSQL","getTaskDBName_connection");
        getTaskDBName_db.setHostName(ui->IpComboBox->currentText());
        getTaskDBName_db.setUserName("manager");
        getTaskDBName_db.setPassword("AgvManager");
        getTaskDBName_db.setDatabaseName("agvtransfer_log");
        if(!getTaskDBName_db.open())
        {
            QMessageBox::warning(this,"agvtransfer_log数据库打开失败",getTaskDBName_db.lastError().text());
            return ;
        }
        else
        {
            qDebug()<<"agvtransfer_log数据库打开成功";
            QSqlQuery query1(getTaskDBName_db);
            QString tempstr = QString("select dbname from %1;").arg("CTL_DB_NAME");
            query1.exec(tempstr);
            if(query1.next() == true)
            {
                qDebug()<<"name is :"<<query1.value(0).toString();
                taskDBName = query1.value(0).toString();
            }
            else
            {
                QMessageBox::warning(this,"","dbname未找到");
            }
        }
    }
    else
    {
        qDebug()<<"已经找到daname:"<<taskDBName;
    }


    getTaskInfo_db = QSqlDatabase::addDatabase("QMYSQL","getTaskInfo_connection");
    getTaskInfo_db.setHostName(ui->IpComboBox->currentText());
    getTaskInfo_db.setUserName("manager");
    getTaskInfo_db.setPassword("AgvManager");
    getTaskInfo_db.setDatabaseName(taskDBName);
    if(!getTaskInfo_db.open())
    {
        QMessageBox::warning(this, taskDBName+"数据库打开失败",getTaskInfo_db.lastError().text());
        return ;
    }
    else
    {
        qDebug()<<taskDBName+"数据库打开成功";

//        QSqlQuery testquery(getTaskInfo_db);
//        QString teststr = QString("select creatdate from EVT_WBS_TASK;");
//        testquery.exec(teststr);
//        testquery.next();
//        qDebug()<<"testquery creatdate is : "<<testquery.value(0);
//        qDebug()<<"ui->dateEdit->dateTime().toTime_t() is :"<<ui->dateEdit->dateTime().toTime_t();

        QSqlQuery query2(getTaskInfo_db);
        QString tempstr = QString("select taskid,mes_task,start_id,end_id,startdate,finishdate,agv_id,get_err_ed,get_err_angle,put_err_ed,put_err_angle from EVT_WBS_TASK where  (task_st=4 or task_st=10) and creattime> %2 and agv_id = %1 and (ABS(get_err_ed)>0.025 or ABS(get_err_angle)>2 or ABS(put_err_ed)>0.025 or ABS(put_err_angle)>2);").arg(ui->AGVIDComboBox->currentText()).arg(ui->dateEdit->dateTime().toSecsSinceEpoch());
        query2.exec(tempstr);
        qDebug()<<"query2 size :"<<query2.size();

        QSqlQuery query3(getTaskInfo_db);
        QString tempstr2 = QString("select count(*) from EVT_WBS_TASK where (task_st=4 or task_st=10) and agv_id=%1 and creattime>%2;").arg(ui->AGVIDComboBox->currentText()).arg(ui->dateEdit->dateTime().toSecsSinceEpoch());
        query3.exec(tempstr2);
        query3.next();
//        qDebug()<<"query3 :"<<query3.value(0).toInt();
        ui->taskNumLabel->setText(QString("任务总数: %1").arg(query3.value(0).toInt()));
        int taskNum = query2.size();
        ui->taskSpecialTable->setRowCount(taskNum);

        for(int i = 0; i < taskNum; i++)
        {
            query2.next();
            if(ui->taskSpecialTable->item(i,0) == nullptr)
            {
                qDebug()<<"没有项  新建项 ";
                ui->taskSpecialTable->setItem(i, 0, new QTableWidgetItem());
                ui->taskSpecialTable->item(i,0)->setData(Qt::DisplayRole , query2.value(0).toInt());
                ui->taskSpecialTable->setItem(i, 1, new QTableWidgetItem());
                ui->taskSpecialTable->item(i,1)->setData(Qt::DisplayRole , query2.value(1).toInt());
                ui->taskSpecialTable->setItem(i, 2, new QTableWidgetItem());
                ui->taskSpecialTable->item(i,2)->setData(Qt::DisplayRole , query2.value(2).toInt());
                ui->taskSpecialTable->setItem(i, 3, new QTableWidgetItem());
                ui->taskSpecialTable->item(i,3)->setData(Qt::DisplayRole , query2.value(3).toInt());
                ui->taskSpecialTable->setItem(i, 4, new QTableWidgetItem());
                ui->taskSpecialTable->item(i,4)->setData(Qt::DisplayRole , query2.value(4).toDateTime().toString("yyyy-MM-dd hh:mm:ss"));
                ui->taskSpecialTable->setItem(i, 5, new QTableWidgetItem());
                ui->taskSpecialTable->item(i,5)->setData(Qt::DisplayRole , query2.value(5).toDateTime().toString("yyyy-MM-dd hh:mm:ss"));
                ui->taskSpecialTable->setItem(i, 6, new QTableWidgetItem());
                ui->taskSpecialTable->item(i,6)->setData(Qt::DisplayRole , query2.value(6).toInt());
                ui->taskSpecialTable->setItem(i, 7, new QTableWidgetItem());
                ui->taskSpecialTable->item(i,7)->setData(Qt::DisplayRole , query2.value(7).toFloat());
                ui->taskSpecialTable->setItem(i, 8, new QTableWidgetItem());
                ui->taskSpecialTable->item(i,8)->setData(Qt::DisplayRole , query2.value(8).toFloat());
                ui->taskSpecialTable->setItem(i, 9, new QTableWidgetItem());
                ui->taskSpecialTable->item(i,9)->setData(Qt::DisplayRole , query2.value(9).toFloat());
                ui->taskSpecialTable->setItem(i, 10, new QTableWidgetItem());
                ui->taskSpecialTable->item(i,10)->setData(Qt::DisplayRole , query2.value(10).toFloat());
                for(int j = 0; j <  ui->taskSpecialTable->columnCount(); j++)
                {
                    ui->taskSpecialTable->item(i,j)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
                }
            }
            else
            {
                qDebug()<<"有项  赋值";
                ui->taskSpecialTable->item(i,0)->setData(Qt::DisplayRole , query2.value(0).toInt());
                ui->taskSpecialTable->item(i,1)->setData(Qt::DisplayRole , query2.value(1).toInt());
                ui->taskSpecialTable->item(i,2)->setData(Qt::DisplayRole , query2.value(2).toInt());
                ui->taskSpecialTable->item(i,3)->setData(Qt::DisplayRole , query2.value(3).toInt());
                ui->taskSpecialTable->item(i,4)->setData(Qt::DisplayRole , query2.value(4).toDateTime().toString("yyyy-MM-dd hh:mm:ss"));
                ui->taskSpecialTable->item(i,5)->setData(Qt::DisplayRole , query2.value(5).toDateTime().toString("yyyy-MM-dd hh:mm:ss"));
                ui->taskSpecialTable->item(i,6)->setData(Qt::DisplayRole , query2.value(6).toInt());
                ui->taskSpecialTable->item(i,7)->setData(Qt::DisplayRole , query2.value(7).toFloat());
                ui->taskSpecialTable->item(i,8)->setData(Qt::DisplayRole , query2.value(8).toFloat());
                ui->taskSpecialTable->item(i,9)->setData(Qt::DisplayRole , query2.value(9).toFloat());
                ui->taskSpecialTable->item(i,10)->setData(Qt::DisplayRole , query2.value(10).toFloat());
             }


        }

//        if(query2.next() == true)
//        {
//            qDebug()<<"taskid is :"<<query2.value(0).toInt();
//            qDebug()<<"mes_task is :"<<query2.value(1).toInt();
//            qDebug()<<"start_id is :"<<query2.value(2).toInt();
//        }
//        else
//        {
//            QMessageBox::warning(this,"","未找到数据");
//        }
    }
}
