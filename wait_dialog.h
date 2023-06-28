#ifndef WAIT_DIALOG_H
#define WAIT_DIALOG_H

#include <QDialog>
#include "ui_wait_dialog.h"
namespace Ui {
class Wait_Dialog;
}

class Wait_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Wait_Dialog(QWidget *parent = nullptr);
    ~Wait_Dialog();
    Ui::Wait_Dialog *ui;
private:
protected:
    void paintEvent(QPaintEvent *event);
};

#endif // WAIT_DIALOG_H
