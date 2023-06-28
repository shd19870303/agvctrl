#include "floatingwindow.h"
#include <QPainter>
#include <QDebug>
FloatingWindow::FloatingWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FloatingWindow)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_TranslucentBackground);
//    font.setPixelSize(50);                              //字体大小，等同于PointSize的9点大小

//    ui->avoidFrame->setFont(font);
//    ui->frame_2->setFont(font);
}

FloatingWindow::~FloatingWindow()
{
    delete ui;
}

void FloatingWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
//    painter.scale(1,-1);

//    QPen pen;
//    pen.setWidth(3);
//    pen.setColor(Qt::red);
//    painter.setPen(pen);
//    painter.drawLine(0,0,width(),height());

//    QFont font;
//    font.setPointSize(30);
//    painter.setFont(font);

//    painter.drawText(QPoint(rect().center().x()-15,(rect().center().y())),QString("车号:%1").arg(carId));

    painter.fillRect(rect(), QColor(50, 50, 50, 0));
}
