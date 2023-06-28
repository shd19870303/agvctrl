﻿#include "cartrans_action.h"
#include <QSplitter>
#include <QLabel>
#include <QSplitterHandle>
#include <QDebug>

CarTranS_Action::CarTranS_Action(QObject *parent)
    :QWidgetAction(parent)
{
    SpinBox = new QSpinBox;
    SpinBox->setFixedWidth(63);
    label1 = new QLabel;
    void (QSpinBox::*p)(int) = &QSpinBox::valueChanged;
    connect(SpinBox,p,this,&CarTranS_Action::sendValue);
}

QWidget *CarTranS_Action::createWidget(QWidget *parent)
{
    if(parent->inherits("QMenu") || parent->inherits("QToolBar"))
    {
        QSplitter *splitter = new QSplitter(parent);


        label1->setFixedWidth(55);
        splitter->addWidget(label1);
        splitter->addWidget(SpinBox);
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



void CarTranS_Action::setLabel1Text(QString str)
{
    label1->setText(str);
}
