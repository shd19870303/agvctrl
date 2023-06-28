#include "mainwindow.h"
#include <QtWidgets>
#include "find.h"
#include "moveitem.h"
#include "rotateitem.h"
#include <QPushButton>
#include "car.h"
#include "car_info.h"
#include <QLabel>
#include <QSettings>
#include <QColorDialog>
#include "drawscene.h"
#include <QFontDialog>
#include "drawview.h"
#include "mydebugwidget.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    qDebug() << QSqlDatabase::drivers();


    findDialog = new Find(this);
    connect(findDialog,&Find::finished,this,&MainWindow::findDialogSlot);

    statusBarLabel = new QLabel;
    statusBarLabel->setAlignment(Qt::AlignCenter);
    statusBarLabel->setMinimumSize(statusBarLabel->sizeHint());
//    statusBarLabel->adjustSize();


    QWidget * statusWidget = new QWidget();
    QHBoxLayout *statusLay = new QHBoxLayout();
//    statusLay->addStretch(0);
    statusLay->addWidget(statusBarLabel);
    statusLay->addStretch(1);

    statusWidget->setLayout(statusLay);

    statusBar()->addPermanentWidget(statusWidget);

//    statusBar()->addPermanentWidget(statusBarLabel);

    statusBar()->setStyleSheet(QString("QStatusBar::item{border: 0px}"));


    ParameterInit();

    if(skin.isEmpty())
    {

    }
    else if(skin == "DK")
    {
        QFile f(":qdarkstyle/dark/style.qss");

        if (!f.exists())   {
            QMessageBox::information(this," ","请联系工作人员获取皮肤");
        }
        else   {
            f.open(QFile::ReadOnly | QFile::Text);
            QTextStream ts(&f);
            qApp->setStyleSheet(ts.readAll());
        }
        f.close();
    }
    else if(skin == "Lt")
    {
        QFile f(":qdarkstyle/light/style.qss");

        if (!f.exists())   {
            QMessageBox::information(this," ","请联系工作人员获取皮肤");
        }
        else   {
            f.open(QFile::ReadOnly | QFile::Text);
            QTextStream ts(&f);
            qApp->setStyleSheet(ts.readAll());
        }
        f.close();
    }
    else if(skin == "Ey")
    {
        qApp->setStyleSheet("");
        QApplication::setStyle(QStyleFactory::create("windowsvista"));
    }
    else if(skin == "Ro")
    {
        qApp->setStyleSheet("");
        QApplication::setStyle(QStyleFactory::create("windows"));
    }
    else if(skin == "Fn")
    {
        qApp->setStyleSheet("");
        QApplication::setStyle(QStyleFactory::create("fusion"));
    }


    registryDialog = new RegistryDialog;
    int judgeRet = registryDialog->judgeActivate();
    if(judgeRet < 0)//未激活
    {
         registryDialog->exec();
    }
    else
    {
        registryDialog->accessFlag = true;
    }


    if(registryDialog->accessFlag == false)
    {
        qDebug()<<"未激活，即将退出";
        exit(0);
    }



    mdiArea = new QMdiArea;
    mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea->setViewMode(QMdiArea::TabbedView); //Tab多页显示模式
    //mdiArea->setTabShape(QTabWidget::Triangular);//设置tab的形状，默认长方形，
    mdiArea->setTabsClosable(true); //默认否，设为true时，tab上方形成一个关闭小按钮
    mdiArea->setDocumentMode(true);  // 设置文档模式
    mdiArea->setTabsMovable(true);
    //mdiArea->setStyleSheet("QTabBar::tab{height:100;width:120}");
    //mdiArea->setStyleSheet("QMdiSubWindow:title{width:120}");
    setCentralWidget(mdiArea);   

//    setWindowTitle(tr("QTrafficControl1.0"));
    setUnifiedTitleAndToolBarOnMac(true);

    undoGroup = new QUndoGroup(this);

    if(speedDM.isEmpty())
    {
        speedMDFlag = 0;
    }
    else
    {
        speedMDFlag = speedDM.toInt();
    }

    createActions();
    createMenus();
    createToolbars();

    mdiArea->tileSubWindows();

    connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow *)), this, SLOT(subWidgetSwitched(QMdiSubWindow *)));

    resize(1400,800);

//    subwidget_point = activeMdiChild();
//    config_open();

//    if(!QProcess::startDetached("./AutoUpdate.exe",QStringList() ))
//    {
//        QMessageBox::warning(this,tr("警告信息"),tr("   检测更新程序启动失败！ "));
//    }


}

MainWindow::~MainWindow()
{
}

