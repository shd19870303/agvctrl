#ifndef BATTERYINFO_WIDGET_H
#define BATTERYINFO_WIDGET_H

#include <QWidget>
#include "ui_batteryinfo_widget.h"
namespace Ui {
class BatteryInfo_Widget;
}

class BatteryInfo_Widget : public QWidget
{
    Q_OBJECT

public:
    explicit BatteryInfo_Widget(QWidget *parent = nullptr);
    ~BatteryInfo_Widget();

private slots:
    void on_batteryInfoButton_clicked();

    void on_batteryTableWidget_cellDoubleClicked(int row, int column);


public:
    Ui::BatteryInfo_Widget *ui;

signals:
    void getBatteryData();
    void getBatteryDetailData(int);
};

#endif // BATTERYINFO_WIDGET_H
