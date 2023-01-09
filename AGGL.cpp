#include "AGGL.h"
#include <vector>

namespace AGGL
{

    std::vector<displayInterface*> displays;
    std::vector<graphicsHandle*> elements;
    COLOR_MODE::colormode colormode;
    int32_t background = COLORS::BLACK;




    displayInterface::displayInterface(uint16_t width, uint16_t height)
    {
        _size.x = 0;
        _size.y = 0;
        _size.w = width;
        _size.h = height;
    }

    box* displayInterface::getSize()
    {
        return &_size;
    }

    graphicsHandle::graphicsHandle()
    {
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
        _newArea = _oldArea;
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
    }

    void graphicsHandle::hide()
    {
        _visible = false;
        _needUpdate = true;
    }


    textHandle::textHandle(int16_t x, int16_t y, const char* text, const uint8_t * font)
    {

    }

    int32_t textHandle::getPixelAt(int16_t x, int16_t y)
    {

    }

    STATUS::code addDisplay(displayInterface* display)
    {
        displays.push_back(display);
    }

    STATUS::code update()
    {
        //Walk through elements Background to Foreground
        for (auto const& elem:elements)
        {
            if(elem->needUpdate())
            {
                box bbOld;
                box bbNew;
                elem->getUpdateArea(&bbOld, &bbNew);

                //step1: Redraw old bounding box
                uint8_t *buf = new uint8_t[bbOld.w*bbOld.h];

            }
        }
    }

    int32_t getPixelAt(int16_t x, int16_t y)
    {
        //Walk through elements Background to Foreground and compute pixel
        int32_t cPixel = COLORS::TRANSPARENT;
        for (auto const& elem:elements)
        {
            if(elem->_visible)
            {
                if(elem->_needUpdate)
                {
                    int32_t temp = elem->getPixelAt(x, y);
                    if(temp != COLORS::TRANSPARENT)
                    {
                        cPixel = temp;
                    }
                }
            }
            
        }
    }

    STATUS::code setColorMode(COLOR_MODE::colormode mode)
    {
        colormode = mode;
    }
    
} // namespace AGGL



