#include "map.h"

MapConfig::MapConfig()
{
    map_version = 1.0;
    x_min = -300;
    x_max = 600;
    y_min = -300;
    y_max = 300;
    map_point_width = 6;
    map_segment_width = 3;
    map_font_size = 9;

}

void MapConfig::setMapVersion(double temp)
{
    map_version = temp;
}

void MapConfig::setXMin(int temp)
{
    x_min = temp;
}

void MapConfig::setXMax(int temp)
{
    x_max = temp;
}

void MapConfig::setYMin(int temp)
{
    y_min = temp;
}

void MapConfig::setYMax(int temp)
{
    y_max = temp;
}

void MapConfig::setMapPointWidth(double temp)
{
    map_point_width = temp;
}

void MapConfig::setMapSegmentWidth(double temp)
{
    map_segment_width = temp;
}

void MapConfig::setMapFontSize(double temp)
{
    map_font_size = temp;
}

double MapConfig::getMapVersion()
{
    return map_version;
}

double MapConfig::getXMin()
{
    return x_min;
}

double MapConfig::getXMax()
{
    return x_max;
}

double MapConfig::getYMin()
{
    return y_min;
}

double MapConfig::getYMax()
{
    return y_max;
}

double MapConfig::getMapPointWidth()
{
    return map_point_width;
}

double MapConfig::getMapSegmentWidth()
{
    return map_segment_width;
}

double MapConfig::getMapFontSize()
{
    return map_font_size;
}
