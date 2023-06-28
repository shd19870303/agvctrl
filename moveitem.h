#ifndef MOVEITEM_H
#define MOVEITEM_H
#include "ui_moveitem.h"
#include <QDialog>

namespace Ui {
class MoveItem;
}

class MoveItem : public QDialog
{
    Q_OBJECT

public:
    explicit MoveItem(QWidget *parent = nullptr);
    ~MoveItem();

public:
    Ui::MoveItem *ui;
};

#endif // MOVEITEM_H
