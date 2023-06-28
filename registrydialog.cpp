#include "registrydialog.h"
#include "ui_registrydialog.h"
#include <QMessageBox>
RegistryDialog::RegistryDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegistryDialog)
{
    ui->setupUi(this);
    m_machine = new BMachineControl();


}

RegistryDialog::~RegistryDialog()
{
    delete ui;
}

int RegistryDialog::judgeActivate()
{
   return  m_machine->judgeKey();
}

void RegistryDialog::on_getInfoBtn_clicked()
{
    QString info = m_machine->getInfo();
    ui->textEdit->setText(info);
}

void RegistryDialog::on_activeBtn_clicked()
{
    QString succesStr;
    bool ret = m_machine->activeKey( ui->lineEdit->text() );

    if(m_machine->surplusDays == 1000000)
    {
         succesStr = QString("激活成功 !  已永久激活 ");
    }
    else
    {
         succesStr = QString("激活成功 !  剩余时间为:  %1 天").arg(m_machine->surplusDays);
    }

    if (ret) {
        QMessageBox::information(nullptr, " ", succesStr , QMessageBox::Ok);
        accessFlag = true;
        close();
    } else {
        QMessageBox::information(nullptr, " ", "激活失败, 请重新激活 !", QMessageBox::Ok);
        accessFlag = false;
        close();
    }
}
