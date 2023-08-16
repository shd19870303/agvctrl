#ifndef ADDTASKVIEW_H
#define ADDTASKVIEW_H

#include <QWidget>
#include <QStandardItemModel>
#include <QDebug>

namespace Ui {
class AddTaskView;
}

class AddTaskView : public QWidget
{
    Q_OBJECT

public:
    explicit AddTaskView(QWidget *parent = nullptr);
    ~AddTaskView();

    QStandardItemModel * add_task_item_model;

signals:
    void addTaskManualSignal(int startSatId, int endSatId, int cntDown);

private slots:
    void on_pushBtn_add_clicked();

    void on_pushBtn_addToList_clicked();

    void on_pushBtn_del_clicked();

    void on_pushButton_open_clicked();

    void on_pushButton_save_clicked();

private:
    Ui::AddTaskView *ui;
};

#endif // ADDTASKVIEW_H
