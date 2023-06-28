#ifndef REGISTRYDIALOG_H
#define REGISTRYDIALOG_H

#include <QDialog>
#include <bmachinecontrol.h>

namespace Ui {
class RegistryDialog;
}

class RegistryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegistryDialog(QWidget *parent = nullptr);
    ~RegistryDialog();
    int judgeActivate();
private slots:
    void on_getInfoBtn_clicked();

    void on_activeBtn_clicked();

private:
    Ui::RegistryDialog *ui;
    BMachineControl *m_machine;
public:
    bool accessFlag = false;
};

#endif // REGISTRYDIALOG_H
