#ifndef CXJDATADIALOG_H
#define CXJDATADIALOG_H

#include <QDialog>
#include "ui_cxjdatadialog.h"
namespace Ui {
class cxjDataDialog;
}

class cxjDataDialog : public QDialog
{
    Q_OBJECT

public:
    explicit cxjDataDialog(QWidget *parent = nullptr);
    ~cxjDataDialog();
public:
        Ui::cxjDataDialog *ui;
private slots:
    void on_getCxjDataButton_clicked();
signals:
    void getCxjData();
private:

};

#endif // CXJDATADIALOG_H
