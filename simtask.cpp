#include "simtask.h"
#include <QDebug>

simTask::simTask()
{
    taskId = 100;
    simType = RANDOM;
    taskList.clear();
}

void simTask::setSimType(SIMTYPE& type)
{
    simType = type;
}

simTask::SIMTYPE simTask::getSimType(void)
{
    return simType;
}


int simTask::deleteTaskByID(int taskID)
{
    int index = 0;
    int taskNum = taskList.size();
    //遍历任务
    for(index=0;index<taskNum;index++)
    {
        if(taskList[index].taskId == taskID)
        {
            taskList.remove(index);
            FilterTaskForSend();
            break;
        }
    }

    //遍历完成，没有该任务ID
    if(index==taskNum)
        return -1;

    return 0;
}

void simTask::FilterTaskForSend()
{
    QVector<SIMTASK> tempTaskSend;
    QVector<SIMTASK> tempTaskManualAdd;

    int index = 0;

    for(int i = 0; i < taskList.size(); i++)
    {
        if(taskList[i].manualCreat == 0)
        {
            if(i == 0)
            {
                tempTaskSend.append(taskList[0]);
            }
            else
            {
    //            if(!tempTaskSend.contains(taskList[i]))
    //            {
    //                tempTaskSend.append(taskList[i]);
    //            }
                //查找发送的任务列表中是否包含该任务
                int j;
                for(j = 0; j < tempTaskSend.size(); j++)
                {
                    if(tempTaskSend[j].startStaID == taskList[i].startStaID)
                    {
                        break;
                    }
                }

                if(j == tempTaskSend.size())
                {
                    tempTaskSend.append(taskList[i]);
                }
            }
        }
        else    // 手动添加的任务
        {
            tempTaskManualAdd.append(taskList[i]);
        }
    }
    taskSend.clear();
    SIMTASKSEND temp;
    for(int i = 0; i < tempTaskSend.size(); i++)
    {
        temp.taskId = tempTaskSend[i].taskId;
        temp.type = tempTaskSend[i].type;
        temp.group = tempTaskSend[i].group;
        temp.priority = tempTaskSend[i].priority;
        temp.startStaID = tempTaskSend[i].startStaID;
        temp.endStaID = tempTaskSend[i].endStaID;
        temp.ctimes = tempTaskSend[i].ctimes;
        taskSend.append(temp);
    }
    //taskSend = tempTaskSend;
}

void simTask::run(void)
{
    int pntIdx = 0;
    int pntNum = stationList.size();
    qint64 preTimeMs = 0;

    while(true)
    {
        qint64 nowTimeMs = QDateTime::currentMSecsSinceEpoch();
        if(nowTimeMs-preTimeMs > 100)
        {
            if(stationList[pntIdx].type != STOREOUT && stationList[pntIdx].type != OUT_IN)
            {
                pntIdx++; pntIdx %= pntNum;
                continue;
            }

            bool findEndSta = false;
            int endStaIdx = 0;
            qint64 nowTimeS = QDateTime::currentSecsSinceEpoch();
            if(stationList[pntIdx].expectTasTime <= nowTimeS)
            {
                qint64 startFind = QDateTime::currentMSecsSinceEpoch();
                while(findEndSta==false)
                {
                    int epIndex = QRandomGenerator::global()->bounded(0,pntNum);
                    int distance = qAbs(stationList[pntIdx].mileage - stationList[epIndex].mileage);
                    if(stationList[pntIdx].mileage>stationList[epIndex].mileage)
                    {
                        distance = totalMileage - distance;
                    }
                    if((stationList[pntIdx].tarGroup[0] == 0x30 /*"0"*/
                            ||stationList[pntIdx].tarGroup[0] == stationList[epIndex].group
                      /*      ||stationList[pntIdx].tarGroup[1] == stationList[epIndex].group
                            ||stationList[pntIdx].tarGroup[2] == stationList[epIndex].group
                            ||stationList[pntIdx].tarGroup[3] == stationList[epIndex].group
                            ||stationList[pntIdx].tarGroup[4] == stationList[epIndex].group */)
                            &&(stationList[epIndex].type==STOREIN || stationList[epIndex].type==OUT_IN)
                            && pntIdx != epIndex
                            && distance <= totalMileage*4/5 && distance >=totalMileage/5)
                    {
                        findEndSta = true;
                        endStaIdx = epIndex;
                    }

                    qint64 find2Now = QDateTime::currentMSecsSinceEpoch();
                    if(find2Now - startFind > 100)
                        break;
                }
                //找到终点
                if(findEndSta)
                {
                    //添加任务
                    qint64 createTime = QDateTime::currentSecsSinceEpoch();
                    qint64 createTimeMS = QDateTime::currentMSecsSinceEpoch();
                    SIMTASK tmpTask;
                    tmpTask.taskId = taskId++;
                    tmpTask.group = 1;          //编组
                    tmpTask.priority = 1;
                    tmpTask.startStaID = stationList[pntIdx].pntID;
                    tmpTask.endStaID = stationList[endStaIdx].pntID;
                    tmpTask.ctimes = createTimeMS;
                    taskList.push_back(tmpTask);
                    tmpTask.manualCreat = 0;
                    tmpTask.countDown = 0;
                    FilterTaskForSend();
                    //设置下次任务时间
                    qint64 maxTime =createTime+stationList[pntIdx].frequency*60+stationList[pntIdx].freOffset;
                    qint64 randomOffset = QRandomGenerator::global()->bounded(0,stationList[pntIdx].freOffset*2);
                    stationList[pntIdx].expectTasTime = maxTime-randomOffset;
                }
            }

            pntIdx++; pntIdx %= pntNum;
            preTimeMs = nowTimeMs;
        }
        msleep(100);
    }
}

