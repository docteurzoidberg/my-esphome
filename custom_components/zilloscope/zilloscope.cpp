#include "esphome/core/log.h"
#include "zilloscope.h"

namespace esphome {
namespace zilloscope {

//consts
static const char *TAG = "zilloscope.component";
static const Color _color_blue = Color(0x0000FF);
static const Color _color_white = Color(0xFFFFFF);

//state
static mode _default_mode = mode::time;   //can be overidden in config
static mode _lastmode=_default_mode;      //last mode history
static mode _mode =_default_mode;
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
  if(_state==state::main && !_queue.empty()) {
    next_notification();
    _state=state::notify;
    return;
  }

  //notification finished ?
  if(_state==state::notify) {
    if(_current_notification->is_finished() || _current_notification->is_timed_out()) {
      if(_queue.empty()) {
        _state=state::main;
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
  else if(_state==state::main) {
    //mode: time
    if(_mode==mode::time) {
      if(render_time_f_(it,_frame_counter_time))
        _frame_counter_time++;
      else
        _frame_counter_time=0;
      return;
    }
    else if(_mode==mode::effects) {
      //call current effect's apply() method
      DisplayEffect *effect = get_active_effect_();
      if(effect!=nullptr){
        effect->apply(*&*_display);
        return;
      }
    }
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

void ZilloScope::service_mode(std::string name) {
  mode newmode = get_mode_by_name(name);
  if(newmode==mode::unknown) {
    ESP_LOGE(TAG, "Unknown mode %s", name.c_str());
    return;
  }
  ESP_LOGD(TAG, "Entering mode %s", name.c_str());
  enter_mode(newmode);
}

void ZilloScope::service_effect_start(std::string name) {
  uint32_t effect_index = get_effect_index(name);
  if(effect_index==0)
    return;
  start_effect_(effect_index);
}

void ZilloScope::service_effect_stop() {
  stop_effect_();
}

uint32_t ZilloScope::get_effect_index(std::string name) {
  //todo
  return 1;
}

//modes

mode ZilloScope::get_mode_by_name(std::string name) {
  if(name=="time") {
    return mode::time;
  }
  else if(name=="meteo") {
    return mode::meteo;
  }
  else if(name=="effects") {
    return mode::effects;
  }
  else if(name=="paint") {
    return mode::paint;
  }
  else {
    //todo: log error
    return mode::unknown;
    ESP_LOGE(TAG, "Unknown mode %s", name.c_str());
  }
}

void ZilloScope::enter_mode(mode newmode) {
  _lastmode=_mode;
  _mode=newmode;
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
  _state=state::main;
  _mode=_default_mode;
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

mode ZilloScope::get_mode() {
  return _mode;
}

uint32_t ZilloScope::get_notification_type() {
  return _current_notification->get_type();
}

std::string ZilloScope::get_notification_text() {
  return _current_notification->get_text();
}

std::string ZilloScope::get_effect_name() {
  if (this->active_effect_index_ > 0)
    return this->effects_[this->active_effect_index_ - 1]->get_name();
  else
    return "None";
}

void ZilloScope::start_effect_(uint32_t effect_index) {
  this->stop_effect_();
  if (effect_index == 0)
    return;

  this->active_effect_index_ = effect_index;
  auto *effect = this->get_active_effect_();
  effect->start_internal();
}

void ZilloScope::stop_effect_() {
  auto *effect = this->get_active_effect_();
  if (effect != nullptr) {
    effect->stop();
  }
  this->active_effect_index_ = 0;
}

void ZilloScope::add_effects(const std::vector<DisplayEffect *> effects) {
  this->effects_.reserve(this->effects_.size() + effects.size());
  for (auto *effect : effects) {
    this->effects_.push_back(effect);
  }
}

DisplayEffect *ZilloScope::get_active_effect_() {
  if (this->active_effect_index_ == 0)
    return nullptr;
  else
    return this->effects_[this->active_effect_index_ - 1];
}

void ZilloScope::set_mode(mode mode) {
  _mode=mode;
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

void ZilloScope::set_config_default_mode(std::string value) {
  mode newmode = get_mode_by_name(value);
  if(newmode==mode::unknown) {
    ESP_LOGE(TAG, "Unknown mode %s in config, defaulting to time", value.c_str());
    _default_mode = mode::time;
  } else {
    _default_mode = newmode;
  }
}

void ZilloScope::set_config_use_splash(bool value) {
  _config_use_splash=value;
}

} // namespace zilloscope

}  // namespace esphome