#include "car.h"
#include <QPainter>
#include <QDebug>
#include "floating_dialog.h"
#include <QGraphicsSceneMouseEvent>
#include <QCursor>
#include <QColor>
Car::Car(qreal x ,qreal y,int flag)
{
    setZValue(4);
    this->x = x;
    this->y = y;
    pix.load(":/images/admin.png");
    setAcceptHoverEvents(true);
//    f_dialog = new Floating_Dialog();
    font.setBold(true);
//    this->installEventFilter(this);
    color_flag = flag;
}

int Car::get_id()
{
    return dev_id;
}

int Car::get_dev_type()
{
    return dev_type;
}

qreal Car::get_pos_x()
{
    return pos_x;
}

qreal Car::get_pos_y()
{
    return pos_y;
}

void Car::set_id(int id)
{
    this->dev_id = id;
}

void Car::set_xy(qreal x, qreal y)
{
    this->x = x;
    this->y = y;
}

void Car::set_f_dialog_xy(int x, int y)
{
    this->f_dialog_x = x;
    this->f_dialog_y = y;
}

void Car::set_f_dialog_info(int dev_id, int device_type, int mission_id, int mission_type, int speed, int state, int status, int error_type, int battery, int timestamp)
{
//    qDebug()<<"set_f_dialog_info        dev_id ="<<dev_id;
    this->f_dialog->set_widgets_info(dev_id, device_type, mission_id, mission_type, speed, state, status, error_type, battery, timestamp);
}

void Car::set_polygon_points(int polygon_num, int type, QPointF point1, QPointF point2, QPointF point3, QPointF point4)
{
    this->polygon_num = polygon_num;
    if(type == 1)
    {
        polygon1.clear();
        polygon1.append(mapFromScene(point1));
        polygon1.append(mapFromScene(point2));
        polygon1.append(mapFromScene(point3));
        polygon1.append(mapFromScene(point4));
    }else if(type == 2)
    {
        polygon2.clear();
        polygon2.append(mapFromScene(point1));
        polygon2.append(mapFromScene(point2));
        polygon2.append(mapFromScene(point3));
        polygon2.append(mapFromScene(point4));
    }else if(type == 3)
    {
        polygon3.clear();
        polygon3.append(mapFromScene(point1));
        polygon3.append(mapFromScene(point2));
        polygon3.append(mapFromScene(point3));
        polygon3.append(mapFromScene(point4));
    }

}

void Car::set_car_values(unsigned int dev_id, unsigned char dev_type, int mission_id, char mission_type, float pos_x, float pos_y, float pos_ang, char dir, float speed, char state, char status, int err_type, float battery, int loadnum, int cur_segment, int nxt_segment, unsigned int ip, long long timestamp, int online, int start_id, int end_id, float hyd_height, char protection1, char protection2, char protection3, char softstop, int idle_wait_ts, int avoid_set_ts, int manual_on_ts, int softstop_on_ts, int warning_on_ts, int stopStatus, int stopLevel, int stopByAGVId, int stopReason, int speedLevel, int speedByAGVId, int confPoint)
{
    this->dev_id = dev_id;
    this->dev_type = dev_type;
    this->mission_id = mission_id;
    this-> mission_type = mission_type;
    this->pos_x = pos_x;
    this->pos_y = pos_y;
    this->pos_ang = pos_ang;
    this->dir = dir;
    this->speed = speed;
    this->state = state;
    this->status = status;
    this->err_type = err_type;
    this->battery = battery;
    this->loadnum = loadnum;
    this->cur_segment = cur_segment;
    this->nxt_segment = nxt_segment;
    this->ip = ip;
    this->timestamp = timestamp;
    this->online = online;
    this->start_id = start_id;
    this->end_id = end_id;
    this->hyd_height = hyd_height;
    this->protection1 = protection1;
    this->protection2 = protection2;
    this->protection3 = protection3;
    this->softstop = softstop;
    this->idle_wait_ts = idle_wait_ts;
    this->avoid_set_ts = avoid_set_ts;
    this->manual_on_ts = manual_on_ts;
    this->softstop_on_ts = softstop_on_ts;
    this->warning_on_ts = warning_on_ts;
    this->stopStatus = stopStatus;
    this->stopLevel = stopLevel;
    this->stopByAGVId = stopByAGVId;
    this->stopReason = stopReason;
    this->speedLevel= speedLevel;
    this->speedByAGVId = speedByAGVId;
    this->confPoint = confPoint;
}

