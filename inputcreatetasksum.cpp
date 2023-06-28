#include "inputcreatetasksum.h"
#include "ui_inputcreatetasksum.h"
//#include <QRegExpValidator>
#include <QRegularExpressionValidator>
#include <QCheckBox>
#include <QVBoxLayout>

InputCreateTaskSum::InputCreateTaskSum(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InputCreateTaskSum)
{
    ui->setupUi(this);

//    QRegExp regx("[0-9]+$");
    QRegularExpression regx("[0-9]+$");
//    QValidator* validator = new QRegExpValidator(regx, ui->lineEdit);
    QValidator* validator = new QRegularExpressionValidator(regx, ui->lineEdit);
    ui->lineEdit->setValidator(validator);    // 设置输入框只能输入数字

    QVBoxLayout *vbox = new QVBoxLayout(this);
    QCheckBox *checkBox1 = new QCheckBox("A");
    QCheckBox *checkBox2 = new QCheckBox("B");
    vbox->addWidget(checkBox1);
    vbox->addWidget(checkBox2);
    for(int i = 0; i < 6; i++)
    {
        QCheckBox *checkBox3 = new QCheckBox("A");
        vbox->addWidget(checkBox3);
    }
    //ui->groupBox->setLayout(vbox);
    ui->widget->setLayout(vbox);
}

InputCreateTaskSum::~InputCreateTaskSum()
{
    delete ui;
}

int InputCreateTaskSum::getInputNum()
{
    return ui->lineEdit->text().toInt();
}
