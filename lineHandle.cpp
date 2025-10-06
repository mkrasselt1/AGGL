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
    _newArea.x = AGGL_MIN(x1, x2) - thickness/2;
    _newArea.y = AGGL_MIN(y1, y2) - thickness/2;
    _newArea.w = AGGL_ABS(x2 - x1) + thickness;
    _newArea.h = AGGL_ABS(y2 - y1) + thickness;
    
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
    _newArea.x = AGGL_MIN(x1, x2) - _thickness/2;
    _newArea.y = AGGL_MIN(y1, y2) - _thickness/2;
    _newArea.w = AGGL_ABS(x2 - x1) + _thickness;
    _newArea.h = AGGL_ABS(y2 - y1) + _thickness;
    
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
    _newArea.x = AGGL_MIN(_x1, _x2) - _thickness/2;
    _newArea.y = AGGL_MIN(_y1, _y2) - _thickness/2;
    _newArea.w = AGGL_ABS(_x2 - _x1) + _thickness;
    _newArea.h = AGGL_ABS(_y2 - _y1) + _thickness;
    
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
        // Optimized line drawing algorithm
        int32_t dx = _x2 - _x1;
        int32_t dy = _y2 - _y1;
        
        // Handle special cases for horizontal and vertical lines (faster)
        if(dx == 0) // Vertical line
        {
            if(AGGL_ABS(x - _x1) <= _thickness/2 && 
               y >= AGGL_MIN(_y1, _y2) && y <= AGGL_MAX(_y1, _y2))
            {
                return _color;
            }
        }
        else if(dy == 0) // Horizontal line
        {
            if(AGGL_ABS(y - _y1) <= _thickness/2 && 
               x >= AGGL_MIN(_x1, _x2) && x <= AGGL_MAX(_x1, _x2))
            {
                return _color;
            }
        }
        else // Diagonal line - use optimized distance calculation
        {
            // Use integer arithmetic for better performance
            int32_t A = dy;
            int32_t B = -dx;
            int32_t C = dx * _y1 - dy * _x1;
            
            // Distance squared from point to line (avoiding sqrt for performance)
            int64_t distance_sq = (int64_t)(A * x + B * y + C) * (A * x + B * y + C);
            int64_t line_length_sq = (int64_t)dx * dx + (int64_t)dy * dy;
            
            // Check if point is within thickness of the line
            int64_t threshold = ((int64_t)_thickness * _thickness * line_length_sq) / 4;
            
            if(distance_sq <= threshold)
            {
                // Check if point is within the line segment bounds using dot product
                int64_t dot_product = (int64_t)(x - _x1) * dx + (int64_t)(y - _y1) * dy;
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

bool lineHandle::canUseHardwareAcceleration()
{
    return true; // Lines are commonly hardware-accelerated
}

STATUS::code lineHandle::renderToDisplay(displayInterface* display)
{
    if(display->hasHardwareAcceleration())
    {
        // Try hardware-accelerated rendering first
        STATUS::code result = display->drawLine(_x1, _y1, _x2, _y2, _color, _thickness);
        if(result == STATUS::OK)
        {
            return STATUS::OK;
        }
    }
    
    // Fall back to software rendering if hardware acceleration not available
    return STATUS::NOT_SUPPORTED;
}