#include "AGGL.h"

using namespace AGGL;

displayInterface::displayInterface(int16_t xOffset, int16_t yOffset, uint16_t width, uint16_t height, COLOR_MODE::colormode mode)
{
    _screen.x = xOffset;
    _screen.y = yOffset;
    _screen.w = width;
    _screen.h = height;
    _colormode = mode;
}

box* displayInterface::getSize()
{
    return &_screen;
}

COLOR_MODE::colormode AGGL::displayInterface::getColorMode()
{
    return _colormode;
}
