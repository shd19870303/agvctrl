#include "driftinput.h"
#include <QDebug>
//#include <QRegExpValidator>
#include <QRegularExpressionValidator>
DriftInput::DriftInput(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DriftInput)
{
    ui->setupUi(this);
    //setAttribute(Qt::WA_TranslucentBackground, true);

//    QRegExp regx("-?[0-9]+$");
    QRegularExpression regx("-?[0-9]+$");
//    QValidator* validator = new QRegExpValidator(regx, ui->lineEdit_x);
    QValidator* validator = new QRegularExpressionValidator(regx, ui->lineEdit_x);
    ui->lineEdit_x->setValidator(validator);    // 设置输入框只能输入数字
    ui->lineEdit_y->setValidator(validator);    // 设置输入框只能输入数字
    ui->lineEdit_y->setEnabled(false);

    connect(ui->lineEdit_x, SIGNAL(returnPressed()), this, SLOT(nextFocus()));
    connect(ui->lineEdit_y, SIGNAL(returnPressed()), this, SLOT(nextFocus()));
}

DriftInput::~DriftInput()
{
    delete ui;
}


void DriftInput::nextFocus()
{
    QLineEdit *lineEdit = (QLineEdit *)sender();
    if (lineEdit == ui->lineEdit_x)
    {
        ui->lineEdit_y->setEnabled(true);
        ui->lineEdit_y->setFocus();
        ui->lineEdit_y->selectAll();
        ui->lineEdit_x->setEnabled(false);

    }
    else if (lineEdit == ui->lineEdit_y)
    {
        ui->lineEdit_x->setEnabled(true);
        ui->lineEdit_x->setFocus();
        ui->lineEdit_x->selectAll();
        ui->lineEdit_y->setEnabled(false);
        emit coordinateUpdate(ui->lineEdit_x->text().toInt(),ui->lineEdit_y->text().toInt());
    }
}
