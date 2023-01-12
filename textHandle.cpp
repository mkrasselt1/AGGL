#include "AGGL.h"

using namespace AGGL;

bool textHandle::selectGlyph(uint16_t code)
{
    _glyph = &(_font[23]);

    if((code >= 'A') && (code <= 'Z'))
    {
        _glyph = &(_font[23 + BDFHeader.offset_A]);
    }

    if((code >= 'a') && (code <= 'z'))
    {
        _glyph = &(_font[23 + BDFHeader.offset_a]);
    }
    
    uint16_t charCode = 0;
    uint8_t nextOffset = 0;
    const uint8_t * glyphStart = _glyph;
    // Serial.printf("Searching for %02X\r\n", code);
    while(true)
    {
        //step 1: check if 1 or 2 byte character:
        charCode = *_glyph;
        _glyph++;
        if(charCode & 0x80){
            charCode = charCode << 8;
            charCode |= *_glyph;
            _glyph++;
        }
        // Serial.printf("Current Glyph is: %04X @ font[%ld + 23]\r\n", charCode, (uint32_t)(glyphStart - _font - 23));
        //step 2: get character size:
        nextOffset = *_glyph;
        _glyph++;
        
        if((charCode == (uint16_t)code) || (!nextOffset))
        {
            break;
        }

        glyphStart += nextOffset;
        _glyph = glyphStart;
    }
    
    if(charCode != code)
    {
        Serial.printf("Glyph %04X not found\r\n", code);
        return false;
    }

    //step 3 collect glyph data and set _glyphbitmap to datastart
    uint16_t bitOffset = 0;
    _glyphData.bb.w = readBitStringU(_glyph, bitOffset, BDFHeader.bitCntW);
    bitOffset += BDFHeader.bitCntW;
    _glyphData.bb.h = readBitStringU(_glyph, bitOffset, BDFHeader.bitCntH);
    bitOffset += BDFHeader.bitCntH;
    _glyphData.bb.x = readBitString(_glyph, bitOffset, BDFHeader.bitCntX);
    bitOffset += BDFHeader.bitCntX;
    _glyphData.bb.y = readBitString(_glyph, bitOffset, BDFHeader.bitCntY);
    bitOffset += BDFHeader.bitCntY;
    _glyphData.pitch = readBitStringU(_glyph, bitOffset, BDFHeader.bitCntD);
    bitOffset += BDFHeader.bitCntD;
    _glyphBitmap = &_glyph[bitOffset/8];
    _glyphBitmapBitOffset = bitOffset % 8;

    // Serial.printf("Glyph width: %d\r\n", _glyphData.bb.w);
    // Serial.printf("Glyph height: %d\r\n", _glyphData.bb.h);
    // Serial.printf("Glyph xOff: %d\r\n", _glyphData.bb.x);
    // Serial.printf("Glyph yOff: %d\r\n", _glyphData.bb.y);
    // Serial.printf("Glyph pitch: %d\r\n", _glyphData.pitch);
    return true;
}

int32_t textHandle::getGlyphPixel(uint8_t x, uint8_t y)
{
    uint8_t cx = 0;
    uint8_t cy = 0;

    uint16_t gOffset = _glyphBitmapBitOffset;

    uint16_t nZeros = 0;
    uint16_t nOnes = 0;
    uint16_t nRepetition = 0;
    uint16_t end = _glyphData.bb.w * _glyphData.bb.h; //number of pixel in glyph
    uint16_t dx = 0;
    uint16_t dy = 0;
    
    while (true)
    {
        //run through glyph until pixel is reached
        nZeros = readBitStringU(_glyphBitmap, gOffset, BDFHeader.zeroBitRLE);
        gOffset += BDFHeader.zeroBitRLE;
        nOnes = readBitStringU(_glyphBitmap, gOffset, BDFHeader.oneBitRLE);
        gOffset += BDFHeader.oneBitRLE;
        nRepetition = countOnes(_glyphBitmap, gOffset) + 1;
        gOffset += nRepetition;

        for (size_t i = 0; i < nRepetition; i++)
        {
            for (size_t j = 0; j < nZeros; j++)
            {            
                if(dx == x && dy == y){
                    return _background;
                }
                dx++;    
                if(dx == _glyphData.bb.w){
                    dx = 0;
                    dy++;
                }
                                    
            }
            for (size_t j = 0; j < nOnes; j++)
            {          
                if(dx == x && dy == y){
                    return _foreground;
                }   
                dx++;      
                if(dx == _glyphData.bb.w){
                    dx = 0;
                    dy++;
                }
                
            }
        }
        if(dy == _glyphData.bb.h)
            break;

    }
    return COLORS::TRANSPARENT;
}

