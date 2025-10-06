#ifndef AGGL_H
#define AGGL_H

// Include platform abstraction layer
#include "aggl_platform.h"

namespace AGGL
{
    namespace STATUS
    {
        enum code{
            OK,
            GENERAL_ERROR,
            OUT_OF_MEMORY,
            FONT_NOT_SUPPORTED,
            COMM_ERROR,
            NOT_SUPPORTED
        }; 
    } // namespace STATUS

    namespace COLOR_MODE
    {
        enum colormode{
            TWOCOLOR,
            RGB_8BIT,
            RGB_16BIT,
            RGB_24BIT
        }; 
    } // namespace COLOR_MODE

    namespace COLORS
    {
        enum color{
            TRANSPARENT = -1,
            BLACK =     0x00000000,
            WHITE =     0x00FFFFFF,
            RED =       0x00FF0000,
            GREEN =     0x0000FF00,
            BLUE =      0x000000FF,
            YELLOW =    0x00FFFF00,
            PEANUTPAY_8BIT = 0x004060C0
        }; 
        int32_t fromRGB(uint8_t R, uint8_t G, uint8_t B);
        uint8_t convert8Bit(int32_t color);
    } // namespace COLORS

    typedef struct{
        int16_t x;
        int16_t y;
        uint16_t w;
        uint16_t h;
    } box;
    
#define AGGL_BUFFER_SIZE_UNLIMITED (-1)
    // Forward declarations for optimized rendering
    class lineHandle;
    class rectangleHandle;

    class displayInterface
    {
        protected:
        box _screen = {0};
        COLOR_MODE::colormode _colormode;
        public:
        displayInterface(int16_t xOffset, int16_t yOffset, uint16_t width, uint16_t height, COLOR_MODE::colormode mode);
        virtual ~displayInterface(){}
        virtual bool isPresent(){return false;}
        virtual STATUS::code init(){return STATUS::GENERAL_ERROR;}
        virtual int32_t getMaxBufferSize(){return AGGL_BUFFER_SIZE_UNLIMITED;}
        virtual STATUS::code update(box bb, uint8_t* buffer){return STATUS::GENERAL_ERROR;}
        virtual box adjustUpdateBox(box bb){return bb;}
        
        // Hardware-accelerated rendering functions (optional overrides)
        virtual bool hasHardwareAcceleration(){return false;}
        virtual STATUS::code drawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int32_t color, uint8_t thickness){return STATUS::NOT_SUPPORTED;}
        virtual STATUS::code drawRectangle(int16_t x, int16_t y, uint16_t w, uint16_t h, int32_t fillColor, int32_t borderColor, uint8_t borderThickness){return STATUS::NOT_SUPPORTED;}
        virtual STATUS::code fillRectangle(int16_t x, int16_t y, uint16_t w, uint16_t h, int32_t color){return STATUS::NOT_SUPPORTED;}
        
        box* getSize();
        
