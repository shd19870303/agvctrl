#ifndef ROTATEITEM_H
#define ROTATEITEM_H
#include "ui_rotateitem.h"
#include <QDialog>

namespace Ui {
class RotateItem;
}

class RotateItem : public QDialog
{
    Q_OBJECT

public:
    explicit RotateItem(QWidget *parent = nullptr);
    ~RotateItem();

public:
    Ui::RotateItem *ui;
};

#endif // ROTATEITEM_H