void MainWindow::createActions()
{
//    newAct = new QAction(QIcon(":/icons/new.png"),tr("&New"),this);
//    newAct->setShortcuts(QKeySequence::New);
//    newAct->setStatusTip(tr("Create a new file"));
//    connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

    openAct = new QAction(QIcon(":/icons/open.png"),tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

//    saveAct = new QAction(QIcon(":/icons/save.png"),tr("&Save"), this);
//    saveAct->setShortcuts(QKeySequence::Save);
//    saveAct->setStatusTip(tr("Save the document to disk"));
//    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

//    exitAct = new QAction(QIcon(":/icons/exit.png"),tr("E&xit"), this);
//    exitAct->setShortcuts(QKeySequence::Quit);
//    exitAct->setStatusTip(tr("Exit the application"));
    //connect(exitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));

    closeAct = new QAction(tr("Cl&ose"), this);
    closeAct->setStatusTip(tr("Close the active window"));
    //connect(closeAct, SIGNAL(triggered()),mdiArea, SLOT(closeActiveSubWindow()));

    closeAllAct = new QAction(tr("Close &All"), this);
    closeAllAct->setStatusTip(tr("Close all the windows"));
    //connect(closeAllAct, SIGNAL(triggered()),mdiArea, SLOT(closeAllSubWindows()));

    tileAct = new QAction(tr("&Tile"), this);
    tileAct->setStatusTip(tr("Tile the windows"));
    //connect(tileAct, SIGNAL(triggered()), mdiArea, SLOT(tileSubWindows()));

    cascadeAct = new QAction(tr("&Cascade"), this);
    cascadeAct->setStatusTip(tr("Cascade the windows"));
    //connect(cascadeAct, SIGNAL(triggered()), mdiArea, SLOT(cascadeSubWindows()));

    nextAct = new QAction(tr("Ne&xt"), this);
    nextAct->setShortcuts(QKeySequence::NextChild);
    nextAct->setStatusTip(tr("Move the focus to the next window"));
    //connect(nextAct, SIGNAL(triggered()),mdiArea, SLOT(activateNextSubWindow()));

    previousAct = new QAction(tr("Pre&vious"), this);
    previousAct->setShortcuts(QKeySequence::PreviousChild);
    previousAct->setStatusTip(tr("Move the focus to the previous "
                                 "window"));
    //connect(previousAct, SIGNAL(triggered()),mdiArea, SLOT(activatePreviousSubWindow()));

    separatorAct = new QAction(this);
    separatorAct->setSeparator(true);

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    //connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    groupAct        = new QAction(QIcon(":/icons/group.png"),tr("group"),this);
    unGroupAct        = new QAction(QIcon(":/icons/ungroup.png"),tr("ungroup"),this);

    //connect(groupAct,SIGNAL(triggered()),this,SLOT(on_group_triggered()));
    //connect(unGroupAct,SIGNAL(triggered()),this,SLOT(on_unGroup_triggered()));

    //create draw actions
    selectAct = new QAction(QIcon(":/icons/click.png"),tr("选择"),this);
    selectAct->setCheckable(true);
    lineAct = new QAction(QIcon(":/icons/line.png"),tr("直线"),this);
    lineAct->setCheckable(true);
    arcAct = new QAction(QIcon(":/icons/arc.png"),tr("弧线"),this);
    arcAct->setCheckable(true);
    pointAct = new QAction(QIcon(":/icons/point.png"),tr("点"),this);
    pointAct->setCheckable(true);
    polylineAct = new QAction(QIcon(":/icons/polyline.png"),tr("多段线"),this);
    polylineAct->setCheckable(true);
    polygonAct = new QAction(QIcon(":/icons/polygon.png"),tr("多变形"),this);
    polygonAct->setCheckable(true);
    bezierAct= new QAction(QIcon(":/icons/bezier.png"),tr("贝塞尔"),this);
    bezierAct->setCheckable(true);

    rotateAct = new QAction(QIcon(":/icons/rotate.png"),tr("旋转"),this);
    connect(rotateAct,SIGNAL(triggered()),this,SLOT(rotateItems()));

    interrputAct = new QAction(QIcon(":/icons/interrupt.png"),tr("打断"),this);
    interrputAct->setCheckable(true);
    mergeAct = new QAction(QIcon(":/icons/merge.png"),tr("合并"),this);
    mergeAct->setCheckable(true);
    moveAct = new QAction(QIcon(":/icons/move.png"),tr("平移"),this);
    connect(moveAct,SIGNAL(triggered()),this,SLOT(moveItems()));
    findAct = new QAction(QIcon(":/icons/find.png"),tr("查找"),this);
    findAct->setShortcut(tr("ctrl+f"));
    connect(findAct,SIGNAL(triggered()),this,SLOT(find()));
    measureAct = new QAction(QIcon(":/icons/measure.png"),tr("测距"),this);
    measureAct->setShortcut(tr("ctrl+l"));
    measureAct->setCheckable(true);
    updateAct = new QAction(QIcon(":/icons/update.png"),tr("更新辅助点"),this);
    connect(updateAct,SIGNAL(triggered()),this,SLOT(update()));
    gridAct = new QAction(QIcon(":/icons/grid.png"),tr("显示网格"),this);
    gridAct->setCheckable(true);
    connect(gridAct,SIGNAL(triggered(bool )),this,SLOT(showGrid(bool )));

    arcAct_three_point = new QAction(QIcon(":/icons/arc/three_point.png"),tr("三点"),this);
    arcAct_three_point->setCheckable(true);
    arcAct_start_cen_angle = new QAction(QIcon(":/icons/arc/start_cen_angle.png"),tr("起点，圆心，角度"),this);
    arcAct_start_cen_angle->setCheckable(true);
    arcAct_start_cen_end = new QAction(QIcon(":/icons/arc/start_cen_end.png"),tr("起点，圆心，端点"),this);
    arcAct_start_cen_end->setCheckable(true);
    arcAct_start_cen_length = new QAction(QIcon(":/icons/arc/start_cen_length.png"),tr("起点，圆心，长度"),this);
    arcAct_start_cen_length->setCheckable(true);
    arcAct_start_end_angle = new QAction(QIcon(":/icons/arc/start_end_angle.png"),tr("起点，端点，角度"),this);
    arcAct_start_end_angle->setCheckable(true);
    arcAct_start_end_direct = new QAction(QIcon(":/icons/arc/start_end_direct.png"),tr("起点，端点，方向"),this);
    arcAct_start_end_direct->setCheckable(true);
    arcAct_start_end_radius = new QAction(QIcon(":/icons/arc/start_end_radius.png"),tr("起点，端点，半径"),this);
    arcAct_start_end_radius->setCheckable(true);
    arcAct_cen_start_angle = new QAction(QIcon(":/icons/arc/cen_start_angle.png"),tr("圆心，起点，角度"),this);
    arcAct_cen_start_angle->setCheckable(true);
    arcAct_cen_start_end = new QAction(QIcon(":/icons/arc/cen_start_end.png"),tr("圆心，起点，端点"),this);
    arcAct_cen_start_end->setCheckable(true);
    arcAct_cen_start_length = new QAction(QIcon(":/icons/arc/cen_start_length.png"),tr("圆心，起点，长度"),this);
    arcAct_cen_start_length->setCheckable(true);
    arcAct_continu = new QAction(QIcon(":/icons/arc/continu.png"),tr("连续"),this);
    arcAct_continu->setCheckable(true);

    whole_mapAct = new QAction(QIcon(":/images/whole_map.png"),tr("总览"),this);
    connect(whole_mapAct,&QAction::triggered,this,&MainWindow::whole_map_slot);

//    carTransparency_Act = new QAction(tr("[开/关]车透明"),this);
    carTransparency_Act = new QAction(QIcon(":/icons/agvtm.png"),tr("AGV显示开关"),this);
    connect(carTransparency_Act,&QAction::triggered,this,&MainWindow::carTransparency_slot);

    station_background_Act = new QAction(tr("[开/关]区域编号"),this);
    connect(station_background_Act,&QAction::triggered,this,&MainWindow::station_background_slot);

    scene_color_Act= new QAction(QIcon(":/images/palette.png"),tr("背景色"),this);
    connect(scene_color_Act,&QAction::triggered,this,&MainWindow::change_scene_color);

    route_color_Act= new QAction(QIcon(":/images/palette.png"),tr("路径颜色"),this);
    connect(route_color_Act,&QAction::triggered,this,&MainWindow::change_route_color);

    point_font_Act= new QAction(QIcon(":/images/font.png"),tr("站点字体"),this);
    connect(point_font_Act,&QAction::triggered,this,&MainWindow::change_point_font);

    control_pointfont = new QAction(QIcon(":/icons/idhide.png"),tr("站点名称显示"),this);
    control_pointfont->setCheckable(true);
    connect(control_pointfont,&QAction::triggered,this,&MainWindow::pointfont_control);

    control_floatwindow = new QAction(tr("[开/关]浮动信息"),this);
    connect(control_floatwindow,&QAction::triggered,this,&MainWindow::floatwindow_control);

    control_saBackground = new QAction(tr("[开/关]SA背景图"),this);
    connect(control_saBackground,&QAction::triggered,this,&MainWindow::saBackground_control);

    control_errorInfoHint = new QAction(tr("[开/关]报警信息提醒"),this);
    connect(control_errorInfoHint,&QAction::triggered,this,&MainWindow::errorInfoHint_control);

    control_propertyList = new QAction(QIcon(":/icons/attr.png"),tr("隐藏属性列表"),this);
    control_propertyList->setCheckable(true);
    connect(control_propertyList,&QAction::triggered,this,&MainWindow::control_propertyList_slot);

    if(speedMDFlag == 0)
    {
        speedDisplayMode = new QAction(tr("切换车速为(米/分)"),this);
    }
    else if(speedMDFlag == 1)
    {
        speedDisplayMode = new QAction(tr("切换车速为(毫米/秒)"),this);
    }
    else
    {
        speedDisplayMode = new QAction(tr("切换车速为()"),this);
    }
    connect(speedDisplayMode,&QAction::triggered,this,&MainWindow::speedDisplayMode_control);

    showFullScreen_Act = new QAction(QIcon(":/images/monitorFullScreen.png"),tr("监控全屏"),this);
    connect(showFullScreen_Act,&QAction::triggered,this,&MainWindow::showFullScreenSlot);


    line_Act = new Segment_Action;
    connect(line_Act,&Segment_Action::getText,this,&MainWindow::change_segment_width);
    arrow_Act = new Arrow_Action;
    arrow_Act->setLabel1Text("箭头宽度");
    arrow_Act->DSpinBox->setMaximum(99999);
    arrow_Act->DSpinBox->setDecimals(0);
    arrow_Act->DSpinBox->setSingleStep(1);
    connect(arrow_Act,&Arrow_Action::send_arrow_float,this,&MainWindow::change_arrow_height);


    arrow_Act2 = new Arrow_Action;
    arrow_Act2->setLabel1Text("箭头长度");
    arrow_Act2->DSpinBox->setMaximum(99999);
    arrow_Act2->DSpinBox->setDecimals(0);
    arrow_Act2->DSpinBox->setSingleStep(1);
    connect(arrow_Act2,&Arrow_Action::send_arrow_float,this,&MainWindow::change_arrow_width);


    cartrans_Act = new CarTranS_Action;
    cartrans_Act->setLabel1Text("车透明度");
    cartrans_Act->SpinBox->setMaximum(255);
    cartrans_Act->SpinBox->setMinimum(0);
    cartrans_Act->SpinBox->setSingleStep(1);
    connect(cartrans_Act,&CarTranS_Action::send_trans,this,&MainWindow::change_cartransparency);


    pointSize_Act = new PointSize_Action;
    pointSize_Act->setLabel1Text("站点大小");
    pointSize_Act->SpinBox->setMaximum(99999);
    pointSize_Act->SpinBox->setMinimum(0);
    pointSize_Act->SpinBox->setSingleStep(1);
    connect(pointSize_Act,&PointSize_Action::send_pointSize,this,&MainWindow::change_pointSize);

    reload_tool_Act = new QAction(QIcon(":/icons/reload.png"),tr("重新加载地图"),this);
    reload_tool_Act->setShortcut(tr("ctrl+r"));
    connect(reload_tool_Act,&QAction::triggered,this,&MainWindow::reloadmap);


    playback_tool_Act = new QAction(QIcon(":/images/playbacktool.png"),tr("回放工具"),this);
    playback_tool_Act->setShortcut(tr("ctrl+b"));
    connect(playback_tool_Act,&QAction::triggered,this,&MainWindow::show_playback_tool);

    agvctrl_tool_Act = new QAction(QIcon(":/icons/agvctrltool.png"),tr("控制工具"),this);
    connect(agvctrl_tool_Act,&QAction::triggered,this,&MainWindow::show_agvctrl_tool);

    batteryInfo_Act  = new QAction(QIcon(":/images/batteryinfo.png"),tr("电池数据"),this);
    connect(batteryInfo_Act,&QAction::triggered,this,&MainWindow::show_batteryInfoWidget);

    taskInfo_Act  = new QAction(QIcon(":/icons/task.png"),tr("任务数据"),this);
    taskInfo_Act->setShortcut(tr("ctrl+t"));
    connect(taskInfo_Act,&QAction::triggered,this,&MainWindow::show_taskInfoDialog);

    taskInfoSpecial_Act  = new QAction(QIcon(":/icons/degree.png"),tr("精度数据"),this);
    connect(taskInfoSpecial_Act,&QAction::triggered,this,&MainWindow::show_taskInfoSpecialDialog);

    cxjData_Act  = new QAction(QIcon(":/icons/device.png"),tr("成型机数据"),this);
    connect(cxjData_Act,&QAction::triggered,this,&MainWindow::show_cxjDataDialog);

    cxjBack_Act  = new QAction(QIcon(":/icons/rack.png"),tr("回空数据"),this);
    connect(cxjBack_Act,&QAction::triggered,this,&MainWindow::show_cxjBackDialog);


    checkUpdate_Act = new QAction(tr("检测更新"),this);
    connect(checkUpdate_Act,&QAction::triggered,this,&MainWindow::checkUpdateApp);

    debugWidget_Act = new QAction(QIcon(":/icons/zhushou.png"),tr("调试助手"),this);
    connect(debugWidget_Act,&QAction::triggered,this,&MainWindow::showDebugWidget);

    clockwise_Act = new QAction(QIcon(":/images/clockwise.png"),tr("顺时针旋转"),this);
    connect(clockwise_Act,&QAction::triggered,this,&MainWindow::clockwise_slot);
    anti_clockwise_Act = new QAction(QIcon(":/images/anticlockwise.png"),tr("逆时针旋转"),this);
    connect(anti_clockwise_Act,&QAction::triggered,this,&MainWindow::anti_clockwise_slot);


    skinDark_Act = new QAction(tr("深海蓝"),this);
    connect(skinDark_Act,&QAction::triggered,
            [=]()
    {
        QFile f(":qdarkstyle/dark/style.qss");

        if (!f.exists())   {
            QMessageBox::information(this," ","请联系工作人员获取皮肤");
        }
        else   {
            f.open(QFile::ReadOnly | QFile::Text);
            QTextStream ts(&f);
            qApp->setStyleSheet(ts.readAll());
            qDebug()<<"skinDark ";
            ParameterSave("skin","DK");
        }
        f.close();

    }
            );

    skinLight_Act = new QAction(tr("太子灰"),this);
    connect(skinLight_Act,&QAction::triggered,
            [=]()
    {
        QFile f(":qdarkstyle/light/style.qss");

        if (!f.exists())   {
            QMessageBox::information(this," ","请联系工作人员获取皮肤");
        }
        else   {
            f.open(QFile::ReadOnly | QFile::Text);
            QTextStream ts(&f);
            qApp->setStyleSheet(ts.readAll());
            qDebug()<<"skinLight ";
            ParameterSave("skin","Lt");
        }
        f.close();


    }
            );

    skinEmpty_Act = new QAction(tr("冰晶白"),this);
    connect(skinEmpty_Act,&QAction::triggered,
            [=]()
    {
        qApp->setStyleSheet("");
        QApplication::setStyle(QStyleFactory::create("windowsvista"));
        ParameterSave("skin","Ey");
    }
            );

    skinRetro_Act = new QAction(tr("复古风"),this);
    connect(skinRetro_Act,&QAction::triggered,
            [=]()
    {
        qApp->setStyleSheet("");
        QApplication::setStyle(QStyleFactory::create("windows"));
        qDebug()<<"windows";
        ParameterSave("skin","Ro");
    }
            );

    skinFusion_Act = new QAction(tr("科技风"),this);
    connect(skinFusion_Act,&QAction::triggered,
            [=]()
    {
        qApp->setStyleSheet("");
        QApplication::setStyle(QStyleFactory::create("fusion"));
        qDebug()<<"fusion";
        ParameterSave("skin","Fn");
    }
            );

    //楼层
    signalMapper = new QSignalMapper (this) ;
    for(int i = 0 ; i < 10; i++)
    {
        floor_Act[i] = new QAction(tr("显示")+QString::number(i)+tr("楼"),this);
        if(i < 10)
            floor_Act[i]->setShortcut(tr("ctrl+%1").arg(i));
        connect (floor_Act[i], SIGNAL(triggered()), signalMapper, SLOT(map())) ;
        signalMapper -> setMapping (floor_Act[i], i) ;
    }
    connect (signalMapper, SIGNAL(mapped(int)), this, SLOT(whole_floor_show(int))) ;

    carSizeWidget_Act = new QAction(QIcon(":/icons/agvmodule.png"),tr("模型修改"),this);
    connect(carSizeWidget_Act,&QAction::triggered,this,&MainWindow::showCarSizeWidget);

    drawActionGroup = new QActionGroup(this);    
    drawActionGroup->addAction(selectAct);
    drawActionGroup->addAction(lineAct);
    drawActionGroup->addAction(arcAct);
    drawActionGroup->addAction(pointAct);
    drawActionGroup->addAction(polylineAct);
    drawActionGroup->addAction(polygonAct);
    drawActionGroup->addAction(bezierAct);
    drawActionGroup->addAction(interrputAct);
    drawActionGroup->addAction(mergeAct);
    drawActionGroup->addAction(measureAct);
    drawActionGroup->addAction(arcAct_three_point);
    drawActionGroup->addAction(arcAct_start_cen_angle);
    drawActionGroup->addAction(arcAct_start_cen_end);
    drawActionGroup->addAction(arcAct_start_cen_length);
    drawActionGroup->addAction(arcAct_start_end_angle);
    drawActionGroup->addAction(arcAct_start_end_direct);
    drawActionGroup->addAction(arcAct_start_end_radius);
    drawActionGroup->addAction(arcAct_cen_start_angle);
    drawActionGroup->addAction(arcAct_cen_start_end);
    drawActionGroup->addAction(arcAct_cen_start_length);
    drawActionGroup->addAction(arcAct_continu);
    selectAct->setChecked(true);

    connect(selectAct,SIGNAL(triggered()),this,SLOT(addShape()));
    connect(lineAct,SIGNAL(triggered()),this,SLOT(addShape()));
    connect(arcAct,SIGNAL(triggered()),this,SLOT(addShape()));
    connect(pointAct,SIGNAL(triggered()),this,SLOT(addShape()));
    connect(polylineAct,SIGNAL(triggered()),this,SLOT(addShape()));
    connect(polygonAct,SIGNAL(triggered()),this,SLOT(addShape()));
    connect(bezierAct,SIGNAL(triggered()),this,SLOT(addShape()));

    connect(interrputAct,SIGNAL(triggered()),this,SLOT(addShape()));
    connect(mergeAct,SIGNAL(triggered()),this,SLOT(addShape()));
    connect(measureAct,SIGNAL(triggered()),this,SLOT(addShape()));
    connect(arcAct_three_point,SIGNAL(triggered()),this,SLOT(addShape()));
    connect(arcAct_start_cen_angle,SIGNAL(triggered()),this,SLOT(addShape()));
    connect(arcAct_start_cen_end,SIGNAL(triggered()),this,SLOT(addShape()));
    connect(arcAct_start_cen_length,SIGNAL(triggered()),this,SLOT(addShape()));
    connect(arcAct_start_end_angle,SIGNAL(triggered()),this,SLOT(addShape()));
    connect(arcAct_start_end_direct,SIGNAL(triggered()),this,SLOT(addShape()));
    connect(arcAct_start_end_radius,SIGNAL(triggered()),this,SLOT(addShape()));
    connect(arcAct_cen_start_angle,SIGNAL(triggered()),this,SLOT(addShape()));
    connect(arcAct_cen_start_end,SIGNAL(triggered()),this,SLOT(addShape()));
    connect(arcAct_cen_start_length,SIGNAL(triggered()),this,SLOT(addShape()));
    connect(arcAct_continu,SIGNAL(triggered()),this,SLOT(addShape()));

    deleteAct = new QAction(QIcon(":/icons/delete.png"),tr("&Delete"), this);
    deleteAct->setShortcut(QKeySequence::Delete);
 //   deleteAct->setEnabled(false);

    //undoAct = undoStack->createUndoAction(this,tr("撤销"));
    undoAct = undoGroup->createUndoAction(this,tr("撤销"));
    undoAct->setIcon(QIcon(":/icons/undo.png"));
    undoAct->setShortcuts(QKeySequence::Undo);

    //redoAct = undoStack->createRedoAction(this,tr("重做"));
    redoAct = undoGroup->createRedoAction(this,tr("重做"));
    redoAct->setIcon(QIcon(":/icons/redo.png"));
    redoAct->setShortcuts(QKeySequence::Redo);

    zoomInAct = new QAction(QIcon(":/icons/zoomin.png"),tr("放大"),this);
    zoomInAct->setShortcut(tr("ctrl+up"));
    zoomOutAct = new QAction(QIcon(":/icons/zoomout.png"),tr("缩小"),this);
    zoomOutAct->setShortcut(tr("ctrl+down"));
    creatMapAct = new QAction(QIcon(":/icons/creat_map.png"),tr("下载地图"),this);
    connect(creatMapAct,&QAction::triggered,this,&MainWindow::show_agvctrl_tool);


    bringToFrontAct = new QAction(QIcon(":/icons/bringtofront.png"),tr("控件移到顶层"),this);
    sendToBackAct   = new QAction(QIcon(":/icons/sendtoback.png"),tr("控件移到底层"),this);

    connect(bringToFrontAct,SIGNAL(triggered()),this,SLOT(on_actionBringToFront_triggered()));
    connect(sendToBackAct,SIGNAL(triggered()),this,SLOT(on_actionSendToBack_triggered()));
//    copyAct = new QAction(QIcon(":/icons/copy.png"),tr("复制"),this);
//    copyAct->setShortcut(QKeySequence::Copy);

//    pasteAct = new QAction(QIcon(":/icons/paste.png"),tr("粘贴"),this);
//    pasteAct->setShortcut(QKeySequence::Paste);
//    pasteAct->setEnabled(false);
//    cutAct = new QAction(QIcon(":/icons/cut.png"),tr("剪切"),this);
//    cutAct->setShortcut(QKeySequence::Cut);

//    connect(copyAct,SIGNAL(triggered()),this,SLOT(copyItems()));
//    connect(pasteAct,SIGNAL(triggered()),this,SLOT(on_paste()));
//    connect(cutAct,SIGNAL(triggered()),this,SLOT(on_cut()));

    connect(zoomInAct , SIGNAL(triggered()),this,SLOT(zoomIn()));
    connect(zoomOutAct , SIGNAL(triggered()),this,SLOT(zoomOut()));
    connect(deleteAct, SIGNAL(triggered()), this, SLOT(deleteItem()));
//    connect(creatMapAct,SIGNAL(triggered()),this,SLOT(creatMap()));

    funcAct = new QAction(QIcon(":/icons/setting.png"),tr("地图设置"),this);
    connect(funcAct,SIGNAL(triggered()),this,SLOT(on_func_test_triggered()));

    control_Area = new QAction(tr("显示避让区图形"),this);
    control_Area->setShortcut(tr("ctrl+a"));
    control_Area->setCheckable(true);
    connect(control_Area,&QAction::triggered,this,&MainWindow::avoid_Area_control);

    showLiftLineAct = new QAction(tr("显示电梯连线"),this);
    showLiftLineAct->setShortcut(tr("ctrl+d"));
    showLiftLineAct->setCheckable(true);
    showLiftLineAct->setChecked(true);
    connect(showLiftLineAct,SIGNAL(triggered(bool )),this,SLOT(showLiftLine(bool )));

    showDisableAct = new QAction(tr("显示禁用点和片段"),this);
    showDisableAct->setShortcut(tr("ctrl+j"));
    showDisableAct->setCheckable(true);
    showDisableAct->setChecked(true);
    connect(showDisableAct,SIGNAL(triggered(bool )),this,SLOT(showDisable(bool )));
}

void MainWindow::createMenus()
{
//    qDebug()<<"here here";

    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
//    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
//    fileMenu->addAction(saveAct);
//    fileMenu->addSeparator();
//    fileMenu->addAction(exitAct);

    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
//    editMenu->addAction(undoAct);
//    editMenu->addAction(redoAct);
//    editMenu->addAction(cutAct);
//    editMenu->addAction(copyAct);
//    editMenu->addAction(pasteAct);
//    editMenu->addAction(deleteAct);
    editMenu->addAction(creatMapAct);

    viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(zoomInAct);
    viewMenu->addAction(zoomOutAct);
    viewMenu->addAction(carSizeWidget_Act);
    viewMenu->addAction(scene_color_Act);
    viewMenu->addAction(route_color_Act);
    viewMenu->addAction(gridAct);
    viewMenu->addAction(point_font_Act);
    viewMenu->addSeparator();

    viewMenu->addAction(station_background_Act);
    //viewMenu->addAction(control_pointfont);
    viewMenu->addAction(control_floatwindow);
//        qDebug()<<"here here2";
    viewMenu->addAction(control_saBackground);
//    qDebug()<<"here here3";

    viewMenu->addAction(control_errorInfoHint);
//    qDebug()<<"here here4";
    viewMenu->addSeparator();
    viewMenu->addAction(control_Area);
    viewMenu->addAction(showLiftLineAct);
    viewMenu->addAction(showDisableAct);

    viewMenu->addSeparator();
    viewMenu->addAction(speedDisplayMode);


    viewMenu->addSeparator();
    viewMenu->addAction(line_Act);
    viewMenu->addAction(arrow_Act);
    viewMenu->addAction(arrow_Act2);
    viewMenu->addAction(cartrans_Act);
    viewMenu->addAction(pointSize_Act);


    QMenu *toolMenu = menuBar()->addMenu(tr("&Tools"));
    //toolMenu->addAction(selectAct);
    toolMenu->addAction(reload_tool_Act);
    toolMenu->addAction(playback_tool_Act);
    toolMenu->addAction(agvctrl_tool_Act);
    toolMenu->addAction(batteryInfo_Act);
    toolMenu->addAction(taskInfo_Act);
    toolMenu->addAction(taskInfoSpecial_Act);
    toolMenu->addAction(cxjData_Act);
    toolMenu->addAction(cxjBack_Act);

//    toolMenu->addAction(lineAct);
//    toolMenu->addAction(arcAct);
//    toolMenu->addAction(pointAct);
//    toolMenu->addAction(polylineAct);
//    toolMenu->addAction(polygonAct);
//    toolMenu->addAction(bezierAct);
//    toolMenu->addAction(rotateAct);

    windowMenu = menuBar()->addMenu(tr("&Window"));
    windowMenu->addAction(showFullScreen_Act);
    windowMenu->addAction(debugWidget_Act);
    windowMenu->addAction(clockwise_Act);
    windowMenu->addAction(anti_clockwise_Act);
    skinMenu = windowMenu->addMenu(QIcon(":/icons/skin.png"),"选择皮肤");
    skinMenu->addAction(skinDark_Act);
    skinMenu->addAction(skinLight_Act);
    skinMenu->addAction(skinEmpty_Act);
    skinMenu->addAction(skinRetro_Act);
    skinMenu->addAction(skinFusion_Act);

//    updateWindowMenu();
    //connect(windowMenu, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
    helpMenu->addAction(checkUpdate_Act);
    helpMenu->addAction(funcAct);
}

void MainWindow::actionsSlot()
{
    QAction *action = (QAction*)sender();
    QString actionText = action->text();

    qDebug()<<actionText<<"triggerred!"<<endl;

    SubWidget *tempSubWidget = activeMdiChild();
    if(tempSubWidget == nullptr)
    {
        QMessageBox::warning(this,"","请先打开地图！");
    }
    else
    {
        trackBtn->setText(actionText);//tr("->")+
        tempSubWidget->agv_track_flag = actionText.toInt();
    }
}

void MainWindow::createToolbars()
{
    // create edit toolbar
    editToolBar = addToolBar(tr("edit"));
    editToolBar->setIconSize(QSize(24,24));
    editToolBar->addAction(openAct);
//    editToolBar->addAction(newAct);
//    editToolBar->addAction(saveAct);
//    editToolBar->addAction(copyAct);
//    editToolBar->addAction(pasteAct);
//    editToolBar->addAction(cutAct);
//    editToolBar->addAction(deleteAct);
//    editToolBar->addAction(undoAct);
//    editToolBar->addAction(redoAct);
    editToolBar->addAction(reload_tool_Act);
    editToolBar->addAction(playback_tool_Act);
    editToolBar->addAction(agvctrl_tool_Act);
    editToolBar->addAction(taskInfo_Act);
//    editToolBar->addAction(zoomOutAct);


    //editToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    // create draw toolbar
    drawToolBar = addToolBar(tr("drawing"));
    drawToolBar->setIconSize(QSize(24,24));
    drawToolBar->addAction(selectAct);
//    drawToolBar->addAction(pointAct);
//    drawToolBar->addAction(lineAct);
//    drawToolBar->addAction(arcAct);

    //弧线的选择下拉框
//    QMenu* menuArc=new QMenu(this);
    //menuArc->addAction(arcAct_three_point);menuArc->addSeparator();
    //menuArc->addAction(arcAct_start_cen_angle);
    //menuArc->addAction(arcAct_start_cen_end);
    //menuArc->addAction(arcAct_start_cen_length);menuArc->addSeparator();
    //menuArc->addAction(arcAct_start_end_angle);
    //menuArc->addAction(arcAct_start_end_direct);
//    menuArc->addAction(arcAct_start_end_radius);menuArc->addSeparator();
    //menuArc->addAction(arcAct_cen_start_angle);
    //menuArc->addAction(arcAct_cen_start_end);
    //menuArc->addAction(arcAct_cen_start_length);menuArc->addSeparator();
//    menuArc->addAction(arcAct_continu);

    QMenu* floormenu=new QMenu(this);
    for(int i = 0 ; i < 10; i++)
    {
        floormenu->addAction(floor_Act[i]);
    }

    toolBtn = new QToolButton(this);
    toolBtn->setMenu(floormenu);
    toolBtn->setPopupMode(QToolButton::InstantPopup);
    QIcon icon;
    icon.addFile(QString::fromUtf8(":/icons/floor.png"), QSize(), QIcon::Normal, QIcon::Off);
    toolBtn->setToolButtonStyle(Qt::ToolButtonIconOnly);
    toolBtn->setToolTip("显示楼层");
    toolBtn->setIcon(icon);

    //追踪菜单
    QMenu* trackmenu=new QMenu(this);
    QMenu* trackmenu1 = trackmenu->addMenu("00_20");
    for(int i = 0 ; i < 21; i++)
    {
        trackmenu1->addAction(QString::number(i),this,SLOT(actionsSlot()));
    }
    QMenu* trackmenu2 = trackmenu->addMenu("21_40");
    for(int i = 21 ; i < 41; i++)
    {
        trackmenu2->addAction(QString::number(i),this,SLOT(actionsSlot()));
    }
    QMenu* trackmenu3 = trackmenu->addMenu("41_60");
    for(int i = 41 ; i < 60; i++)
    {
        trackmenu3->addAction(QString::number(i),this,SLOT(actionsSlot()));
    }
    QMenu* trackmenu4 = trackmenu->addMenu("61_80");
    for(int i = 61 ; i < 81; i++)
    {
        trackmenu4->addAction(QString::number(i),this,SLOT(actionsSlot()));
    }

    trackBtn = new QPushButton(this);
    trackBtn->setMenu(trackmenu);
    QIcon trackicon;
    trackicon.addFile(QString::fromUtf8(":/icons/track.png"), QSize(), QIcon::Normal, QIcon::Off);
    //trackBtn->setStyleSheet("font: bold;background-color: red;font-size: 36px;height: 48px;width: 120px;");
    trackBtn->setStyleSheet( "QPushButton{background: transparent;}" );
    trackBtn->setToolTip("跟踪AGV");
//    trackBtn->setText("-");
    trackBtn->setIcon(trackicon);
    trackBtn->setIconSize(QSize(24,24));

//    drawToolBar->addAction(polylineAct);
//    drawToolBar->addAction(polygonAct);
//    drawToolBar->addAction(bezierAct);
//    drawToolBar->addAction(interrputAct);
//    drawToolBar->addAction(mergeAct);
    drawToolBar->addAction(measureAct);
//    drawToolBar->addAction(groupAct);
//    drawToolBar->addAction(unGroupAct);
//    drawToolBar->addAction(rotateAct);
//    drawToolBar->addAction(moveAct);
    drawToolBar->addAction(findAct);
//    drawToolBar->addAction(updateAct);

    //drawToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

//    otherToolBar = addToolBar(tr("other"));
    drawToolBar->addAction(whole_mapAct);
    drawToolBar->addWidget(toolBtn);
    drawToolBar->addAction(bringToFrontAct);
    drawToolBar->addAction(sendToBackAct);
    drawToolBar->addAction(control_pointfont);
    drawToolBar->addAction(carTransparency_Act);
    drawToolBar->addAction(control_propertyList);
    drawToolBar->addWidget(trackBtn);
}

void MainWindow::reloadmap()
{
    SubWidget *tempSubWidget = activeMdiChild();
    if(tempSubWidget == 0)
    {
        QMessageBox::warning(this,"","请先打开地图！");
    }
    else
    {
        QString curOpenFile = tempSubWidget->ui->graphicsView->currentFile();
        mdiArea->closeActiveSubWindow();
        if (openFile(curOpenFile))
        {
            statusBar()->showMessage(tr("File loaded: %1").arg(curOpenFile), 2000);
            absolutePath = QFileInfo(fileName).absolutePath();
        }
    }
}


void MainWindow::checkUpdateApp()
{
    if(!QProcess::startDetached("./AutoUpdate.exe",QStringList() << "1"))
    {
        QMessageBox::warning(this,tr("警告信息"),tr("   检测更新程序启动失败！ "));
    }
}

void MainWindow::showDebugWidget()
{
        bool ok;
        QString text = QInputDialog::getText(this, tr("调试功能"),tr("请输入暗号"), QLineEdit::Password,0, &ok,Qt::Widget);
        if (ok && !text.isEmpty())
        {
            if(text == "666")
            {
                SubWidget *tempSubWidget = activeMdiChild();
                if(tempSubWidget == nullptr)
                {
                    QMessageBox::warning(this,"","请先打开地图！");
                }
                else
                {
                    if(tempSubWidget->debugWidget->isVisible())
                    {
                        tempSubWidget->debugWidget->showNormal();
                    }
                    else
                    {
                        tempSubWidget->debugWidget->show();
                    }
                }
            }
            else
            {
                return;
            }
        }
}

void MainWindow::showCarSizeWidget()
{
    SubWidget *tempSubWidget = activeMdiChild();
    if(tempSubWidget == 0)
    {
        QMessageBox::warning(this,"","请先打开地图！");
    }
    else
    {
        if(tempSubWidget->carSizeWidget->isVisible())
        {
            tempSubWidget->carSizeWidget->raise();
//            qDebug()<<"raise";
        }
        else
        {
            tempSubWidget->carSizeWidget->show();
//            qDebug()<<"show";
        }
    }
}

SubWidget *MainWindow::activeMdiChild()
{
    if (QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow())
        return qobject_cast<SubWidget *>(activeSubWindow->widget());
    return 0;
}

QMdiSubWindow *MainWindow::findMdiChild(const QString &fileName)
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    foreach (QMdiSubWindow *window, mdiArea->subWindowList()) {
        SubWidget *mdiChild = qobject_cast<SubWidget *>(window->widget());
        if (mdiChild->ui->graphicsView->currentFile() == canonicalFilePath)
            return window;
    }
    return 0;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    mdiArea->closeAllSubWindows();
    if (mdiArea->currentSubWindow()) {
        event->ignore();
    } else {
        event->accept();
    }
}


