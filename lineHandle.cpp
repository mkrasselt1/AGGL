#include "_aggl_glob.h"
#include "AGGL.h"

using namespace AGGL;

lineHandle::lineHandle(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int32_t color, uint8_t thickness)
{
    _x1 = x1;
    _y1 = y1;
    _x2 = x2;
    _y2 = y2;
    _color = color;
    _thickness = thickness;
    
    // Calculate bounding box
    _newArea.x = min(x1, x2) - thickness/2;
    _newArea.y = min(y1, y2) - thickness/2;
    _newArea.w = abs(x2 - x1) + thickness;
    _newArea.h = abs(y2 - y1) + thickness;
    
    // Handle horizontal and vertical lines
    if(_newArea.w == thickness) _newArea.w = thickness;
    if(_newArea.h == thickness) _newArea.h = thickness;
    
    _oldArea = _newArea;
}

lineHandle::~lineHandle()
{
}

void lineHandle::changeLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
    _x1 = x1;
    _y1 = y1;
    _x2 = x2;
    _y2 = y2;
    
    // Recalculate bounding box
    _newArea.x = min(x1, x2) - _thickness/2;
    _newArea.y = min(y1, y2) - _thickness/2;
    _newArea.w = abs(x2 - x1) + _thickness;
    _newArea.h = abs(y2 - y1) + _thickness;
    
    // Handle horizontal and vertical lines
    if(_newArea.w == _thickness) _newArea.w = _thickness;
    if(_newArea.h == _thickness) _newArea.h = _thickness;
    
    if(_visible)
    {
        _needUpdate = true;
    }
}

void lineHandle::setColor(int32_t color)
{
    _color = color;
    if(_visible)
    {
        _needUpdate = true;
    }
}

void lineHandle::setThickness(uint8_t thickness)
{
    _thickness = thickness;
    
    // Recalculate bounding box
    _newArea.x = min(_x1, _x2) - _thickness/2;
    _newArea.y = min(_y1, _y2) - _thickness/2;
    _newArea.w = abs(_x2 - _x1) + _thickness;
    _newArea.h = abs(_y2 - _y1) + _thickness;
    
    // Handle horizontal and vertical lines
    if(_newArea.w == _thickness) _newArea.w = _thickness;
    if(_newArea.h == _thickness) _newArea.h = _thickness;
    
    if(_visible)
    {
        _needUpdate = true;
    }
}

int32_t lineHandle::getPixelAt(int16_t x, int16_t y)
{
    if( (x >= _newArea.x) && (x < _newArea.x + _newArea.w) && 
        (y >= _newArea.y) && (y < _newArea.y + _newArea.h)
    )
    {
        // Calculate distance from point to line using the point-to-line distance formula
        int32_t dx = _x2 - _x1;
        int32_t dy = _y2 - _y1;
        
        // Handle special cases for horizontal and vertical lines
        if(dx == 0) // Vertical line
        {
            if(abs(x - _x1) <= _thickness/2 && 
               y >= min(_y1, _y2) && y <= max(_y1, _y2))
            {
                return _color;
            }
        }
        else if(dy == 0) // Horizontal line
        {
            if(abs(y - _y1) <= _thickness/2 && 
               x >= min(_x1, _x2) && x <= max(_x1, _x2))
            {
                return _color;
            }
        }
        else // Diagonal line
        {
            // Distance from point (x,y) to line
            int32_t A = dy;
            int32_t B = -dx;
            int32_t C = dx * _y1 - dy * _x1;
            
            int32_t distance_sq = (A * x + B * y + C) * (A * x + B * y + C);
            int32_t line_length_sq = dx * dx + dy * dy;
            
            // Check if point is within thickness of the line
            if(distance_sq <= (_thickness * _thickness * line_length_sq) / 4)
            {
                // Check if point is within the line segment bounds
                int32_t dot_product = (x - _x1) * dx + (y - _y1) * dy;
                if(dot_product >= 0 && dot_product <= line_length_sq)
                {
                    return _color;
                }
            }
        }
    }
    return COLORS::TRANSPARENT;
}

box lineHandle::getCurrentSize()
{
    return _newArea;
}