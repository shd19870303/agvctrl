#ifndef POINTSIZE_ACTION_H
#define POINTSIZE_ACTION_H

#include <QWidgetAction>
#include <QSpinBox>
#include <QLabel>
class PointSize_Action : public QWidgetAction
{
    Q_OBJECT
public:
    explicit PointSize_Action(QObject *parent = 0);
protected:
    QWidget * createWidget(QWidget * parent);
signals:
    void send_pointSize(int);
public:
    void sendValue(int trans)
    {
        emit send_pointSize(trans);
    }
    void setLabel1Text(QString str);
public:
    QSpinBox *SpinBox;
    QLabel *label1;
};


#endif // POINTSIZE_ACTION_H


