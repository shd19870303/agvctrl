#ifndef MYDEBUGWIDGET_H
#define MYDEBUGWIDGET_H

#include <QWidget>
#include "ui_mydebugwidget.h"

namespace Ui {
class MyDebugWidget;
}

class MyDebugWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MyDebugWidget(QWidget *parent = nullptr);
    ~MyDebugWidget();

public:
    Ui::MyDebugWidget *ui;

private slots:
    void on_clearEditButton_clicked();

private:
};

#endif // MYDEBUGWIDGET_H
