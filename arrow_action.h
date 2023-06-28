#ifndef ARROW_ACTION_H
#define ARROW_ACTION_H

#include <QWidgetAction>
#include <QDoubleSpinBox>
#include <QLabel>
class Arrow_Action : public QWidgetAction
{
    Q_OBJECT
public:
    explicit Arrow_Action(QObject *parent = 0);
protected:
    QWidget * createWidget(QWidget * parent);
signals:
    void send_arrow_float(float);
public:
    void sendValue(double arrow_flaot)
    {
        emit send_arrow_float(arrow_flaot);
    }
    void setLabel1Text(QString str);
public:
    QDoubleSpinBox *DSpinBox;
    QLabel *label1;
};

#endif // ARROW_ACTION_H
