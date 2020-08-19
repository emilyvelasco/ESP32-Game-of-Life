#ifndef VIRTUAL_GFX_H
#define VIRTUAL_GFX_H

struct VirtualCoords {
  int16_t x;
  int16_t y;
};

class VirtualGFX : public Adafruit_GFX
{

  public:
    int16_t virtualResX;
    int16_t virtualResY;

    int16_t rows;
    int16_t cols;

    int16_t screenResX;
    int16_t screenResY;

    RGB64x32MatrixPanel_I2S_DMA *display;

    VirtualGFX(RGB64x32MatrixPanel_I2S_DMA &disp, int vRows, int vCols, int screenX, int screenY, int vResX, int vResY)
      : Adafruit_GFX(vResX, vResY)
    {
      this->display = &disp;
      rows = vRows;
      cols = vCols;
      screenResX = screenX;
      screenResY = screenY;

      virtualResX = vResX;
      virtualResY = vResY;
    }

    virtual void drawPixel(int16_t x, int16_t y, uint16_t color);
    void drawPixelRGB888(int16_t x, int16_t y, uint8_t r, uint8_t g, uint8_t b);
    virtual void fillScreen(uint16_t color);                        // overwrite adafruit implementation


    VirtualCoords getCoords(int16_t x, int16_t y);
}; // end Class header

inline VirtualCoords VirtualGFX::getCoords(int16_t x, int16_t y) {
  VirtualCoords coords;
  int16_t xOffset = (y / screenResY) * (cols * screenResX);
  coords.x = x + xOffset;
  coords.y = y % screenResY;
  return coords;
}

inline void VirtualGFX::drawPixel(int16_t x, int16_t y, uint16_t color)
{
  VirtualCoords coords = getCoords(x, y);
  this->display->drawPixel(coords.x, coords.y, color);
}

inline void VirtualGFX::fillScreen(uint16_t color)  // adafruit virtual void override
{
  this->display->fillScreen(color);
}

inline void VirtualGFX::drawPixelRGB888(int16_t x, int16_t y, uint8_t r, uint8_t g, uint8_t b)
{
  VirtualCoords coords = getCoords(x, y);
  this->display->drawPixelRGB888( coords.x, coords.y, r, g, b);
}
#endif
