#include "AGGL.h"
// Example implementation for a display with hardware acceleration
// This would be customized for specific display controllers like ST7789, ILI9341, etc.

using namespace AGGL;

class ExampleAcceleratedDisplay : public displayInterface
{
private:
    // Display-specific variables would go here
    
public:
    ExampleAcceleratedDisplay(int16_t xOffset, int16_t yOffset, uint16_t width, uint16_t height, COLOR_MODE::colormode mode)
        : displayInterface(xOffset, yOffset, width, height, mode)
    {
    }
    
    bool hasHardwareAcceleration() override
    {
        return true; // This display supports hardware acceleration
    }
    
    STATUS::code drawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int32_t color, uint8_t thickness) override
    {
        // Example implementation for hardware line drawing
        // This would call the display controller's native line drawing function
        
        // Check bounds
        if(x1 < _screen.x || x1 >= _screen.x + _screen.w ||
           y1 < _screen.y || y1 >= _screen.y + _screen.h ||
           x2 < _screen.x || x2 >= _screen.x + _screen.w ||
           y2 < _screen.y || y2 >= _screen.y + _screen.h)
        {
            // Line is outside display bounds
            return STATUS::GENERAL_ERROR;
        }
        
        // Convert color based on display mode
        uint16_t displayColor = 0;
        switch(_colormode)
        {
            case COLOR_MODE::TWOCOLOR:
                displayColor = (color > 0) ? 1 : 0;
                break;
            case COLOR_MODE::RGB_8BIT:
                displayColor = COLORS::convert8Bit(color);
                break;
            case COLOR_MODE::RGB_16BIT:
                // Convert 24-bit to 16-bit RGB565
                displayColor = ((color >> 8) & 0xF800) | ((color >> 5) & 0x07E0) | ((color >> 3) & 0x001F);
                break;
            default:
                return STATUS::NOT_SUPPORTED;
        }
        
        // Hardware-specific line drawing implementation
        // Example pseudo-code:
        /*
        display.setWindow(AGGL_MIN(x1,x2), AGGL_MIN(y1,y2), AGGL_ABS(x2-x1)+thickness, AGGL_ABS(y2-y1)+thickness);
        display.drawLine(x1, y1, x2, y2, displayColor, thickness);
        */
        
        // For demonstration, we'll return OK
        // In a real implementation, you'd call the actual hardware function
        return STATUS::OK;
    }
    
    STATUS::code drawRectangle(int16_t x, int16_t y, uint16_t w, uint16_t h, 
                              int32_t fillColor, int32_t borderColor, uint8_t borderThickness) override
    {
        // Check bounds
        if(x < _screen.x || x + w > _screen.x + _screen.w ||
           y < _screen.y || y + h > _screen.y + _screen.h)
        {
            return STATUS::GENERAL_ERROR;
        }
        
        // Convert colors
        uint16_t displayFillColor = 0;
        uint16_t displayBorderColor = 0;
        
        switch(_colormode)
        {
            case COLOR_MODE::TWOCOLOR:
                displayFillColor = (fillColor > 0) ? 1 : 0;
                displayBorderColor = (borderColor > 0) ? 1 : 0;
                break;
            case COLOR_MODE::RGB_8BIT:
                displayFillColor = COLORS::convert8Bit(fillColor);
                displayBorderColor = COLORS::convert8Bit(borderColor);
                break;
            case COLOR_MODE::RGB_16BIT:
                displayFillColor = ((fillColor >> 8) & 0xF800) | ((fillColor >> 5) & 0x07E0) | ((fillColor >> 3) & 0x001F);
                displayBorderColor = ((borderColor >> 8) & 0xF800) | ((borderColor >> 5) & 0x07E0) | ((borderColor >> 3) & 0x001F);
                break;
            default:
                return STATUS::NOT_SUPPORTED;
        }
        
        // Hardware-specific rectangle drawing
        // Example pseudo-code:
        /*
        if(fillColor != COLORS::TRANSPARENT)
        {
            display.fillRect(x, y, w, h, displayFillColor);
        }
        
        if(borderThickness > 0 && borderColor != COLORS::TRANSPARENT)
        {
            display.drawRect(x, y, w, h, displayBorderColor, borderThickness);
        }
        */
        
        return STATUS::OK;
    }
    
    STATUS::code fillRectangle(int16_t x, int16_t y, uint16_t w, uint16_t h, int32_t color) override
    {
        // Simple rectangle fill without border
        return drawRectangle(x, y, w, h, color, COLORS::TRANSPARENT, 0);
    }
    
    // Standard display interface methods would still be implemented
    STATUS::code init() override
    {
        // Initialize display hardware
        return STATUS::OK;
    }
    
    STATUS::code update(box bb, uint8_t* buffer) override
    {
        // Fallback software rendering for complex elements
        // This would push the buffer to the display
        return STATUS::OK;
    }
};