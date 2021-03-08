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

//config
static bool _config_use_splash = false;

//references
static display::DisplayBuffer *_display=NULL;
static time::RealTimeClock *_time=NULL;

//display frame counters
static uint32_t _frame_counter_boot=0;
static uint32_t _frame_counter_splash=0;
static uint32_t _frame_counter_time=0;
static uint32_t _frame_counter_ota=0;
static uint32_t _frame_counter_notify=0;
static uint32_t _frame_counter_shutdown=0;

//notifications
static std::queue<Notification*> _queue;
static Notification* _current_notification;

//component

void ZilloScope::setup() {
}

void ZilloScope::loop() {

  //Waiting until we got a valid time to enter state::time after boot
  if(!_has_time && _time->now().is_valid()) {
    _has_time=true;
    on_splash();
    return;
  }

  //Start new notification if present in queue and in time mode
  if(_state==state::time && !_queue.empty()) {
    next_notification();
    _state=state::notify;
    return;
  }

  //notification finished ?
  if(_state==state::notify) {
    if(_current_notification->is_finished() || _current_notification->is_timed_out()) {
      if(_queue.empty()) {
        _state=state::time;
        return;
      }
      next_notification();
      return;
    }
  }
}

void ZilloScope::dump_config(){
  ESP_LOGCONFIG(TAG, "Zilloscope component");
}

void ZilloScope::next_notification() {
  _frame_counter_notify=0;
  _current_notification = _queue.front();
  _current_notification->start();//update started timer
  _queue.pop();
}

void ZilloScope::end_notification() {
   _current_notification->end();
   if(!_queue.empty()) {
    next_notification();
   }
}

//display

void ZilloScope::display_lambdacall(display::DisplayBuffer & it) {

  if(_state== state::booting) {
    if(render_boot_f_(it,_frame_counter_boot))
      _frame_counter_boot++;
    else
      _frame_counter_boot=0;
    return;
  }
  else if(_state==state::splash) {
    if(render_splash_f_(it,_frame_counter_splash))
      _frame_counter_splash++;
    else {
      _frame_counter_splash=0;
      if(_has_time)
        on_ready();
    }
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
    if(render_notification_f_(it,_frame_counter_notify, get_notification_text(), get_notification_type()))
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
}

//services

void ZilloScope::service_notify(int type, std::string text, unsigned long timeout=-1) {
  _queue.push(new Notification((uint32_t) type,text,timeout));
}

//events

void ZilloScope::on_boot() {
  ESP_LOGD(TAG, "boot");
  _state=state::booting; //already set at boot at startup
}

void ZilloScope::on_splash() {
  if(!_config_use_splash) {
    on_ready();
    return;
  }
  ESP_LOGD(TAG, "splash");
  _state=state::splash;
}

void ZilloScope::on_ready() {
  ESP_LOGD(TAG, "ready");
  _state=state::time;
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

uint32_t ZilloScope::get_notification_type() {
  return _current_notification->get_type();
}

std::string ZilloScope::get_notification_text() {
  return _current_notification->get_text();
}

void ZilloScope::set_state(state state) {
  _state=state;
}

void ZilloScope::set_display(display::DisplayBuffer *it) {
  _display=it;
}

void ZilloScope::set_time(time::RealTimeClock *time) {
  _time=time;
}

void ZilloScope::set_config_use_splash(bool value) {
  _config_use_splash=value;
}

} // namespace zilloscope

}  // namespace esphome