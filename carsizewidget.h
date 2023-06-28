#ifndef CARSIZEWIDGET_H
#define CARSIZEWIDGET_H

#include <QWidget>

namespace Ui {
class CarSizeWidget;
}

class CarSizeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CarSizeWidget(QWidget *parent = nullptr);
    ~CarSizeWidget();

private slots:
    void on_confimButton_clicked();

    void on_initButton_clicked();

private:
    Ui::CarSizeWidget *ui;
signals:
    void sendCarSizeInfo(QString, QString, QString);
};

#endif // CARSIZEWIDGET_H
