#include "textscroller.h"

namespace esphome {
namespace textscroller {

TextScroller::TextScroller() {
}

void TextScroller::setup(display::DisplayBuffer & it, display::Font * font, std::string text, int times, int xoffset=1, int ypos=0) {
  _times=times;
  _text=text;
  _screen_width=it.get_width();
  _indexscroll=_screen_width+xoffset;
  _ypos=ypos;
  _xoffset=xoffset;
  it.get_text_bounds(0,0,_text.c_str(), font, display::TextAlign::LEFT, &_text_x1,&_text_y1,&_text_width,&_text_height);
}

bool TextScroller::scroll() {
  _indexscroll--;
  if(_indexscroll< -_text_width) {
    _indexscroll=_screen_width+_xoffset;
    _times--;
    if(_times==0) {
      return true;
    }
  }
  return false;
}

void TextScroller::draw(display::DisplayBuffer & it, display::Font * font, Color color) {
  it.print(_indexscroll,_ypos,font,color,_text.c_str());
}

}
}