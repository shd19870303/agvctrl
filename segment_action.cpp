#include "segment_action.h"
#include <QSplitter>
#include <QLabel>
#include <QDebug>
#include <QHBoxLayout>
Segment_Action::Segment_Action(QObject *parent)
    :QWidgetAction(parent)
{
    LineEdit = new QLineEdit;
    LineEdit->setFixedWidth(50);
    connect(LineEdit,SIGNAL(returnPressed()),this,SLOT(sendText()));
}
QWidget *Segment_Action::createWidget(QWidget *parent)
{
    if(parent->inherits("QMenu") || parent->inherits("QToolBar"))
    {
        QSplitter *splitter = new QSplitter(parent);
        QLabel *label1 = new QLabel;
        QLabel *label2 = new QLabel;
        label1->setText(" 修改线宽:");
        label1->setFixedWidth(55);
        label2->setPixmap(QPixmap(":/images/return.png"));
        splitter->addWidget(label1);
        splitter->addWidget(LineEdit);
        splitter->addWidget(label2);
        QSplitterHandle *splitterHandle1 = splitter->handle(1);
        QSplitterHandle *splitterHandle2 = splitter->handle(2);
        splitterHandle1->setDisabled(true);
        splitterHandle2->setDisabled(true);
        splitter->setFixedWidth(135);
        return splitter;
    }
    else
    {
        return 0;
    }
}

void Segment_Action::sendText()
{
    emit getText(LineEdit->text());
    LineEdit->clear();
}
