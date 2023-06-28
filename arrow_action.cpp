#include "arrow_action.h"
#include <QSplitter>
#include <QLabel>
#include <QSplitterHandle>
#include <QDebug>
Arrow_Action::Arrow_Action(QObject *parent)
    :QWidgetAction(parent)
{
    DSpinBox = new QDoubleSpinBox;
    DSpinBox->setFixedWidth(63);
    label1 = new QLabel;
    void (QDoubleSpinBox::*p)(double) = &QDoubleSpinBox::valueChanged;
    connect(DSpinBox,p,this,&Arrow_Action::sendValue);
}

QWidget *Arrow_Action::createWidget(QWidget *parent)
{
    if(parent->inherits("QMenu") || parent->inherits("QToolBar"))
    {
        QSplitter *splitter = new QSplitter(parent);

//        label1->setText("箭头高度:");
        label1->setFixedWidth(55);
        splitter->addWidget(label1);
        splitter->addWidget(DSpinBox);
        QSplitterHandle *splitterHandle1 = splitter->handle(1);
        splitterHandle1->setDisabled(true);
        splitter->setFixedWidth(135);


        return splitter;
    }
    else
    {
        return 0;
    }
}



void Arrow_Action::setLabel1Text(QString str)
{
    label1->setText(str);
}
