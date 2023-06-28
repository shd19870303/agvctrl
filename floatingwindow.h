#ifndef FLOATINGWINDOW_H
#define FLOATINGWINDOW_H

#include <QWidget>
#include "ui_floatingwindow.h"
namespace Ui {
class FloatingWindow;
}

class FloatingWindow : public QWidget
{
    Q_OBJECT

public:
    explicit FloatingWindow(QWidget *parent = nullptr);
    ~FloatingWindow();

    Ui::FloatingWindow *ui;
    int carId = -1;
    int mission_id = -1;
private:
    QFont font;
protected:
    void paintEvent(QPaintEvent *event);
};

#endif // FLOATINGWINDOW_H
