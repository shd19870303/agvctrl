#include "moveitem.h"
//#include<QRegExpValidator>
#include <QRegularExpressionValidator>
MoveItem::MoveItem(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MoveItem)
{
    ui->setupUi(this);

//    QRegExp regx("-?[0-9]+$");
    QRegularExpression regx("-?[0-9]+$");
//    QValidator* validator = new QRegExpValidator(regx, ui->lineEdit_dx);
    QValidator* validator = new QRegularExpressionValidator(regx, ui->lineEdit_dx);
    ui->lineEdit_dx->setValidator(validator);    // 设置输入框只能输入数字

    validator = new QRegularExpressionValidator(regx, ui->lineEdit_dy);
    ui->lineEdit_dy->setValidator(validator);
    ui->lineEdit_dx->setFocus();

    Qt::WindowFlags flags=Qt::Dialog;
    flags |=Qt::WindowCloseButtonHint;
    setWindowFlags(flags);
}

MoveItem::~MoveItem()
{
    delete ui;
}