void MainWindow::updateWindowMenu()
{
    windowMenu->clear();
    windowMenu->addAction(closeAct);
    windowMenu->addAction(closeAllAct);
    windowMenu->addSeparator();
    windowMenu->addAction(tileAct);
    windowMenu->addAction(cascadeAct);
    windowMenu->addSeparator();
    windowMenu->addAction(nextAct);
    windowMenu->addAction(previousAct);
    windowMenu->addAction(separatorAct);
}

void MainWindow::zoomIn()
{
    if (!activeMdiChild()) return ;
     activeMdiChild()->ui->graphicsView->zoomIn();
}

void MainWindow::zoomOut()
{
    if (!activeMdiChild()) return ;
    activeMdiChild()->ui->graphicsView->zoomOut();
}

void MainWindow::newFile()
{
    SubWidget *child = createMdiChild();
    child->ui->graphicsView->newFile();
    child->setWindowTitle(QString(child->ui->graphicsView->curFile + "[*]"));
    child->show();
}

QString MainWindow::ParameterInit(int flag)
{
    QSettings settings("Config.ini", QSettings::IniFormat);
    if(flag == 1)
    {
        QString fileName = settings.value("map").toString();
        return fileName;
    }
    else if(flag == 2)
    {
        QString colorName = settings.value("color").toString();
        return colorName;
    }
    else if(flag ==3)
    {
        QString speedDM = settings.value("SDM").toString();
        return speedDM;
    }
}

