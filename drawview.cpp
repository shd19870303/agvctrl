#include "drawview.h"
#include "drawtool.h"
#include "checksum.h"
#include <QTcpSocket>
#include <QSqlRecord>
extern LineTool   lineTool;
extern PointTool   pointTool;

DrawView::DrawView(QWidget *parent)
    :QGraphicsView(parent)
{
//    m_hruler = new QtRuleBar(Qt::Horizontal,this,this);
//    m_vruler = new QtRuleBar(Qt::Vertical,this,this);
//    box = new QtCornerBox(this);

    //label_test = new QLabel(this);
    //label_test->setText("hello");
    //label_test->setGeometry(0,0,100,100);

   // setViewport(new QWidget);

    setAttribute(Qt::WA_DeleteOnClose);
    isUntitled = true;

    modified = false;


    //setTransform(QTransform().shear(0.0,0.0),false);
    //setTransform(QTransform().rotate(30, Qt::YAxis).rotate(30, Qt::XAxis),false);

//    scale(1/50.0,1/50.0);
//    for(int i = 0 ; i < 10 ; i++)
//    {
//        scale(1 / 1.2, 1 / 1.2);
//    }

}

void DrawView::zoomIn()
{
    scale(1.2,1.2);
    updateRuler();
}

void DrawView::zoomOut()
{
    scale(1 / 1.2, 1 / 1.2);
    updateRuler();
}

void DrawView::self_adaption()
{
//    qDebug()<<"自动缩放";
    QRectF view=this->rect();    //qDebug() <<" X: "<<view.x() <<" Y: "<<view.y() <<" width: "<<view.width() <<" height: "<<view.height() <<endl;
    QRectF sceneF=this->scene()->sceneRect();    //qDebug() <<" X: "<<sceneF.x() <<" Y: "<<sceneF.y() <<" width: "<<sceneF.width() <<" height: "<<sceneF.height() <<endl;
    QPoint PoView0(0,0);
    QPointF Poscene0=this->mapToScene(PoView0);    //qDebug()<<" X: "<<Poscene0.x()<<" Y: "<< Poscene0.y()<<endl;
    QPoint PoView1(view.width(),0);
    QPointF Poscene1=this->mapToScene(PoView1);    //qDebug()<<" X: "<<Poscene1.x()<<" Y: "<< Poscene1.y()<<endl;
    QPoint PoView2(0,view.height());
    QPointF Poscene2=this->mapToScene(PoView2);    //qDebug()<<" X: "<<Poscene2.x()<<" Y: "<< Poscene2.y()<<endl;

    double dx = qAbs(sceneF.width()/(Poscene1.x()-Poscene0.x()));
    double dy = qAbs(sceneF.height()/(Poscene2.y()-Poscene0.y()));    //qDebug()<<" dx: "<< dx <<" dy: "<< dy;
    double min=dx>dy?dy:dx;
    double max=dx>dy?dx:dy;    //cout<<max<<endl;
    if(max!=0 && max!=1)
    {
        scale(1.3/max,1.3/max);
    }
}

void DrawView::clockwise()
{
    rotate(-5);
}

void DrawView::anti_clockwise()
{
    rotate(5);
}

void DrawView::newFile()
{
    static int sequenceNumber = 1;

    isUntitled = true;
    curFile = tr("drawing%1.db").arg(sequenceNumber++);
    setWindowTitle(curFile + "[*]");
}

bool DrawView::loadFile(const QString &fileName)
{

    QFile file(fileName);
    if(!file.open(QFile::ReadOnly))
    {
        QMessageBox::warning(this, tr("Open File"),
                                     tr("Cannot open file %1:\n%2.")
                                     .arg(fileName)
                                     .arg(file.errorString()));
        return false;
    }
    QByteArray map_byte_array = file.readAll();
    if(map_byte_array.at(0) == 0x4D && map_byte_array.at(1) == 0x50)
    {
        uint16_t crc16 = crc_calculate((uint8_t*)map_byte_array.data(),map_byte_array.size()-2);
        if((crc16 & 0x00FF) == (map_byte_array.at(map_byte_array.size()-2) & 0xFF) && ((crc16 >> 8) & 0x00FF) == (map_byte_array.at(map_byte_array.size()-1) & 0xFF))
        {
            mapArr_pb.ParseFromArray((uint8_t*)map_byte_array.data()+6,map_byte_array.size()-8);

            vecMapIndex.clear();

            for(int i = 0; i < mapArr_pb.map_size(); i++)
            {
                vecMapIndex.append(i);
            }

            int Margin = 10000;

            if(mapArr_pb.map_size() > 0)
            {

                for(int i = 0; i < mapArr_pb.map_size(); i++)
                {
                    for(auto it = mapArr_pb.map(i).point().cbegin(); it != mapArr_pb.map(i).point().cend(); ++it)
                    {
                        if(it == mapArr_pb.map(i).point().cbegin())
                        {
                            mapConfig[i].x_min = it->second.basic().x();
                            mapConfig[i].x_max = it->second.basic().x();
                            mapConfig[i].y_min = it->second.basic().y();
                            mapConfig[i].y_max = it->second.basic().y();
                        }
                        else
                        {
                            if(mapConfig[i].x_min > it->second.basic().x())
                            {
                                mapConfig[i].x_min = it->second.basic().x();
                            }
                            if(mapConfig[i].x_max < it->second.basic().x())
                            {
                                mapConfig[i].x_max = it->second.basic().x();
                            }
                            if(mapConfig[i].y_min > it->second.basic().y())
                            {
                                mapConfig[i].y_min = it->second.basic().y();
                            }
                            if(mapConfig[i].y_max < it->second.basic().y())
                            {
                                mapConfig[i].y_max = it->second.basic().y();
                            }
                        }
                    }
                    //qDebug() << mapArr_pb.map_size() << mapConfig[i].x_min << mapConfig[i].x_max << mapConfig[i].y_min << mapConfig[i].y_max;
                    mapConfig[i].mapId = mapArr_pb.map(i).conf().map_area();
                    emit send_area_value((double)mapConfig[i].mapId);
                }


                int pointIdMax;
                int segmentIdMax;
                int areaIdMax;
                for(auto it = mapArr_pb.map(1).point().cbegin(); it != mapArr_pb.map(1).point().cend(); ++it)
                {
                    if(it == mapArr_pb.map(1).point().cbegin())
                    {
                        pointIdMax = it->second.basic().id();

                    }
                    else
                    {

                        if(pointIdMax < it->second.basic().id())
                        {
                            pointIdMax = it->second.basic().id();
                        }
                    }
                }

                for(auto it = mapArr_pb.map(1).segment().cbegin(); it != mapArr_pb.map(1).segment().cend(); ++it)
                {
                    if(it == mapArr_pb.map(1).segment().cbegin())
                    {
                        segmentIdMax = it->second.basic().id();

                    }
                    else
                    {

                        if(segmentIdMax < it->second.basic().id())
                        {
                            segmentIdMax = it->second.basic().id();
                        }
                    }
                }


                for(auto it = mapArr_pb.map(1).area().cbegin(); it != mapArr_pb.map(1).area().cend(); ++it)
                {
                    if(it == mapArr_pb.map(1).area().cbegin())
                    {
                        areaIdMax = it->second.basic().id();

                    }
                    else
                    {

                        if(areaIdMax < it->second.basic().id())
                        {
                            areaIdMax = it->second.basic().id();
                        }
                    }
                }
                qDebug()<<"segmentIdMax ="<<segmentIdMax;
                qDebug()<<"pointIdMax ="<<pointIdMax;
                qDebug()<<"areaIdMax ="<<areaIdMax;

                emit send_seg_maxID(segmentIdMax);

                emit send_point_maxID(pointIdMax);

                emit send_area_maxID(areaIdMax);

                if(readsizefromSQL)
                {
                    ParameterSave("X_MAX",QString("%1").arg(mapConfig[0].x_max));

                    ParameterSave("X_MIN",QString("%1").arg(mapConfig[0].x_min));

                    ParameterSave("Y_MAX",QString("%1").arg(mapConfig[0].y_max));

                    ParameterSave("Y_MIN",QString("%1").arg(mapConfig[0].y_min));

                    scene()->setSceneRect(mapConfig[cur_map_index].x_min-Margin, mapConfig[cur_map_index].y_min-Margin,
                            mapConfig[cur_map_index].x_max - mapConfig[cur_map_index].x_min+Margin*2,
                            mapConfig[cur_map_index].y_max - mapConfig[cur_map_index].y_min+Margin*2);
                }


                scene()->setSceneRect(mapConfig[cur_map_index].x_min-Margin, mapConfig[cur_map_index].y_min-Margin,
                        mapConfig[cur_map_index].x_max - mapConfig[cur_map_index].x_min+Margin*2,
                        mapConfig[cur_map_index].y_max - mapConfig[cur_map_index].y_min+Margin*2);

                loadCanvas(mapArr_pb);
            }
            else
            {
                scene()->setSceneRect(mapConfig[cur_map_index].x_min-Margin, mapConfig[cur_map_index].y_min-Margin,
                        mapConfig[cur_map_index].x_max - mapConfig[cur_map_index].x_min+Margin*2,
                        mapConfig[cur_map_index].y_max - mapConfig[cur_map_index].y_min+Margin*2);


            }

            setCurrentFile(fileName);
        }
        else
        {
            QMessageBox::warning(this, tr("Open File"),
                                         tr("crc校验有误： low = %1, high = %2.")
                                         .arg(QString::number((int)map_byte_array.at(map_byte_array.size()-2),16))
                                         .arg(QString::number((int)map_byte_array.at(map_byte_array.size()-1),16)));
        }
    }
    else
    {
        QMessageBox::warning(this, tr("Open File"),
                                     tr("文件的头有误： STX1 = %1, STX2 = %2.")
                                     .arg(QString::number((int)map_byte_array.at(0),16))
                                     .arg(QString::number((int)map_byte_array.at(1),16)));
    }






    return true;













#if 0
    //旧
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(fileName);

    bool ok = db.open();
    if(!ok)
    {
        qDebug() << "数据库打开失败" << db.lastError();
        QMessageBox::warning(this, tr("Qt Drawing"),
                                     tr("Cannot read file %1:\n%2.")
                                     .arg(fileName)
                                     );
        return false;
    }
    else{
        qDebug() << "QSQLITE数据库打开成功";
    }



    db_config_model = new QSqlTableModel(this);
    db_config_model->setTable("CONFIG");
    db_config_model->select();
    QSqlRecord temp_config_record = db_config_model->record(0);
//    qDebug()<<"++++++++++++++++++++++++++++++++++"<<temp_config_record.value(0).toDouble(); // = 1
    emit send_area_value(temp_config_record.value(0).toDouble());



    QSqlQuery sql_query;

    if(!sql_query.exec("select max(ID) from SEGMENT;"))
    {
        qDebug()<<sql_query.lastError();
    }
    else
    {
        while(sql_query.next())
        {
//            qDebug() << "最大的段ID为：" << sql_query.value(0).toInt();
            emit send_seg_maxID(sql_query.value(0).toInt());
        }
    }

    if(!sql_query.exec("select max(ID) from POINT;"))
    {
        qDebug()<<sql_query.lastError();
    }
    else
    {
        while(sql_query.next())
        {
//            qDebug() << "最大的点ID为：" << sql_query.value(0).toInt();
            emit send_point_maxID(sql_query.value(0).toInt());
        }
    }

    if(!sql_query.exec("select max(ID) from CLUED;"))
    {
        qDebug()<<sql_query.lastError();
    }
    else
    {
        while(sql_query.next())
        {
            qDebug() << "最大的避让区ID为：" << sql_query.value(0).toInt();
            emit send_area_maxID(sql_query.value(0).toInt());
        }
    }

    if(readsizefromSQL)
    {
        if(!sql_query.exec("select max(x) from POINT;"))
        {
            qDebug()<<sql_query.lastError();
        }
        else
        {
            while(sql_query.next())
            {
    //            qDebug() << "max point X" << sql_query.value(0).toInt();
                mapConfig.x_max = sql_query.value(0).toInt();
                ParameterSave("X_MAX",QString("%1").arg(mapConfig.x_max));
            }
        }
        if(!sql_query.exec("select min(x) from POINT;"))
        {
            qDebug()<<sql_query.lastError();
        }
        else
        {
            while(sql_query.next())
            {
    //            qDebug() << "min point X" << sql_query.value(0).toFloat();
                mapConfig.x_min = sql_query.value(0).toInt();
                ParameterSave("X_MIN",QString("%1").arg(mapConfig.x_min));
            }
        }

        if(!sql_query.exec("select max(y) from POINT;"))
        {
            qDebug()<<sql_query.lastError();
        }
        else
        {
            while(sql_query.next())
            {
                //qDebug() << "min point Y" << sql_query.value(0).toFloat();
                mapConfig.y_max = sql_query.value(0).toInt();
                ParameterSave("Y_MAX",QString("%1").arg(mapConfig.y_max));
            }
        }
        if(!sql_query.exec("select min(y) from POINT;"))
        {
            qDebug()<<sql_query.lastError();
        }
        else
        {
            while(sql_query.next())
            {
                //qDebug() << "min point Y" << sql_query.value(0).toFloat();
                mapConfig.y_min = sql_query.value(0).toInt();
                ParameterSave("Y_MIN",QString("%1").arg(mapConfig.y_min));
            }
        }
        qDebug()<<"数据库读取了大小";
        scene()->setSceneRect(mapConfig.x_min-4000,mapConfig.y_min-4000,mapConfig.x_max - mapConfig.x_min+8000,mapConfig.y_max - mapConfig.y_min+8000);
    }


    loadCanvas(&sql_query,&db);

    setCurrentFile(fileName);
    db.close();

    return true;
#endif
}

