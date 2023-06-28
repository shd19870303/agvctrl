#include "wait_dialog.h"
#include <QPainter>

Wait_Dialog::Wait_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Wait_Dialog)
{
    ui->setupUi(this);
//    setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint | Qt::WindowStaysOnTopHint | Qt::WindowMinimizeButtonHint | Qt::FramelessWindowHint);
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint );

    setModal(true);
    setAttribute(Qt::WA_TranslucentBackground);
}

Wait_Dialog::~Wait_Dialog()
{
    delete ui;
}

void Wait_Dialog::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    // QColor最后一个参数代表alpha通道，一般用作透明度
    painter.fillRect(rect(), QColor(50, 50, 50, 0));
}
