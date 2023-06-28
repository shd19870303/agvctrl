#ifndef PLAYBACK_TOOL_H
#define PLAYBACK_TOOL_H

#include <QWidget>
#include "ui_playback_tool.h"

namespace Ui {
class PlayBack_Tool;
}

class PlayBack_Tool : public QWidget
{
    Q_OBJECT

public:
    explicit PlayBack_Tool(QWidget *parent = nullptr);
    ~PlayBack_Tool();
    void init_lineEdit();
    Ui::PlayBack_Tool *ui;



protected:
    void keyPressEvent(QKeyEvent *event);

private slots:
    void on_playback_connect_Button_clicked();

    void on_playback_play_Button_clicked();

    void on_playback_stop_Button_clicked();

    void on_playback_Slider_valueChanged(int value);

    void on_playback_confirm_Button_clicked();

private:
signals:
    void clicked_playback_connect_Button();
    void clicked_playback_play_Button();
    void clicked_playback_stop_Button();
    void clicked_playback_confirm_Button();
    void playback_Slider_valueChanged(int changeValue);
};

#endif // PLAYBACK_TOOL_H