void simTask::addTaskManual(int startStaID, int endStaID, int cntDown)
{
    qint64 createTimeMS = QDateTime::currentMSecsSinceEpoch();
    SIMTASK tmpTask;
    tmpTask.taskId = -1;
    tmpTask.group = 1;          //编组
    tmpTask.priority = 1;
    tmpTask.startStaID = startStaID;
    tmpTask.endStaID = endStaID;
    tmpTask.ctimes = createTimeMS;
    tmpTask.manualCreat = 1;
    tmpTask.countDown = cntDown;
    taskList.push_back(tmpTask);
    FilterTaskForSend();
    qDebug() << "simTask::addTaskManual";
}

void simTask::CreateTaskListManual(int number)
{
    int pntIdx = 0;
    int pntNum = stationListManualCreate.size();

    for(int i = 0; i < stationListManualCreate.size(); i++)
    {
        if(stationListManualCreate[i].type == STOREOUT || stationListManualCreate[i].type == OUT_IN)
        {
            pntIdx++;
            stationListManualCreate[i].expectTasTime = stationListManualCreate[i].frequency*60*(pntIdx*0.5);
        }
    }

    pntIdx = 0;

    while(taskListManualCreate.size() < number)
    {
        if(stationListManualCreate[pntIdx].type != STOREOUT && stationListManualCreate[pntIdx].type != OUT_IN)
        {
            pntIdx++; pntIdx %= pntNum;
            continue;
        }

        bool findEndSta = false;
        int endStaIdx = 0;

        qint64 startFind = QDateTime::currentMSecsSinceEpoch();
        while(findEndSta==false)
        {
            int epIndex = QRandomGenerator::global()->bounded(0,pntNum);
            int distance = qAbs(stationListManualCreate[pntIdx].mileage - stationListManualCreate[epIndex].mileage);
            if(stationListManualCreate[pntIdx].mileage>stationListManualCreate[epIndex].mileage)
            {
                distance = totalMileage - distance;
            }
            if((stationListManualCreate[pntIdx].tarGroup[0] == 0x30 /*"0"*/
                    ||stationListManualCreate[pntIdx].tarGroup[0] == stationListManualCreate[epIndex].group
              /*      ||stationListManualCreate[pntIdx].tarGroup[1] == stationListManualCreate[epIndex].group
                    ||stationListManualCreate[pntIdx].tarGroup[2] == stationListManualCreate[epIndex].group
                    ||stationListManualCreate[pntIdx].tarGroup[3] == stationListManualCreate[epIndex].group
                    ||stationListManualCreate[pntIdx].tarGroup[4] == stationListManualCreate[epIndex].group */)
                    &&(stationListManualCreate[epIndex].type==STOREIN || stationListManualCreate[epIndex].type==OUT_IN)
                    && pntIdx != epIndex
                    && distance <= totalMileage*4/5 && distance >=totalMileage/5)
            {
                findEndSta = true;
                endStaIdx = epIndex;
            }

            qint64 find2Now = QDateTime::currentMSecsSinceEpoch();
            if(find2Now - startFind > 100)
                break;
        }
        //找到终点
        if(findEndSta)
        {
            //添加任务
            qint64 createTime = QDateTime::currentSecsSinceEpoch();
            qint64 createTimeMS = QDateTime::currentMSecsSinceEpoch();
            SIMTASK tmpTask;
            tmpTask.taskId = taskId++;
            tmpTask.group = 1;          //编组
            tmpTask.priority = 1;
            tmpTask.startStaID = stationListManualCreate[pntIdx].pntID;
            tmpTask.endStaID = stationListManualCreate[endStaIdx].pntID;

            //设置下次任务时间
            qint64 maxTime = stationListManualCreate[pntIdx].frequency*60+stationListManualCreate[pntIdx].freOffset;
            qint64 randomOffset = QRandomGenerator::global()->bounded(0,stationListManualCreate[pntIdx].freOffset*2);
            stationListManualCreate[pntIdx].expectTasTime += maxTime-randomOffset;

            tmpTask.ctimes = stationListManualCreate[pntIdx].expectTasTime;
            taskListManualCreate.push_back(tmpTask);
            tmpTask.manualCreat = 0;
            tmpTask.countDown = 0;
            FilterTaskForSend();

        }

        pntIdx++; pntIdx %= pntNum;
    }
}

void simTask::randomLastTaskTimes(void)
{
    qint64 nowTimeMs = QDateTime::currentSecsSinceEpoch();    //s

    int pntNum = stationList.size();
    for(int index=0;index<pntNum;index++)
    {
        int staType = stationList[index].type;
        if(staType!=STOREOUT && staType!=OUT_IN)
            continue;

        qint64 maxOffset = stationList[index].frequency*60;   //min->s
        qint64 randomOffset = QRandomGenerator::global()->bounded(0,maxOffset);
        stationList[index].lastTaskTime = nowTimeMs-randomOffset;
    }
}

void simTask::randomTaskExceptTiems(void)
{
    int pntNum = stationList.size();
    for(int index=0;index<pntNum;index++)
    {
        int staType = stationList[index].type;
        if(staType!=STOREOUT && staType!=OUT_IN)
            continue;

        qint64 maxTime =stationList[index].lastTaskTime+stationList[index].frequency*60+stationList[index].freOffset;
        qint64 randomOffset = QRandomGenerator::global()->bounded(0,stationList[index].freOffset*2);
        stationList[index].expectTasTime = maxTime-randomOffset;
    }
}
