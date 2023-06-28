#ifndef INPUTCREATETASKSUM_H
#define INPUTCREATETASKSUM_H

#include <QDialog>

namespace Ui {
class InputCreateTaskSum;
}

class InputCreateTaskSum : public QDialog
{
    Q_OBJECT

public:
    explicit InputCreateTaskSum(QWidget *parent = nullptr);
    ~InputCreateTaskSum();

    int getInputNum();

private:
    Ui::InputCreateTaskSum *ui;
};

#endif // INPUTCREATETASKSUM_H
