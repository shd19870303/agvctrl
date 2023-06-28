#include "mylineeditctrl.h"
#include <QMessageBox>
#include <QDebug>
UserNameCtrl::UserNameCtrl(QWidget *parent):
    QLineEdit(parent)
{
    this->initData();
    this->initConnect();
    this->initDisplay();
    setTipInfo("");

}


UserNameCtrl::~UserNameCtrl()
{

}

void UserNameCtrl::initData()
{
    mainLayout = new QHBoxLayout;
    layout = new QHBoxLayout;
    m_label = new QLabel();
    m_label->resize(0,0);
    m_combo = new QComboBox();
    m_combo->setEditable(true);
    m_combo->resize(80,20);
//    m_combo->setStyleSheet("QComboBox{border: 0px;}""background-color: rgb(139, 139, 139);color: rgb(255, 255, 255);"
//                           "QComboBox::down-arrow{border-image: url(:/images/triangle.png);}"
//                           "QComboBox::down-arrow:hover{border-image: url(:/images/triangle_blue.png);}"
//                           "QComboBox::down-arrow:pressed{border-image: url(:/images/triangle_green.png);}"
//                           "QComboBox::drop-down{subcontrol-origin: padding;subcontrol-position: top right;"
//                           "widthborder-left-width: 0px;border-left-color: gray;border-left-style: solid;border-top-right-radius: 3px;border-bottom-right-radius: 3px;}");
    _bTime = false;
    m_comPleter = nullptr;
}

void UserNameCtrl::initConnect()
{
    connect(m_combo,SIGNAL(currentTextChanged(const QString &)),this,SLOT(slot_TextChanged(const QString &)));
}

void UserNameCtrl::initDisplay()
{
    mainLayout->addWidget(m_label);
    layout->addWidget(m_combo);
    mainLayout->addLayout(layout);
    mainLayout->setStretch(0,0);
    mainLayout->setStretch(1,1);

    setTextMargins(60,0,0,0);
    setContentsMargins(0,0,0,0);
    setLayout(mainLayout);

    m_combo->setCursor(Qt::PointingHandCursor);
}

void UserNameCtrl::setTipInfo(QString sTip)
{
    m_label->setText(sTip);
}

void UserNameCtrl::setUserList(QStringList list)
{
    m_combo->addItems(list);
    if(nullptr != m_comPleter)
    {
        delete m_comPleter;
        m_comPleter = nullptr;
    }
    m_comPleter = new QCompleter(list, this);
    m_comPleter->setCaseSensitivity(Qt::CaseInsensitive);
    m_comPleter->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    m_combo->setCompleter(m_comPleter);
}

void UserNameCtrl::setCurText(QString sText)
{
    if(-1 == m_combo->findText(sText))
    {
        QStringList list;
        list.clear();
        m_combo->addItem(sText);
    }

    m_combo->setCurrentText(sText);
}

QString UserNameCtrl::curText()
{
    return m_combo->currentText();
}

void UserNameCtrl::setEditalbe(bool type)
{
    m_combo->setEditable(type);
}

void UserNameCtrl::slot_TextChanged(const QString &text)
{
    if(text.length() > NameMaxLen)
    {
        m_combo->setCurrentText(text.left(NameMaxLen));
        return ;
    }
    emit sig_TextChanged(text);
}