box textHandle::getTextSize()
{
    const char * ctext = _text;
    box tSize;
    tSize.x = _newArea.x;
    tSize.y = _newArea.y;
    tSize.w = 0;
    tSize.h = 0;
    _numTextLines = 1;

    tSize.h += BDFHeader.bbHeight;

    uint16_t lineW = 0;

    while (*ctext)
    {            
        if(*ctext == '\n'){
            lineW = 0;
            tSize.h += BDFHeader.bbHeight;
            _numTextLines++;
        }else{
            if(isPrintable(*ctext)){
                selectGlyph(*ctext);
                if(BDFHeader.boundingBoxMode >= 2){
                    //Monospace
                    lineW += BDFHeader.bbWidth;
                }else{
                    lineW += _glyphData.pitch;
                }
                
                if(lineW > tSize.w){
                    tSize.w = lineW;
                }
            }
        }
        ctext++;            
    }
    
    return tSize;
}

int16_t textHandle::readBitString(const uint8_t *buf, uint16_t offset, uint16_t len)
{
    int16_t erg = readBitStringU(buf, offset,len);
    if(len < 16){
        if(erg & (1 << (len - 1)))
        {
            erg = -erg;
        }
    }
    
    return erg;
}

uint16_t textHandle::readBitStringU(const uint8_t *buf, uint16_t offset, uint16_t len)
{
    if(len > 16)
        return 0;

    uint16_t byteOffset = offset / 8;
    uint8_t remainingBits = offset % 8;
    uint16_t erg = 0;

    for (size_t i = 0; i < len; i++)
    {
        // Serial.println(((buf[byteOffset] >> (remainingBits)) & 0x01));
        erg |= ((buf[byteOffset] >> (remainingBits)) & 0x01) << (i);
        remainingBits++;
        if(remainingBits == 8){
            remainingBits = 0;
            byteOffset++;
        }
    }
    // Serial.println();
    return erg;
}

uint16_t textHandle::countOnes(const uint8_t *buf, uint16_t offset)
{
    uint16_t byteOffset = offset / 8;
    uint8_t remainingBits = offset % 8;
    uint16_t erg = 0;

    while (true)
    {
        if((buf[byteOffset] >> (remainingBits)) & 0x01)
        {
            erg++;
        }else{
            break;
        }

        remainingBits++;
        if(remainingBits == 8){
            remainingBits = 0;
            byteOffset++;
        }
    }
    return erg;
}

textHandle::textHandle(int16_t x, int16_t y, const char *text, const uint8_t *font)
{
    _newArea.x = x;
    _newArea.y = y;
    _oldArea = _newArea;
    _oldArea.w = 0;
    _oldArea.h = 0;

    _text = text;   
    changeFont(font);     
}

