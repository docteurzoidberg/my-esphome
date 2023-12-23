#pragma once

#include "esphome/core/component.h"
#include "esphome/components/display/display.h"

namespace esphome {
namespace colorlinescroller {

class ColorLineScroller : public Component {
 public:
  uint16_t _color_pixels=4;
  uint16_t _width=32;
  uint16_t _height=1;

  std::vector<Color> _palette = {
    Color(0xa271ff),
    Color(0xff61b2),
    Color(0x61d3e3),
    Color(0x71e392),
    Color(0xebd320),
    Color(0xff7930),
    Color(0xa23000)
  };

  ColorLineScroller();
  void setup(uint16_t width, uint16_t height, uint16_t pixels_per_color);
  void draw(display::Display &it,int xpos, int ypos);
  void scroll();

  protected:
    int _indexscroll=0;
    uint8_t _bufferindex=0;
    std::vector<Color> _buffer;
};

}
}