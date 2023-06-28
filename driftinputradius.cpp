#include "driftinputradius.h"
//#include <QRegExpValidator>
#include <QRegularExpressionValidator>
DriftInputRadius::DriftInputRadius(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DriftInputRadius)
{
    ui->setupUi(this);

//    QRegExp regx("[0-9]+$");
    QRegularExpression regx("[0-9]+$");
//    QValidator* validator = new QRegExpValidator(regx, ui->lineEdit);
    QValidator* validator = new QRegularExpressionValidator(regx, ui->lineEdit);

    ui->lineEdit->setValidator(validator);    // 设置输入框只能输入数字

    connect(ui->lineEdit, SIGNAL(returnPressed()), this, SLOT(nextFocus()));
}

DriftInputRadius::~DriftInputRadius()
{
    delete ui;
}

void DriftInputRadius::nextFocus()
{
    emit radiusUpdate(ui->lineEdit->text().toInt());
}