        COLOR_MODE::colormode getColorMode();
    };

    class graphicsHandle
    {
        private:
        void getUpdateArea(box * oldArea, box * newArea);
        bool needUpdate();
        void removeHandle();

        protected:
        bool _visible = false;
        bool _needUpdate = false;
        box _oldArea;
        box _newArea;

        public:
        graphicsHandle();
        virtual ~graphicsHandle(){removeHandle();}
        virtual int32_t getPixelAt(int16_t x, int16_t y){return 0;}
        virtual box getCurrentSize(){return box();}
        
        // New methods for hardware acceleration
        virtual bool canUseHardwareAcceleration(){return false;}
        virtual STATUS::code renderToDisplay(displayInterface* display){return STATUS::NOT_SUPPORTED;}
        
        void show();
        void hide();
        void changePosition(int16_t x, int16_t y);


        friend STATUS::code update();
        friend STATUS::code updateWithHardwareAcceleration();
        friend int32_t getPixelAt(int16_t x, int16_t y);
    };

    class textHandle : public graphicsHandle
    {
        private:
        char _text[100];
        const uint8_t * _font = nullptr;
        const uint8_t * _glyph = nullptr;
        const uint8_t * _glyphBitmap = nullptr;
        uint8_t _glyphBitmapBitOffset = 0;
        uint8_t _numTextLines = 0;
        int32_t _foreground = COLORS::WHITE;
        int32_t _background = COLORS::TRANSPARENT;
        bool _bufferedMode = false;
        uint8_t * _buffer = nullptr;

        bool selectGlyph(uint16_t code);
        int32_t getGlyphPixel(uint8_t x, uint8_t y);
        void fillBufferGlyph(uint16_t xBits, uint16_t y, uint16_t bWidth);
        box getTextSize();
        int16_t readBitString(const uint8_t * buf, uint16_t offset, uint16_t len);
        uint16_t readBitStringU(const uint8_t * buf, uint16_t offset, uint16_t len);
        uint16_t countOnes(const uint8_t * buf, uint16_t offset);
        void tryUpdateBuffer();
        int32_t getPixelAtBuffered(int16_t x, int16_t y);
        int32_t getPixelAtUnbuffered(int16_t x, int16_t y);
        

        struct{
            box bb;
            uint16_t pitch;
        } _glyphData;

        struct __attribute__ ((packed)){
            uint8_t numberOfGlyphs;
            uint8_t boundingBoxMode;
            uint8_t zeroBitRLE;
            uint8_t oneBitRLE;
            uint8_t bitCntW;
            uint8_t bitCntH;
            uint8_t bitCntX;
            uint8_t bitCntY;
            uint8_t bitCntD;
            uint8_t bbWidth;
            uint8_t bbHeight;
            uint8_t bbX;
            uint8_t bbY;
            uint8_t asc_A;
            uint8_t des_g;
            uint8_t asc_OpenBracket;
            uint8_t des_CloseBracket;
            uint16_t offset_A;
            uint16_t offset_a;
            uint16_t offset_0x0100;
        } BDFHeader;

        public:
        textHandle(int16_t x, int16_t y, const char* text, const uint8_t * font);
        void changeFont(const uint8_t * font);
        void changeScale(uint8_t scale);
        void changeText(const char* text);
        void setForeground(int32_t color);
        void setBackground(int32_t color);
        int32_t getPixelAt(int16_t x, int16_t y);  
        box getCurrentSize();    
    };

    class imageTwoColorHandle : public graphicsHandle
    {
        private:
        const uint8_t * _imgBuf = nullptr;
        int32_t _foreground = COLORS::WHITE;
        int32_t _background = COLORS::TRANSPARENT;
        bool _reverseBitorder;
        uint8_t _scale = 1;

        public:
        imageTwoColorHandle(int16_t x, int16_t y, uint16_t w, uint16_t h, const uint8_t* image, bool reverseBitorder = true);
        ~imageTwoColorHandle();
        void changeImage(uint16_t w, uint16_t h, const uint8_t* image);
        void setForeground(int32_t color);
        void setBackground(int32_t color);
        int32_t getPixelAt(int16_t x, int16_t y);      
        box getCurrentSize();    
        void changeScale(uint8_t scale);
    };

    class image8BitHandle : public graphicsHandle
    {
        private:
        const uint8_t * _imgBuf = nullptr;

        public:
        image8BitHandle(int16_t x, int16_t y, uint16_t w, uint16_t h, const uint8_t* image);
        ~image8BitHandle();
        void changeImage(int16_t x, int16_t y, uint16_t w, uint16_t h, const uint8_t* image);
        int32_t getPixelAt(int16_t x, int16_t y);    
        box getCurrentSize();      
    };

    class lineHandle : public graphicsHandle
    {
        private:
        int16_t _x1, _y1, _x2, _y2;
        int32_t _color = COLORS::WHITE;
        uint8_t _thickness = 1;

        public:
        lineHandle(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int32_t color = COLORS::WHITE, uint8_t thickness = 1);
        ~lineHandle();
        void changeLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2);
        void setColor(int32_t color);
        void setThickness(uint8_t thickness);
        int32_t getPixelAt(int16_t x, int16_t y);
        box getCurrentSize();
        
        // Hardware acceleration support
        bool canUseHardwareAcceleration() override;
        STATUS::code renderToDisplay(displayInterface* display) override;
    };

    class rectangleHandle : public graphicsHandle
    {
        private:
        int32_t _fillColor = COLORS::WHITE;
        int32_t _borderColor = COLORS::TRANSPARENT;
        uint8_t _borderThickness = 0;

        public:
        rectangleHandle(int16_t x, int16_t y, uint16_t w, uint16_t h, int32_t fillColor = COLORS::WHITE, int32_t borderColor = COLORS::TRANSPARENT, uint8_t borderThickness = 0);
        ~rectangleHandle();
        void changeRectangle(int16_t x, int16_t y, uint16_t w, uint16_t h);
        void setFillColor(int32_t color);
        void setBorderColor(int32_t color);
        void setBorderThickness(uint8_t thickness);
        int32_t getPixelAt(int16_t x, int16_t y);
        box getCurrentSize();
        
        // Hardware acceleration support
        bool canUseHardwareAcceleration() override;
        STATUS::code renderToDisplay(displayInterface* display) override;
    };

    STATUS::code addDisplay(displayInterface* display);
    STATUS::code update();
    STATUS::code updateWithHardwareAcceleration();
    STATUS::code start();

    namespace OPTIMIZED_RENDERING
    {
        void drawHorizontalLine(uint8_t* buffer, int16_t x1, int16_t x2, int16_t y, 
                               int16_t bufferWidth, int32_t color, COLOR_MODE::colormode mode);
        void drawVerticalLine(uint8_t* buffer, int16_t x, int16_t y1, int16_t y2, 
                             int16_t bufferWidth, int32_t color, COLOR_MODE::colormode mode);
        void fillRectangle(uint8_t* buffer, int16_t x, int16_t y, uint16_t w, uint16_t h,
                          int16_t bufferWidth, int32_t color, COLOR_MODE::colormode mode);
    }

    namespace TOOLS
    {
        bool rectIntersect(const box *b1, const box *b2);
        uint32_t getRectArea(const box *bb);
        box getBoundingBox(const box *b1, const box *b2);
        box maskRectangle(const box * mask, const box * b);
    } // namespace TOOLS
    
} // namespace AGGL


#endif