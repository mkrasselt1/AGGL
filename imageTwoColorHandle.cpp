#include "_aggl_glob.h"
#include "AGGL.h"

using namespace AGGL;

imageTwoColorHandle::imageTwoColorHandle(int16_t x, int16_t y, uint16_t w, uint16_t h, const uint8_t *image)
{
    _imgBuf = image;
    _newArea.h = h;
    _newArea.w = w;
    _newArea.x = x;
    _newArea.y = y;

    _oldArea = _newArea;
}

imageTwoColorHandle::~imageTwoColorHandle()
{
}

void imageTwoColorHandle::changeImage(int16_t x, int16_t y, uint16_t w, uint16_t h, const uint8_t *image)
{
    _imgBuf = image;
    _newArea.x = x;
    _newArea.y = y;
    _newArea.w = w;
    _newArea.h = h;
    _needUpdate = true;
}

void imageTwoColorHandle::setForeground(int32_t color)
{
    _foreground = color;
}

void imageTwoColorHandle::setBackground(int32_t color)
{
    _background = color;
}

int32_t imageTwoColorHandle::getPixelAt(int16_t x, int16_t y)
{
    if( (x >= _newArea.x) && (x < _newArea.x + _newArea.w) && 
        (y >= _newArea.y) && (y < _newArea.y + _newArea.h)
    )
    {
        
        int16_t lx = x - _newArea.x;
        int16_t ly = y - _newArea.y;
        uint16_t rx = lx / 8;
        uint16_t px = lx % 8;
        uint16_t bytewidth = _newArea.w/8;
        if(_newArea.w % 8) bytewidth++;

        if(_imgBuf[ly*bytewidth + rx] & (1<<px)){
            return _foreground;
        }else{
            return _background;
        }
    }
    return COLORS::TRANSPARENT;
}

box AGGL::imageTwoColorHandle::getCurrentSize()
{
    return _newArea;
}
