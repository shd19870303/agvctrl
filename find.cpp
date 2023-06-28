#include "find.h"
#include <QRegExpValidator>
#include <QDebug>

Find::Find(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Find)
{
    ui->setupUi(this);
    buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(ui->radioButton_Point,0);
    buttonGroup->addButton(ui->radioButton_Segment,1);
//    buttonGroup->addButton(ui->radioButton_Area,2);
    buttonGroup->addButton(ui->radioButton_AGV,3);

    QRegExp regx("[A-Z0-9]+$");
    QValidator* validator = new QRegExpValidator(regx, ui->lineEdit_id);
    ui->lineEdit_id->setValidator(validator);    // 设置输入框只能输入数字
    ui->lineEdit_id->setFocus();

    Qt::WindowFlags flags=Qt::Dialog;
    flags |=Qt::WindowCloseButtonHint;
    setWindowFlags(flags);

    connect(buttonGroup, SIGNAL(buttonClicked(int )), this, SLOT(on_radioButton_switch(int )));
}

Find::~Find()
{
    delete ui;
}

void Find::on_radioButton_switch(int id)
{
    if(id == 0)
    {
        ui->label->setText(tr("请输入要查询的Id或Name："));
    }
    else if(id == 1)
    {
        ui->label->setText(tr("请输入要查询的Id："));
    }
    else if(id == 2)
    {
        ui->label->setText(tr("请输入要查询的Type："));
    }
    else if(id == 3)
    {
        ui->label->setText(tr("请输入要查询的Id："));
    }
}
