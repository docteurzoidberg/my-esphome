#pragma once

#include "esphome/core/component.h"
#include "esphome/components/display/display.h"
#include "esphome/components/font/font.h"

namespace esphome {
namespace textscroller {

class TextScroller : public Component {
 public:

  TextScroller();
  void setup(display::Display & it, font::Font * font, std::string text, int times, int xoffset, int ypos);
  void draw(display::Display & it, font::Font * font, Color color);
  bool scroll();

  protected:
    int _indexscroll=0;
    int _times=2;   //negative = repeat
    int _screen_width=0;
    int _text_width = 0;
    int _text_height = 0;
    int _text_x1 = 0;
    int _text_y1 = 0;
    int _ypos=0;
    int _xoffset=1;
    std::string _text;
};

}
}