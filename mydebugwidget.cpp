#include "mydebugwidget.h"

MyDebugWidget::MyDebugWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyDebugWidget)
{
    ui->setupUi(this);
    setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowStaysOnTopHint | Qt::WindowMinimizeButtonHint);

}

MyDebugWidget::~MyDebugWidget()
{
    delete ui;
}

void MyDebugWidget::on_clearEditButton_clicked()
{
    ui->textEditDebug->clear();
}
