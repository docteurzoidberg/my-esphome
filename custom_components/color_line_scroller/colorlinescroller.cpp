#include "colorlinescroller.h"

namespace esphome {
namespace colorlinescroller {

ColorLineScroller::ColorLineScroller() {
}

void ColorLineScroller::setup(uint16_t width, uint16_t height, uint16_t pixels_per_color) {
  _width=width;
  _height=height;
  _color_pixels = pixels_per_color;
  _buffer.resize(_width);
  for(uint8_t c=0;c<_palette.size();c++) {
    for(int i=0; i<_color_pixels;i++){
        if(_bufferindex+1>=width)
          return;
        _buffer[_bufferindex++] = _palette[c];
    }
  }
}

void ColorLineScroller::scroll() {
  _indexscroll = (_indexscroll + 1) % _width;
}

void ColorLineScroller::draw(display::DisplayBuffer & it,int xpos=0, int ypos=0) {
  for(int x=0;x<_width;x++){
    for(int y=0;y<_height;y++) {
      _bufferindex = (x + _indexscroll + _width) % _width;
      Color c = _buffer[_bufferindex];
      it.draw_pixel_at(xpos+x,ypos+y,c);
    }
  }
}

}
}