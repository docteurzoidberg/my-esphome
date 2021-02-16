#include "esphome/core/log.h"
#include "zilloscope.h"

namespace esphome {
namespace zilloscope {

static const char *TAG = "zilloscope.component";
static state _state = state::booting;
static display::DisplayBuffer *_display=NULL;
static display::Font *_font=NULL;
static time::RealTimeClock *_time=NULL;

static Color _color_blue = Color(0x0000FF);
static Color _color_white = Color(0xFFFFFF);

static bool _has_time=false;


void ZilloScope::setup() {
}

void ZilloScope::loop() {
  if(!_has_time && _time->now().is_valid()) {
    _has_time=true;
    _state=state::time;
  }
}

void ZilloScope::dump_config(){
  ESP_LOGCONFIG(TAG, "Zilloscope component");
}

void ZilloScope::display_lambdacall(display::DisplayBuffer & it) {
  if(_state== state::booting) {
    display_boot();
    return;
  }
  else if(_state==state::time) {
    display_time();
  }
  else if(_state==state::ota) {
    display_ota();
  }
  else if(_state==state::shutdown) {
    display_shutdown();
  }
  else {
    it.print(0,0, _font, _color_blue, "?" );
  }
}

void ZilloScope::display_boot() {
  //_display->print(0,0,_font,_color_blue,"Boot");
}

void ZilloScope::display_time() {
  _display->strftime(3, -1, _font, _color_white, ((millis()/1000)%2==0)?"%H:%M":"%H:%M.", _time->now());
}

void ZilloScope::display_ota() {
  _display->print(0,0,_font,_color_blue,"ota");
}

void ZilloScope::display_shutdown() {
  _display->print(0,0,_font,_color_blue,"Shutdown");
}

void ZilloScope::on_boot() {
  ESP_LOGD(TAG, "boot");
}

void ZilloScope::on_ota() {
  ESP_LOGD(TAG, "ota");
  _state=state::ota;
}

void ZilloScope::on_shutdown() {
  ESP_LOGD(TAG, "shutdown");
  _state=state::shutdown;
}

state ZilloScope::get_state() {
  return _state;
}

void ZilloScope::set_state(state state) {
  _state=state;
}

void ZilloScope::set_display(display::DisplayBuffer *it) {
  _display=it;
}

void ZilloScope::set_font(display::Font *font) {
  _font=font;
}

void ZilloScope::set_time(time::RealTimeClock *time) {
  _time=time;
}


} // namespace zilloscope

}  // namespace esphome