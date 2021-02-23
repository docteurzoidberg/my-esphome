#pragma once

#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/components/display/display_buffer.h"

#include <string>
#include <queue>

namespace esphome {
  namespace zilloscope {

    using display_writer_t = std::function<bool(display::DisplayBuffer &, uint32_t)>;

    //states of the zilloscope
    enum state
    {
      booting,
      time,
      notify,
      ota,
      shutdown
    };

    class ColorLine {
    public:
      uint16_t _color_pixels=5;
      uint16_t _width=35;
      uint16_t _height=2;

      std::vector<Color> _palette = {
        Color(0xa271ff),
        Color(0xff61b2),
        Color(0x61d3e3),
        Color(0x71e392),
        Color(0xebd320),
        Color(0xff7930),
        Color(0xa23000)
      };
      std::vector<Color> _buffer;

      int _indexscroll=0;
      uint8_t _bufferindex=0;

      ColorLine(uint16_t width, uint16_t height, uint16_t pixels_per_color) {
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

      void render(display::DisplayBuffer * it,int xpos=0, int ypos=0) {
        for(int x=0;x<_width;x++){
          for(int y=0;y<_height;y++) {
            _bufferindex = (x + _indexscroll + _width) % _width;
            Color c = _buffer[_bufferindex];
            it->draw_pixel_at(xpos+x,ypos+y,c);
          }
        }
        _indexscroll = (_indexscroll + 1) % _width;
      }
    };

    class TextScroller {
    public:

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

      TextScroller(display::DisplayBuffer * it, display::Font * font, std::string text, int times, int xoffset=1, int ypos=0) {
        _times=times;
        _text=text;
        _screen_width=it->get_width();
        _indexscroll=_screen_width+xoffset;
        _ypos=ypos;
        _xoffset=xoffset;
        it->get_text_bounds(0,0,_text.c_str(), font, display::TextAlign::LEFT, &_text_x1,&_text_y1,&_text_width,&_text_height);
      }
      bool scroll() {
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
      void render(display::DisplayBuffer * it, display::Font * font, Color color) {
        //TODO
        it->print(_indexscroll,_ypos,font,color,_text.c_str());
      }
    };

    enum NotificationType
    {
      INFO,
      WARNING,
      ALERT,
      OTHER
    };

    class Notification {
    public:
      uint8_t _type;
      std::string _text;
      unsigned long _timeout;

      Notification(uint8_t type, std::string text, unsigned long timeout) {
        _type = type;
        _text = text;
        _timeout = timeout;
      }
      bool is_started() {
        return _started>0;
      }
      bool is_finished() {
        return _finished;
      }
      bool is_timed_out() {
        if(_started==0 || _timeout==0)
          return false;
        return (millis()-_started) > _timeout;
      }

      uint8_t get_type() {
        return _type;
      }
      std::string get_text() {
        return _text;
      }
      unsigned long get_timeout() {
        return _timeout;
      }
      unsigned long get_started() {
        return _started;
      }
      void start() {
        _started=millis();
      }
      void end() {
        _finished=true;
      }

    private:
      bool _finished=false;
      unsigned long _started=0;   //ts when set to current
      uint32_t _framecounter=0;   //ellapsed frames
    };

    class ZilloScope : public Component {
    public:

    //component
      void setup() override;
      void loop() override;
      void dump_config() override;
    //getter/setters
      state get_state();
      std::string get_notification_text();
      void set_state(state state);
      void set_display(display::DisplayBuffer * it);
      void set_font(display::Font * font);
      void set_time(time::RealTimeClock * time);
      void set_render_boot(display_writer_t  &&render_boot_f) { this->render_boot_f_ = render_boot_f; }
      void set_render_time(display_writer_t  &&render_time_f) { this->render_time_f_ = render_time_f; }
      void set_render_notification(display_writer_t  &&render_notification_f) { this->render_notification_f_ = render_notification_f; }
      void set_render_ota(display_writer_t  &&render_ota_f) { this->render_ota_f_ = render_ota_f; }
      void set_render_shutdown(display_writer_t &&render_shutdown_f) { this->render_shutdown_f_ = render_shutdown_f; }
      void add_on_ready_callback(std::function<void()> callback) {this->on_ready_callback_.add(std::move(callback));}

      void next_notification();
      void end_notification();

    //display
      void display_lambdacall(display::DisplayBuffer & it);

      void color_line_setup(int width, int height, int ppc);
      void color_line_draw(int xpos, int ypos);

      void text_scroller_setup(display::Font * font, std::string text, int times, int xoffset, int ypos);
      bool text_scroller_scroll();
      void text_scroller_draw(display::Font * font, Color color);

    //events
      void on_boot();
      void on_ota();
      void on_shutdown();

    //services
      void service_notify(int type, std::string text, unsigned long timeout);

    protected:

      ColorLine *_cl;
      TextScroller *_ts;

      //triggers
      CallbackManager<void()> on_ready_callback_;

      //display lambdas
      display_writer_t render_boot_f_;
      display_writer_t render_time_f_;
      display_writer_t render_notification_f_;
      display_writer_t render_ota_f_;
      display_writer_t render_shutdown_f_;
    };

    class ReadyTrigger : public Trigger<> {
    public:
      explicit ReadyTrigger(ZilloScope *parent) {
        parent->add_on_ready_callback([this]() { this->trigger(); });
      }
    };
  }  // namespace alarm_keypad_component
}  // namespace esphome