bool DrawView::save()
{
    if (isUntitled) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool DrawView::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
                                                    curFile);
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}


bool comSegmentId(MapSegment &a, MapSegment &b)
{
    if (b.id > a.id)
    {
        return true;
    }
    else
    {
        return false;
    }
}
bool comPointId(MapPoint &a, MapPoint &b)
{
    if (b.id > a.id)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool DrawView::saveFile(const QString &fileName)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(fileName);

    bool ok = db.open();
    if(!ok)
    {
        qDebug() << "数据库打开失败" << db.lastError();
        QMessageBox::warning(this, tr("Qt Drawing"),
                                     tr("Cannot read file %1:\n%2.")
                                     .arg(fileName)
                                     );
        return false;
    }
    else{
        qDebug() << "QSQLITE数据库打开成功";
    }

    QSqlQuery sql_query;
    QString exist_table = QString("SELECT * FROM sqlite_master WHERE type='table' AND name='CONFIG'");
    sql_query.exec(exist_table);
    if(sql_query.next())
    {
        //表存在
        //qDebug() << "表存在";
        QString update_config = QString("update CONFIG set x_min = %1, x_max = %2, y_min = %3, y_max = %4;").arg(scene()->sceneRect().left())
                .arg(scene()->sceneRect().left() + scene()->sceneRect().width())
                .arg(scene()->sceneRect().top())
                .arg(scene()->sceneRect().top() + scene()->sceneRect().height());

        if(!sql_query.exec(update_config))
        {
            qDebug()<<sql_query.lastError();
            QMessageBox::critical(this, tr("Save Failed!"),
                                         tr("Cannot update CONFIG :\n %1")
                                         .arg(sql_query.lastError().text())
                                         );
            return false;
        }
    }
    else
    {
        //qDebug() << "表不存在";
        QString creat_table = QString("create table CONFIG (x_min INT,x_max INT, y_min INT, y_max INT)");
        if(!sql_query.exec(creat_table))
        {
            qDebug()<<sql_query.lastError();
            QMessageBox::critical(this, tr("Save Failed!"),
                                         tr("Cannot create CONFIG :\n %1")
                                         .arg(sql_query.lastError().text())
                                         );
            return false;
        }
        //qDebug() << creat_table;
        QString update_config = QString("insert into CONFIG values(%1,%2,%3,%4);").arg(scene()->sceneRect().left())
                .arg(scene()->sceneRect().left() + scene()->sceneRect().width())
                .arg(scene()->sceneRect().top())
                .arg(scene()->sceneRect().top() + scene()->sceneRect().height());
        if(!sql_query.exec(update_config))
        {
            qDebug()<<sql_query.lastError();
            QMessageBox::critical(this, tr("Save Failed!"),
                                         tr("Cannot insert into CONFIG :\n %1")
                                         .arg(sql_query.lastError().text())
                                         );
            return false;
        }
    }





    path = QPainterPath();
    vecPoint.clear();
    vecSegment.clear();
    vecArea.clear();

    foreach (QGraphicsItem *item , scene()->items()) {
        if ( item->type() == GraphicsPointItem::Type)
        {
            GraphicsPointItem * my_point_item = dynamic_cast<GraphicsPointItem*>(item);
            //my_point_item->saveToSql(&sql_query);
            vecPoint.push_back(my_point_item->map_point);
        }
        else if(item->type() == GraphicsLineItem::Type)
        {
            GraphicsLineItem * my_line_item = dynamic_cast<GraphicsLineItem*>(item);
            //my_line_item->saveToSql(&sql_query);
            vecSegment.push_back(my_line_item->map_segment);
        }
        else if(item->type() == GraphicsArcItem::Type)
        {
            GraphicsArcItem * my_arc_item = dynamic_cast<GraphicsArcItem*>(item);
            //my_arc_item->saveToSql(&sql_query);
            vecSegment.push_back(my_arc_item->map_segment);
        }
        else if(item->type() == GraphicsAreaItem::Type)
        {
            GraphicsAreaItem * my_area_item = dynamic_cast<GraphicsAreaItem*>(item);
            my_area_item->saveToSql(&sql_query);
            vecArea.push_back(my_area_item->map_area);
        }
    }

//    qSort(vecPoint.begin(),vecPoint.end(), comPointId);   //对现在场景中的点的Id进行从小到大排序
//    qSort(vecPointBackup.begin(),vecPointBackup.end(), comPointId);  // 对打开地图时的点Id进行从小到大的排序
//    qSort(vecSegment.begin(),vecSegment.end(),comSegmentId);   //对现在场景中的段Id进行从小到大排序
//    qSort(vecSegmentBackup.begin(),vecSegmentBackup.end(),comSegmentId); //对打开地图时的段Id进行从小到大排序
    std::sort(vecPoint.begin(),vecPoint.end(), comPointId);
    std::sort(vecPointBackup.begin(),vecPointBackup.end(), comPointId);
    std::sort(vecSegment.begin(),vecSegment.end(),comSegmentId);
    std::sort(vecSegmentBackup.begin(),vecSegmentBackup.end(),comSegmentId);

    int i = 0, j = 0;
    QVector<int> indexToDel;
    QVector<int> indexToAdd;
    QVector<int> indexToMod;
    QVector<int> indexToMod_2;

//************************** 点的操作 ************************************//

    // 找出需要删除的点，如果点被修改，也要被删除
    for(i = 0; i < vecPointBackup.size(); i++)
    {
        for(j = 0; j < vecPoint.size(); j++)
        {
            if(vecPointBackup[i] == vecPoint[j])
            {
                break;
            }
        }

        if(j == vecPoint.size())
        {
            indexToDel.append(i);
        }
    }
    // 找出需要添加的点
    for(i = 0; i < vecPoint.size(); i++)
    {
        for(j = 0; j < vecPointBackup.size(); j++)
        {
            if(vecPoint[i] == vecPointBackup[j])
            {
                break;
            }
        }

        if(j == vecPointBackup.size())
        {
            indexToAdd.append(i);
        }
    }

    // 找出修改的点
    for(i = 0; i < indexToAdd.size(); i ++)
    {
        for(j = 0; j < indexToDel.size(); j++)
        {
            if(vecPoint[indexToAdd[i]].id == vecPointBackup[indexToDel[j]].id)
            {
                indexToMod.append(indexToAdd[i]);
                indexToMod_2.append(indexToDel[j]);
            }
        }
    }

    if(indexToMod.size() > 0)
    {
        for(int i = 0; i < indexToMod.size(); i++)
        {
            indexToAdd.removeOne(indexToMod[i]);
        }
    }
    if(indexToMod_2.size() > 0)
    {
        for(int i = 0; i < indexToMod_2.size(); i++)
        {
            indexToDel.removeOne(indexToMod_2[i]);
        }
    }

    //执行存储数据库操作
    QString sql_str;
    for(int i = 0; i < indexToDel.size(); i++)
    {
        sql_str = QString("delete from POINT where id = %1;").arg(vecPointBackup[indexToDel[i]].id);
        if(!sql_query.exec(sql_str))
        {
            qDebug()<<sql_query.lastError();
            QMessageBox::critical(this, tr("Save Failed!"),
                                         tr("Cannot delete from POINT :\n %1")
                                         .arg(sql_query.lastError().text())
                                         );
            return false;
        }
        qDebug() << sql_str;
    }
    for(int i = 0; i < indexToAdd.size(); i++)
    {
        exist_table = QString("SELECT * FROM sqlite_master WHERE type='table' AND name='POINT'");
        sql_query.exec(exist_table);
        if(sql_query.next())
        {
            //qDebug() << "POINT 表存在";
        }
        else
        {
            //qDebug() << "POINT 表不存在";

            QString creat_table = QString("create table POINT (ID INT,NAME VARCHAR2,X FLOAT, Y FLOAT, QRCODE INT, TYPE TINYINT, STYPE INT, ATYPE INT, "
                                          "DIST INT, Chargedir INT, Infrared FLOAT, ANGLE INT, parm0 double, parm1 double, parm2 double, parm3 double,"
                                          "parm4 double, parm5 double, parm6 double, parm7 double, parm8 double, parm9 double, parm10 double, parm11 double,"
                                          "parm12 double, parm13 double, parm14 double, parm15 double, parm16 double, parm17 double, parm18 double, parm19 double,"
                                          "parm20 double, parm21 double, parm22 double, parm23 double, parm24 double, parm25 double, parm26 double, parm27 double,"
                                          "parm28 double, parm29 double);");

            if(!sql_query.exec(creat_table))
            {
                qDebug()<<sql_query.lastError();
                QMessageBox::critical(this, tr("Save Failed!"),
                                             tr("Cannot create table POINT :\n %1")
                                             .arg(sql_query.lastError().text())
                                             );
                return false;
            }
            //qDebug() << creat_table;
        }

        sql_str = QString("insert into POINT (ID,NAME,X,Y,QRCODE,TYPE,STYPE,ATYPE,DIST,Chargedir,Infrared,angle,"
                          "parm0,parm1,parm2,parm3,parm4,parm5,parm6,parm7,parm8,parm9,parm10,parm11,parm12,parm13,"
                          "parm14,parm15,parm16,parm17,parm18,parm19,parm20,parm21,parm22,parm23,parm24,parm25,parm26,parm27,parm28,parm29) "
                          "values(%1,'%2',%3,%4,%5,%6,%7,%8,%9,%10,%11,%12,%13,%14,%15"
                          ",%16,%17,%18,%19,%20,%21,%22,%23,%24,%25,%26,%27,%28,%29,%30,%31,%32,%33,%34"
                          ",%35,%36,%37,%38,%39,%40,%41,%42);")
                .arg(vecPoint[indexToAdd[i]].id)
                .arg(QString(vecPoint[indexToAdd[i]].name).mid(0,5))
                .arg((qreal)vecPoint[indexToAdd[i]].x)
                .arg((qreal)vecPoint[indexToAdd[i]].y)
                .arg(vecPoint[indexToAdd[i]].QRcode)
                .arg((int)vecPoint[indexToAdd[i]].type)
                .arg(vecPoint[indexToAdd[i]].stype)
                .arg(vecPoint[indexToAdd[i]].atype)
                .arg(vecPoint[indexToAdd[i]].dist)
                .arg(vecPoint[indexToAdd[i]].chargeDir)
                .arg((qreal)vecPoint[indexToAdd[i]].Infrared)
                .arg(vecPoint[indexToAdd[i]].angle)
                .arg(vecPoint[indexToAdd[i]].parm[0]).arg(vecPoint[indexToAdd[i]].parm[1])
                .arg(vecPoint[indexToAdd[i]].parm[2]).arg(vecPoint[indexToAdd[i]].parm[3])
                .arg(vecPoint[indexToAdd[i]].parm[4]).arg(vecPoint[indexToAdd[i]].parm[5])
                .arg(vecPoint[indexToAdd[i]].parm[6]).arg(vecPoint[indexToAdd[i]].parm[7])
                .arg(vecPoint[indexToAdd[i]].parm[8]).arg(vecPoint[indexToAdd[i]].parm[9])
                .arg(vecPoint[indexToAdd[i]].parm[10]).arg(vecPoint[indexToAdd[i]].parm[11])
                .arg(vecPoint[indexToAdd[i]].parm[12]).arg(vecPoint[indexToAdd[i]].parm[13])
                .arg(vecPoint[indexToAdd[i]].parm[14]).arg(vecPoint[indexToAdd[i]].parm[15])
                .arg(vecPoint[indexToAdd[i]].parm[16]).arg(vecPoint[indexToAdd[i]].parm[17])
                .arg(vecPoint[indexToAdd[i]].parm[18]).arg(vecPoint[indexToAdd[i]].parm[19])
                .arg(vecPoint[indexToAdd[i]].parm[20]).arg(vecPoint[indexToAdd[i]].parm[21])
                .arg(vecPoint[indexToAdd[i]].parm[22]).arg(vecPoint[indexToAdd[i]].parm[23])
                .arg(vecPoint[indexToAdd[i]].parm[24]).arg(vecPoint[indexToAdd[i]].parm[25])
                .arg(vecPoint[indexToAdd[i]].parm[26]).arg(vecPoint[indexToAdd[i]].parm[27])
                .arg(vecPoint[indexToAdd[i]].parm[28]).arg(vecPoint[indexToAdd[i]].parm[29])
                ;

        if(!sql_query.exec(sql_str))
        {
            qDebug()<<sql_query.lastError();
            QMessageBox::critical(this, tr("Save Failed!"),
                                         tr("Cannot insert into POINT :\n %1")
                                         .arg(sql_query.lastError().text())
                                         );
            return false;
        }
        qDebug() << sql_str;
    }

    for(int i = 0; i < indexToMod.size(); i++)
    {
        sql_str = QString("update POINT set id = %1, name = '%2', x = %3, y = %4, QRCODE = %5, TYPE = %6, STYPE = %7, ATYPE = %8, "
                          "DIST = %9, Chargedir = %10, Infrared = %11, angle = %12, parm0 = %13, parm1 = %14, parm2 = %15,"
                          "parm3 = %16, parm4 = %17, parm5 = %18,"
                          "parm6 = %19, parm7 = %20, parm8 = %21,"
                          "parm9 = %22, parm10 = %23, parm11 = %24,"
                          "parm12 = %25, parm13 = %26, parm14 = %27,"
                          "parm15 = %28, parm16 = %29, parm17 = %30,"
                          "parm18 = %31, parm19 = %32, parm20 = %33,"
                          "parm21 = %34, parm22 = %35, parm23 = %36,"
                          "parm24 = %37, parm25 = %38, parm26 = %39,"
                          "parm27 = %40, parm28 = %41, parm29 = %42, where id = %43;")
                .arg(vecPoint[indexToMod[i]].id)
                .arg(QString(vecPoint[indexToMod[i]].name).mid(0,5))
                .arg((qreal)vecPoint[indexToMod[i]].x)
                .arg((qreal)vecPoint[indexToMod[i]].y)
                .arg(vecPoint[indexToMod[i]].QRcode)
                .arg((int)vecPoint[indexToMod[i]].type)
                .arg(vecPoint[indexToMod[i]].stype)
                .arg(vecPoint[indexToMod[i]].atype)
                .arg(vecPoint[indexToMod[i]].dist)
                .arg(vecPoint[indexToMod[i]].chargeDir)
                .arg((qreal)vecPoint[indexToMod[i]].Infrared)
                .arg(vecPoint[indexToMod[i]].angle)
                .arg(vecPoint[indexToMod[i]].parm[0]).arg(vecPoint[indexToMod[i]].parm[1])
                .arg(vecPoint[indexToMod[i]].parm[2]).arg(vecPoint[indexToMod[i]].parm[3])
                .arg(vecPoint[indexToMod[i]].parm[4]).arg(vecPoint[indexToMod[i]].parm[5])
                .arg(vecPoint[indexToMod[i]].parm[6]).arg(vecPoint[indexToMod[i]].parm[7])
                .arg(vecPoint[indexToMod[i]].parm[8]).arg(vecPoint[indexToMod[i]].parm[9])
                .arg(vecPoint[indexToMod[i]].parm[10]).arg(vecPoint[indexToMod[i]].parm[11])
                .arg(vecPoint[indexToMod[i]].parm[12]).arg(vecPoint[indexToMod[i]].parm[13])
                .arg(vecPoint[indexToMod[i]].parm[14]).arg(vecPoint[indexToMod[i]].parm[15])
                .arg(vecPoint[indexToMod[i]].parm[16]).arg(vecPoint[indexToMod[i]].parm[17])
                .arg(vecPoint[indexToMod[i]].parm[18]).arg(vecPoint[indexToMod[i]].parm[19])
                .arg(vecPoint[indexToMod[i]].parm[20]).arg(vecPoint[indexToMod[i]].parm[21])
                .arg(vecPoint[indexToMod[i]].parm[22]).arg(vecPoint[indexToMod[i]].parm[23])
                .arg(vecPoint[indexToMod[i]].parm[24]).arg(vecPoint[indexToMod[i]].parm[25])
                .arg(vecPoint[indexToMod[i]].parm[26]).arg(vecPoint[indexToMod[i]].parm[27])
                .arg(vecPoint[indexToMod[i]].parm[28]).arg(vecPoint[indexToMod[i]].parm[29])
                .arg(vecPoint[indexToMod[i]].id)
                ;
        if(!sql_query.exec(sql_str))
        {
            qDebug()<<sql_query.lastError();
            QMessageBox::critical(this, tr("Save Failed!"),
                                         tr("Cannot update POINT :\n %1")
                                         .arg(sql_query.lastError().text())
                                         );
        }
        qDebug() << sql_str;
    }
//**************************** 段的操作  **********************************//

    indexToDel.clear();
    indexToAdd.clear();
    indexToMod.clear();
    indexToMod_2.clear();

    // 找出需要删除的段，如果段被修改，也要被删除
    for(i = 0; i < vecSegmentBackup.size(); i++)
    {
        for(j = 0; j < vecSegment.size(); j++)
        {
            if(vecSegmentBackup[i] == vecSegment[j])
            {
                break;
            }
        }

        if(j == vecSegment.size())
        {
            indexToDel.append(i);
        }
    }
    // 找出需要添加的段
    for(i = 0; i < vecSegment.size(); i++)
    {
        for(j = 0; j < vecSegmentBackup.size(); j++)
        {
            if(vecSegment[i] == vecSegmentBackup[j])
            {
                break;
            }
        }

        if(j == vecSegmentBackup.size())
        {
            indexToAdd.append(i);
        }
    }

    // 找出修改的段
    for(i = 0; i < indexToAdd.size(); i ++)
    {
        for(j = 0; j < indexToDel.size(); j++)
        {
            if(vecSegment[indexToAdd[i]].id == vecSegmentBackup[indexToDel[j]].id)
            {
                indexToMod.append(indexToAdd[i]);
                indexToMod_2.append(indexToDel[j]);
            }
        }
    }

    if(indexToMod.size() > 0)
    {
        for(int i = 0; i < indexToMod.size(); i++)
        {
            indexToAdd.removeOne(indexToMod[i]);
        }
    }
    if(indexToMod_2.size() > 0)
    {
        for(int i = 0; i < indexToMod_2.size(); i++)
        {
            indexToDel.removeOne(indexToMod_2[i]);
        }
    }

    //执行存储数据库操作

    for(int i = 0; i < indexToDel.size(); i++)
    {
        sql_str = QString("delete from SEGMENT where ID = %1;").arg(vecSegmentBackup[indexToDel[i]].id);
        if(!sql_query.exec(sql_str))
        {
            qDebug()<<sql_query.lastError();
            QMessageBox::critical(this, tr("Save Failed!"),
                                         tr("Cannot delete from SEGMENT :\n %1")
                                         .arg(sql_query.lastError().text())
                                         );
            return false;
        }
        qDebug() << sql_str;
    }
    for(int i = 0; i < indexToAdd.size(); i++)
    {
        exist_table = QString("SELECT * FROM sqlite_master WHERE type='table' AND name='SEGMENT'");
        sql_query.exec(exist_table);
        if(sql_query.next())
        {
            //qDebug() << "SEGMENT 表存在";
        }
        else
        {
            //qDebug() << "SEGMENT 表不存在";

            QString creat_table = QString("create table SEGMENT (ID INT,STARTX FLOAT, STARTY FLOAT, ENDX FLOAT, ENDY FLOAT, CENX FLOAT, CENY FLOAT,"
                                          "SEGLEN FLOAT, TYPE TINYINT, RADIUS FLOAT, DIR_MODE TINYINT, ANG_START FLOAT, ANG_END FLOAT, TMODE TINYINT,"
                                          "AGVTYPE1 TINYINT, AGVTYPE2 TINYINT, AGVTYPE3 TINYINT, ALLOW_ID INT, BLOCK_ID INT,DETECTION TINYINT, SpeedLimit TINYINT,"
                                          "reserved0 double,reserved1 double,reserved2 double,reserved3 double,reserved4 double,reserved5 double,reserved6 double,"
                                          "reserved7 double,reserved8 double,reserved9 double,reserved10 double,reserved11 double,reserved12 double,reserved13 double,"
                                          "reserved14 double,reserved15 double,reserved16 double,reserved17 double,reserved18 double,reserved19 double,reserved20 double,"
                                          "reserved21 double,reserved22 double,reserved23 double,reserved24 double,reserved25 double,reserved26 double,reserved27 double,"
                                          "reserved28 double,reserved29 double, START_ID INT, END_ID INT)");

            if(!sql_query.exec(creat_table))
            {
                qDebug()<<sql_query.lastError();
                QMessageBox::critical(this, tr("Save Failed!"),
                                             tr("Cannot create table SEGMENT :\n %1")
                                             .arg(sql_query.lastError().text())
                                             );
                return false;
            }
            //qDebug() << creat_table;
        }

        sql_str = QString("insert into SEGMENT (ID,STARTX, STARTY, ENDX, ENDY, CENX, CENY,"
                          "SEGLEN, TYPE, RADIUS, DIR_MODE, ANG_START, ANG_END, TMODE,"
                          "AGVTYPE1, AGVTYPE2, AGVTYPE3, ALLOW_ID, BLOCK_ID,DETECTION, SpeedLimit,"
                          "reserved0,reserved1,reserved2,reserved3,reserved4,reserved5,reserved6,"
                          "reserved7,reserved8,reserved9,reserved10,reserved11,reserved12,reserved13,"
                          "reserved14,reserved15,reserved16,reserved17,reserved18,reserved19,reserved20,"
                          "reserved21,reserved22,reserved23,reserved24,reserved25,reserved26,reserved27,"
                          "reserved28,reserved29, START_ID, END_ID) "
                          "values(%1,%2,%3,%4,%5,%6,%7,%8,%9,%10,%11,%12,%13,%14,%15,%16,%17,%18,%19,%20,"
                                "%21,%22,%23,%24,%25,%26,%27,%28,%29,%30,%31,%32,%33,%34,%35,%36,%37,%38,%39,%40,"
                                "%41,%42,%43,%44,%45,%46,%47,%48,%49,%50,%51,%52,%53);")
                .arg(vecSegment[indexToAdd[i]].id)
                .arg(vecSegment[indexToAdd[i]].start_x)
                .arg(vecSegment[indexToAdd[i]].start_y)
                .arg(vecSegment[indexToAdd[i]].end_x)
                .arg(vecSegment[indexToAdd[i]].end_y)
                .arg(vecSegment[indexToAdd[i]].cen_x)
                .arg(vecSegment[indexToAdd[i]].cen_y)                
                .arg(vecSegment[indexToAdd[i]].lenght)
                .arg(vecSegment[indexToAdd[i]].type)
                .arg(vecSegment[indexToAdd[i]].radius)
                .arg(vecSegment[indexToAdd[i]].dirmode)
                .arg(vecSegment[indexToAdd[i]].start_ang)
                .arg(vecSegment[indexToAdd[i]].end_ang)
                .arg(vecSegment[indexToAdd[i]].Tmode)
                .arg(vecSegment[indexToAdd[i]].AgvType1)
                .arg(vecSegment[indexToAdd[i]].AgvType2)
                .arg(vecSegment[indexToAdd[i]].AgvType3)
                .arg(vecSegment[indexToAdd[i]].allowId)
                .arg(vecSegment[indexToAdd[i]].blockId)
                .arg(vecSegment[indexToAdd[i]].detection)
                .arg(vecSegment[indexToAdd[i]].speed_limit)
                .arg(vecSegment[indexToAdd[i]].reserved[0])
                .arg(vecSegment[indexToAdd[i]].reserved[1])
                .arg(vecSegment[indexToAdd[i]].reserved[2])
                .arg(vecSegment[indexToAdd[i]].reserved[3])
                .arg(vecSegment[indexToAdd[i]].reserved[4])
                .arg(vecSegment[indexToAdd[i]].reserved[5])
                .arg(vecSegment[indexToAdd[i]].reserved[6])
                .arg(vecSegment[indexToAdd[i]].reserved[7])
                .arg(vecSegment[indexToAdd[i]].reserved[8])
                .arg(vecSegment[indexToAdd[i]].reserved[9])
                .arg(vecSegment[indexToAdd[i]].reserved[10])
                .arg(vecSegment[indexToAdd[i]].reserved[11])
                .arg(vecSegment[indexToAdd[i]].reserved[12])
                .arg(vecSegment[indexToAdd[i]].reserved[13])
                .arg(vecSegment[indexToAdd[i]].reserved[14])
                .arg(vecSegment[indexToAdd[i]].reserved[15])
                .arg(vecSegment[indexToAdd[i]].reserved[16])
                .arg(vecSegment[indexToAdd[i]].reserved[17])
                .arg(vecSegment[indexToAdd[i]].reserved[18])
                .arg(vecSegment[indexToAdd[i]].reserved[19])
                .arg(vecSegment[indexToAdd[i]].reserved[20])
                .arg(vecSegment[indexToAdd[i]].reserved[21])
                .arg(vecSegment[indexToAdd[i]].reserved[22])
                .arg(vecSegment[indexToAdd[i]].reserved[23])
                .arg(vecSegment[indexToAdd[i]].reserved[24])
                .arg(vecSegment[indexToAdd[i]].reserved[25])
                .arg(vecSegment[indexToAdd[i]].reserved[26])
                .arg(vecSegment[indexToAdd[i]].reserved[27])
                .arg(vecSegment[indexToAdd[i]].reserved[28])
                .arg(vecSegment[indexToAdd[i]].reserved[29])
                .arg(vecSegment[indexToAdd[i]].start_id)
                .arg(vecSegment[indexToAdd[i]].end_id)
                ;
        if(!sql_query.exec(sql_str))
        {
            qDebug()<<sql_query.lastError();
            QMessageBox::critical(this, tr("Save Failed!"),
                                         tr("Cannot insert into SEGMENT :\n %1")
                                         .arg(sql_query.lastError().text())
                                         );
            return false;
        }
        qDebug() << sql_str;
    }

    for(int i = 0; i < indexToMod.size(); i++)
    {

        sql_str = QString("update SEGMENT set ID = %1, STARTX = %2, STARTY = %3, ENDX = %4, ENDY = %5, CENX = %6, CENY = %7, SEGLEN = %8, "
                          "TYPE = %9, RADIUS = %10, DIR_MODE = %11, ANG_START = %12, ANG_END = %13, TMODE = %14, AGVTYPE1 = %15, AGVTYPE2 = %16,"
                          "AGVTYPE3 = %17, ALLOW_ID = %18, BLOCK_ID = %19, DETECTION = %20, SpeedLimit = %21, reserved0 = %22, reserved1 = %23,"
                          "reserved2 = %24, reserved3 = %25, reserved4 = %26, reserved5 = %27, reserved6 = %28, reserved7 = %29, reserved8 = %30,"
                          " reserved9 = %31, reserved10 = %32, reserved11 = %33, reserved12 = %34, reserved13 = %35, reserved14 = %36, reserved15 = %37,"
                          " reserved16 = %38, reserved17 = %39, reserved18 = %40, reserved19 = %41, reserved20 = %42, reserved21 = %43, reserved22 = %44,"
                          " reserved23 = %45, reserved24 = %46, reserved25 = %47, reserved26 = %48, reserved27 = %49, reserved28 = %50, reserved29 = %51,"
                          " START_ID = %52, END_ID = %53,  where ID = %54;")
                .arg(vecSegment[indexToMod[i]].id)
                .arg(vecSegment[indexToMod[i]].start_x)
                .arg(vecSegment[indexToMod[i]].start_y)
                .arg(vecSegment[indexToMod[i]].end_x)
                .arg(vecSegment[indexToMod[i]].end_y)
                .arg(vecSegment[indexToMod[i]].cen_x)
                .arg(vecSegment[indexToMod[i]].cen_y)                
                .arg(vecSegment[indexToMod[i]].lenght)
                .arg(vecSegment[indexToMod[i]].type)
                .arg(vecSegment[indexToMod[i]].radius)
                .arg(vecSegment[indexToMod[i]].dirmode)
                .arg(vecSegment[indexToMod[i]].start_ang)
                .arg(vecSegment[indexToMod[i]].end_ang)
                .arg(vecSegment[indexToMod[i]].Tmode)
                .arg(vecSegment[indexToMod[i]].AgvType1)
                .arg(vecSegment[indexToMod[i]].AgvType2)
                .arg(vecSegment[indexToMod[i]].AgvType3)
                .arg(vecSegment[indexToMod[i]].allowId)
                .arg(vecSegment[indexToMod[i]].blockId)
                .arg(vecSegment[indexToMod[i]].detection)
                .arg(vecSegment[indexToMod[i]].speed_limit)
                .arg(vecSegment[indexToMod[i]].reserved[0])
                .arg(vecSegment[indexToMod[i]].reserved[1])
                .arg(vecSegment[indexToMod[i]].reserved[2])
                .arg(vecSegment[indexToMod[i]].reserved[3])
                .arg(vecSegment[indexToMod[i]].reserved[4])
                .arg(vecSegment[indexToMod[i]].reserved[5])
                .arg(vecSegment[indexToMod[i]].reserved[6])
                .arg(vecSegment[indexToMod[i]].reserved[7])
                .arg(vecSegment[indexToMod[i]].reserved[8])
                .arg(vecSegment[indexToMod[i]].reserved[9])
                .arg(vecSegment[indexToMod[i]].reserved[10])
                .arg(vecSegment[indexToMod[i]].reserved[11])
                .arg(vecSegment[indexToMod[i]].reserved[12])
                .arg(vecSegment[indexToMod[i]].reserved[13])
                .arg(vecSegment[indexToMod[i]].reserved[14])
                .arg(vecSegment[indexToMod[i]].reserved[15])
                .arg(vecSegment[indexToMod[i]].reserved[16])
                .arg(vecSegment[indexToMod[i]].reserved[17])
                .arg(vecSegment[indexToMod[i]].reserved[18])
                .arg(vecSegment[indexToMod[i]].reserved[19])
                .arg(vecSegment[indexToMod[i]].reserved[20])
                .arg(vecSegment[indexToMod[i]].reserved[21])
                .arg(vecSegment[indexToMod[i]].reserved[22])
                .arg(vecSegment[indexToMod[i]].reserved[23])
                .arg(vecSegment[indexToMod[i]].reserved[24])
                .arg(vecSegment[indexToMod[i]].reserved[25])
                .arg(vecSegment[indexToMod[i]].reserved[26])
                .arg(vecSegment[indexToMod[i]].reserved[27])
                .arg(vecSegment[indexToMod[i]].reserved[28])
                .arg(vecSegment[indexToMod[i]].reserved[29])
                .arg(vecSegment[indexToMod[i]].start_id)
                .arg(vecSegment[indexToMod[i]].end_id)

                .arg(vecSegment[indexToMod[i]].id)
                ;

        if(!sql_query.exec(sql_str))
        {
            qDebug()<<sql_query.lastError();
            QMessageBox::critical(this, tr("Save Failed!"),
                                         tr("Cannot update SEGMENT :\n %1")
                                         .arg(sql_query.lastError().text())
                                         );
            return false;
        }
        qDebug() << sql_str;
    }

    vecPointBackup = vecPoint;
    vecSegmentBackup = vecSegment;
    vecAreaBackup = vecArea;
    selfCheckMap();

    db.close();
    setCurrentFile(fileName);


    return true;
}

