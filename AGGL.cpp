#include "_aggl_glob.h"
#include <vector>
#include "AGGL.h"

namespace AGGL
{

    std::vector<displayInterface*> displays;
    std::vector<graphicsHandle*> elements;
    int32_t background = COLORS::BLACK;

    STATUS::code addDisplay(displayInterface* display)
    {
        if(display){
            displays.push_back(display);
            return STATUS::OK;
        }
        return STATUS::GENERAL_ERROR;
    }

    STATUS::code update()
    {
        //Walk through elements Background to Foreground
        const uint8_t numAreas = 20;
        box updateAreas[numAreas];
        int areaIndex = 0;

        box bbOld;
        box bbNew;
        box bbBound;
        //Step 1: Collect all Screen Areas needing Updates
        for (auto const& elem:elements)
        {
            if(elem->needUpdate())
            {
                if(numAreas - areaIndex < 2)
                    break;

                
                elem->getUpdateArea(&bbOld, &bbNew);

                bbBound = TOOLS::getBoundingBox(&bbOld, &bbNew);

                //If Area of Combined Old and New is less than their sum, we better only add one Update Area, else both
                if(TOOLS::getRectArea(&bbBound) <= TOOLS::getRectArea(&bbOld) + TOOLS::getRectArea(&bbNew))
                {
                    updateAreas[areaIndex] = bbBound;
                    areaIndex++;
                }
                else
                {
                    updateAreas[areaIndex] = bbOld;
                    areaIndex++;
                    updateAreas[areaIndex] = bbNew;
                    areaIndex++;
                }
            }
        }

        // Serial.println("Before Combination:");
        // for (size_t i = 0; i < areaIndex; i++)
        // {
        //     Serial.printf("AREA[%d] = (%d,%d) %d x %d\r\n", i, updateAreas[i].x, updateAreas[i].y, updateAreas[i].w, updateAreas[i].h);
        // }

        //Step 2: Combine Areas from list, if the Area of their intersection is less than their sum
        //should run nlogn or something
        for (size_t i = 0; i < areaIndex; i++)
        {
            //no need to compare with itself or it's predeccessors
            for (size_t j = i+1; j < areaIndex; j++)
            {
                bbBound = TOOLS::getBoundingBox(&(updateAreas[i]), &(updateAreas[j]));

                if(TOOLS::getRectArea(&bbBound) <= TOOLS::getRectArea(&(updateAreas[i])) + TOOLS::getRectArea(&(updateAreas[j])))
                {
                    updateAreas[i] = bbBound;
                    
                    //remove Area j from list
                    for (size_t k = j; i < areaIndex - 1; i++)
                    {
                        updateAreas[k] = updateAreas[k + 1];
                    }
                    areaIndex--;
                }
            }
        }

        // Serial.println("After Combination:");
        // for (size_t i = 0; i < areaIndex; i++)
        // {
        //     Serial.printf("AREA[%d] = (%d,%d) %d x %d\r\n", i, updateAreas[i].x, updateAreas[i].y, updateAreas[i].w, updateAreas[i].h);
        // }

        //Step 3: go through all available displays and create their respective Screen Update Areas

        for(auto const& disp:displays)
        {
            for (size_t i = 0; i < areaIndex; i++)
            {
                if(TOOLS::rectIntersect(disp->getSize(), &(updateAreas[i])))
                {
                    box drawArea = TOOLS::maskRectangle(disp->getSize(), &(updateAreas[i]));
                    if(TOOLS::getRectArea(&drawArea))
                    {

                        //step1: Redraw old bounding box if has area
                        uint8_t *buf;
                        int i = 0;


                        //decide on buffersize depending on display colormode
                        switch (disp->getColorMode())
                        {

                            case COLOR_MODE::RGB_8BIT:
                                buf = new uint8_t[drawArea.w*drawArea.h];
                                break;
                            
                            default:
                                return STATUS::NOT_SUPPORTED;
                                break;
                        }
                        

                        if(!buf)
                        {
                            return STATUS::OUT_OF_MEMORY;
                        }
                    
                        for (int16_t y = drawArea.y; y < drawArea.y + drawArea.h; y++)
                        {
                            for (int16_t x = drawArea.x; x < drawArea.x + drawArea.w; x++)
                            {

                                switch (disp->getColorMode())
                                {

                                    case COLOR_MODE::RGB_8BIT:
                                        buf[i] = COLORS::BLACK;
                                        break;
                                    
                                    default:
                                        
                                        break;
                                }
                                
                                for(auto const& lelem:elements)
                                {
                                    if(lelem->_visible){
                                        //todo: build list of graphicsElements intersecting redrawarea before
                                        int32_t col = lelem->getPixelAt(x,y);
                                        if(col != COLORS::TRANSPARENT)
                                        {
                                            switch (disp->getColorMode())
                                            {

                                                case COLOR_MODE::RGB_8BIT:
                                                    buf[i] = COLORS::convert8Bit(col);
                                                    break;
                                                
                                                default:                                                    
                                                    break;
                                            }
                                            
                                        }
                                    }
                                    
                                }
                                i++;                        
                            }
                        }

                        disp->update(drawArea, buf);
                            
                        delete[] buf;
                    }
                }
                
            }
        }
        
        return STATUS::OK;
    }

