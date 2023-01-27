#include "_aggl_glob.h"

using namespace AGGL;

image8BitHandle::image8BitHandle(int16_t x, int16_t y, uint16_t w, uint16_t h, const uint8_t *image)
{
    _imgBuf = image;
    _newArea.h = h;
    _newArea.w = w;
    _newArea.x = x;
    _newArea.y = y;

    _oldArea = _newArea;
}

image8BitHandle::~image8BitHandle()
{
}

void image8BitHandle::changeImage(int16_t x, int16_t y, uint16_t w, uint16_t h, const uint8_t *image)
{
    _imgBuf = image;
    _newArea.x = x;
    _newArea.y = y;
    _newArea.w = w;
    _newArea.h = h;
    _needUpdate = true;
}

int32_t image8BitHandle::getPixelAt(int16_t x, int16_t y)
{
    if( (x >= _newArea.x) && (x < _newArea.x + _newArea.w) && 
        (y >= _newArea.y) && (y < _newArea.y + _newArea.h)
    )
    {
        
        int16_t lx = x - _newArea.x;
        int16_t ly = y - _newArea.y;
        int32_t color = _imgBuf[ly*_newArea.w + lx];
        return ((color & 0x03) << 6) |
                ((color & 0x1C) << 11) |
                ((color & 0xE0) << 16);
    }
    return COLORS::TRANSPARENT;
}

box image8BitHandle::getCurrentSize()
{
    return _newArea;
}