QString DrawView::userFriendlyCurrentFile()
{
    return strippedName(curFile);
}

void DrawView::find(int type, QString str_id)
{
    scene()->clearSelection();
    foreach (QGraphicsItem *item , scene()->items())
    {
        if(type == 0)  //  查找点
        {
            if(item->type() == GraphicsPointItem::Type)
            {
                GraphicsPointItem *pointItem = dynamic_cast<GraphicsPointItem *>(item);
//                if(pointItem->map_point.id == str_id.toInt()) //old
                if(pointItem->map_point_pb.basic().id() == str_id.toInt())
                {
                    //qDebug() << "Find Point id = " << pointItem->map_point.id;
                    scene()->clearSelection();
                    pointItem->setZValue(99);
                    pointItem->setSelected(true);
                    //centerOn(pointItem);
                    fitInView(pointItem,Qt::KeepAspectRatio);
                    scale(0.03,0.03);

//                    emit send_point_xy(pointItem->map_point.x,pointItem->map_point.y);
                }
//                else if(QString(pointItem->map_point.name) == str_id) //old
                else if(QString::fromStdString(pointItem->map_point_pb.basic().name()).mid(0,5) == str_id)
                {
                    qDebug() << "Find Point name = " << QString(pointItem->map_point.name);
                    scene()->clearSelection();
                    pointItem->setZValue(99);
                    pointItem->setSelected(true);
                    //centerOn(pointItem);
                    fitInView(pointItem,Qt::KeepAspectRatio);
                    scale(0.03,0.03);
//                    emit send_point_xy(pointItem->map_point.x,pointItem->map_point.y);
                }
            }
        }
        else if(type == 1)   // 查找段
        {
            if(item->type() == GraphicsLineItem::Type)
            {
                GraphicsLineItem *lineItem = dynamic_cast<GraphicsLineItem *>(item);
//                if(lineItem->map_segment.id == str_id.toInt())
                if(lineItem->map_segment_pb.basic().id() == str_id.toInt())
                {
                    //qDebug() << "Find Segment id = " << lineItem->map_segment.id;
                    scene()->clearSelection();
                    lineItem->setSelected(true);
                    //centerOn(pointItem);
                    fitInView(lineItem,Qt::KeepAspectRatio);
                    scale(0.5,0.5);
                }
            }
            else if(item->type() == GraphicsArcItem::Type)
            {
                GraphicsArcItem *arcItem = dynamic_cast<GraphicsArcItem *>(item);
//                if(arcItem->map_segment.id == str_id.toInt())
                if(arcItem->map_segment_pb.basic().id() == str_id.toInt())
                {
                    //qDebug() << "Find Segment id = " << lineItem->map_segment.id;
                    scene()->clearSelection();
                    arcItem->setSelected(true);
                    //centerOn(pointItem);
                    fitInView(arcItem,Qt::KeepAspectRatio);
                    scale(0.5,0.5);
                }
            }
        }
        else if(type == 2)   // 查找AGV
        {

        }
    }
}

