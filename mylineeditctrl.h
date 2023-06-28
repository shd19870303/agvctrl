#ifndef MYLINEEDITCTRL_H
#define MYLINEEDITCTRL_H


#include <QLineEdit>
#include <QHBoxLayout>
#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QCompleter>

#define NameMaxLen 20

class UserNameCtrl : public QLineEdit
{
    Q_OBJECT
signals:
    void sig_TextChanged(const QString &sName);

public:
    explicit UserNameCtrl(QWidget *parent = nullptr);
    ~UserNameCtrl();

public:
    void setTipInfo(QString sTip);
    void setUserList(QStringList list);
    void setCurText(QString sText);
    QString curText();
    void setEditalbe(bool type);
private slots:
    void slot_TextChanged(const QString &text);

private:
    void initData();
    void initConnect();
    void initDisplay();

private:
    QLabel *m_label;
    QComboBox *m_combo;
    QHBoxLayout *mainLayout;
    QHBoxLayout *layout;
    QCompleter *m_comPleter;

private:
    bool _bTime;
};

#endif // MYLINEEDITCTRL_H