QRectF Car::boundingRect() const
{
    return QRectF(-500,-1250,2500,2500);
}

void Car::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(Qt::NoBrush);
    QPen dashPen;
    dashPen.setWidth(30);
    dashPen.setStyle(Qt::DashLine);
    dashPen.setColor(Qt::red);

    if(dev_type >=1 && dev_type <=20)
    {
        if(loadnum >= 0)//有货
        {
            if(online == 0)
            {
                pix.load(":/images/type1_loaded_gray.png");
            }
            else if(err_type > 0)
            {
                pix.load(":/images/type1_loaded_red.png");
            }
            else if(state == 4)
            {
                pix.load(":/images/type1_loaded_yellow.png");
            }
            else if(softstop == 1)
            {
                pix.load(":/images/type1_loaded_orange.png");
            }
            else if(status == 3)
            {
                pix.load(":/images/type1_loaded_blue.png");

            }
            else
            {
                pix.load(":/images/type1_loaded_green.png");
            }
        }
        else//无货
        {
            if(mission_type == 2)
            {
                if(online == 0)
                {
                    pix.load(":/images/type1_loaded_gray.png");
                    painter->setPen(dashPen);
                    painter->drawRect(-380,-550,1100,1100);
                }
                else if(err_type > 0)
                {
                    pix.load(":/images/type1_loaded_red.png");
                    painter->setPen(dashPen);
                    painter->drawRect(-380,-550,1100,1100);
                }
                else if(state == 4)
                {
                    pix.load(":/images/type1_loaded_yellow.png");
                    painter->setPen(dashPen);
                    painter->drawRect(-380,-550,1100,1100);
                }
                else if(softstop == 1)
                {
                    pix.load(":/images/type1_loaded_orange.png");
                    painter->setPen(dashPen);
                    painter->drawRect(-380,-550,1100,1100);
                }
                else if(status == 3)
                {
                    pix.load(":/images/type1_loaded_blue.png");
                    painter->setPen(dashPen);
                    painter->drawRect(-380,-550,1100,1100);
                }
                else
                {
                    pix.load(":/images/type1_loaded_green.png");
                    painter->setPen(dashPen);
                    painter->drawRect(-380,-550,1100,1100);
                }
            }
            else
            {
                if(online == 0)
                {
                    pix.load(":/images/type1_unloaded_gray.png");
                }
                else if(err_type > 0)
                {
                    pix.load(":/images/type1_unloaded_red.png");
                }
                else if(state == 4)
                {
                    pix.load(":/images/type1_unloaded_yellow.png");
                }
                else if(softstop == 1)
                {
                    pix.load(":/images/type1_unloaded_orange.png");
                }
                else if(status == 3)
                {
                    pix.load(":/images/type1_unloaded_blue.png");
                }
                else
                {
                    pix.load(":/images/type1_unloaded_green.png");
                }
            }
        }

    }
    else if(dev_type >=21 && dev_type <=30)
    {
        if(loadnum >= 0)//有货
        {
            if(online == 0)
            {
                pix.load(":/images/qragv_loaded_gray.png");
            }
            else if(err_type > 0)
            {
                pix.load(":/images/qragv_loaded_red.png");
            }
            else if(state == 4)
            {
                pix.load(":/images/qragv_loaded_yellow.png");
            }
            else if(softstop == 1)
            {
                pix.load(":/images/qragv_loaded_orange.png");
            }
            else if(status == 3)
            {
                pix.load(":/images/qragv_loaded_blue.png");
            }
            else
            {
                pix.load(":/images/qragv_loaded_green.png");
            }
        }
        else//无货
        {
            if(mission_type == 2)
            {
                if(online == 0)
                {
                    pix.load(":/images/qragv_loaded_gray.png");
                    painter->setPen(dashPen);
                    painter->drawRect(-650,-550,1100,1100);
                }
                else if(err_type > 0)
                {
                    pix.load(":/images/qragv_loaded_red.png");
                    painter->setPen(dashPen);
                    painter->drawRect(-650,-550,1100,1100);
                }
                else if(state == 4)
                {
                    pix.load(":/images/qragv_loaded_yellow.png");
                    painter->setPen(dashPen);
                    painter->drawRect(-650,-550,1100,1100);
                }
                else if(softstop == 1)
                {
                    pix.load(":/images/qragv_loaded_orange.png");
                    painter->setPen(dashPen);
                    painter->drawRect(-650,-550,1100,1100);
                }
                else if(status == 3)
                {
                    pix.load(":/images/qragv_loaded_blue.png");
                    painter->setPen(dashPen);
                    painter->drawRect(-650,-550,1100,1100);
                }
                else
                {
                    pix.load(":/images/qragv_loaded_green.png");
                    painter->setPen(dashPen);
                    painter->drawRect(-650,-550,1100,1100);
                }
            }
            else
            {
                if(online == 0)
                {
                    pix.load(":/images/qragv_gray.png");
                }
                else if(err_type > 0)
                {
                    pix.load(":/images/qragv_red.png");
                }
                else if(state == 4)
                {
                    pix.load(":/images/qragv_yellow.png");
                }
                else if(softstop == 1)
                {
                    pix.load(":/images/qragv_orange.png");
                }
                else if(status == 3)
                {
                    pix.load(":/images/qragv_blue.png");
                }
                else
                {
                    pix.load(":/images/qragv_green.png");
                }
            }


        }

    }
    else if(dev_type >=31 && dev_type <=40)
    {
        if(loadnum >= 0)//有货
        {
            if(online == 0)
            {
                pix.load(":/images/type31_loaded_gray.png");
            }
            else if(err_type > 0)
            {
                pix.load(":/images/type31_loaded_red.png");
            }
            else if(state == 4)
            {
                pix.load(":/images/type31_loaded_yellow.png");
            }
            else if(softstop == 1)
            {
                pix.load(":/images/type31_loaded_orange.png");
            }
            else if(status == 3)
            {
                pix.load(":/images/type31_loaded_blue.png");
            }
            else
            {
                pix.load(":/images/type31_loaded_green.png");
            }
        }
        else//无货
        {
            if(mission_type == 2)
            {
                if(online == 0)
                {
                    pix.load(":/images/type31_loaded_gray.png");
                    painter->setPen(dashPen);
                    painter->drawRect(-550,-550,1100,1100);
                }
                else if(err_type > 0)
                {
                    pix.load(":/images/type31_loaded_red.png");
                    painter->setPen(dashPen);
                    painter->drawRect(-550,-550,1100,1100);
                }
                else if(state == 4)
                {
                    pix.load(":/images/type31_loaded_yellow.png");
                    painter->setPen(dashPen);
                    painter->drawRect(-550,-550,1100,1100);
                }
                else if(softstop == 1)
                {
                    pix.load(":/images/type31_loaded_orange.png");
                    painter->setPen(dashPen);
                    painter->drawRect(-550,-550,1100,1100);
                }
                else if(status == 3)
                {
                    pix.load(":/images/type31_loaded_blue.png");
                    painter->setPen(dashPen);
                    painter->drawRect(-550,-550,1100,1100);
                }
                else
                {
                    pix.load(":/images/type31_loaded_green.png");
                    painter->setPen(dashPen);
                    painter->drawRect(-550,-550,1100,1100);
                }
            }
            else
            {
                if(online == 0)
                {
                    pix.load(":/images/type31_unload_gray.png");
                }
                else if(err_type > 0)
                {
                    pix.load(":/images/type31_unload_red.png");
                }
                else if(state == 4)
                {
                    pix.load(":/images/type31_unload_yellow.png");
                }
                else if(softstop == 1)
                {
                    pix.load(":/images/type31_unload_orange.png");
                }
                else if(status == 3)
                {
                    pix.load(":/images/type31_unload_blue.png");
                }
                else
                {
                    pix.load(":/images/type31_unload_green.png");
                }
            }


        }

    }
    else if(dev_type >= 41)
    {
        if(loadnum >= 0)//有货
        {
            if(online == 0)
            {
                pix.load(":/images/type41_loaded_gray.png");
            }
            else if(err_type > 0)
            {
                pix.load(":/images/type41_loaded_red.png");
            }
            else if(state == 4)
            {
                pix.load(":/images/type41_loaded_yellow.png");
            }
            else if(softstop == 1)
            {
                pix.load(":/images/type41_loaded_orange.png");
            }
            else if(status == 3)
            {
                pix.load(":/images/type41_loaded_blue.png");
            }
            else
            {
                pix.load(":/images/type41_loaded_green.png");
            }
        }
        else//无货
        {
            if(mission_type == 2)
            {
                if(online == 0)
                {
                    pix.load(":/images/type41_loaded_gray_dotted.png");
                }
                else if(err_type > 0)
                {
                    pix.load(":/images/type41_loaded_red_dotted.png");
                }
                else if(state == 4)
                {
                    pix.load(":/images/type41_loaded_yellow_dotted.png");
                }
                else if(softstop == 1)
                {
                    pix.load(":/images/type41_loaded_orange_dotted.png");
                }
                else if(status == 3)
                {
                    pix.load(":/images/type41_loaded_blue_dotted.png");
                }
                else
                {
                    pix.load(":/images/type41_loaded_green_dotted.png");
                }
            }
            else
            {
                if(online == 0)
                {
                    pix.load(":/images/type41_unloaded_gray.png");
                }
                else if(err_type > 0)
                {
                    pix.load(":/images/type41_unloaded_red.png");
                }
                else if(state == 4)
                {
                    pix.load(":/images/type41_unloaded_yellow.png");
                }
                else if(softstop == 1)
                {
                    pix.load(":/images/type41_unloaded_orange.png");
                }
                else if(status == 3)
                {
                    pix.load(":/images/type41_unloaded_blue.png");
                }
                else
                {
                    pix.load(":/images/type41_unloaded_green.png");
                }
            }
        }

    }

    QPen pen;
    pen.setWidth(1);

    switch (color_flag) {
    case 0:
        painter->setBrush(QColor(130, 130, 130,80));
        pen.setColor(QColor(130, 130, 130,80));
        break;
    case 1:
        painter->setBrush(QColor(255, 0, 0,80));
        pen.setColor(QColor(255, 0, 0,80));
        break;
    case 2:
        painter->setBrush(QColor(215, 120, 24,80));
        pen.setColor(QColor(215, 120, 24,80));
        break;
    case 3:
        painter->setBrush(QColor(215, 215, 130,80));
        pen.setColor(QColor(215, 215, 130,80));
        break;
    case 4:
        painter->setBrush(QColor(73, 215, 130,80));
        pen.setColor(QColor(73, 215, 130,80));
        break;
    case 5:
//        painter->setBrush(QColor(0, 61, 215,80));
//        pen.setColor(QColor(0, 61, 215,80));
        painter->setBrush(QColor(255, 46, 169,80));
        pen.setColor(QColor(255, 46, 169,80));
        break;
    case 6:
        painter->setBrush(QColor(0, 215, 196,80));
        pen.setColor(QColor(0, 215, 196,80));
        break;
    case 7:
        painter->setBrush(QColor(177, 3, 215,80));
        pen.setColor(QColor(177, 3, 215,80));
        break;
    default:
        painter->setBrush(QColor(130, 130, 130,80));
        pen.setColor(QColor(130, 130, 130,80));
        break;
    }

    painter->setPen(pen);
    if(polygon_num == 1)
    {
        painter->drawPolygon(polygon1);
    }
    else if(polygon_num == 2)
    {
        painter->drawPolygon(polygon1);
        painter->drawPolygon(polygon2);
    }
    else if(polygon_num == 3)
    {
        painter->drawPolygon(polygon1);
        painter->drawPolygon(polygon2);
        painter->drawPolygon(polygon3);
    }

    int carL;
    int carW;

    if(dev_type >=1 && dev_type <=20)
    {
        carL = 2351;
        carW = 1172;
        if(customCarL > 0)
        {
            carL = customCarL;
        }
        if(customCarW > 0)
        {
            carW = customCarW;
        }

        carRectF.setRect(-379,-carW/2,carL,carW);//379 车上报点距离车尾距离

    }
    else if(dev_type >=21 && dev_type <=30)
    {
        carL = 1502;
        carW = 1475;
        if(customCarL > 0)
        {
            carL = customCarL;
        }
        if(customCarW > 0)
        {
            carW = customCarW;
        }

        carRectF.setRect(-carL/2,-carW/2,carL,carW);
    }
    else if(dev_type >=31 && dev_type <=40)
    {
        carL = 1502;
        carW = 1100;
        if(customCarL > 0)
        {
            carL = customCarL;
        }
        if(customCarW > 0)
        {
            carW = customCarW;
        }

        carRectF.setRect(-carL/2,-carW/2,carL,carW);
    }
    else if(dev_type >= 41)
    {
        carL = 4100;
        carW = 1150;
        if(customCarL > 0)
        {
            carL = customCarL;
        }
        if(customCarW > 0)
        {
            carW = customCarW;
        }
        if(loadnum >= 0)
        {
            carW*=2;
        }
        else
        {
            if(mission_type == 2)
            {
                carW*=2;
            }
        }
        carRectF.setRect(-carL/2,-carW/2,carL,carW);
    }
    else
    {
        carRectF.setRect(0,0,8000,8000);
    }

    QPixmap pix1(":/PixmapTest/Resources/Chrysanthemum.jpg");
    QPixmap temp(pix.size());
    temp.fill(Qt::transparent);
    QPainter p1(&temp);
    p1.setCompositionMode(QPainter::CompositionMode_Source);
    p1.drawPixmap(0, 0, pix);
    p1.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    p1.fillRect(temp.rect(), QColor(0, 0, 0, transparency));

    p1.end();
    pix = temp;

    painter->drawPixmap(carRectF.toRect(),pix);



    painter->save();
        QPen font_pen;
        font_pen.setColor(Qt::white);
        painter->setPen(font_pen);
        font = painter->font();
        font.setUnderline(true);
        font.setBold(true);
        font.setPointSize(350);
        painter->setFont(font);
        painter->scale(1,-1);
        if(dev_type >=1 && dev_type <=20)
        {
            painter->drawText(1200,150,QString::number(dev_id));
        }
        else if(dev_type >=21 && dev_type <=30)
        {
            painter->drawText(0,130,QString::number(dev_id));
        }
        else if(dev_type >=31 && dev_type <=40)
        {
            painter->drawText(0,130,QString::number(dev_id));
        }
        else if(dev_type >= 41)
        {
            painter->drawText(850,150,QString::number(dev_id));

        }

        else
        {
            painter->drawText(0,0,"99999999999");

        }
    painter->restore();

}

void Car::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    emit floatwindow_show( dev_id, dev_type, mission_id, mission_type, pos_x, pos_y, pos_ang, dir, speed, state, status, err_type, battery, loadnum, cur_segment, nxt_segment, ip, timestamp, online, start_id, end_id, hyd_height, protection1, protection2, protection3, softstop, idle_wait_ts, avoid_set_ts, manual_on_ts, softstop_on_ts, warning_on_ts, stopStatus, stopLevel, stopByAGVId, stopReason, speedLevel, speedByAGVId, errorStr, confPoint, event->scenePos().x(), event->scenePos().y());

}

void Car::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    emit floatwindow_hide();
}

void Car::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        emit car_id(dev_id,flag601,port9104);
    }
    else
        QGraphicsItem::mouseMoveEvent(event);
}

