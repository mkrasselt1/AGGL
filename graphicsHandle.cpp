#include "_aggl_glob.h"

using namespace AGGL;



graphicsHandle::graphicsHandle()
{
    _visible = false;
    _needUpdate = false;
    elements.push_back(this);
}

void graphicsHandle::removeHandle()
{
    if(std::find(elements.begin(), elements.end(), this) != elements.end()){
        elements.erase(std::find(elements.begin(), elements.end(), this));
    }
}

void graphicsHandle::getUpdateArea(box * oldArea, box * newArea)
{
    *oldArea = _oldArea;
    *newArea = _newArea;
    _oldArea = _newArea;
    if(!_visible)
    {
        _oldArea.w = 0;
        _oldArea.h = 0;
    }
    _needUpdate = false;
}

bool graphicsHandle::needUpdate()
{
    return _needUpdate;
}

void graphicsHandle::show()
{
    _visible = true;
    _needUpdate = true;
    _newArea = getCurrentSize();
}

void graphicsHandle::hide()
{
    _visible = false;
    _needUpdate = true;
}

void graphicsHandle::changePosition(int16_t x, int16_t y)
{
    _newArea.x = x;
    _newArea.y = y;
    _needUpdate = true;
}