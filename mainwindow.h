#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QTimer>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include "drawscene.h"
#include "drawview.h"
#include <QUndoStack>
#include <QSqlQueryModel>
#include "commands.h"
#include "qtvariantproperty.h"
#include "qttreepropertybrowser.h"
#include <qtpropertymanager.h>
#include <qteditorfactory.h>
#include "checksum.h"
#include "monitor.h"
#include "addtaskview.h"
#include "updatestation.h"
#include "subwidget.h"
#include "segment_action.h"
#include "arrow_action.h"
#include "cartrans_action.h"
#include "registrydialog.h"
#include "pointsize_action.h"
#include "find.h"
QT_BEGIN_NAMESPACE
class QAction;
class QActionGroup;
class QToolBox;
class QSpinBox;
class QComboBox;
class QFontComboBox;
class QButtonGroup;
class QLineEdit;
class QGraphicsTextItem;
class QFont;
class QToolButton;
class QAbstractButton;
class QGraphicsView;
class QListWidget;
class QStatusBar;
class QMdiArea;
class QMdiSubWindow;
class QSignalMapper;
class QUndoStack;
class QUndoView;
class QPushButton;
class Car;
class Car_Info;
class QLabel;

QT_END_NAMESPACE



#pragma pack(push, 1)



#pragma pack(pop)





class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    bool openFile(const QString &fileName);

    void tcp_connect_button_slot();

     int have_open_path_flag = 0;
    QString absolutePath =".";
    int subwidget_num = 0;
    QMenu *viewMenu;
    int speedMDFlag = 0;
    QString speedMDStr = "";


    AgvCtrl_Tool *agvctrl_tool = nullptr;
public slots:
    void newFile();
    void open();
    void openFile_slot(QString file);
    void save();
    SubWidget *createMdiChild();
    void updateWindowMenu();

    void addShape();
    void deleteItem();
    void valueChangedConfig(QtProperty *property, const QVariant &value);
    void valueChangedConfigEnum(QtProperty *property,int value);

    void showMessage(QString mes);

    void zoomIn();
    void zoomOut();
//    void creatMap();
    void on_actionBringToFront_triggered();
    void on_actionSendToBack_triggered();

    void on_func_test_triggered();

    void positionChanged(int x, int y );

    void subWidgetSwitched(QMdiSubWindow *arg1);

    void find();

    void update();

    void showGrid(bool show_grid);

    void moveItems();
    void copyItems();

    void rotateItems();
    void whole_map_slot();
    void whole_floor_show(int floor);
    void clockwise_slot();
    void anti_clockwise_slot();
    void change_scene_color();
    void change_route_color();
    void change_point_font();
    void carTransparency_slot();
    void station_background_slot();
//    void carNumber_combobox_slot(int);
    QString ParameterInit(int flag);
    void ParameterInit();
    void ParameterSave(QString Type, QString p);
    void config_open();
    void pointfont_control();
    void avoid_Area_control();
    void floatwindow_control();
    void saBackground_control();
    void errorInfoHint_control();
    void control_propertyList_slot();
    void speedDisplayMode_control();
    void change_segment_width(QString widthValue);
    void change_arrow_height(float arrow_float);
    void change_arrow_width(float arrow_float);
    void change_cartransparency(int trans);
    void change_pointSize(int size);
    void show_playback_tool();
    void show_agvctrl_tool();
    void show_batteryInfoWidget();
    void show_taskInfoDialog();
    void show_taskInfoSpecialDialog();
    void show_cxjDataDialog();
    void show_cxjBackDialog();
    void showFullScreenSlot();
    void addToMidAreaSlot(SubWidget *subwidget);
    void showLiftLine(bool show_line);
    void showDisable(bool show_disable);
    void findDialogSlot(int);

    void actionsSlot();
signals:
public:
    QString fileName;
    QString colorName;
    QString speedDM;
    QString skin;
    QString ip;
    QLabel *statusBarLabel;
    Find *findDialog;
private:
    void createActions();
    void createMenus();
    void createToolbars();

    void createToolBox();
    void reloadmap();
    void checkUpdateApp();
    void showDebugWidget();
    void showCarSizeWidget();
    QLineEdit *tcp_lineEdit;
    RegistryDialog *registryDialog;