void DrawView::selfCheckMap()
{

}

//void DrawView::closeEvent(QCloseEvent *event)
//{
//    if (maybeSave()) {
//        event->accept();
//    } else {
//        event->ignore();
//    }
//}

void DrawView::mouseMoveEvent(QMouseEvent *event)
{
    QPointF pt =mapToScene(event->pos());
//    m_hruler->updatePosition(event->pos());
//    m_vruler->updatePosition(event->pos());
    emit positionChanged( pt.x() , pt.y() );

    //qDebug() << "w:" << this->geometry().width() << "h:" << this->geometry().height();

    if ( event->buttons() & Qt::RightButton )
    {
        //QPointF mouseDelta = mapToScene(event->pos()) - mapToScene(m_lastMousePos);
        QPointF mouseDelta = (event->pos()) - (m_lastMousePos);

        translate((mouseDelta).x(), (mouseDelta).y());
        m_lastMousePos = event->pos();
    }

    QGraphicsView::mouseMoveEvent(event);
}

void DrawView::wheelEvent(QWheelEvent *event)
{
    //qDebug() << event->delta();
    if(event->angleDelta().y() > 0)
    {
        zoomIn();
    }
    else{
        zoomOut();
    }
}

void DrawView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);

 //   this->setViewportMargins(RULER_SIZE-1,RULER_SIZE-1,0,0);
