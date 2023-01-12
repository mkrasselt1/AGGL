#include "_aggl_glob.h"
#include <vector>
#include "AGGL.h"

namespace AGGL
{

    std::vector<displayInterface*> displays;
    std::vector<graphicsHandle*> elements;
    COLOR_MODE::colormode colormode;
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
        for (auto const& elem:elements)
        {
            if(elem->needUpdate())
            {
                box bbOld;
                box bbNew;
                elem->getUpdateArea(&bbOld, &bbNew);

                //step1: Redraw old bounding box if has area

                uint8_t *buf;
                int i = 0;

                // Serial.printf("Drawing Old Area is %d x %d at %d,%d\r\n", bbOld.w, bbOld.h, bbOld.x, bbOld.y);
                if(bbOld.w*bbOld.h > 0)
                {
                    buf = new uint8_t[bbOld.w*bbOld.h];
                
                    for (int16_t y = bbOld.y; y < bbOld.y + bbOld.h; y++)
                    {
                        for (int16_t x = bbOld.x; x < bbOld.x + bbOld.w; x++)
                        {
                            buf[i] = COLORS::BLACK;
                            for(auto const& lelem:elements)
                            {
                                if(lelem->_visible){
                                    //todo: build list of graphicsElements intersecting redrawarea before
                                    int32_t col = lelem->getPixelAt(x,y);
                                    if(col != COLORS::TRANSPARENT)
                                    {
                                        buf[i] = col;
                                    }
                                }
                                
                            }
                            i++;                        
                        }
                    }
                    
                    //step2: find display where to draw the data
                    for(auto const& disp:displays)
                    {
                        disp->update(bbOld, buf);
                    }

                    delete[] buf;
                }


                


                //step1: Redraw new bounding box if has area
                // Serial.printf("Drawing New Area is %d x %d at %d,%d\r\n", bbNew.w, bbNew.h, bbNew.x, bbNew.y);
                if(bbNew.w*bbNew.h > 0)
                {
                    buf = new uint8_t[bbNew.w*bbNew.h];
                    i = 0;
                    for (int16_t y = bbNew.y; y < bbNew.y + bbNew.h; y++)
                    {
                        for (int16_t x = bbNew.x; x < bbNew.x + bbNew.w; x++)
                        {
                            buf[i] = COLORS::BLACK;
                            for(auto const& lelem:elements)
                            {
                                if(lelem->_visible)
                                {
                                    //todo: build list of graphicsElements intersecting redrawarea before
                                    int32_t col = lelem->getPixelAt(x,y);
                                    if(col != COLORS::TRANSPARENT)
                                    {
                                        buf[i] = col;
                                    }
                                }
                                
                            }
                            i++;     
                                               
                        }
                    }
                    
                    //step2: find display where to draw the data
                    for(auto const& disp:displays)
                    {
                        disp->update(bbNew, buf);
                    }

                    delete[] buf;
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


    STATUS::code setColorMode(COLOR_MODE::colormode mode)
    {
        colormode = mode;
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
            return _imgBuf[ly*_newArea.w + lx];
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

} // namespace AGGL




