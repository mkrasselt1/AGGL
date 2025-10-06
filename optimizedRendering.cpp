#include "_aggl_glob.h"
#include <vector>
#include "AGGL.h"

namespace AGGL
{
    // Optimized update function that uses hardware acceleration when available
    STATUS::code updateWithHardwareAcceleration()
    {
        // Step 1: Identify elements that can use hardware acceleration
        vector<graphicsHandle*> hardwareElements;
        vector<graphicsHandle*> softwareElements;
        
        for (auto const& elem : elements)
        {
            if(elem->needUpdate() && elem->_visible)
            {
                if(elem->canUseHardwareAcceleration())
                {
                    hardwareElements.push_back(elem);
                }
                else
                {
                    softwareElements.push_back(elem);
                }
            }
        }
        
        // Step 2: Process hardware-accelerated elements first
        for(auto const& disp : displays)
        {
            if(disp->hasHardwareAcceleration())
            {
                for(auto const& elem : hardwareElements)
                {
                    box elemBounds = elem->getCurrentSize();
                    if(TOOLS::rectIntersect(disp->getSize(), &elemBounds))
                    {
                        STATUS::code result = elem->renderToDisplay(disp);
                        if(result == STATUS::OK)
                        {
                            // Mark as updated by clearing the update flag
                            box dummy1, dummy2;
                            elem->getUpdateArea(&dummy1, &dummy2);
                        }
                    }
                }
            }
        }
        
        // Step 3: For remaining elements or displays without hardware acceleration,
        // fall back to the original pixel-by-pixel rendering
        if(!softwareElements.empty() || !hardwareElements.empty())
        {
            // Call original update function for software rendering
            return update();
        }
        
        return STATUS::OK;
    }

    // Enhanced rendering functions for optimized software rendering
    namespace OPTIMIZED_RENDERING
    {
        void drawHorizontalLine(uint8_t* buffer, int16_t x1, int16_t x2, int16_t y, 
                               int16_t bufferWidth, int32_t color, COLOR_MODE::colormode mode)
        {
            if(x1 > x2) swap(x1, x2);
            
            for(int16_t x = x1; x <= x2; x++)
            {
                int pos = y * bufferWidth + x;
                switch(mode)
                {
                    case COLOR_MODE::TWOCOLOR:
                        buffer[pos] = (color > 0) ? 1 : 0;
                        break;
                    case COLOR_MODE::RGB_8BIT:
                        buffer[pos] = COLORS::convert8Bit(color);
                        break;
                    default:
                        break;
                }
            }
        }
        
        void drawVerticalLine(uint8_t* buffer, int16_t x, int16_t y1, int16_t y2, 
                             int16_t bufferWidth, int32_t color, COLOR_MODE::colormode mode)
        {
            if(y1 > y2) swap(y1, y2);
            
            for(int16_t y = y1; y <= y2; y++)
            {
                int pos = y * bufferWidth + x;
                switch(mode)
                {
                    case COLOR_MODE::TWOCOLOR:
                        buffer[pos] = (color > 0) ? 1 : 0;
                        break;
                    case COLOR_MODE::RGB_8BIT:
                        buffer[pos] = COLORS::convert8Bit(color);
                        break;
                    default:
                        break;
                }
            }
        }
        
        void fillRectangle(uint8_t* buffer, int16_t x, int16_t y, uint16_t w, uint16_t h,
                          int16_t bufferWidth, int32_t color, COLOR_MODE::colormode mode)
        {
            for(int16_t row = y; row < y + h; row++)
            {
                drawHorizontalLine(buffer, x, x + w - 1, row, bufferWidth, color, mode);
            }
        }
    }

} // namespace AGGL