//    m_hruler->resize(this->size().width()- RULER_SIZE - 1,RULER_SIZE);
//    m_hruler->move(RULER_SIZE,0);
//    m_vruler->resize(RULER_SIZE,this->size().height() - RULER_SIZE - 1);
//    m_vruler->move(0,RULER_SIZE);

//    box->resize(RULER_SIZE,RULER_SIZE);
//    box->move(0,0);
//    updateRuler();
}

void DrawView::scrollContentsBy(int dx, int dy)
{
    QGraphicsView::scrollContentsBy(dx,dy);
    updateRuler();
}

void DrawView::updateRuler()
{
    if ( scene() == 0) return;
    QRectF viewbox = this->rect();

    QPointF offset = mapFromScene(scene()->sceneRect().topLeft());
    offset += QPointF(scene()->sceneRect().topLeft().x()*-1,scene()->sceneRect().topLeft().y());
    double factor =  1./transform().m11();
    double lower_x = factor * ( viewbox.left()  - offset.x() );
    double upper_x = factor * ( viewbox.right() -RULER_SIZE- offset.x()  );
//    m_hruler->setRange(lower_x,upper_x,upper_x - lower_x );
//    m_hruler->update();

    double lower_y = factor * ( viewbox.top() - offset.y()) * -1;
    double upper_y = factor * ( viewbox.bottom() - RULER_SIZE - offset.y() ) * -1;

//    m_vruler->setRange(lower_y,upper_y,upper_y - lower_y );
//    m_vruler->update();

    //qDebug()<<viewbox<<QPoint(lower_x,upper_x) << QPoint(lower_y,upper_y) << offset;
}