    STATUS::code start()
    {
        for(auto const& disp:displays)
        {
            disp->init();
        }
        return STATUS::OK;
    }

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
            return (color & 0xE0) |
                    ((color & 0x1C) << 11) |
                    ((color & 0x03) << 22);
        }
        return COLORS::TRANSPARENT;
    }

    box image8BitHandle::getCurrentSize()
    {
        return _newArea;
    }

    uint32_t TOOLS::getRectArea(const box *bb)
    {
        return bb->w*bb->h;
    }

    bool TOOLS::rectIntersect(const box *b1, const box *b2)
    {
        return !(
                    (b1->x + b1->w) < b2->x || 
                    b1->x > (b2->x + b2->w) ||
                    b1->y > (b2->y + b2->h) || 
                    (b1->y + b1->h) < b2->y
                );
    }

    box TOOLS::getBoundingBox(const box *b1, const box *b2)
    {
        //if one of the 2 has Zero Area, return the other
        if(!getRectArea(b1))
        {
            return *b2;
        }
        if(!getRectArea(b2))
        {
            return *b1;
        }

        int16_t xMin = b1->x;
        int16_t xMax = b1->x + b1->w;

        int16_t yMin = b1->y;
        int16_t yMax = b1->y + b1->h;

        if(b2->x < xMin)
            xMin = b2->x;

        if(b2->y < yMin)
            yMin = b2->y;

        if(b2->x + b2->w > xMax)
            xMax = b2->x + b2->w;

        if(b2->y + b2->h > yMax)
            yMax = b2->y + b2->h;

        box outBB;
        outBB.x = xMin;
        outBB.y = yMin;
        outBB.w = xMax - xMin;
        outBB.h = yMax - yMin;

        return outBB;
    }

    box TOOLS::maskRectangle(const box *mask, const box *b)
    {
        box erg;
        if(b->x < mask->x)
            erg.x = mask->x;
        else
            erg.x = b->x;

        if(b->y < mask->y)
            erg.y = mask->y;
        else
            erg.y = b->y;

        if( (b->x + b->w) > (mask->x + mask->w) )
            erg.w = (mask->x + mask->w) - erg.x;
        else
            erg.w = (b->x + b->w) - erg.x;

        if( (b->y + b->h) > (mask->y + mask->h) )
            erg.h = (mask->y + mask->h) - erg.y;
        else
            erg.h = (b->y + b->h) - erg.y;

        return erg;
    }

    uint8_t COLORS::convert8Bit(int32_t color)
    {        
        return  ((color) & 0xE0) |
                ((color >> 11) & 0x1C) |
                ((color >> 22) & 0x03);
    }

} // namespace AGGL


