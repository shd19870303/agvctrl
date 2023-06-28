#ifndef FLOATING_DIALOG_H
#define FLOATING_DIALOG_H

#include <QDialog>

namespace Ui {
class Floating_Dialog;
}

class Floating_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Floating_Dialog(QWidget *parent = nullptr);
    ~Floating_Dialog();
    void set_widgets_info(int dev_id, int device_type, int mission_id, int mission_type, int speed, int state, int status, int error_type, int battery, int timestamp);
protected:
    void paintEvent(QPaintEvent *event);

private:
    Ui::Floating_Dialog *ui;
};

#endif // FLOATING_DIALOG_H