bool DrawView::maybeSave()
{
//    if (isModified()) {
//        QMessageBox::StandardButton ret;
//        ret = QMessageBox::warning(this, tr("MDI"),
//                     tr("'%1' has been modified.\n"
//                        "Do you want to save your changes?")
//                     .arg(userFriendlyCurrentFile()),
//                     QMessageBox::Save | QMessageBox::Discard
//                     | QMessageBox::Cancel);
//        if (ret == QMessageBox::Save)
//            return save();
//        else if (ret == QMessageBox::Cancel)
//            return false;
//    }
    return true;
}

void DrawView::updateAssistPoint()
{
    QList<MapPoint> pointList;
    pointList = createPoint(vecPoint,vecSegment);

    for(int i = 0; i < pointList.size(); i++)
    {
        //GraphicsPointItem * item = new GraphicsPointItem(QRect(-100,-100,200,200));
        pointTool.addPointByXY(pointList[i].x,pointList[i].y);
        pointTool.item->map_point.type = 8;
        scene()->update();
    }

}

QList<MapPoint> DrawView::createPoint(QVector<MapPoint> vecPoint, QVector<MapSegment> vecSegment)
{
    QList<MapPoint> pointList;



    return pointList;
}

void DrawView::clearMeasureLine()
{
    foreach (QGraphicsItem *item , scene()->items())
    {
        if(item->type() == GraphicsMeasureItem::Type)
        {
            item->hide();
            scene()->removeItem(item);
        }
    }

}

