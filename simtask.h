#ifndef SIMTASK_H
#define SIMTASK_H

#include <QThread>
#include <QVector>
#include <QDateTime>
#include <QRandomGenerator>

;
#pragma pack(push, 1) //保存对齐状态

class simTask: public QThread
{
public:
    //仿真类型
    typedef enum {
        NO,
        LIST,               //列表模式
        RANDOM              //随机模式
    }SIMTYPE;

    //站点类型
    typedef enum {
        DISABLE,            //禁用
        STOREOUT,                //出货站台
        STOREIN,                 //入货站台
        OUT_IN              //出入货站台
    }STATYPE;

    //任务结构体
    typedef struct SIMTASK{
        int taskId;
        char type = 0;
        char group;          //编组
        char priority;
        int startStaID;
        int endStaID;
        long long ctimes;        //任务创建时间

        int manualCreat = 0;
        int countDown = 0;

//        bool operator == ( const SIMTASK &b)
//        {
//            if(startStaID == b.startStaID)
//            {
//                return true;
//            }
//            else{
//                return false;
//            }
//        }
    };

    typedef struct SIMTASKSEND{
        int taskId;
        char type = 0;
        char group;          //编组
        char priority;
        int startStaID;
        int endStaID;
        long long ctimes;        //任务创建时间
    };

    //站点仿真配置
    typedef struct {
        int pntID;
        int mileage;
        char group;
        STATYPE type;                       //0：禁用 1：出站 2：入站 3：出入站
        int  staEnable;                     // 0: 禁用  1：使能
        char tarGroup[5];
        int frequency;
        int freOffset;
        long lastTaskTime;              //上次产生任务时间
        long expectTasTime;             //期望下次任务时间
    }STATIONSET;

public:
    //构造函数
    simTask();
    //设置仿真任务模式
    void setSimType(SIMTYPE& type);
    //获取仿真任务模式
    SIMTYPE getSimType(void);
    //根据任务ID删除
    int deleteTaskByID(int taskID);
    //过滤出需要下发的任务
    void FilterTaskForSend(void);
    //生成任务线程函数
    void run(void);

public slots:
    void addTaskManual(int startStaID, int endStaID, int cntDown);

    void CreateTaskListManual(int number);

public:
    //随机上次任务时间
    void randomLastTaskTimes(void);
    //随机下次任务时间
    void randomTaskExceptTiems(void);

public:
    int taskId;
    SIMTYPE simType;
    QVector<SIMTASK> taskList;
    //QVector<SIMTASK> taskSend;
    QVector<SIMTASK> taskManualAdd;
    QVector<SIMTASKSEND> taskSend;

    QVector<STATIONSET> stationList;
    int totalMileage = 0;

    QVector<SIMTASK> taskListManualCreate;
    QVector<STATIONSET> stationListManualCreate;
};

#pragma pack(pop)//恢复对齐状态

#endif // SIMTASK_H
