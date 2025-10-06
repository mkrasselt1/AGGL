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
    auto it = find(elements.begin(), elements.end(), this);
    if(it != elements.end()){
        elements.erase(it);
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
        newArea->w = 0;
        newArea->h = 0;
    }
    _needUpdate = false;
}

bool graphicsHandle::needUpdate()
{
    return _needUpdate;
}

void graphicsHandle::show()
{
    if(!_visible)
    {
        _visible = true;
        _needUpdate = true;
    }
}

void graphicsHandle::hide()
{
    if(_visible)
    {
        _visible = false;
        _needUpdate = true;
    }
    
}

void graphicsHandle::changePosition(int16_t x, int16_t y)
{
    _newArea.x = x;
    _newArea.y = y;
    _needUpdate = true;
}