void DrawView::moveItems(int dx, int dy)
{
    foreach (QGraphicsItem *item , scene()->selectedItems())
    {
        if(item->type() == GraphicsPointItem::Type)
        {
            GraphicsPointItem * pointItem = dynamic_cast<GraphicsPointItem *>(item);
            pointItem->moveBy(dx,dy);
        }
        else if(item->type() == GraphicsLineItem::Type)
        {
            GraphicsLineItem * lineItem = dynamic_cast<GraphicsLineItem *>(item);
            lineItem->moveBy(dx,dy);

        }else if(item->type() == GraphicsArcItem::Type)
        {
            GraphicsArcItem * arcItem = dynamic_cast<GraphicsArcItem *>(item);
            arcItem->moveBy(dx,dy);
        }
    }

    emit dynamic_cast<DrawScene *>(scene())->itemMoved(NULL, QPointF(dx, dy));

}

void DrawView::copyItems(int dx, int dy)
{
    QList<QGraphicsItem *> myItems;

    foreach (QGraphicsItem *item , scene()->selectedItems())
    {
        if(item->type() == GraphicsPointItem::Type)
        {
            GraphicsPointItem * pointItem = dynamic_cast<GraphicsPointItem *>(item);
            GraphicsPointItem * newPointItem = new GraphicsPointItem(QRect(-100,-100,200,200));
            newPointItem->setPos(pointItem->pos() + QPoint(dx,dy));
            newPointItem->map_point.x = pointItem->map_point.x + dx;
            newPointItem->map_point.y = pointItem->map_point.y + dy;
            newPointItem->map_point.id = pointTool.findPointId(dynamic_cast<DrawScene *>(scene()));
            newPointItem->map_point.QRcode = pointItem->map_point.QRcode;
            newPointItem->map_point.type = pointItem->map_point.type;
            newPointItem->map_point.stype = pointItem->map_point.stype;
            newPointItem->map_point.atype = pointItem->map_point.atype;
            newPointItem->map_point.dist = pointItem->map_point.dist;
            newPointItem->map_point.chargeDir = pointItem->map_point.chargeDir;
            newPointItem->map_point.Infrared = pointItem->map_point.Infrared;
            newPointItem->map_point.angle = pointItem->map_point.angle;
            memcpy((uint8_t*)&newPointItem->map_point.parm, (uint8_t*)&pointItem->map_point.parm, sizeof(newPointItem->map_point.parm));
            memcpy((uint8_t*)&newPointItem->map_point.name, (uint8_t*)&pointItem->map_point.name, sizeof(newPointItem->map_point.name));
            scene()->addItem(newPointItem);

            myItems.append(newPointItem);
        }
    }
    foreach (QGraphicsItem *item , scene()->selectedItems())
    {
        if(item->type() == GraphicsLineItem::Type)
        {
            GraphicsLineItem * lineItem = dynamic_cast<GraphicsLineItem *>(item);
            GraphicsLineItem * newLineItem = new GraphicsLineItem(0);
            newLineItem->setPos(lineItem->pos() + QPoint(dx,dy));
            newLineItem->m_points = lineItem->m_points;
            newLineItem->map_segment.id = lineTool.findSegmentId(dynamic_cast<DrawScene *>(scene()));
            newLineItem->map_segment.type = lineItem->map_segment.type;
            newLineItem->map_segment.dirmode = lineItem->map_segment.dirmode;
            newLineItem->map_segment.Tmode = lineItem->map_segment.Tmode;
            newLineItem->map_segment.AgvType1 = lineItem->map_segment.AgvType1;
            newLineItem->map_segment.AgvType2 = lineItem->map_segment.AgvType2;
            newLineItem->map_segment.AgvType3 = lineItem->map_segment.AgvType3;
            newLineItem->map_segment.allowId = lineItem->map_segment.allowId;
            newLineItem->map_segment.blockId = lineItem->map_segment.blockId;
            newLineItem->map_segment.detection = lineItem->map_segment.detection;
            newLineItem->map_segment.speed_limit = lineItem->map_segment.speed_limit;
            memcpy((uint8_t*)&newLineItem->map_segment.reserved, (uint8_t*)&lineItem->map_segment.reserved, sizeof(lineItem->map_segment.reserved));

            QGraphicsItem *temp = scene()->itemAt(newLineItem->mapToScene(newLineItem->m_points.at(0)),QTransform());
            if(temp != 0 && temp->type() == GraphicsPointItem::Type)
            {
                GraphicsPointItem * pointItem = dynamic_cast<GraphicsPointItem *>(temp);
                newLineItem->map_segment.start_id = pointItem->map_point.id;
            }
            else
            {
                newLineItem->map_segment.start_id = -1;
            }
            temp = scene()->itemAt(newLineItem->mapToScene(newLineItem->m_points.at(1)),QTransform());
            if(temp != 0 && temp->type() == GraphicsPointItem::Type)
            {
                GraphicsPointItem * pointItem = dynamic_cast<GraphicsPointItem *>(temp);
                newLineItem->map_segment.end_id = pointItem->map_point.id;
            }
            else
            {
                newLineItem->map_segment.end_id = -1;
            }
            scene()->addItem(newLineItem);

            myItems.append(newLineItem);

        }else if(item->type() == GraphicsArcItem::Type)
        {
            GraphicsArcItem * arcItem = dynamic_cast<GraphicsArcItem *>(item);
            GraphicsArcItem * newArcItem = new GraphicsArcItem(0);
            newArcItem->setPos(arcItem->pos() + QPoint(dx,dy));
            newArcItem->m_points = arcItem->m_points;
            newArcItem->map_segment.cen_x = arcItem->map_segment.cen_x + dx;
            newArcItem->map_segment.cen_y = arcItem->map_segment.cen_y + dy;
            newArcItem->map_segment.radius = arcItem->map_segment.radius;

            newArcItem->map_segment.id = lineTool.findSegmentId(dynamic_cast<DrawScene *>(scene()));
            newArcItem->map_segment.type = arcItem->map_segment.type;
            newArcItem->map_segment.dirmode = arcItem->map_segment.dirmode;
            newArcItem->map_segment.Tmode = arcItem->map_segment.Tmode;
            newArcItem->map_segment.AgvType1 = arcItem->map_segment.AgvType1;
            newArcItem->map_segment.AgvType2 = arcItem->map_segment.AgvType2;
            newArcItem->map_segment.AgvType3 = arcItem->map_segment.AgvType3;
            newArcItem->map_segment.allowId = arcItem->map_segment.allowId;
            newArcItem->map_segment.blockId = arcItem->map_segment.blockId;
            newArcItem->map_segment.detection = arcItem->map_segment.detection;
            newArcItem->map_segment.speed_limit = arcItem->map_segment.speed_limit;
            memcpy((uint8_t*)&newArcItem->map_segment.reserved, (uint8_t*)&arcItem->map_segment.reserved, sizeof(arcItem->map_segment.reserved));
            newArcItem->get_arc_path(newArcItem->map_segment.radius,1);

            QGraphicsItem *temp = scene()->itemAt(newArcItem->mapToScene(newArcItem->m_points.at(0)),QTransform());
            if(temp != 0 && temp->type() == GraphicsPointItem::Type)
            {
                GraphicsPointItem * pointItem = dynamic_cast<GraphicsPointItem *>(temp);
                newArcItem->map_segment.start_id = pointItem->map_point.id;
            }
            else
            {
                newArcItem->map_segment.start_id = -1;
            }
            temp = scene()->itemAt(newArcItem->mapToScene(newArcItem->m_points.at(1)),QTransform());
            if(temp != 0 && temp->type() == GraphicsPointItem::Type)
            {
                GraphicsPointItem * pointItem = dynamic_cast<GraphicsPointItem *>(temp);
                newArcItem->map_segment.end_id = pointItem->map_point.id;
            }
            else
            {
                newArcItem->map_segment.end_id = -1;
            }
            scene()->addItem(newArcItem);

            myItems.append(newArcItem);
        }
    }

    emit dynamic_cast<DrawScene *>(scene())->itemAdded(myItems);
}

