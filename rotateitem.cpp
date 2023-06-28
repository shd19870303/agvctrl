#include "rotateitem.h"
//#include <QRegExpValidator>
#include <QRegularExpressionValidator>
RotateItem::RotateItem(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RotateItem)
{
    ui->setupUi(this);
//    QRegExp regx("-?[0-9]+$");
    QRegularExpression regx("-?[0-9]+$");
    QValidator* validator = new QRegularExpressionValidator(regx, ui->lineEdit_x);
    ui->lineEdit_x->setValidator(validator);    // 设置输入框只能输入数字

    validator = new QRegularExpressionValidator(regx, ui->lineEdit_y);
    ui->lineEdit_y->setValidator(validator);

//    QRegExp regx2("-?[0-9\.]+$");
    QRegularExpression regx2("-?[0-9\.]+$");
    validator = new QRegularExpressionValidator(regx2, ui->lineEdit_angle);
    ui->lineEdit_angle->setValidator(validator);

    ui->lineEdit_x->setFocus();

    Qt::WindowFlags flags=Qt::Dialog;
    flags |=Qt::WindowCloseButtonHint;
    setWindowFlags(flags);
}

RotateItem::~RotateItem()
{
    delete ui;
}
