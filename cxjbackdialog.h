#ifndef CXJBACKDIALOG_H
#define CXJBACKDIALOG_H

#include <QDialog>
#include "ui_cxjbackdialog.h"

namespace Ui {
class cxjBackDialog;
}

class cxjBackDialog : public QDialog
{
    Q_OBJECT

public:
    explicit cxjBackDialog(QWidget *parent = nullptr);
    ~cxjBackDialog();
signals:
    void getCxjBack();
public:
    Ui::cxjBackDialog *ui;
private slots:
    void on_getCxjBackButton_clicked();
};

#endif // CXJBACKDIALOG_H
