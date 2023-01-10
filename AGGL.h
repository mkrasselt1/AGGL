#ifndef AGGL_H
#define AGGL_H
#include <Arduino.h>

namespace AGGL
{
    namespace STATUS
    {
        enum code{
            OK,
            GENERAL_ERROR,
            OUT_OF_MEMORY,
            FONT_NOT_SUPPORTED,
            COMM_ERROR
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
            RED =       0x000000FF,
            GREEN =     0x0000FF00,
            BLUE =      0x00FF0000,
        }; 
        int32_t fromRGB(uint8_t R, uint8_t G, uint8_t B);
    } // namespace COLORS

    typedef struct{
        int16_t x;
        int16_t y;
        uint16_t w;
        uint16_t h;
    } box;
    

    class displayInterface
    {
        protected:
        box _screen = {0};
        public:
        displayInterface(int16_t xOffset, int16_t yOffset, uint16_t width, uint16_t height);
        virtual ~displayInterface(){}
        virtual bool isPresent(){return false;}
        virtual STATUS::code init(){return STATUS::GENERAL_ERROR;}
        virtual STATUS::code update(box bb, uint8_t* buffer){return STATUS::GENERAL_ERROR;}
        box* getSize();
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
        int32_t _foreground;
        int32_t _background;

        public:
        graphicsHandle();
        virtual ~graphicsHandle(){removeHandle();}
        virtual int32_t getPixelAt(int16_t x, int16_t y){return 0;}
        void show();
        void hide();
        void changePosition(int16_t x, int16_t y);


        friend STATUS::code update();
        friend int32_t getPixelAt(int16_t x, int16_t y);
    };

    class textHandle : public graphicsHandle
    {
        private:
        const char * _text = nullptr;
        const uint8_t * _font = nullptr;
        const uint8_t * _glyph = nullptr;
        const uint8_t * _glyphBitmap = nullptr;
        uint8_t _glyphBitmapBitOffset = 0;
        bool selectGlyph(uint16_t code);
        int32_t getGlyphPixel(uint8_t x, uint8_t y);
        box getTextSize();
        int16_t readBitString(const uint8_t * buf, uint16_t offset, uint16_t len);
        uint16_t readBitStringU(const uint8_t * buf, uint16_t offset, uint16_t len);
        uint16_t countOnes(const uint8_t * buf, uint16_t offset);

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
        int32_t getPixelAt(int16_t x, int16_t y);      
    };

    class imageTwoColorHandle : public graphicsHandle
    {
        private:
        const uint8_t * _imgBuf = nullptr;

        public:
        imageTwoColorHandle(int16_t x, int16_t y, uint16_t w, uint16_t h, const uint8_t* image);
        void changeImage(int16_t x, int16_t y, uint16_t w, uint16_t h, const uint8_t* image);
        int32_t getPixelAt(int16_t x, int16_t y);      
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
    };

    STATUS::code addDisplay(displayInterface* display);
    STATUS::code update();
    STATUS::code setColorMode(COLOR_MODE::colormode mode);

    namespace TOOLS
    {
        bool rectIntersect(const box *b1, const box *b2);
    } // namespace TOOLS
    
} // namespace AGGL


#endif