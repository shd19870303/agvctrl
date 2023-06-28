#ifndef MAP_H
#define MAP_H

#include <QString>

class MapConfig
{
public:
    MapConfig();

private:
    double map_version;
    int x_min;
    int x_max;
    int y_min;
    int y_max;
    double map_point_width;
    double map_segment_width;
    double map_font_size;

public:
    void setMapVersion(double temp);
    void setXMin(int temp);
    void setXMax(int temp);
    void setYMin(int temp);
    void setYMax(int temp);
    void setMapPointWidth(double temp);
    void setMapSegmentWidth(double temp);
    void setMapFontSize(double temp);

    double getMapVersion();
    double getXMin();
    double getXMax();
    double getYMin();
    double getYMax();
    double getMapPointWidth();
    double getMapSegmentWidth();
    double getMapFontSize();
};


typedef struct Map_Config
{
    double map_version;
    int x_min = 0;
    int x_max = 20000;
    int y_min = 0;
    int y_max = 10000;
    int clock_dir = 0;

    int map_point_width = 150;
    int map_segment_width = 50;
    int map_front_size = 180;

    int mapId = 1;
    int project_id = 0;
    int startAngle = 0;
    int maxFlow = 50;  //最大物流量
};

#pragma pack(push, 1)

typedef struct Point
{
    float x = 0;
    float y = 0;
};

typedef struct MapPoint
{
    int		id;			//point ID start from 1
    char    name[6] = {'0','0','0','0','0','\0'};
    float	x;
    float	y;
    int 	QRcode = 0;
    char    type = 0;
    int     stype = 0;
    int     atype = 0;
    int     dist = 0;
    int     chargeDir = 0;
    float   Infrared = 0;
    int     angle = 0;
    double  parm[30] = {0};
    int     disable = 0;
    int     lock    = 0;
    bool operator== (const MapPoint b)
    {
        bool equ = true;
        for(unsigned i = 0; i < sizeof(parm)/sizeof(parm[0]); i++)
        {
            if(static_cast<int>(parm[i]*1000) != static_cast<int>(b.parm[i]*1000))
            {
                equ = false;
            }
        }
        if(id == b.id
         && (memcmp(name,b.name,5) == 0)
         && static_cast<int>(x) == static_cast<int>(b.x)
         && static_cast<int>(y) == static_cast<int>(b.y)         
         && QRcode == b.QRcode
         && type == b.type
         && stype == b.stype
         && atype == b.atype
         && dist == b.dist
         && chargeDir == b.chargeDir
         && static_cast<int>(Infrared) == static_cast<int>(b.Infrared)
         && angle == b.angle
         && equ
         )
        {
            return true;
        }
        else{
            return false;
        }
    }
};

typedef struct MapSegment
{
    int		id;			//segment ID start from 1
    float	start_x;
    float	start_y;
    float	end_x;
    float	end_y;
    float   cen_x=0;
    float   cen_y=0;
    float	lenght;
    int     type;
    float   radius=0;    
    int     dirmode = 0;
    float   start_ang;
    float   end_ang;
    int     Tmode = 0;
    int     AgvType = 0xFF;
    int     AgvType1 = 1;
    int     AgvType2 = 1;
    int     AgvType3 = 1;
    int     allowId = -1;
    int     blockId = -1;
    int     detection = 0;
    int     speed_limit = 0;
    double  reserved[30] = {0};
    int     start_id;
    int     end_id;
    int     disable = 0;
    int     lock    = 0;

    bool operator== (const MapSegment b)
    {
        bool equ = true;
        for(unsigned i = 0; i < sizeof(reserved)/sizeof(reserved[0]); i++)
        {
            if(static_cast<int>(reserved[i]*1000) != static_cast<int>(b.reserved[i]*1000))
            {
                equ = false;
            }
        }
        if(id == b.id
         && static_cast<int>(start_x) == static_cast<int>(b.start_x)
         && static_cast<int>(start_y) == static_cast<int>(b.start_y)
         && static_cast<int>(end_x) == static_cast<int>(b.end_x)
         && static_cast<int>(end_y) == static_cast<int>(b.end_y)
         && static_cast<int>(cen_x) == static_cast<int>(b.cen_x)
         && static_cast<int>(cen_y) == static_cast<int>(b.cen_y)
         && static_cast<int>(radius) == static_cast<int>(b.radius)
         && static_cast<int>(lenght) == static_cast<int>(b.lenght)
         && type == b.type
         && dirmode == b.dirmode
         && static_cast<int>(start_ang) == static_cast<int>(b.start_ang)
         && static_cast<int>(end_ang) == static_cast<int>(b.end_ang)
         && Tmode == b.Tmode
         && AgvType1 == b.AgvType1
         && AgvType2 == b.AgvType2
         && AgvType3 == b.AgvType3
         && allowId == b.allowId
         && blockId == b.blockId
         && detection == b.detection
         && speed_limit == b.speed_limit
         && start_id == b.start_id
         && end_id == b.end_id
         && equ

                )
        {
            return true;
        }
        else{
            return false;
        }
    }
};