void DrawView::rotateItems(int center_x, int center_y, float angle)
{
    QTransform transform;
    transform.rotate(angle);  //旋转角度
    QPoint center(center_x, center_y);

    foreach (QGraphicsItem *item , scene()->selectedItems())
    {
        if(item->type() == GraphicsPointItem::Type)
        {
            GraphicsPointItem * pointItem = dynamic_cast<GraphicsPointItem *>(item);
            QPoint pos = transform.map(pointItem->scenePos().toPoint() - center) + center;   //得到旋转后的坐标
            pointItem->setPos(pos);
        }
        else if(item->type() == GraphicsLineItem::Type)
        {
            GraphicsLineItem * lineItem = dynamic_cast<GraphicsLineItem *>(item);
            QPoint start_point = transform.map(lineItem->mapToScene(lineItem->m_points.at(0)).toPoint() - center) + center;
            QPoint end_point = transform.map(lineItem->mapToScene(lineItem->m_points.at(1)).toPoint() - center) + center;
            lineItem->setPos(start_point);
            lineItem->m_points.begin()[0] = lineItem->mapFromScene(start_point);
            lineItem->m_points.begin()[1] = lineItem->mapFromScene(end_point);
        }
        else if(item->type() == GraphicsArcItem::Type)
        {
            GraphicsArcItem * arcItem = dynamic_cast<GraphicsArcItem *>(item);
            QPoint start_point = transform.map(arcItem->mapToScene(arcItem->m_points.at(0)).toPoint() - center) + center;
            QPoint end_point = transform.map(arcItem->mapToScene(arcItem->m_points.at(1)).toPoint() - center) + center;
            QPoint cen_point = transform.map(arcItem->mapToScene(arcItem->cen).toPoint() - center) + center;
            arcItem->setPos(start_point);
            arcItem->m_points.begin()[0] = arcItem->mapFromScene(start_point);
            arcItem->m_points.begin()[1] = arcItem->mapFromScene(end_point);
            arcItem->cen = arcItem->mapFromScene(cen_point);
            arcItem->get_arc_path(arcItem->Radius,1);
        }
    }

    emit dynamic_cast<DrawScene *>(scene())->itemRotate(NULL, angle, center);
}

void DrawView::scene_add_AreaItem()
{
    for(int i = 0; i<vec_AreaItem.size(); i++)
    {
        scene()->addItem(vec_AreaItem[i]);
    }
}

void DrawView::scene_remove_AreaItem()
{
    for(int i = 0; i<vec_AreaItem.size(); i++)
    {
        scene()->removeItem(vec_AreaItem[i]);
    }

}

void DrawView::ParameterSave(QString Type, QString p)
{
    QSettings settings("Config.ini", QSettings::IniFormat);
    settings.setValue(Type,p);
}

void DrawView::setCurrentFile(const QString &fileName)
{
    curFile = QFileInfo(fileName).canonicalFilePath();
    isUntitled = false;
    setModified(false);
    setWindowModified(false);
    setWindowTitle(userFriendlyCurrentFile() + "[*]");
}

QString DrawView::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();

}



bool  db_add_column(QSqlQuery *sql_query,QSqlDatabase *db, QString table, QString columu, QString type, QString value)
{
    QString sql_exec = QString("ALTER TABLE ")+table+QString(" ADD COLUMN ")+columu+QString(" ")+type+QString("  DEFAULT ")+value+QString(";");
    db->transaction();
    sql_query->exec(sql_exec);
    if(columu == "AGVTYPE" && table == "SEGMENT")
    {
        sql_query->exec("UPDATE SEGMENT SET AGVTYPE=255 WHERE AGVTYPE1=0 AND AGVTYPE2=0 AND AGVTYPE3=0;");
        sql_query->exec("UPDATE SEGMENT SET AGVTYPE=255 WHERE AGVTYPE1=1 AND AGVTYPE2=1 AND AGVTYPE3=1;");
        sql_query->exec("UPDATE SEGMENT SET AGVTYPE=1   WHERE AGVTYPE1=1 AND AGVTYPE2=0 AND AGVTYPE3=0;");
        sql_query->exec("UPDATE SEGMENT SET AGVTYPE=2   WHERE AGVTYPE1=0 AND AGVTYPE2=1 AND AGVTYPE3=0;");
        sql_query->exec("UPDATE SEGMENT SET AGVTYPE=3   WHERE AGVTYPE1=1 AND AGVTYPE2=1 AND AGVTYPE3=0;");
        sql_query->exec("UPDATE SEGMENT SET AGVTYPE=4   WHERE AGVTYPE1=0 AND AGVTYPE2=0 AND AGVTYPE3=1;");
    }

    bool ret = db->commit();
    return  ret;
}

//void DrawView::loadCanvas( QSqlQuery *sql_query,QSqlDatabase *db )
void DrawView::loadCanvas(agv_map::MAP_ARR mapArr_pb)

{
    map_mapPointItem.clear();
    map_mapLineItem.clear();
    map_mapArcItem.clear();
    map_mapAreaItem.clear();
    map_mapAction.clear();

    for(int i = 0; i < mapArr_pb.map_size(); i++)
    {
        //[ 1.Point ]
        for(auto point_it = mapArr_pb.map(i).point().cbegin(); point_it != mapArr_pb.map(i).point().cend(); ++point_it)
        {
            int map_point_width = mapConfig[i].map_point_width;
//            qDebug()<<"------------------------map_point_width :"<<map_point_width;
            GraphicsPointItem * item = new GraphicsPointItem(QRect(-map_point_width/2,-map_point_width/2,map_point_width,map_point_width));
            if (item && item->loadFromPb(point_it->second))
            {
                //scene()->addItem(item);
                map_mapPointItem[i][point_it->second.basic().id()] = item;
                //scene()->removeItem(item);
            }
            else if ( item )
            {
                delete item;
            }
        }

        //[ 2. Segment ]
        for(auto seg_it = mapArr_pb.map(i).segment().cbegin(); seg_it != mapArr_pb.map(i).segment().cend(); ++seg_it)
        {
            if(seg_it->second.basic().type() == 0)
            {
                GraphicsLineItem * item = new GraphicsLineItem(0);
                if (item && item->loadFromPb(seg_it->second))
                {
                    //scene()->addItem(item);
                    map_mapLineItem[i][seg_it->second.basic().id()] = item;
                    //scene()->removeItem(item);
                }
                else if ( item )
                {
                    delete item;
                }
            }
            else if(seg_it->second.basic().type() == 1)
            {
                GraphicsArcItem * item = new GraphicsArcItem(0);
                if (item && item->loadFromPb(seg_it->second))
                {
                    //scene()->addItem(item);
                    map_mapArcItem[i][seg_it->second.basic().id()] = item;
                    //scene()->removeItem(item);
                }
                else if ( item )
                {
                    delete item;
                }
            }
        }

        //[ 3. Area ]

        for(auto area_it = mapArr_pb.map(i).area().cbegin(); area_it != mapArr_pb.map(i).area().cend(); ++area_it)
        {
            GraphicsAreaItem * item = new GraphicsAreaItem(0);
            //隐藏楼层区域
            if (item && item->loadFromPb(area_it->second))
            {
                //scene()->addItem(item);
                map_mapAreaItem[i][area_it->second.basic().id()] = item;
                //scene()->removeItem(item);
            }
            else if ( item )
            {
                delete item;
            }
        }

        //[ 4. Action]
        for(auto action_it = mapArr_pb.map(i).fpoint().cbegin(); action_it != mapArr_pb.map(i).fpoint().cend(); ++action_it)
        {
            map_mapAction[i][action_it->second.basic().id()] = action_it->second;
        }

//        emit actionListUpdateSignal(map_mapAction[cur_map_index]);功能点

//        qSort(vecPoint_pb.begin(),vecPoint_pb.end(),comPointId);
//        qSort(vecSegment_pb.begin(),vecSegment_pb.end(),comSegmentId);
//        qSort(vecArea_pb.begin(),vecArea_pb.end(),comAreaId);
    }




    for(int i = 0; i < map_mapPointItem[cur_map_index].size(); i++)
    {
        scene()->addItem(map_mapPointItem[cur_map_index][map_mapPointItem[cur_map_index].keys()[i]]);
    }
    for(int i = 0; i < map_mapLineItem[cur_map_index].size(); i++)
    {
        scene()->addItem(map_mapLineItem[cur_map_index][map_mapLineItem[cur_map_index].keys()[i]]);
    }
    for(int i = 0; i < map_mapArcItem[cur_map_index].size(); i++)
    {
        scene()->addItem(map_mapArcItem[cur_map_index][map_mapArcItem[cur_map_index].keys()[i]]);
    }

    for(int i = 0; i < map_mapAreaItem[cur_map_index].size(); i++)
    {
        if(area_flag == 1) scene()->addItem(map_mapAreaItem[cur_map_index][map_mapAreaItem[cur_map_index].keys()[i]]);
        vec_AreaItem.push_back(map_mapAreaItem[cur_map_index][map_mapAreaItem[cur_map_index].keys()[i]]);
    }
}


qreal DrawView::convertAngle(qreal srcAngle)
{
    qreal tempAngle = srcAngle;
    if(tempAngle > 0)
    {
        tempAngle = 360.0 - tempAngle;
    }

    return tempAngle;
}

