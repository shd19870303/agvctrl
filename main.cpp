#include "mainwindow.h"

#include <QApplication>
//#include <QDesktopWidget>
#include <QTextCodec>
#include <QMessageBox>
#include <QString>
int main(int argc, char *argv[])
{

//    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
//    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    //qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "1");
    QApplication a(argc, argv);

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
//    QApplication::setStyle(QStyleFactory::create("windows"));
    MainWindow w;
    w.setWindowTitle("AgvMonitor");
    w.showMaximized();
    w.show();
    //w.showMaximized();

    w.config_open();
    if(!QProcess::startDetached("./AutoUpdate.exe",QStringList() ))
    {
//        QMessageBox::warning(&w,tr("警告信息"),tr("   检测更新程序启动失败！ "));
//        QMessageBox::warning(&w,QString("警告信息"),QString("   检测更新程序启动失败！ "));
    }



    return a.exec();
}