#define EC16(w) ((w>>8)|((w<<8)&0xFF00))
void textHandle::changeFont(const uint8_t *font)
{
    _font = font;
    memcpy(&BDFHeader, _font, sizeof(BDFHeader));
    BDFHeader.offset_A = EC16(BDFHeader.offset_A);
    BDFHeader.offset_a = EC16(BDFHeader.offset_a);
    BDFHeader.offset_0x0100 = EC16(BDFHeader.offset_0x0100);

    // Serial.printf("numberOfGlyphs: %d\r\n", BDFHeader.numberOfGlyphs);
    // Serial.printf("boundingBoxMode: %d\r\n", BDFHeader.boundingBoxMode);
    // Serial.printf("zeroBitRLE: %d\r\n", BDFHeader.zeroBitRLE);
    // Serial.printf("oneBitRLE: %d\r\n", BDFHeader.oneBitRLE);
    // Serial.printf("bitCntW: %d\r\n", BDFHeader.bitCntW);
    // Serial.printf("bitCntH: %d\r\n", BDFHeader.bitCntH);
    // Serial.printf("bitCntX: %d\r\n", BDFHeader.bitCntX);
    // Serial.printf("bitCntY: %d\r\n", BDFHeader.bitCntY);
    // Serial.printf("bitCntD: %d\r\n", BDFHeader.bitCntD);
    // Serial.printf("bbWidth: %d\r\n", BDFHeader.bbWidth);
    // Serial.printf("bbHeight: %d\r\n", BDFHeader.bbHeight);
    // Serial.printf("bbX: %d\r\n", BDFHeader.bbX);
    // Serial.printf("bbY: %d\r\n", BDFHeader.bbY);
    // Serial.printf("asc_A: %d\r\n", BDFHeader.asc_A);
    // Serial.printf("des_g: %d\r\n", BDFHeader.des_g);
    // Serial.printf("asc_OpenBracket: %d\r\n", BDFHeader.asc_OpenBracket);
    // Serial.printf("des_CloseBracket: %d\r\n", BDFHeader.des_CloseBracket);
    // Serial.printf("offset_A: %d\r\n", BDFHeader.offset_A);
    // Serial.printf("offset_a: %d\r\n", BDFHeader.offset_a);
    // Serial.printf("offset_0x0100: %d\r\n", BDFHeader.offset_0x0100);
    _newArea = getTextSize();
    _needUpdate = true;
}

void textHandle::changeText(const char *text)
{
    _text = text;
    _newArea = getTextSize();
    _needUpdate = true;
}

void textHandle::setForeground(int32_t color)
{
    _foreground = color;
}

void textHandle::setBackground(int32_t color)
{
    _background = color;
}

int32_t textHandle::getPixelAt(int16_t x, int16_t y)
{
    // Serial.printf("GetPix (%d,%d) in (%d,%d) %d x %d\r\n", x, y, _newArea.x, _newArea.y, _newArea.w, _newArea.h);
    if( (x >= _newArea.x) && (x < _newArea.x + _newArea.w) && 
        (y >= _newArea.y) && (y < _newArea.y + _newArea.h)
    )
    {
        
        int16_t lx = x - _newArea.x;
        int16_t ly = y - _newArea.y;

        const char * ctext = _text;
        uint16_t lineX = 0;
        uint16_t lineY = 0;


        if(BDFHeader.boundingBoxMode >= 2){
            //monospace Font
            while (*ctext)
            {            
                if(*ctext == '\n'){
                    lineX = 0;
                    lineY += BDFHeader.bbHeight;
                    _numTextLines++;
                }else{
                    if(isPrintable(*ctext)){
                        if( (lineX + BDFHeader.bbWidth > lx) &&
                            (lineY + BDFHeader.bbHeight > ly) 
                        )
                        {
                            //we got a hit
                            if(selectGlyph(*ctext)){
                                return getGlyphPixel(lx - lineX, ly - lineY);
                            }else{
                                return COLORS::TRANSPARENT;
                            }
                                                            
                        }
                        
                        lineX += BDFHeader.bbWidth;
                    }
                }
                ctext++;            
            }
        }else{
            //need to work from start of text to end
            while (*ctext)
            {            
                if(*ctext == '\n'){
                    lineX = 0;
                    lineY += BDFHeader.bbHeight;
                    _numTextLines++;
                }else{
                    if(isPrintable(*ctext)){
                        selectGlyph(*ctext);
                        if( (lineX + _glyphData.pitch > lx) &&
                            (lineY + BDFHeader.bbHeight > ly) 
                        )
                        {
                            //we got a hit
                            return getGlyphPixel(lx - lineX, ly - lineY);
                        }
                        
                        lineX += _glyphData.pitch;
                    }
                }
                ctext++;            
            }
        }

    }
    return COLORS::TRANSPARENT;
}

box textHandle::getCurrentSize()
{
    return getTextSize();
}