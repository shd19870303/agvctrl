#ifndef TASKSPECIALDIALOG_H
#define TASKSPECIALDIALOG_H

#include <QDialog>
#include "ui_taskspecialdialog.h"
#include <QSqlRecord>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QMessageBox>
#include <QDebug>
namespace Ui {
class taskSpecialDialog;
}

class taskSpecialDialog : public QDialog
{
    Q_OBJECT

public:
    explicit taskSpecialDialog(QWidget *parent = nullptr);
    ~taskSpecialDialog();
public:
        Ui::taskSpecialDialog *ui;
        QSqlDatabase getTaskDBName_db;
        QSqlDatabase getTaskInfo_db;

        QString taskDBName;
private slots:
        void on_queryButton_clicked();

private:

};

#endif // TASKSPECIALDIALOG_H
