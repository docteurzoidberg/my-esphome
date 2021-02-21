#include "esphome/core/log.h"
#include "zilloscope.h"

namespace esphome {
namespace zilloscope {

//consts
static const char *TAG = "zilloscope.component";
static const Color _color_blue = Color(0x0000FF);
static const Color _color_white = Color(0xFFFFFF);

//state
static state _state = state::booting;
static bool _has_time=false;
static unsigned long _notify_start;
static std::string _notification_text;
static unsigned long _notification_timeout;

//references
static display::DisplayBuffer *_display=NULL;
static display::Font *_font=NULL;
static time::RealTimeClock *_time=NULL;

//component

void ZilloScope::setup() {
}

void ZilloScope::loop() {
  if(!_has_time && _time->now().is_valid()) {
    _has_time=true;
    _state=state::time;
  }

  if(_state==state::notify) {
    //notification timeout
    if(millis()-_notify_start>_notification_timeout) {
      _state=state::time;
    }
  }

}

void ZilloScope::dump_config(){
  ESP_LOGCONFIG(TAG, "Zilloscope component");
}

//display


uint32_t _frame_counter_boot=0;
uint32_t _frame_counter_time=0;
uint32_t _frame_counter_ota=0;
uint32_t _frame_counter_notify=0;
uint32_t _frame_counter_shutdown=0;


void ZilloScope::display_lambdacall(display::DisplayBuffer & it) {

  if(_state== state::booting) {
    if(render_boot_f_(it,_frame_counter_boot))
      _frame_counter_boot++;
    else
      _frame_counter_boot=0;
    return;
  }
  else if(_state==state::time) {
    if(render_time_f_(it,_frame_counter_time))
      _frame_counter_time++;
    else
      _frame_counter_time=0;
    return;
  }
  else if(_state==state::ota) {
    if(render_ota_f_(it,_frame_counter_ota))
      _frame_counter_ota++;
    else
      _frame_counter_ota=0;
    return;
  }
  else if(_state==state::notify) {
    if(render_notification_f_(it,_frame_counter_notify))
      _frame_counter_notify++;
    else
      _frame_counter_notify=0;
    return;
  }
  else if(_state==state::shutdown) {
    if(render_shutdown_f_(it,_frame_counter_shutdown))
      _frame_counter_shutdown++;
    else
      _frame_counter_shutdown=0;
    return;
  }
  else {
    it.print(0,0, _font, _color_blue, "?" );
  }
}

//services

void ZilloScope::service_notify(int type, std::string text, unsigned long timeout) {
  //todo: add notification to a queue
  _state=state::notify;
  _notify_start = millis();
  _notification_text = text;
  _notification_timeout = timeout;
}

//events

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

//getters / setters

state ZilloScope::get_state() {
  return _state;
}

std::string ZilloScope::get_notification_text() {
  return _notification_text;
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