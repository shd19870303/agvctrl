#ifndef UPDATESTATION_H
#define UPDATESTATION_H

#include <QDialog>

namespace Ui {
class UpdateStation;
}

class UpdateStation : public QDialog
{
    Q_OBJECT

public:
    explicit UpdateStation(QWidget *parent = nullptr);
    ~UpdateStation();

    QString getInputIP();

private:
    Ui::UpdateStation *ui;
};

#endif // UPDATESTATION_H
