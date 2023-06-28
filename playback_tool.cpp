#include "playback_tool.h"
#include <QKeyEvent>
#include <QDebug>
PlayBack_Tool::PlayBack_Tool(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlayBack_Tool)
{
    ui->setupUi(this);
    setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowStaysOnTopHint | Qt::WindowMinimizeButtonHint);
    init_lineEdit();
//    ui->playback_connect_Button->setEnabled(false);
    ui->playback_confirm_Button->setEnabled(false);

    ui->playback_play_Button->setEnabled(false);
    ui->playback_stop_Button->setVisible(false);
    ui->playback_Slider->setEnabled(false);
}

PlayBack_Tool::~PlayBack_Tool()
{
    delete ui;
}

void PlayBack_Tool::init_lineEdit()
{
    QStringList list2;
    list2<<QString("192.168.66.234")<<QString("172.16.115.6")<<QString("172.16.115.7")<<QString("172.16.115.9")<<QString("172.16.251.234")
        <<QString("192.168.1.220")<<QString("192.168.1.223")<<QString("192.168.1.234")<<QString("192.168.8.234")<<QString("192.168.227.140")<<QString("10.217.204.88");
    ui->playback_ip_lineEdit->setUserList(list2);



    QStringList list3;
    list3<<QString("1")<<QString("10")<<QString("100")<<QString("200")<<QString("300")<<QString("500")<<QString("1000")<<QString("2000")<<QString("4000");
    ui->playback_delay_lineEdit->setUserList(list3);
    ui->playback_delay_lineEdit->setCurText(QString("500"));
}

void PlayBack_Tool::keyPressEvent(QKeyEvent *event)
{

}

void PlayBack_Tool::on_playback_connect_Button_clicked()
{
    ui->playback_Slider->setValue(0);
    emit clicked_playback_connect_Button();
}

void PlayBack_Tool::on_playback_play_Button_clicked()
{
    emit clicked_playback_play_Button();
}

void PlayBack_Tool::on_playback_stop_Button_clicked()
{
    emit clicked_playback_stop_Button();
}

void PlayBack_Tool::on_playback_Slider_valueChanged(int value)
{
    emit playback_Slider_valueChanged(value);
}

void PlayBack_Tool::on_playback_confirm_Button_clicked()
{
   emit clicked_playback_confirm_Button();
}
