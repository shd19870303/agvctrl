#include "addtaskview.h"
#include "ui_addtaskview.h"
#include <QFileDialog>
#include <QMessageBox>

AddTaskView::AddTaskView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddTaskView)
{
    ui->setupUi(this);
    add_task_item_model = new QStandardItemModel(this);
    ui->tableView->setModel(add_task_item_model);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    QStringList column;
    column << "Start_Id" << "End_Id" << "cntDown";
    add_task_item_model->setHorizontalHeaderLabels(column);
}

AddTaskView::~AddTaskView()
{
    delete ui;
}

void AddTaskView::on_pushBtn_add_clicked()
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

void AddTaskView::on_pushBtn_addToList_clicked()
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

void AddTaskView::on_pushBtn_del_clicked()
{
    int current = ui->tableView->currentIndex().row();
    if(current >= 0)
    {
        add_task_item_model->removeRow(current);
    }
}

void AddTaskView::on_pushButton_open_clicked()
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

void AddTaskView::on_pushButton_save_clicked()
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
                             .arg(add_task_item_model->data(add_task_item_model->index(i,2)).toInt()) << '\n';
            }
        }

        file.close();

    }
}
