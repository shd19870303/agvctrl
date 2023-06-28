#ifndef CARTRANS_ACTION_H
#define CARTRANS_ACTION_H

#include <QWidgetAction>
#include <QSpinBox>
#include <QLabel>
class CarTranS_Action : public QWidgetAction
{
    Q_OBJECT
public:
    explicit CarTranS_Action(QObject *parent = 0);
protected:
    QWidget * createWidget(QWidget * parent);
signals:
    void send_trans(int);
public:
    void sendValue(int trans)
    {
        emit send_trans(trans);
    }
    void setLabel1Text(QString str);
public:
    QSpinBox *SpinBox;
    QLabel *label1;
};

#endif // CARTRANS_ACTION_H