//    QComboBox *carNumber_combobox;
    QLabel *carNumber_label;

    QPushButton *oneCarInfo_button;
    SubWidget * subwidget_point;

    SubWidget *activeMdiChild();
    QMdiSubWindow *findMdiChild(const QString &fileName);

    QMenu *windowMenu;
    QMenu *skinMenu;
    QMenu *floorMenu;
    QToolButton *toolBtn;
    QPushButton *trackBtn;

    QMdiArea *mdiArea;
    QSignalMapper *windowMapper;

    // update ui
    QTimer      m_timer;
    // toolbox
    QToolBox *toolBox;
    // edit toolbar;
    QToolBar * editToolBar;

    QToolBar * otherToolBar;


    QAction  * bringToFrontAct;
    QAction  * sendToBackAct;

    QAction  * funcAct;
    // file
    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *exitAct;

    QAction  * groupAct;
    QAction  * unGroupAct;

    // edit action
    QAction  * deleteAct;
    QAction  * undoAct;
    QAction  * redoAct;
    QAction  * copyAct;
    QAction  * pasteAct;
    QAction  * cutAct;
    QAction  * zoomInAct;
    QAction  * zoomOutAct;
    QAction  * creatMapAct;

    // drawing toolbar
    QToolBar * drawToolBar;
    QActionGroup * drawActionGroup;
    QAction  * selectAct;
    QAction  * lineAct;
    QAction  * pointAct;
    QAction  * arcAct;
    //-----------------------------------
    QAction  * arcAct_three_point;
    QAction  * arcAct_start_cen_angle;
    QAction  * arcAct_start_cen_end;
    QAction  * arcAct_start_cen_length;
    QAction  * arcAct_start_end_angle;
    QAction  * arcAct_start_end_direct;
    QAction  * arcAct_start_end_radius;
    QAction  * arcAct_cen_start_angle;
    QAction  * arcAct_cen_start_end;
    QAction  * arcAct_cen_start_length;
    QAction  * arcAct_continu;

    //-----------------------------------
    //QAction  * rectAct;
    //QAction  * roundRectAct;
    //QAction  * ellipseAct;
    QAction  * polygonAct;
    QAction  * polylineAct;
    QAction  * bezierAct;
    QAction  * rotateAct;
    //QAction  * updateStationAct;
    QAction  * interrputAct;
    QAction  * mergeAct;
    QAction  * moveAct;
    QAction  * findAct;
    QAction  * measureAct;

    QAction  * updateAct;

    QAction  * gridAct;

    QAction *closeAct;
    QAction *closeAllAct;
    QAction *tileAct;
    QAction *cascadeAct;
    QAction *nextAct;
    QAction *previousAct;
    QAction *separatorAct;
    QAction *aboutAct;
    QAction *aboutQtAct;


    QAction *whole_mapAct;
    QAction *clockwise_Act;
    QAction *anti_clockwise_Act;
    QAction *scene_color_Act;
    QAction *route_color_Act;
    QAction *point_font_Act;
    QAction *showFullScreen_Act;
    QAction *carTransparency_Act;
    QAction *station_background_Act;

    Segment_Action *line_Act;
    Arrow_Action *arrow_Act;
    Arrow_Action *arrow_Act2;
    CarTranS_Action *cartrans_Act;
    PointSize_Action *pointSize_Act;

    QAction *control_pointfont;
    QAction *control_Area;
    QAction *control_floatwindow;
    QAction *control_saBackground;
    QAction *control_errorInfoHint;
    QAction *control_propertyList;
    QMenu   *floormenu;
    QAction *speedDisplayMode;

    QAction *reload_tool_Act;
    QAction *playback_tool_Act;
    QAction *agvctrl_tool_Act;
    QAction *batteryInfo_Act;
    QAction *taskInfo_Act;
    QAction *taskInfoSpecial_Act;
    QAction *cxjData_Act;
    QAction *cxjBack_Act;

    QAction *checkUpdate_Act;

    QAction *debugWidget_Act;
    QAction *skinDark_Act;
    QAction *skinLight_Act;
    QAction *skinEmpty_Act;
    QAction *skinRetro_Act;
    QAction *skinFusion_Act;

    QSignalMapper* signalMapper;
    QAction *floor_Act[10];

    QAction *carSizeWidget_Act;


    QAction *showLiftLineAct;
    QAction *showDisableAct;

    QUndoGroup* undoGroup;

    //property editor
//    QDockWidget *dockProperty;
//    QtTreePropertyBrowser *propertyEditor;
//    QtVariantPropertyManager *variantManager;
//    QtEnumPropertyManager *enumManager;
//    QGraphicsItem *currentItem;

//    QMap<QtProperty *, QString> propertyToId;
//    QMap<QString, QtVariantProperty *> idToProperty;
//    QMap<QString, bool> idToExpanded;
    QMap<QtProperty*,QString> m_propertyToId;
    QMap<QString, QtVariantProperty *> m_idToProperty;

    QPushButton *tcp_connect_button;


protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private:
    bool leftBtnClk;
    QPoint m_Press;
    QPoint m_Move;
};
#endif // MAINWINDOW_H
