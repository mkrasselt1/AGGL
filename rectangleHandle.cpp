#include "_aggl_glob.h"
#include "AGGL.h"

using namespace AGGL;

rectangleHandle::rectangleHandle(int16_t x, int16_t y, uint16_t w, uint16_t h, int32_t fillColor, int32_t borderColor, uint8_t borderThickness)
{
    _fillColor = fillColor;
    _borderColor = borderColor;
    _borderThickness = borderThickness;
    
    _newArea.x = x;
    _newArea.y = y;
    _newArea.w = w;
    _newArea.h = h;
    
    _oldArea = _newArea;
}

rectangleHandle::~rectangleHandle()
{
}

void rectangleHandle::changeRectangle(int16_t x, int16_t y, uint16_t w, uint16_t h)
{
    _newArea.x = x;
    _newArea.y = y;
    _newArea.w = w;
    _newArea.h = h;
    
    if(_visible)
    {
        _needUpdate = true;
    }
}

void rectangleHandle::setFillColor(int32_t color)
{
    _fillColor = color;
    if(_visible)
    {
        _needUpdate = true;
    }
}

void rectangleHandle::setBorderColor(int32_t color)
{
    _borderColor = color;
    if(_visible)
    {
        _needUpdate = true;
    }
}

void rectangleHandle::setBorderThickness(uint8_t thickness)
{
    _borderThickness = thickness;
    if(_visible)
    {
        _needUpdate = true;
    }
}

int32_t rectangleHandle::getPixelAt(int16_t x, int16_t y)
{
    if( (x >= _newArea.x) && (x < _newArea.x + _newArea.w) && 
        (y >= _newArea.y) && (y < _newArea.y + _newArea.h)
    )
    {
        // Check if we're in the border area
        bool inLeftBorder = (x >= _newArea.x) && (x < _newArea.x + _borderThickness);
        bool inRightBorder = (x >= _newArea.x + _newArea.w - _borderThickness) && (x < _newArea.x + _newArea.w);
        bool inTopBorder = (y >= _newArea.y) && (y < _newArea.y + _borderThickness);
        bool inBottomBorder = (y >= _newArea.y + _newArea.h - _borderThickness) && (y < _newArea.y + _newArea.h);
        
        // If we have a border and we're in the border area
        if(_borderThickness > 0 && _borderColor != COLORS::TRANSPARENT && 
           (inLeftBorder || inRightBorder || inTopBorder || inBottomBorder))
        {
            return _borderColor;
        }
        
        // Otherwise, return fill color
        return _fillColor;
    }
    return COLORS::TRANSPARENT;
}

box rectangleHandle::getCurrentSize()
{
    return _newArea;
}