typedef struct Action
{
    int act = -1;
    float value = 0;
};

typedef struct MapAction
{
    int id = 0;
    int point_id = 0;
    Action action[5];
};

typedef struct MapArea
{
    int id = 0;
    int type = 0;
    int gateway[10] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
    int point_sum  = 0;
    Point   point[20];
};

typedef struct MapTxtHeader
{
    char	STX = 0xFE;
    int 	mapTimestamp = 0;
    char	projectId = 0;
    char	mapArea = 0;
    int     mapLen = 0;
    int     headerLen = 0;
    int     pointLen = 0;
    int     segmentLen = 0;
    int     endLen = 0;
    int     functionLen = 0;
    int     areaLen = 0;   // 避让区
    int     point_num = 0;       //Totoal number of  points
    int     segment_num = 0;     //Totoal number of  segments
    int     function_num = 0;    //Totoal number of  function
    int     area_num = 0;        //Totoal number of Area
    char    CKA;     //Checksum low byte
    char    CKB;     //Checksum high byte
};

typedef struct MapTxtPoint
{
    char    STX = 0xFD;			//
    int 	length = 0;         //length of a point struct
    int     id = 0;
    char    name[5];
    float   X = 0;
    float   Y = 0;
    int     QRcode = 0;
    char    type = 0;
    int     stype = 0;
    int     atype = 0;
    char    dist = 0;        //area gate check  dist(m),default -1
    char    agv_dir = 0;
    float   infrared = 0;
    float   param[30];
    char    CKA;     //Checksum low byte
    char    CKB;     //Checksum high byte
};

typedef struct MapTxtSegment
{
    char    STX = 0xF9;
    int     length = 0;         //length of a segment struct
    int     id = 0;
    int     start_id = 0;
    float   start_x = 0;
    float   start_y = 0;
    int     end_id = 0;
    float   end_x = 0;
    float   end_y = 0;
    float   cen_x = 0;
    float   cen_y = 0;
    float   radius = 0;
    float   seglen = 0;
    char    type = 0;
    char    dir_mode = 0;
    float   start_angle = 0;
    float   end_angle = 0;
    char    trace_mode = 0;
    char    agv_type = 0;
    int     allow_id = 0;
    int     block_id = 0;
    char    detection = 0;
    char    speed_limit = 0;
    float   reserved[30];
    char    CKA;     //Checksum low byte
    char    CKB;     //Checksum high byte
};

typedef struct MapTxtFunction
{
    char    STX = 0xF8;
    int     length = 0;         //length of a segment struct
    int     id = 0;
    int     point_id = 0;
    int     action1 = -1;
    float   value1  = 0;
    int     action2 = -1;
    float   value2  = 0;
    int     action3 = -1;
    float   value3  = 0;
    int     action4 = -1;
    float   value4  = 0;
    int     action5 = -1;
    float   value5  = 0;
    char    CKA;     //Checksum low byte
    char    CKB;     //Checksum high byte
};

typedef struct MapTxtArea
{
    char    STX = 0xFC;
    int     length = 0;         //length of a segment struct
    int     id = 0;
    char    type = 0;
    int     param[10];     // gateway
    int     point_sum  = 0;
    Point   point[20];

    char    CKA;     //Checksum low byte
    char    CKB;     //Checksum high byte
};

typedef struct MapTxtEnd
{
    char    STX = 0xFB;
    char    length = 0;         //length of a segment struct
    char    mgc0 = 0x00;
    char    mgc1 = 0x01;
    char    mgc2 = 0x02;
    char    mgc3 = 0x03;
    char    mgc4 = 0x04;
    char    mgc5 = 0x05;

    char    CKA;     //Checksum low byte
    char    CKB;     //Checksum high byte
};

typedef struct RgvInfo
{
    char dev_id = 0;
    char rgv_main_state = 0;
    char rgv_sub_state = 0;
    char loading = 0;
    unsigned char error_code = 0;
    int  mileage;
    int  task_id = 0;
    int  task_start_id = 0;
    int  task_end_id = 0;
    char  task_main_state = 0;
    char  task_sub_state = 0;
};

#pragma pack(pop)

enum RGV_STATE
{
    DEFAULT = 0,
    RGV_OFFLINE,
    RGV_OFFLINE_LOAD,
    RGV_MANUAL,
    RGV_MANUAL_LOAD,
    RGV_WARNING,
    RGV_WARNING_LOAD,
    RGV_MOVING,
    RGV_MOVING_LOAD,
    RGV_PICKDOWN,
    RGV_PICKDOWN_LOAD,
    RGV_PICKUP,
    RGV_PICKUP_LOAD,
    RGV_STANDBY,
    RGV_STANDBY_LOAD
};

enum RGV_MAIN_STATE
{
    MANUAL = 0,
    AUTO,
    OFFLINE
};

enum RGV_SUB_STATE
{
    STANDBY = 0,
    TASKRUNNING,
    MOVING,
    ERROR_STOP
};


#endif // MAP_H
