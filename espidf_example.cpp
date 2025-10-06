#include "aggl_platform.h"
#include "AGGL.h"

// ESP-IDF specific display implementation example
// This shows how to integrate AGGL with ESP-IDF displays

using namespace AGGL;

class ESPIDF_ST7789_Display : public displayInterface
{
private:
    // ESP-IDF specific display driver handles would go here
    // spi_device_handle_t spi_handle;
    // gpio_num_t dc_pin, rst_pin, cs_pin;

public:
    ESPIDF_ST7789_Display(int16_t xOffset, int16_t yOffset, uint16_t width, uint16_t height, COLOR_MODE::colormode mode)
        : displayInterface(xOffset, yOffset, width, height, mode)
    {
    }

    bool hasHardwareAcceleration() override
    {
        return true; // ST7789 has hardware acceleration
    }

    STATUS::code init() override
    {
        // ESP-IDF specific initialization
        AGGL_LOG("Initializing ST7789 display");

        // Initialize SPI bus
        // spi_bus_config_t buscfg = {...};
        // spi_bus_initialize(HSPI_HOST, &buscfg, SPI_DMA_CH_AUTO);

        // Initialize display pins
        // gpio_set_direction(dc_pin, GPIO_MODE_OUTPUT);
        // gpio_set_direction(rst_pin, GPIO_MODE_OUTPUT);

        // Hardware reset sequence
        // gpio_set_level(rst_pin, 0);
        // AGGL_DELAY(100);
        // gpio_set_level(rst_pin, 1);
        // AGGL_DELAY(100);

        // Send initialization commands
        // sendCommand(0x01); // Software reset
        // AGGL_DELAY(150);

        return STATUS::OK;
    }

    STATUS::code drawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2,
                         int32_t color, uint8_t thickness) override
    {
        // Convert color to display format
        uint16_t displayColor = 0;
        switch(_colormode)
        {
            case COLOR_MODE::RGB_16BIT:
                displayColor = ((color >> 8) & 0xF800) | ((color >> 5) & 0x07E0) | ((color >> 3) & 0x001F);
                break;
            case COLOR_MODE::RGB_8BIT:
                displayColor = COLORS::convert8Bit(color);
                break;
            default:
                return STATUS::NOT_SUPPORTED;
        }

        // Set drawing window
        int16_t minX = AGGL_MIN(x1, x2);
        int16_t minY = AGGL_MIN(y1, y2);
        int16_t maxX = AGGL_MAX(x1, x2);
        int16_t maxY = AGGL_MAX(y1, y2);

        // ESP-IDF ST7789 specific line drawing
        // This would call the actual display driver functions
        /*
        st7789_set_window(minX, minY, maxX - minX + 1, maxY - minY + 1);
        st7789_draw_line(x1, y1, x2, y2, displayColor, thickness);
        */

        AGGL_LOG("Drawing line from (%d,%d) to (%d,%d) with color 0x%04X", x1, y1, x2, y2, displayColor);
        return STATUS::OK;
    }

    STATUS::code drawRectangle(int16_t x, int16_t y, uint16_t w, uint16_t h,
                              int32_t fillColor, int32_t borderColor, uint8_t borderThickness) override
    {
        // ESP-IDF rectangle drawing implementation
        if(fillColor != COLORS::TRANSPARENT)
        {
            // Fill rectangle
            uint16_t displayFillColor = convertColor(fillColor);
            /*
            st7789_fill_rect(x, y, w, h, displayFillColor);
            */
            AGGL_LOG("Filling rectangle at (%d,%d) %dx%d with color 0x%04X", x, y, w, h, displayFillColor);
        }

        if(borderThickness > 0 && borderColor != COLORS::TRANSPARENT)
        {
            // Draw border
            uint16_t displayBorderColor = convertColor(borderColor);
            /*
            st7789_draw_rect(x, y, w, h, displayBorderColor, borderThickness);
            */
            AGGL_LOG("Drawing rectangle border at (%d,%d) %dx%d with thickness %d", x, y, w, h, borderThickness);
        }

        return STATUS::OK;
    }

    STATUS::code update(box bb, uint8_t* buffer) override
    {
        // Fallback software rendering for complex elements
        // Push buffer to display using ESP-IDF SPI functions
        /*
        st7789_set_window(bb.x, bb.y, bb.w, bb.h);
        spi_transaction_t t = {
            .length = bb.w * bb.h * 2, // 16-bit colors
            .tx_buffer = buffer,
            .user = (void*)0,
        };
        spi_device_transmit(spi_handle, &t);
        */

        AGGL_LOG("Software rendering update: (%d,%d) %dx%d", bb.x, bb.y, bb.w, bb.h);
        return STATUS::OK;
    }

private:
    uint16_t convertColor(int32_t color)
    {
        switch(_colormode)
        {
            case COLOR_MODE::RGB_16BIT:
                return ((color >> 8) & 0xF800) | ((color >> 5) & 0x07E0) | ((color >> 3) & 0x001F);
            case COLOR_MODE::RGB_8BIT:
                return COLORS::convert8Bit(color);
            default:
                return 0;
        }
    }
};

// Example usage in ESP-IDF application
extern "C" void app_main()
{
    // Initialize AGGL with ESP-IDF display
    ESPIDF_ST7789_Display* display = new ESPIDF_ST7789_Display(0, 0, 240, 320, COLOR_MODE::RGB_16BIT);
    AGGL::addDisplay(display);
    AGGL::start();

    // Create graphics elements
    lineHandle* line = new lineHandle(10, 10, 230, 310, COLORS::RED, 3);
    rectangleHandle* rect = new rectangleHandle(50, 50, 140, 100, COLORS::BLUE, COLORS::WHITE, 2);

    line->show();
    rect->show();

    // Use hardware-accelerated update
    while(true)
    {
        AGGL::updateWithHardwareAcceleration();
        AGGL_DELAY(16); // ~60 FPS
    }
}