void MainWindow::ParameterInit()
{
    QSettings settings("Config.ini", QSettings::IniFormat);

    fileName = settings.value("map").toString();

    colorName = settings.value("color").toString();

    speedDM = settings.value("SDM").toString();

    skin = settings.value("skin").toString();

    ip = settings.value("ip").toString();

}

void MainWindow::ParameterSave(QString Type, QString p)
{
    QSettings settings("Config.ini", QSettings::IniFormat);
    settings.setValue(Type,p);
}

void MainWindow::config_open()
{
   if(fileName.isEmpty())
   {
       qDebug()<<"配置文件没有地图";
       return;
   }
   else
   {
       QString fileNametmp = fileName;

       absolutePath = QFileInfo(fileNametmp).absolutePath();

       if (!fileNametmp.isEmpty()) {
           QMdiSubWindow *existing = findMdiChild(fileNametmp);
           if (existing) {
               mdiArea->setActiveSubWindow(existing);
               return;
           }
            if(QFileInfo(fileNametmp).exists() == false)
            {
                statusBar()->showMessage(QString("%1 : 文件不存在，请重新打开地图").arg(fileNametmp));
                ParameterSave("map","");
                return;
            }
           if (openFile(fileNametmp))
           {
               ParameterSave("map",fileNametmp);
               statusBar()->showMessage(tr("File loaded: %1").arg(fileNametmp), 2000);
           }
       }
   }
}

void MainWindow::pointfont_control()
{
    SubWidget *tempSubWidget = activeMdiChild();
    int temp_flag;
    for(int i = 0 ; i < tempSubWidget->vec_PointItem.size(); i++)
    {
        if(tempSubWidget->vec_PointItem[i] != nullptr)
        {
            temp_flag = tempSubWidget->vec_PointItem[i]->point_font_flag;
            break;
        }
    }
    if(temp_flag == 0)//已关，打开
    {
        for(int i = 0 ; i < tempSubWidget->vec_PointItem.size(); i++)
        {
            if(tempSubWidget->vec_PointItem[i] != nullptr)
            {
                tempSubWidget->vec_PointItem[i]->point_font_flag = 1;
            }
        }
        ParameterSave("font_S",QString("1"));
    }
    else
    {
        for(int i = 0 ; i < tempSubWidget->vec_PointItem.size(); i++)
        {
            if(tempSubWidget->vec_PointItem[i] != nullptr)
            {
                tempSubWidget->vec_PointItem[i]->point_font_flag = 0;
            }
        }
        ParameterSave("font_S",QString("0"));
    }
    tempSubWidget->scene->update();

}

void MainWindow::avoid_Area_control()
{
    SubWidget *tempSubWidget = activeMdiChild();
    int temp_flag = tempSubWidget->avoid_Area_flag;
    if(temp_flag == 0)
    {
        tempSubWidget->ui->graphicsView->scene_add_AreaItem();
        tempSubWidget->avoid_Area_flag = 1;
        ParameterSave("area_status","1");
        control_Area->setChecked(true);
    }
    else
    {
        tempSubWidget->ui->graphicsView->scene_remove_AreaItem();
        tempSubWidget->avoid_Area_flag = 0;
        ParameterSave("area_status","0");
        control_Area->setChecked(false);
    }
}

void MainWindow::floatwindow_control()
{
    SubWidget *tempSubWidget = activeMdiChild();
    int temp_flag = tempSubWidget->floatwindow_flag;
    if(temp_flag == 0)
    {
        tempSubWidget->floatwindow_flag = 1;
        ParameterSave("fw_status","1");
    }
    else
    {
        tempSubWidget->floatwindow_flag = 0;
        ParameterSave("fw_status","0");
    }
}

void MainWindow::saBackground_control()
{
    SubWidget *tempSubWidget = activeMdiChild();
    int temp_flag = tempSubWidget->saBackground_flag;
    if(temp_flag == 0)
    {
        tempSubWidget->control_SAItem(1);
        tempSubWidget->saBackground_flag = 1;
        ParameterSave("sa_bg","1");
    }
    else
    {
        tempSubWidget->control_SAItem(0);
        tempSubWidget->saBackground_flag = 0;
        ParameterSave("sa_bg","0");
    }
}

void MainWindow::errorInfoHint_control()
{
    if(activeMdiChild() == 0)
    {
        QMessageBox::warning(this,"","请先打开地图！");
    }
    else
    {
        SubWidget *tempSubWidget = activeMdiChild();
        if(tempSubWidget->errorInfoSwitch == true)
        {
            tempSubWidget->errorInfoSwitch = false;
        }
        else
        {
            tempSubWidget->errorInfoSwitch = true;
        }
    }
}

void MainWindow::control_propertyList_slot()
{
    SubWidget *tempSubWidget = activeMdiChild();
    if(tempSubWidget == 0)
    {
        QMessageBox::warning(this,"","请先打开地图！");
    }
    else
    {
        QString curText = control_propertyList->text();
        if(curText == tr("隐藏属性列表"))
        {
            tempSubWidget->sp2Size.clear();
            tempSubWidget->sp2Size<<1200<<0;
            tempSubWidget->ui->splitter_2->setSizes(tempSubWidget->sp2Size);
            control_propertyList->setText(tr("打开属性列表"));
            control_propertyList->setCheckable(true);
        }
        else if(curText == tr("打开属性列表"))
        {
            tempSubWidget->sp2Size.clear();
            tempSubWidget->sp2Size<<950<<250;
            tempSubWidget->ui->splitter_2->setSizes(tempSubWidget->sp2Size);
            control_propertyList->setText(tr("隐藏属性列表"));
        }
    }

}


void MainWindow::speedDisplayMode_control()
{
    if(activeMdiChild() == 0)
    {
        QMessageBox::warning(this,"","请先打开地图！");
    }
    else
    {
        SubWidget *tempSubWidget = activeMdiChild();
        if(speedMDFlag == 0)//当前显示为     毫米/秒
        {
            speedDisplayMode->setText(tr("切换车速为(毫米/秒)"));
            speedMDFlag = 1;
        }
        else if(speedMDFlag == 1)
        {
            speedDisplayMode->setText(tr("切换车速为(米/分)"));
            speedMDFlag = 0;
        }

        if(tempSubWidget->speedDisplayMode == 0)
        {
            tempSubWidget->speedDisplayMode = 1;
            QStringList headers;
            headers  << "车号" << "任务号"<<"时间"<< "类型(T)"<<"主状态"<<"子状态"<<"软停"<<"在线"<<"报警"<<"电量"<<"充电时间"<<"载货"
                     <<"起始站点"<<"目标站点"<<"货叉高度"<< "x坐标"<< "y坐标"<< "角度" << "方向"<<"速度(米/分)"<<"当前段"<<"下一段"<<"当前站点"<<"下个站点"
                    <<"前安全防护"<<"后安全防护"<<"货叉光电"<<"IP"<< "类型(A)"<<"电池止挡"<<"1分钟心跳次数"<<"5分钟心跳次数"<<"15分钟心跳次数"
                      <<"程序版本"<<"编译时间"<<"版本类型"<<"绑定区"<<"库位限定";
            tempSubWidget->ui->carInfo_tableWidget->setHorizontalHeaderLabels(headers);
            ParameterSave("SDM",QString("1"));
        }
        else if(tempSubWidget->speedDisplayMode == 1)
        {
            tempSubWidget->speedDisplayMode = 0;
            QStringList headers;
            headers  << "车号" << "任务号"<<"时间"<< "类型(T)"<<"主状态"<<"子状态"<<"软停"<<"在线"<<"报警"<<"电量"<<"充电时间"<<"载货"
                   <<"起始站点"<<"目标站点"<<"货叉高度"<< "x坐标"<< "y坐标"<< "角度" << "方向"<<"速度(毫米/秒)"<<"当前段"<<"下一段"<<"当前站点"<<"下个站点"
                   <<"前安全防护"<<"后安全防护"<<"货叉光电"<<"IP"<< "类型(A)"<<"电池止挡"<<"1分钟心跳次数"<<"5分钟心跳次数"<<"15分钟心跳次数"
                     <<"程序版本"<<"编译时间"<<"版本类型"<<"绑定区"<<"库位限定";
            tempSubWidget->ui->carInfo_tableWidget->setHorizontalHeaderLabels(headers);
            ParameterSave("SDM",QString("0"));
        }

    }
}

void MainWindow::change_segment_width(QString widthValue)
{
    viewMenu->close();
    SubWidget *tempSubWidget = activeMdiChild();

    for(int i = 0 ; i < tempSubWidget->vec_LineItem.size(); i++)
    {
        if(tempSubWidget->vec_LineItem[i] != nullptr)
        {
            tempSubWidget->vec_LineItem[i]->custom_line_width = widthValue.toInt();
        }
    }
    for(int i = 0 ; i < tempSubWidget->vec_ArcItem.size(); i++)
    {
        if(tempSubWidget->vec_ArcItem[i] != nullptr)
        {
            tempSubWidget->vec_ArcItem[i]->custom_line_width = widthValue.toInt()/2;
        }
    }

    tempSubWidget->scene->update();
    ParameterSave("seg_w",widthValue);
}

void MainWindow::change_arrow_height(float arrow_float)
{
    SubWidget *tempSubWidget = activeMdiChild();

    for(int i = 0 ; i < tempSubWidget->vec_LineItem.size(); i++)
    {
        if(tempSubWidget->vec_LineItem[i] != nullptr)
        {
            tempSubWidget->vec_LineItem[i]->custom_n = arrow_float;
        }
    }
    for(int i = 0 ; i < tempSubWidget->vec_ArcItem.size(); i++)
    {
        if(tempSubWidget->vec_ArcItem[i] != nullptr)
        {
            tempSubWidget->vec_ArcItem[i]->custom_n = arrow_float;
        }
    }

    tempSubWidget->scene->update();
    ParameterSave("arrow",QString("%1").arg(arrow_float));
}

void MainWindow::change_arrow_width(float arrow_float)
{
    SubWidget *tempSubWidget = activeMdiChild();

    for(int i = 0 ; i < tempSubWidget->vec_LineItem.size(); i++)
    {
        if(tempSubWidget->vec_LineItem[i] != nullptr)
        {
            tempSubWidget->vec_LineItem[i]->custom_l = arrow_float;
        }
    }
    for(int i = 0 ; i < tempSubWidget->vec_ArcItem.size(); i++)
    {
        if(tempSubWidget->vec_ArcItem[i] != nullptr)
        {
            tempSubWidget->vec_ArcItem[i]->custom_l = arrow_float;
        }
    }

    tempSubWidget->scene->update();
    ParameterSave("arrow2",QString("%1").arg(arrow_float));
}

void MainWindow::change_cartransparency(int trans)
{
    SubWidget *tempSubWidget = activeMdiChild();
    if(tempSubWidget == 0)
    {
        QMessageBox::warning(this,"","请先打开地图！");
    }
    else
    {
        for(int i = 0; i < tempSubWidget->vec_Car.size(); i++)
        {
            tempSubWidget->vec_Car[i]->transparency = trans;
        }
    }
}

void MainWindow::change_pointSize(int size)
{
    SubWidget *tempSubWidget = activeMdiChild();

    if(tempSubWidget == 0)
    {
        QMessageBox::warning(this,"","请先打开地图！");
    }
    else
    {
        for(int i = 0 ; i < tempSubWidget->vec_PointItem.size(); i++)
        {
            if(tempSubWidget->vec_PointItem[i] != nullptr)
            {
                tempSubWidget->vec_PointItem[i]->m_localRect = QRectF(-size/2, -size/2, size, size);
            }
        }

        qDebug()<<"改变了点大小";

        tempSubWidget->scene->update();
        //        ParameterSave("point",QString("%1").arg(size));
    }

}

