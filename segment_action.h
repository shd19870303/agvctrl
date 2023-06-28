#ifndef SEGMENT_ACTION_H
#define SEGMENT_ACTION_H

#include <QWidgetAction>
#include <QLineEdit>
//class QLinkeList;
class Segment_Action : public QWidgetAction
{
    Q_OBJECT
public:
    explicit Segment_Action(QObject *parent = 0);
protected:
    QWidget * createWidget(QWidget * parent);
signals:
    void getText(const QString &string);
private slots:
    void sendText();
private:
    QLineEdit *LineEdit;
};

#endif // SEGMENT_ACTION_H
