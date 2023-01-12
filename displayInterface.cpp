#include "AGGL.h"

using namespace AGGL;

displayInterface::displayInterface(int16_t xOffset, int16_t yOffset, uint16_t width, uint16_t height)
{
    _screen.x = xOffset;
    _screen.y = yOffset;
    _screen.w = width;
    _screen.h = height;
}

box* displayInterface::getSize()
{
    return &_screen;
}