void MainWindow::show_playback_tool()
{
    SubWidget *tempSubWidget = activeMdiChild();
    if(tempSubWidget == 0)
    {
        QMessageBox::warning(this,"","请先打开地图！");
    }
    else
    {
        if(tempSubWidget->playback_tool->isVisible())
        {
            tempSubWidget->playback_tool->showNormal();
        }
        else
        {
            tempSubWidget->playback_tool->show();
        }
    }
}

void MainWindow::show_agvctrl_tool()
{
    if(agvctrl_tool == nullptr)
    {
        agvctrl_tool = new  AgvCtrl_Tool();//openFile_slot
        connect(agvctrl_tool,&AgvCtrl_Tool::download_map_finished,this,&MainWindow::openFile_slot);
    }
    if(agvctrl_tool != nullptr)
    {
        agvctrl_tool->show();
    }
}

void MainWindow::show_batteryInfoWidget()
{
    if(activeMdiChild() == 0)
    {
        QMessageBox::warning(this,"","请先打开地图！");
    }
    else
    {
        activeMdiChild()->batteryInfo_widget->show();
    }
}

void MainWindow::show_taskInfoDialog()
{
    SubWidget *tempSubWidget = activeMdiChild();
    if(tempSubWidget == 0)
    {
        QMessageBox::warning(this,"","请先打开地图！");
    }
    else
    {
        if(tempSubWidget->taskInfoDialog->isVisible())
        {
            tempSubWidget->taskInfoDialog->raise();
//            qDebug()<<"raise";
        }
        else
        {
            tempSubWidget->taskInfoDialog->show();
//            qDebug()<<"show";
        }
    }
}

void MainWindow::show_taskInfoSpecialDialog()
{
    SubWidget *tempSubWidget = activeMdiChild();
    if(tempSubWidget == 0)
    {
        QMessageBox::warning(this,"","请先打开地图！");
    }
    else
    {
        tempSubWidget->taskInfoSpecialDialog->ui->IpComboBox->setCurrentText(tempSubWidget->ui->tcp_lineEdit->currentText());
        if(tempSubWidget->taskInfoSpecialDialog->isVisible())
        {
            tempSubWidget->taskInfoSpecialDialog->raise();
//            qDebug()<<"raise";
        }
        else
        {
            tempSubWidget->taskInfoSpecialDialog->show();
//            qDebug()<<"show";
        }
    }
}

void MainWindow::show_cxjDataDialog()
{
    SubWidget *tempSubWidget = activeMdiChild();
    if(tempSubWidget == 0)
    {
        QMessageBox::warning(this,"","请先打开地图！");
    }
    else
    {
        if(tempSubWidget->cxjDataInfoDialog->isVisible())
        {
            tempSubWidget->cxjDataInfoDialog->raise();
        }
        else
        {
            tempSubWidget->cxjDataInfoDialog->show();
        }
    }
}

void MainWindow::show_cxjBackDialog()
{
    SubWidget *tempSubWidget = activeMdiChild();
    if(tempSubWidget == 0)
    {
        QMessageBox::warning(this,"","请先打开地图！");
    }
    else
    {
        if(tempSubWidget->cxjBackInfoDialog->isVisible())
        {
            tempSubWidget->cxjBackInfoDialog->raise();
        }
        else
        {
            tempSubWidget->cxjBackInfoDialog->show();
        }
    }
}

void MainWindow::showFullScreenSlot()
{
    if(activeMdiChild() == 0)
    {
        QMessageBox::warning(this,"","请先打开地图！");
    }
    else
    {
        SubWidget *tempSubWidget = activeMdiChild();
        mdiArea->removeSubWindow(tempSubWidget);
        tempSubWidget->showFullScreen();
        tempSubWidget->isFullScreenState = true;
    }
}

void MainWindow::addToMidAreaSlot(SubWidget *subwidget)
{
    mdiArea->addSubWindow(subwidget);
    subwidget->isFullScreenState = false;
    subwidget->activateWindow();

        QList<QMdiSubWindow *> list = mdiArea->subWindowList();
        foreach (QMdiSubWindow *mdisubwindow, list) {
            if(mdisubwindow->windowTitle() == ""){
                mdisubwindow->close();
            }
        }


}

void MainWindow::showLiftLine(bool show_line)
{
    SubWidget *subWidget = activeMdiChild();
    if(!subWidget)
    {
        QMessageBox::warning(this,"Warning", "    请先打开地图文件！   ");
        return;
    }
    else
    {
        int cur_map_index = subWidget->ui->graphicsView->cur_map_index;
        for(auto it = subWidget->ui->graphicsView->map_mapLineItem[cur_map_index].begin(); it != subWidget->ui->graphicsView->map_mapLineItem[cur_map_index].end(); it++)
        {
            int startId = it.value()->map_segment_pb.basic().start_id();
            int endId   = it.value()->map_segment_pb.basic().end_id();
            //qDebug() << startId << endId << it.value()->map_segment_pb.basic().id();

            if(subWidget->ui->graphicsView->map_mapPointItem[cur_map_index].contains(startId) && subWidget->ui->graphicsView->map_mapPointItem[cur_map_index].contains(endId))
            {
                if(subWidget->ui->graphicsView->map_mapPointItem[cur_map_index][startId]->map_point_pb.basic().type() == 13
                  && subWidget->ui->graphicsView->map_mapPointItem[cur_map_index][endId]->map_point_pb.basic().type() == 13
                  && subWidget->ui->graphicsView->map_mapPointItem[cur_map_index][startId]->map_point_pb.basic().floor() != subWidget->ui->graphicsView->map_mapPointItem[cur_map_index][endId]->map_point_pb.basic().floor())
                {
                    if(show_line)
                    {
                        it.value()->show();
                    }
                    else
                    {
                        it.value()->hide();
                    }
                }
            }
        }
        for(auto it = subWidget->ui->graphicsView->map_mapArcItem[cur_map_index].begin(); it != subWidget->ui->graphicsView->map_mapArcItem[cur_map_index].end(); it++)
        {
            int startId = it.value()->map_segment_pb.basic().start_id();
            int endId   = it.value()->map_segment_pb.basic().end_id();
            //qDebug() << startId << endId << it.value()->map_segment_pb.basic().id();

            if(subWidget->ui->graphicsView->map_mapPointItem[cur_map_index].contains(startId) && subWidget->ui->graphicsView->map_mapPointItem[cur_map_index].contains(endId))
            {
                if(subWidget->ui->graphicsView->map_mapPointItem[cur_map_index][startId]->map_point_pb.basic().type() == 13
                  && subWidget->ui->graphicsView->map_mapPointItem[cur_map_index][endId]->map_point_pb.basic().type() == 13
                  && subWidget->ui->graphicsView->map_mapPointItem[cur_map_index][startId]->map_point_pb.basic().floor() != subWidget->ui->graphicsView->map_mapPointItem[cur_map_index][endId]->map_point_pb.basic().floor())
                {
                    if(show_line)
                    {
                        it.value()->show();
                    }
                    else
                    {
                        it.value()->hide();
                    }
                }
            }
        }
    }
}
void MainWindow::showDisable(bool show_disable)
{
    //qDebug() << show_disable;
    SubWidget * subWidget = activeMdiChild();
    if(subWidget)
    {
        QGraphicsScene * scene = subWidget->ui->graphicsView->scene();
        foreach(QGraphicsItem *item, scene->items())
        {
            if(item->type() == GraphicsPointItem::Type)
            {
                GraphicsPointItem *point_item = dynamic_cast<GraphicsPointItem *>(item);
                if(show_disable == false && point_item->map_point_pb.basic().disable() == true)
                {
                    item->hide();
                }
                else
                {
                    item->show();
                }
            }
            else if(item->type() == GraphicsLineItem::Type)
            {
                GraphicsLineItem *line_item = dynamic_cast<GraphicsLineItem *>(item);
                if(show_disable == false && line_item->map_segment_pb.basic().disable() == true)
                {
                    item->hide();
                }
                else
                {
                    item->show();
                }
            }
            else if(item->type() == GraphicsArcItem::Type)
            {
                GraphicsArcItem *arc_item = dynamic_cast<GraphicsArcItem *>(item);
                if(show_disable == false && arc_item->map_segment_pb.basic().disable() == true)
                {
                    item->hide();
                }
                else
                {
                    item->show();
                }
            }
        }
        showLiftLine(showLiftLineAct->isChecked());

        subWidget->ui->graphicsView->scene()->update();

    }
}

void MainWindow::findDialogSlot(int result)
{
    if(result == QDialog::Accepted)
    {
        if(findDialog->ui->lineEdit_id->text().isEmpty())
        {
            QMessageBox::warning(this,"Find Warning","     请输入需要查找的Id号！    ");
        }
        else
        {
            //qDebug() << findDialog->buttonGroup->checkedId() << findDialog->ui->lineEdit_id->text().toInt();
            activeMdiChild()->ui->graphicsView->find(findDialog->buttonGroup->checkedId(), findDialog->ui->lineEdit_id->text());
        }
    }
    else if(result == QDialog::Rejected)
    {

    }
}



void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(this,"打开文件",absolutePath,"*.pb");
//        QString fileName = QFileDialog::getOpenFileName(this,"打开文件",".","*.hs");
        if (!fileName.isEmpty()) {
            QMdiSubWindow *existing = findMdiChild(fileName);
            if (existing) {
                mdiArea->setActiveSubWindow(existing);
                return;
            }
            if (openFile(fileName))
            {
                ParameterSave("map",fileName);
                statusBar()->showMessage(tr("File loaded: %1").arg(fileName), 2000);

                absolutePath = QFileInfo(fileName).absolutePath();
            }
        }
}

void MainWindow::openFile_slot(QString file)
{
    if (!file.isEmpty()) {
        QMdiSubWindow *existing = findMdiChild(file);
        if (existing) {
            mdiArea->setActiveSubWindow(existing);
            return;
        }
        if (openFile(file))
        {
            ParameterSave("map",file);
            statusBar()->showMessage(tr("File loaded: %1").arg(file), 2000);

            absolutePath = QFileInfo(file).absolutePath();
        }
    }
}

bool MainWindow::openFile(const QString &fileName)
{
    SubWidget *child = createMdiChild();
    QString temp_filename = QFileInfo(fileName).fileName();
    child->playback_tool->setWindowTitle(QString("MonitorDataPlay (%1)").arg(temp_filename));
    //child->agvctrl_tool->setWindowTitle(QString("AgvCtrlTool (%1)").arg(temp_filename));

    const bool succeeded = child->ui->graphicsView->loadFile(fileName);
    if (succeeded)
    {
        child->setWindowTitle(QString(child->ui->graphicsView->curFile));
        child->showMaximized();

        child->ui->graphicsView->self_adaption();
        child->save_items_of_scene();

        QString point_font = child->font;

        if(point_font.isEmpty())
        {
            for(int i = 0 ; i < child->vec_PointItem.size(); i++)
            {
                if(child->vec_PointItem[i] != nullptr)
                {
                    child->vec_PointItem[i]->point_font.setPointSize(40);
                }
            }

            ParameterSave("font",QString("40"));
        }
        else
        {
            for(int i = 0 ; i < child->vec_PointItem.size(); i++)
            {
                if(child->vec_PointItem[i] != nullptr)
                {
                    child->vec_PointItem[i]->point_font.setPointSize(point_font.toInt());
                }
            }
//            ParameterSave("font",QString("%1").arg(point_font.toInt()));
        }
//        qDebug()<<"parameerinit(5)   end------";
        QString font_S = child->font_S;
        if(font_S.isEmpty())
        {
            for(int i = 0 ; i < child->vec_PointItem.size(); i++)
            {
                if(child->vec_PointItem[i] != nullptr)
                {
                    child->vec_PointItem[i]->point_font_flag = 0;
                }
            }
            //
            control_pointfont->setChecked(true);
            ParameterSave("font_S",QString("0"));
        }
        else
        {
            if(font_S.toInt() == 0)
            {
                for(int i = 0 ; i < child->vec_PointItem.size(); i++)
                {
                    if(child->vec_PointItem[i] != nullptr)
                    {
                        child->vec_PointItem[i]->point_font_flag = 0;
                    }
                }
                control_pointfont->setChecked(true);
//                ParameterSave("font_S",QString("0"));
            }
            else
            {
                for(int i = 0 ; i < child->vec_PointItem.size(); i++)
                {
                    if(child->vec_PointItem[i] != nullptr)
                    {
                        child->vec_PointItem[i]->point_font_flag = 1;
                    }
                }
                control_pointfont->setChecked(false);
//                ParameterSave("font_S",QString("1"));
            }
        }

        QString area_s = child->area_status;
        if(area_s.isEmpty())
        {
            control_Area->setChecked(true);
            ParameterSave("area_status",QString("1"));
        }
        else
        {
            if(area_s.toInt() == 0)
            {
                control_Area->setChecked(false);
            }
            else
            {
                control_Area->setChecked(true);
            }
        }

        QString seg_width = child->seg_w;
        if(seg_width.isEmpty())
        {
            for(int i = 0 ; i < child->vec_LineItem.size(); i++)
            {
                if(child->vec_LineItem[i] != nullptr)
                {
                    child->vec_LineItem[i]->custom_line_width = 10;
                }
            }
            for(int i = 0 ; i < child->vec_ArcItem.size(); i++)
            {
                if(child->vec_ArcItem[i] != nullptr)
                {
                    child->vec_ArcItem[i]->custom_line_width = 5;
                }
            }
            ParameterSave("seg_w",QString("10"));
        }
        else
        {
            for(int i = 0 ; i < child->vec_LineItem.size(); i++)
            {
                if(child->vec_LineItem[i] != nullptr)
                {
                    child->vec_LineItem[i]->custom_line_width = seg_width.toInt();
                }
            }
            for(int i = 0 ; i < child->vec_ArcItem.size(); i++)
            {
                if(child->vec_ArcItem[i] != nullptr)
                {
                    child->vec_ArcItem[i]->custom_line_width = seg_width.toInt()/2;
                }
            }
//            ParameterSave("seg_w",seg_width);
        }
        QString route_color = child->routeColorName;
        QColor r_color = route_color.toUInt(NULL,16);
        if(route_color.isEmpty())
        {

        }
        else
        {
            for(int i = 0 ; i < child->vec_LineItem.size(); i++)
            {
                if(child->vec_LineItem[i] != nullptr)
                {
                    child->vec_LineItem[i]->routeColor = r_color;
                }
            }
            for(int i = 0 ; i < child->vec_ArcItem.size(); i++)
            {
                if(child->vec_ArcItem[i] != nullptr)
                {
                    child->vec_ArcItem[i]->routeColor = r_color;
                }
            }
        }

        QString arrow_n = child->arrow;
        if(arrow_n.isEmpty())
        {
            arrow_Act->DSpinBox->setValue(35);
            for(int i = 0 ; i < child->vec_LineItem.size(); i++)
            {
                if(child->vec_LineItem[i] != nullptr)
                {
                    child->vec_LineItem[i]->custom_n = 35;
                }
            }
            for(int i = 0 ; i < child->vec_ArcItem.size(); i++)
            {
                if(child->vec_ArcItem[i] != nullptr)
                {
                    child->vec_ArcItem[i]->custom_n = 35;
                }
            }
            ParameterSave("arrow",QString("35"));
        }
        else
        {
            arrow_Act->DSpinBox->setValue(arrow_n.toFloat());
            for(int i = 0 ; i < child->vec_LineItem.size(); i++)
            {
                if(child->vec_LineItem[i] != nullptr)
                {
                    child->vec_LineItem[i]->custom_n = arrow_n.toFloat();
                }
            }
            for(int i = 0 ; i < child->vec_ArcItem.size(); i++)
            {
                if(child->vec_ArcItem[i] != nullptr)
                {
                    child->vec_ArcItem[i]->custom_n = arrow_n.toFloat();
                }
            }
        }
#if 1
        QString arrow_l = child->arrow2;
        if(arrow_l.isEmpty())
        {
            arrow_Act2->DSpinBox->setValue(200);
            for(int i = 0 ; i < child->vec_LineItem.size(); i++)
            {
                if(child->vec_LineItem[i] != nullptr)
                {
                    child->vec_LineItem[i]->custom_l = 200;
                }
            }
            for(int i = 0 ; i < child->vec_ArcItem.size(); i++)
            {
                if(child->vec_ArcItem[i] != nullptr)
                {
                    child->vec_ArcItem[i]->custom_l = 200;
                }
            }
            ParameterSave("arrow2",QString("200"));
        }
        else
        {
            arrow_Act2->DSpinBox->setValue(arrow_l.toFloat());
            for(int i = 0 ; i < child->vec_LineItem.size(); i++)
            {
                if(child->vec_LineItem[i] != nullptr)
                {
                    child->vec_LineItem[i]->custom_l = arrow_l.toFloat();
                }
            }
            for(int i = 0 ; i < child->vec_ArcItem.size(); i++)
            {
                if(child->vec_ArcItem[i] != nullptr)
                {
                    child->vec_ArcItem[i]->custom_l = arrow_l.toFloat();
                }
            }
        }
#endif



        child->scene->update();
    }
    else
    {
        child->close();
    }
    return succeeded;

}

