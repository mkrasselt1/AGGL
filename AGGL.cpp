#include "_aggl_glob.h"
#include <vector>
#include "AGGL.h"

namespace AGGL
{

    std::vector<displayInterface*> displays;
    std::vector<graphicsHandle*> elements;
    int32_t background = COLORS::BLACK;

    namespace COLORS
    {
        int32_t fromRGB(uint8_t R, uint8_t G, uint8_t B)
        {
            return ((R<<16) | (G<<8) | (B));
        }

        uint8_t convert8Bit(int32_t color)
        {        
            return  ((color >> 16) & 0xE0) |
                    ((color >> 11) & 0x1C) |
                    ((color >> 6) & 0x03);
        }
    } // namespace COLORS
    
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

        // if(areaIndex)
        // {
        //     Serial.println("Before Combination:");
        //     for (size_t i = 0; i < areaIndex; i++)
        //     {
        //         Serial.printf("AREA[%d] = (%d,%d) %d x %d\r\n", i, updateAreas[i].x, updateAreas[i].y, updateAreas[i].w, updateAreas[i].h);
        //     }
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

                    // Serial.printf("combined [%d] and [%d] into (%d,%d) %d x %d\r\n", i, j, bbBound.x, bbBound.y, bbBound.w, bbBound.h);
                    
                    //remove Area j from list
                    for (size_t k = j; k < areaIndex - 1; k++)
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
            // if(areaIndex)
            // {
            //     Serial.println("After Combination:");
            //     for (size_t i = 0; i < areaIndex; i++)
            //     {
            //         Serial.printf("AREA[%d] = (%d,%d) %d x %d\r\n", i, updateAreas[i].x, updateAreas[i].y, updateAreas[i].w, updateAreas[i].h);
            //     }
            //     Serial.println();
            // }            

            for (size_t i = 0; i < areaIndex; i++)
            {
                if(TOOLS::rectIntersect(disp->getSize(), &(updateAreas[i])))
                {
                    box drawArea = TOOLS::maskRectangle(disp->getSize(), &(updateAreas[i]));
                    if(TOOLS::getRectArea(&drawArea))
                    {
                        //step 1: Check if drawArea fits in Buffer, else split it up
                        uint32_t bytesPerLine = 1;

                        //decide on buffersize depending on display colormode
                        switch (disp->getColorMode())
                        {
                            case COLOR_MODE::TWOCOLOR:
                                bytesPerLine = drawArea.w;
                                break;

                            case COLOR_MODE::RGB_8BIT:
                                bytesPerLine = drawArea.w;
                                break;
                            
                            default:
                                Serial.println("Unsupported Color Mode");
                                return STATUS::NOT_SUPPORTED;
                                break;
                        }

                        uint16_t maxRows = disp->getMaxBufferSize() / bytesPerLine;

                        if(!maxRows)
                        {
                            //can't even do a single line
                            Serial.println("OOM");
                            return STATUS::OUT_OF_MEMORY;
                        }

                        box areaPart = drawArea;

                        if(areaPart.h > maxRows)
                            areaPart.h = maxRows;

                        areaPart = disp->adjustUpdateBox(areaPart);

                        uint8_t *buf;

                        buf = new uint8_t[disp->getMaxBufferSize()];

                        if(!buf)
                        {
                            return STATUS::OUT_OF_MEMORY;
                        }

                        while (areaPart.h)
                        {
                            int buffPos = 0;

                            // Serial.printf("Partial AREA = (%d,%d) %d x %d\r\n", areaPart.x, areaPart.y, areaPart.w, areaPart.h);
                        
                            for (int16_t y = areaPart.y; y < areaPart.y + areaPart.h; y++)
                            {
                                for (int16_t x = areaPart.x; x < areaPart.x + areaPart.w; x++)
                                {

                                    switch (disp->getColorMode())
                                    {
                                        case COLOR_MODE::TWOCOLOR:
                                        case COLOR_MODE::RGB_8BIT:
                                            buf[buffPos] = COLORS::BLACK;
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
                                                    case COLOR_MODE::TWOCOLOR:
                                                        if(col > 0)
                                                            buf[buffPos] = 1;
                                                        else
                                                            buf[buffPos] = 0;
                                                        break;

                                                    case COLOR_MODE::RGB_8BIT:
                                                        buf[buffPos] = COLORS::convert8Bit(col);
                                                        break;
                                                    
                                                    default:                                                    
                                                        break;
                                                }
                                                
                                            }
                                        }
                                        
                                    }
                                    buffPos++;                        
                                }
                            }

                            disp->update(areaPart, buf);
                                
                            areaPart.y += areaPart.h;
                            if(areaPart.y > drawArea.y + drawArea.h)
                            {
                                areaPart.h = 0;
                            }
                            else if(areaPart.y + areaPart.h > drawArea.y + drawArea.h)
                            {
                                areaPart.h = drawArea.y - areaPart.y + drawArea.h;
                            }
                            areaPart = disp->adjustUpdateBox(areaPart);
                        }

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

} // namespace AGGL
