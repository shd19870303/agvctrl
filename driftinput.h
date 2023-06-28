#ifndef DRIFTINPUT_H
#define DRIFTINPUT_H

#include "ui_driftinput.h"

#include <QWidget>

namespace Ui {
class DriftInput;
}

class DriftInput : public QWidget
{
    Q_OBJECT

public:
    explicit DriftInput(QWidget *parent = nullptr);
    ~DriftInput();


signals:
    void coordinateUpdate(int x, int y);

public slots:
    void nextFocus();

public:
    Ui::DriftInput *ui;
};

#endif // DRIFTINPUT_H
