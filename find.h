#ifndef FIND_H
#define FIND_H

#include <QDialog>
#include <QButtonGroup>
#include "ui_find.h"

namespace Ui {
class Find;
}

class Find : public QDialog
{
    Q_OBJECT

public:
    explicit Find(QWidget *parent = nullptr);
    ~Find();

public:
    Ui::Find *ui;
    QButtonGroup *buttonGroup;

public slots:
    void on_radioButton_switch(int id);
};

#endif // FIND_H
