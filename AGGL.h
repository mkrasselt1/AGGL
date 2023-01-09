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
        displayInterface(uint16_t width, uint16_t height);
        virtual ~displayInterface(){}
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

        friend STATUS::code update();
        friend int32_t getPixelAt(int16_t x, int16_t y);
    };

    class textHandle : public graphicsHandle
    {
        private:
        const char * _text = nullptr;
        const uint8_t * _font = nullptr;

        public:
        textHandle(int16_t x, int16_t y, const char* text, const uint8_t * font);
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