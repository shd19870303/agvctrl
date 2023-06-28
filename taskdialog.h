#ifndef TASKDIALOG_H
#define TASKDIALOG_H

#include <QDialog>
#include "ui_taskdialog.h"
namespace Ui {
class taskDialog;
}

class taskDialog : public QDialog
{
    Q_OBJECT

public:
    explicit taskDialog(QWidget *parent = nullptr);
    ~taskDialog();
public:
    Ui::taskDialog *ui;
    bool sendTaskFlag = false;
private slots:
    void on_getTaskButton_clicked();

private:
signals:
    void getTaskData();
};

#endif // TASKDIALOG_H