void MainWindow::tcp_connect_button_slot()
{

}




void MainWindow::save()
{
    if (activeMdiChild() && activeMdiChild()->ui->graphicsView->save())
        statusBar()->showMessage(tr("File saved"), 2000);
}

SubWidget *MainWindow::createMdiChild()
{
    SubWidget *subWidget = new SubWidget(this);
    connect(subWidget,&SubWidget::addToMidArea,this,&MainWindow::addToMidAreaSlot);
    connect(subWidget,&SubWidget::sendMessage,this,&MainWindow::showMessage);

    subWidget->myport = 9105 + subwidget_num;
    subwidget_num++;
    subWidget->undoStack = new QUndoStack(this);
    undoGroup->addStack(subWidget->undoStack);
    connect(subWidget->ui->graphicsView,SIGNAL(positionChanged(int,int)),this,SLOT(positionChanged(int,int)));
    mdiArea->addSubWindow(subWidget);

//    connect(subWidget,&SubWidget::send_area,subWidget,&SubWidget::get_area_value);

    return subWidget;
}

void MainWindow::addShape()
{
    SubWidget *sunWidget = activeMdiChild();
    if(sunWidget)
    {
        if ( sender() != measureAct && DrawTool::c_drawShape == measure)
        {
            sunWidget->ui->graphicsView->clearMeasureLine();
            sunWidget->ui->graphicsView->scene()->update();
            MeasureTool * tool = dynamic_cast<MeasureTool *>(DrawTool::findTool( DrawTool::c_drawShape ));
//            tool->resetVariable(dynamic_cast<DrawScene *>(sunWidget->ui->graphicsView->scene()));
            tool->resetVariable();

        }

    }

    if(DrawTool::c_drawShape != selection)
    {
        DrawTool * tool = dynamic_cast<DrawTool *>(DrawTool::findTool( DrawTool::c_drawShape ));
        tool->inited_drift = false;
        tool->inited_drift_one_param = false;

        if(sunWidget)
        {
            sunWidget->ui->graphicsView->scene()->removeItem(tool->driftItem);
            sunWidget->ui->graphicsView->scene()->removeItem(tool->driftItemRadius);
        }
    }

    if ( sender() == selectAct )
    {
        DrawTool::c_drawShape = selection;
        toolBtn->setChecked(false);
    }
    else if (sender() == lineAct )
    {
        DrawTool::c_drawShape = line;
        toolBtn->setChecked(false);
    }
    else if ( sender() == pointAct )
    {
        DrawTool::c_drawShape = point;
        toolBtn->setChecked(false);
    }
    else if ( sender() == arcAct )
    {
        DrawTool::c_drawShape = arc_startEndRadius;
        toolBtn->setChecked(false);
    }
    else if ( sender() == polylineAct )
    {
        DrawTool::c_drawShape = polyline ;
        toolBtn->setChecked(false);
    }
    else if ( sender() == polygonAct )
    {
        DrawTool::c_drawShape = polygon;
        toolBtn->setChecked(false);
    }
    else if ( sender() == bezierAct )
    {
        DrawTool::c_drawShape = bezier ;
        toolBtn->setChecked(false);
    }
    else if (sender() == polylineAct )
    {
        DrawTool::c_drawShape = polyline;
        toolBtn->setChecked(false);
    }
    else if ( sender() == interrputAct )
    {
        DrawTool::c_drawShape = interrput;
        toolBtn->setChecked(false);
    }
    else if ( sender() == mergeAct )
    {
        DrawTool::c_drawShape = merge;
        toolBtn->setChecked(false);
    }
    else if ( sender() == measureAct )
    {
        DrawTool::c_drawShape = measure;
        toolBtn->setChecked(false);
    }

    else if(sender() == arcAct_three_point )
    {
        toolBtn->setIcon(arcAct_three_point->icon());
        toolBtn->setChecked(true);
    }
    else if(sender() == arcAct_start_cen_angle )
    {
        toolBtn->setIcon(arcAct_start_cen_angle->icon());
        toolBtn->setChecked(true);
    }
    else if(sender() == arcAct_start_cen_end )
    {
        toolBtn->setIcon(arcAct_start_cen_end->icon());
        toolBtn->setChecked(true);
    }
    else if(sender() == arcAct_start_cen_length )
    {
        toolBtn->setIcon(arcAct_start_cen_length->icon());
        toolBtn->setChecked(true);
    }
    else if(sender() == arcAct_start_end_angle )
    {
        toolBtn->setIcon(arcAct_start_end_angle->icon());
        toolBtn->setChecked(true);
    }
    else if(sender() == arcAct_start_end_direct )
    {
        toolBtn->setIcon(arcAct_start_end_direct->icon());
        toolBtn->setChecked(true);
    }
    else if(sender() == arcAct_start_end_radius )
    {
        toolBtn->setIcon(arcAct_start_end_radius->icon());
        DrawTool::c_drawShape = arc_startEndRadius;
        toolBtn->setChecked(true);
    }
    else if(sender() == arcAct_cen_start_angle )
    {
        toolBtn->setIcon(arcAct_cen_start_angle->icon());
        toolBtn->setChecked(true);
    }
    else if(sender() == arcAct_cen_start_end )
    {
        toolBtn->setIcon(arcAct_cen_start_end->icon());
        toolBtn->setChecked(true);
    }
    else if(sender() == arcAct_cen_start_length )
    {
        toolBtn->setIcon(arcAct_cen_start_length->icon());
        toolBtn->setChecked(true);
    }
    else if(sender() == arcAct_continu )
    {
        toolBtn->setIcon(arcAct_continu->icon());
        DrawTool::c_drawShape = arc_directRadiusAngle;
        toolBtn->setChecked(true);
    }

    if ( sender() != selectAct && sender() != rotateAct && sunWidget){
        sunWidget->ui->graphicsView->scene()->clearSelection();
    }

}

void MainWindow::deleteItem()
{
//    qDebug()<<"deleteItem";
    if (!activeMdiChild()) return ;
    activeMdiChild()->deleteItem();
}


void MainWindow::valueChangedConfig(QtProperty *property, const QVariant &value)
{
    SubWidget *subWidget = activeMdiChild();
    if (!subWidget) return ;
    QGraphicsScene * scene = subWidget->ui->graphicsView->scene();

//    if (!propertyToId.contains(property))
//        return;

    QString id = m_propertyToId[property];

    //qDebug() << id << value.toInt();

    if(id == QStringLiteral("X最小值："))
    {
        //qDebug() << id << value.toInt();
        subWidget->ui->graphicsView->mapConfig[ subWidget->ui->graphicsView->cur_map_index].x_min = value.toInt();
    }
    else if(id == QStringLiteral("X最大值："))
    {
        //qDebug() << id << value.toInt();
        subWidget->ui->graphicsView->mapConfig[ subWidget->ui->graphicsView->cur_map_index].x_max = value.toInt();
    }
    else if(id == QStringLiteral("Y最小值："))
    {
        //qDebug() << id << value.toInt();
        subWidget->ui->graphicsView->mapConfig[ subWidget->ui->graphicsView->cur_map_index].y_min = value.toInt();
    }
    else if(id == QStringLiteral("Y最大值："))
    {
        //qDebug() << id << value.toInt();
        subWidget->ui->graphicsView->mapConfig[ subWidget->ui->graphicsView->cur_map_index].y_max = value.toInt();
    }
    else if(id == QStringLiteral("地图id："))
    {
        //qDebug() << id << value.toInt();
        subWidget->ui->graphicsView->mapConfig[ subWidget->ui->graphicsView->cur_map_index].mapId = value.toInt();
    }
    else if(id == QStringLiteral("站点大小："))
    {
        //qDebug() << id << value.toInt();
        subWidget->ui->graphicsView->mapConfig[ subWidget->ui->graphicsView->cur_map_index].map_point_width = value.toInt();
        subWidget->ui->graphicsView->scene()->update();
    }
    else if(id == QStringLiteral("字体大小："))
    {
        //qDebug() << id << value.toInt();
        subWidget->ui->graphicsView->mapConfig[ subWidget->ui->graphicsView->cur_map_index].map_front_size = value.toInt();
        subWidget->ui->graphicsView->scene()->update();
    }
    else if(id == QStringLiteral("片段宽度："))
    {
        //qDebug() << id << value.toInt();
        subWidget->ui->graphicsView->mapConfig[ subWidget->ui->graphicsView->cur_map_index].map_segment_width = value.toInt();
        subWidget->ui->graphicsView->scene()->update();
    }

    int Margin = 10000;
    scene->setSceneRect(subWidget->ui->graphicsView->mapConfig[ subWidget->ui->graphicsView->cur_map_index].x_min-Margin, subWidget->ui->graphicsView->mapConfig[ subWidget->ui->graphicsView->cur_map_index].y_min-Margin,
                        subWidget->ui->graphicsView->mapConfig[ subWidget->ui->graphicsView->cur_map_index].x_max - subWidget->ui->graphicsView->mapConfig[ subWidget->ui->graphicsView->cur_map_index].x_min+Margin*2
                        ,subWidget->ui->graphicsView->mapConfig[ subWidget->ui->graphicsView->cur_map_index].y_max - subWidget->ui->graphicsView->mapConfig[ subWidget->ui->graphicsView->cur_map_index].y_min+Margin*2);

}


void MainWindow::valueChangedConfigEnum(QtProperty *property, int value)
{
    QString id = m_propertyToId[property];

    //qDebug() << id << value;
    if(id == QStringLiteral("环的方向："))
    {
        activeMdiChild()->ui->graphicsView->mapConfig[activeMdiChild()->ui->graphicsView->cur_map_index].clock_dir = value;
    }
}

