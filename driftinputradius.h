#ifndef DRIFTINPUTRADIUS_H
#define DRIFTINPUTRADIUS_H

#include <QWidget>
#include "ui_driftinputradius.h"

namespace Ui {
class DriftInputRadius;
}

class DriftInputRadius : public QWidget
{
    Q_OBJECT

public:
    explicit DriftInputRadius(QWidget *parent = nullptr);
    ~DriftInputRadius();

public:
    Ui::DriftInputRadius *ui;

signals:
    void radiusUpdate(int param);

public slots:
    void nextFocus();
};

#endif // DRIFTINPUTRADIUS_H