void MainWindow::showMessage(QString mes)
{
//    statusBar()->showMessage(mes);
    statusBarLabel->setText(mes);
}
#if 0
void MainWindow::on_func_test_triggered()
{
       SubWidget * activeView = activeMdiChild();
       if (!activeView)
       {
           QMessageBox::warning(this,"Warning", "请先打开地图或新建地图文件    ", QMessageBox::Yes, QMessageBox::Yes);
           return ;
       }

       QtTreePropertyBrowser *editor = new QtTreePropertyBrowser();
       m_propertyToId.clear();
       m_idToProperty.clear();
       QtVariantPropertyManager *m_pVarManager = new QtVariantPropertyManager();



       QtProperty *groupItem =m_pVarManager->addProperty(QtVariantPropertyManager::groupTypeId(),QStringLiteral("组1"));
       QtVariantProperty *item = m_pVarManager->addProperty(QVariant::Int, QStringLiteral("整形数据："));
       item->setValue(101);
       QString id = QStringLiteral("整形数据：");
       m_propertyToId[item] = id;
       m_idToProperty[id] = item;
       groupItem->addSubProperty(item);

       item =m_pVarManager->addProperty(QVariant::Bool, QStringLiteral("布尔型数据："));
       item->setValue(true);
       id = QStringLiteral("布尔型数据：");
       m_propertyToId[item] = id;
       m_idToProperty[id] = item;
       groupItem->addSubProperty(item);

       item =m_pVarManager->addProperty(QVariant::Double, QStringLiteral("浮点数据："));
       item->setValue(3.1415926);
       id = QStringLiteral("浮点数据：");
       m_propertyToId[item] = id;
       m_idToProperty[id] = item;
       groupItem->addSubProperty(item);

       item =m_pVarManager->addProperty(QVariant::String, QStringLiteral("字符串数据："));
       item->setValue(QStringLiteral("尘中远"));
       id = QStringLiteral("字符串数据：");
       m_propertyToId[item] = id;
       m_idToProperty[id] = item;
       groupItem->addSubProperty(item);


       QtEnumPropertyManager *m_pEnumManager = new QtEnumPropertyManager();
       QtProperty *enum_item = m_pEnumManager->addProperty(tr("区域"));
       QStringList styleNames;
       styleNames<<tr("A")<<tr("B")<<tr("C")<<tr("D")
                 <<tr("E")<<tr("F")<<tr("G");
       m_pEnumManager->setEnumNames(enum_item,styleNames);
       m_pEnumManager->setValue(enum_item,0);
       groupItem->addSubProperty(enum_item);
       m_propertyToId[enum_item] = QStringLiteral("区域");

//       editor->addProperty(groupItem);

       //地图配置
       QtProperty *groupItemConfig = m_pVarManager->addProperty(QtVariantPropertyManager::groupTypeId(),QStringLiteral("地图配置"));
       item = m_pVarManager->addProperty(QVariant::Int, QStringLiteral("X最小值："));
       qDebug()<<"此时场景x最小值："<<activeView->ui->graphicsView->mapConfig.x_min;
       item->setValue(activeView->ui->graphicsView->mapConfig.x_min);
       qDebug()<<"赋值操作进行";
       id = QStringLiteral("X最小值：");
       m_propertyToId[item] = id;
       m_idToProperty[id] = item;
       groupItemConfig->addSubProperty(item);

       item = m_pVarManager->addProperty(QVariant::Int, QStringLiteral("X最大值："));
       item->setValue(activeView->ui->graphicsView->mapConfig.x_max);
       id = QStringLiteral("X最大值：");
       m_propertyToId[item] = id;
       m_idToProperty[id] = item;
       groupItemConfig->addSubProperty(item);

       item = m_pVarManager->addProperty(QVariant::Int, QStringLiteral("Y最小值："));
       item->setValue(activeView->ui->graphicsView->mapConfig.y_min);
       id = QStringLiteral("Y最小值：");
       m_propertyToId[item] = id;
       m_idToProperty[id] = item;
       groupItemConfig->addSubProperty(item);

       item = m_pVarManager->addProperty(QVariant::Int, QStringLiteral("Y最大值："));
       item->setValue(activeView->ui->graphicsView->mapConfig.y_max);
       id = QStringLiteral("Y最大值：");
       m_propertyToId[item] = id;
       m_idToProperty[id] = item;
       groupItemConfig->addSubProperty(item);

       item = m_pVarManager->addProperty(QVariant::Int, QStringLiteral("地图id："));
       item->setValue(activeView->ui->graphicsView->mapConfig.mapId);
       id = QStringLiteral("地图id：");
       m_propertyToId[item] = id;
       m_idToProperty[id] = item;
       groupItemConfig->addSubProperty(item);

       item = m_pVarManager->addProperty(QVariant::Int, QStringLiteral("环起始点角度："));
       item->setValue(activeView->ui->graphicsView->mapConfig.startAngle);
       id = QStringLiteral("环起始点角度：");
       m_propertyToId[item] = id;
       m_idToProperty[id] = item;
       groupItemConfig->addSubProperty(item);

       enum_item = m_pEnumManager->addProperty(tr("环的方向："));
       styleNames.clear();
       styleNames<<tr("逆时针")<<tr("<-->")<<tr("顺时针");
       m_pEnumManager->setEnumNames(enum_item,styleNames);
       m_pEnumManager->setValue(enum_item,activeView->ui->graphicsView->mapConfig.clock_dir);
       id = QStringLiteral("环的方向：");
       m_propertyToId[enum_item] = id;
       groupItemConfig->addSubProperty(enum_item);

       item = m_pVarManager->addProperty(QVariant::Int, QStringLiteral("最大物流量/小时："));
       item->setValue(activeView->ui->graphicsView->mapConfig.maxFlow);
       id = QStringLiteral("最大物流量/小时：");
       m_propertyToId[item] = id;
       m_idToProperty[id] = item;
       groupItemConfig->addSubProperty(item);

       editor->addProperty(groupItemConfig);

       editor->setFactoryForManager(m_pVarManager,new QtVariantEditorFactory());
       editor->setFactoryForManager(m_pEnumManager,new QtEnumEditorFactory());

       connect(m_pVarManager, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
                   this, SLOT(valueChangedConfig(QtProperty *, const QVariant &)));
       connect(m_pEnumManager, SIGNAL(valueChanged(QtProperty *, int)),
                   this, SLOT(valueChangedConfigEnum(QtProperty *, int )));
       editor->resize(300,500);
       editor->show();


/*
        QtGradientEditor * editor = new QtGradientEditor(NULL);
        editor->show();
*/
//    dockProperty->showNormal();
}
#endif
void MainWindow::on_func_test_triggered()
{
       SubWidget * activeView = activeMdiChild();
       if (!activeView)
       {
           QMessageBox::warning(this,"Warning", "请先打开地图或新建地图文件    ", QMessageBox::Yes, QMessageBox::Yes);
           return ;
       }

       QtTreePropertyBrowser *editor = new QtTreePropertyBrowser();
       m_propertyToId.clear();
       m_idToProperty.clear();
       QtVariantPropertyManager *m_pVarManager = new QtVariantPropertyManager();
       QtProperty *groupItem =m_pVarManager->addProperty(QtVariantPropertyManager::groupTypeId(),QStringLiteral("组1"));
       QtVariantProperty *item = m_pVarManager->addProperty(QVariant::Int, QStringLiteral("整形数据："));
       item->setValue(101);
       QString id = QStringLiteral("整形数据：");
       m_propertyToId[item] = id;
       m_idToProperty[id] = item;
       groupItem->addSubProperty(item);

       item =m_pVarManager->addProperty(QVariant::Bool, QStringLiteral("布尔型数据："));
       item->setValue(true);
       id = QStringLiteral("布尔型数据：");
       m_propertyToId[item] = id;
       m_idToProperty[id] = item;
       groupItem->addSubProperty(item);

       item =m_pVarManager->addProperty(QVariant::Double, QStringLiteral("浮点数据："));
       item->setValue(3.1415926);
       id = QStringLiteral("浮点数据：");
       m_propertyToId[item] = id;
       m_idToProperty[id] = item;
       groupItem->addSubProperty(item);

       item =m_pVarManager->addProperty(QVariant::String, QStringLiteral("字符串数据："));
       item->setValue(QStringLiteral("尘中远"));
       id = QStringLiteral("字符串数据：");
       m_propertyToId[item] = id;
       m_idToProperty[id] = item;
       groupItem->addSubProperty(item);


       QtEnumPropertyManager *m_pEnumManager = new QtEnumPropertyManager();
       QtProperty *enum_item = m_pEnumManager->addProperty(tr("区域"));
       QStringList styleNames;
       styleNames<<tr("A")<<tr("B")<<tr("C")<<tr("D")
                 <<tr("E")<<tr("F")<<tr("G");
       m_pEnumManager->setEnumNames(enum_item,styleNames);
       m_pEnumManager->setValue(enum_item,0);
       groupItem->addSubProperty(enum_item);
       m_propertyToId[enum_item] = QStringLiteral("区域");

//       editor->addProperty(groupItem);

       //地图配置
       QtProperty *groupItemConfig = m_pVarManager->addProperty(QtVariantPropertyManager::groupTypeId(),QStringLiteral("地图配置"));
       item = m_pVarManager->addProperty(QVariant::Int, QStringLiteral("X最小值："));
       item->setValue(activeView->ui->graphicsView->mapConfig[activeView->ui->graphicsView->cur_map_index].x_min);
       id = QStringLiteral("X最小值：");
       m_propertyToId[item] = id;
       m_idToProperty[id] = item;
       groupItemConfig->addSubProperty(item);

       item = m_pVarManager->addProperty(QVariant::Int, QStringLiteral("X最大值："));
       item->setValue(activeView->ui->graphicsView->mapConfig[activeView->ui->graphicsView->cur_map_index].x_max);
       id = QStringLiteral("X最大值：");
       m_propertyToId[item] = id;
       m_idToProperty[id] = item;
       groupItemConfig->addSubProperty(item);

       item = m_pVarManager->addProperty(QVariant::Int, QStringLiteral("Y最小值："));
       item->setValue(activeView->ui->graphicsView->mapConfig[activeView->ui->graphicsView->cur_map_index].y_min);
       id = QStringLiteral("Y最小值：");
       m_propertyToId[item] = id;
       m_idToProperty[id] = item;
       groupItemConfig->addSubProperty(item);

       item = m_pVarManager->addProperty(QVariant::Int, QStringLiteral("Y最大值："));
       item->setValue(activeView->ui->graphicsView->mapConfig[activeView->ui->graphicsView->cur_map_index].y_max);
       id = QStringLiteral("Y最大值：");
       m_propertyToId[item] = id;
       m_idToProperty[id] = item;
       groupItemConfig->addSubProperty(item);

       item = m_pVarManager->addProperty(QVariant::Int, QStringLiteral("地图id："));
       item->setValue(activeView->ui->graphicsView->mapConfig[activeView->ui->graphicsView->cur_map_index].mapId);
       id = QStringLiteral("地图id：");
       m_propertyToId[item] = id;
       m_idToProperty[id] = item;
       groupItemConfig->addSubProperty(item);

       item = m_pVarManager->addProperty(QVariant::Int, QStringLiteral("站点大小："));
       item->setValue(activeView->ui->graphicsView->mapConfig[activeView->ui->graphicsView->cur_map_index].map_point_width);
       id = QStringLiteral("站点大小：");
       m_propertyToId[item] = id;
       m_idToProperty[id] = item;
       groupItemConfig->addSubProperty(item);

       item = m_pVarManager->addProperty(QVariant::Int, QStringLiteral("字体大小："));
       item->setValue(activeView->ui->graphicsView->mapConfig[activeView->ui->graphicsView->cur_map_index].map_front_size);
       id = QStringLiteral("字体大小：");
       m_propertyToId[item] = id;
       m_idToProperty[id] = item;
       groupItemConfig->addSubProperty(item);

       item = m_pVarManager->addProperty(QVariant::Int, QStringLiteral("片段宽度："));
       item->setValue(activeView->ui->graphicsView->mapConfig[activeView->ui->graphicsView->cur_map_index].map_segment_width);
       id = QStringLiteral("片段宽度：");
       m_propertyToId[item] = id;
       m_idToProperty[id] = item;
       groupItemConfig->addSubProperty(item);

//       enum_item = m_pEnumManager->addProperty(tr("环的方向："));
//       styleNames.clear();
//       styleNames<<tr("逆时针")<<tr("<-->")<<tr("顺时针");
//       m_pEnumManager->setEnumNames(enum_item,styleNames);
//       m_pEnumManager->setValue(enum_item,activeView->ui->graphicsView->mapConfig.clock_dir);
//       id = QStringLiteral("环的方向：");
//       m_propertyToId[enum_item] = id;
//       groupItemConfig->addSubProperty(enum_item);

       editor->addProperty(groupItemConfig);

       editor->setFactoryForManager(m_pVarManager,new QtVariantEditorFactory());
       editor->setFactoryForManager(m_pEnumManager,new QtEnumEditorFactory());

       connect(m_pVarManager, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
                   this, SLOT(valueChangedConfig(QtProperty *, const QVariant &)));
       connect(m_pEnumManager, SIGNAL(valueChanged(QtProperty *, int)),
                   this, SLOT(valueChangedConfigEnum(QtProperty *, int )));
       editor->resize(300,500);
       editor->show();


/*
        QtGradientEditor * editor = new QtGradientEditor(NULL);
        editor->show();
*/
//    dockProperty->showNormal();
}

void MainWindow::positionChanged(int x, int y)
{
   char buf[255];
   sprintf(buf,"%d,%d",x,y);
   statusBar()->showMessage(buf);
}

void MainWindow::subWidgetSwitched(QMdiSubWindow *arg1)
{
//    qDebug() << "subWidgetSwitched";
    if (mdiArea->subWindowList().count()==0)
    { //若子窗口个数为零

    }
    else
    {
//        SubWidget *subWidget=static_cast<SubWidget*>(mdiArea->activeSubWindow()->widget());
//        subWidget->undoStack->setActive(true);

//        //窗口切换时或者新建窗口时，更新是否显示网格
//        dynamic_cast<DrawScene *>(subWidget->ui->graphicsView->scene())->show_grid = gridAct->isChecked();
//        dynamic_cast<DrawScene *>(subWidget->ui->graphicsView->scene())->drawGrid(gridAct->isChecked());
//        subWidget->ui->graphicsView->scene()->update();
    }


    //if(DrawTool::c_drawShape != selection)
    {
        DrawTool * tool = dynamic_cast<DrawTool *>(DrawTool::findTool( DrawTool::c_drawShape ));
        tool->inited_drift = false;
        tool->inited_drift_one_param = false;

//        if(activeMdiChild())
//        {
//            activeMdiChild()->ui->graphicsView->scene()->removeItem(tool->driftItem);
//            activeMdiChild()->ui->graphicsView->scene()->removeItem(tool->driftItemRadius);
//        }
    }
}

void MainWindow::find()
{
    if(!activeMdiChild())
    {
        QMessageBox::warning(this,"Warning", "    请先打开地图文件！   ");
        return;
    }
    else
    {
        findDialog->show();
        findDialog->ui->lineEdit_id->setFocus();

    }
}

void MainWindow::update()
{
    if(!activeMdiChild())
    {
        QMessageBox::warning(this,"Warning", "    请先打开地图文件！   ");
        return;
    }
    else
    {
        activeMdiChild()->ui->graphicsView->updateAssistPoint();
    }
}

void MainWindow::showGrid(bool show_grid)
{
    //qDebug() << "show Grid" << show_grid;
    SubWidget * subWidget = activeMdiChild();
    if(subWidget)
    {
        dynamic_cast<DrawScene *>(subWidget->ui->graphicsView->scene())->show_grid = show_grid;
        dynamic_cast<DrawScene *>(subWidget->ui->graphicsView->scene())->drawGrid(show_grid);

        subWidget->ui->graphicsView->scene()->update();

    }
}

void MainWindow::moveItems()
{
    SubWidget *subWidget = activeMdiChild();
    if(!subWidget)
    {
        QMessageBox::warning(this,"Warning", "    请先打开地图文件！   ");
        return;
    }
    else
    {
        if(subWidget->ui->graphicsView->scene()->selectedItems().count() == 0)
        {
            QMessageBox::warning(this,"Warning", "    请先选择平移的元素！   ");
        }
        else
        {
            MoveItem *moveItem = new MoveItem(this);
            int result = moveItem->exec();
            if(result == QDialog::Accepted)
            {
                if(moveItem->ui->lineEdit_dx->text().isEmpty())
                {
                    QMessageBox::warning(this,"Warning", "    请输入平移的距离:dx ！   ");
                    return;
                }
                else if(moveItem->ui->lineEdit_dy->text().isEmpty())
                {
                    QMessageBox::warning(this,"Warning", "    请输入平移的距离:dy ！   ");
                    return;
                }

                subWidget->ui->graphicsView->moveItems(moveItem->ui->lineEdit_dx->text().toInt(),moveItem->ui->lineEdit_dy->text().toInt());
            }
            else if(result == QDialog::Rejected)
            {

            }
        }
    }
}

void MainWindow::copyItems()
{
    SubWidget *subWidget = activeMdiChild();
    if(!subWidget)
    {
        QMessageBox::warning(this,"Warning", "    请先打开地图文件！   ");
        return;
    }
    else
    {
        if(subWidget->ui->graphicsView->scene()->selectedItems().count() == 0)
        {
            QMessageBox::warning(this,"Warning", "    请先选择平移的元素！   ");
        }
        else
        {
            MoveItem *copyItem = new MoveItem(this);
            copyItem->setWindowTitle(tr("Copy"));
            copyItem->ui->label_dx->setText(tr("复制dx:"));
            copyItem->ui->label_dy->setText(tr("复制dy:"));
            int result = copyItem->exec();
            if(result == QDialog::Accepted)
            {
                if(copyItem->ui->lineEdit_dx->text().isEmpty())
                {
                    QMessageBox::warning(this,"Warning", "    请输入复制的距离:dx ！   ");
                    return;
                }
                else if(copyItem->ui->lineEdit_dy->text().isEmpty())
                {
                    QMessageBox::warning(this,"Warning", "    请输入复制的距离:dy ！   ");
                    return;
                }

                subWidget->ui->graphicsView->copyItems(copyItem->ui->lineEdit_dx->text().toInt(),copyItem->ui->lineEdit_dy->text().toInt());
            }
            else if(result == QDialog::Rejected)
            {

            }
        }
    }
}

void MainWindow::rotateItems()
{
    //qDebug() << "rotateItems";
    SubWidget *subWidget = activeMdiChild();
    if(!subWidget)
    {
        QMessageBox::warning(this,"Warning", "    请先打开地图文件！   ");
        return;
    }
    if(subWidget->ui->graphicsView->scene()->selectedItems().count() == 0)
    {
        QMessageBox::warning(this,"Warning", "    请先选择平移的元素！   ");
    }
    else
    {
        RotateItem *rotateItem = new RotateItem(this);
        int result = rotateItem->exec();
        if(result == QDialog::Accepted)
        {
            if(rotateItem->ui->lineEdit_x->text().isEmpty())
            {
                QMessageBox::warning(this,"Warning", "    请输入旋转中心点坐标:X ！   ");
                return;
            }
            else if(rotateItem->ui->lineEdit_y->text().isEmpty())
            {
                QMessageBox::warning(this,"Warning", "    请输入旋转中心点坐标:Y ！   ");
                return;
            }
            else if(rotateItem->ui->lineEdit_angle->text().isEmpty())
            {
                QMessageBox::warning(this,"Warning", "    请输入旋转角度 ！   ");
                return;
            }
            subWidget->ui->graphicsView->rotateItems(rotateItem->ui->lineEdit_x->text().toInt(),rotateItem->ui->lineEdit_y->text().toInt(),rotateItem->ui->lineEdit_angle->text().toFloat());
        }
        else if(result == QDialog::Rejected)
        {

        }
    }
}


void MainWindow::whole_map_slot()
{
    activeMdiChild()->ui->graphicsView->self_adaption();
}

void MainWindow::whole_floor_show(int floor)
{
    qDebug()<<tr("楼层:")<<floor;
    //取避让区 16 的范围
    SubWidget *subWidget = activeMdiChild();
    if(!subWidget)
    {
        QMessageBox::warning(this,"Warning", "    请先打开地图文件！   ");
        return;
    }
    else
    {
        int cur_map_index = subWidget->ui->graphicsView->cur_map_index;
        for(auto it = subWidget->ui->graphicsView->map_mapAreaItem[cur_map_index].begin(); it != subWidget->ui->graphicsView->map_mapAreaItem[cur_map_index].end(); it++)
        {
            //楼层区域
            if(it.value()->map_area_pb.basic().type() == 16 && it.value()->map_area_pb.param().contains(0))
            {
                //qDebug()<<tr("1区域id:")<<it.value()->map_area_pb.basic().id()<<tr(",:")<<it.value()->map_area_pb.param().at(0);
                if(it.value()->map_area_pb.param().at(0) == floor)
                {
                    //获取  最小x/最小y,最大x,最大y
                    int min_x = -75000,min_y = -55000,max_x = 0,max_y = 0;
                    for(int i = 0;  i < it.value()->map_area_pb.coord_size(); i++)
                    {
                        if(i == 0)
                        {
                            min_x = it.value()->map_area_pb.coord(i).x();
                            max_y = it.value()->map_area_pb.coord(i).x();
                            min_y = it.value()->map_area_pb.coord(i).y();
                            max_y = it.value()->map_area_pb.coord(i).y();
                        }else{
                            int x = it.value()->map_area_pb.coord(i).x();
                            int y = it.value()->map_area_pb.coord(i).y();
                            if(x < min_x) min_x = x;
                            if(x > max_x) max_x = x;

                            if(y < min_y) min_y = y;
                            if(y > max_y) max_y = y;
                        }
                    }
                    activeMdiChild()->ui->graphicsView->fitInView(QRectF(min_x,min_y,max_x-min_x,max_y-min_y),Qt::KeepAspectRatio);
                    return;
                }
            }
        }

        //不存在显示全部
        subWidget->ui->graphicsView->self_adaption();
    }
}

void MainWindow::clockwise_slot()
{
    activeMdiChild()->ui->graphicsView->clockwise();
}

void MainWindow::anti_clockwise_slot()
{
    activeMdiChild()->ui->graphicsView->anti_clockwise();
}

void MainWindow::change_scene_color()
{
//    QColorDialog::getColor(Qt::white,this,"选择你心仪的颜色");
     DrawScene * my_DrawScene = dynamic_cast<DrawScene*>(activeMdiChild()->ui->graphicsView->scene());
     my_DrawScene->scene_color = QColorDialog::getColor(Qt::white,this,"选择你心仪的颜色");
     my_DrawScene->setBackgroundBrush(my_DrawScene->scene_color);

     QRgb mRgb = qRgb(my_DrawScene->scene_color.red(),my_DrawScene->scene_color.green(),my_DrawScene->scene_color.blue());
     QString mRgbStr = QString::number(mRgb,16);
     ParameterSave("color",mRgbStr);

}

void MainWindow::change_route_color()
{
    QColor routeColor = QColorDialog::getColor(Qt::white,this,"选择你心仪的颜色");
    QRgb mRgb = qRgb(routeColor.red(),routeColor.green(),routeColor.blue());
    QString mRgbStr = QString::number(mRgb,16);


    if(activeMdiChild() == 0)
    {
        QMessageBox::warning(this,"","请先打开地图！");
    }
    else
    {
        SubWidget *tempSubWidget = activeMdiChild();

        for(int i = 0 ; i < tempSubWidget->vec_LineItem.size(); i++)
        {
            if(tempSubWidget->vec_LineItem[i] != nullptr)
            {
                tempSubWidget->vec_LineItem[i]->routeColor = routeColor;
            }
        }
        for(int i = 0 ; i < tempSubWidget->vec_ArcItem.size(); i++)
        {
            if(tempSubWidget->vec_ArcItem[i] != nullptr)
            {
                tempSubWidget->vec_ArcItem[i]->routeColor = routeColor;
            }
        }

        tempSubWidget->scene->update();
        ParameterSave("rcolor",mRgbStr);
    }


}


void MainWindow::change_point_font()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok,this);
    if(ok)
    {
        SubWidget *tempsubWidget = activeMdiChild();
        if(tempsubWidget->has_save_items_of_scene == 0)
        {
            tempsubWidget->save_items_of_scene();
        }
        for(int i = 0 ; i < tempsubWidget->vec_PointItem.size(); i++)
        {
            if(tempsubWidget->vec_PointItem[i] != nullptr)
            {
                tempsubWidget->vec_PointItem[i]->point_font = font;
            }
        }
        tempsubWidget->scene->update();
        ParameterSave("font",QString("%1").arg(font.pointSize()));
    }
    else
        qDebug()<<tr("没有选择字体");
}

void MainWindow::carTransparency_slot()
{
    if(activeMdiChild() == 0)
    {
        QMessageBox::warning(this,"","请先打开地图！");
    }
    else
    {
        SubWidget *tempSubWidget = activeMdiChild();

        if(tempSubWidget->vec_Car.size() > 0)
        {
            if(tempSubWidget->vec_Car[0]->transparency != 0)
            {
                change_cartransparency(0);
            }
            else
            {
                change_cartransparency(255);
            }
        }
        else
        {
            return;
        }

    }
}

void MainWindow::station_background_slot()
{
    if(activeMdiChild() == 0)
    {
        QMessageBox::warning(this,"","请先打开地图！");
    }
    else
    {
        SubWidget *tempSubWidget = activeMdiChild();
        tempSubWidget->station_background_slot();
    }
}


void MainWindow::mousePressEvent(QMouseEvent *event)
{

    if( event->button() == Qt::LeftButton)
    {
        m_Press = event->globalPos();
        leftBtnClk = true;
    }
    else
    {

    }
    event->ignore();
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{

    if( event->button() == Qt::LeftButton ){
        leftBtnClk = false;
    }
    event->ignore();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if( leftBtnClk ){
        m_Move = event->globalPos();
        move(this->pos() + m_Move - m_Press );

        m_Press = m_Move;
    }
    event->ignore();
}



void MainWindow::on_actionBringToFront_triggered()
{
    SubWidget *subWidget = activeMdiChild();
    if (!subWidget) return ;
    QGraphicsScene * scene = subWidget->ui->graphicsView->scene();

    if (scene->selectedItems().isEmpty())
        return;

    QGraphicsItem *selectedItem = scene->selectedItems().first();

    QList<QGraphicsItem *> overlapItems = selectedItem->collidingItems();
    qreal zValue = 0;
    foreach (QGraphicsItem *item, overlapItems) {
        if (item->zValue() >= zValue && (item->type() == GraphicsAreaItem::Type || item->type() == GraphicsArcItem::Type))
            zValue = item->zValue() + 0.01;
    }
    selectedItem->setZValue(zValue);

}

void MainWindow::on_actionSendToBack_triggered()
{
    SubWidget *subWidget = activeMdiChild();
    if (!subWidget) return ;
    QGraphicsScene * scene = subWidget->ui->graphicsView->scene();

    if (scene->selectedItems().isEmpty())
        return;

    QGraphicsItem *selectedItem = scene->selectedItems().first();
    QList<QGraphicsItem *> overlapItems = selectedItem->collidingItems();

    qreal zValue = 0;
    foreach (QGraphicsItem *item, overlapItems) {
        if (item->zValue() <= zValue && item->type() == GraphicsAreaItem::Type)
            zValue = item->zValue() - 0.01;
    }
    selectedItem->setZValue(zValue);
}


