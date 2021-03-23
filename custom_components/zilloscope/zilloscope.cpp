#include "esphome/core/log.h"
#include "zilloscope.h"

namespace esphome {
namespace zilloscope {

//consts
static const char *TAG = "zilloscope.component";
static const Color _color_blue = Color(0x0000FF);
static const Color _color_white = Color(0xFFFFFF);

//state
static uint32_t _default_mode_index = 0;     //can be overidden in config

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

    Mode *mode = get_active_mode_();
    //no active mode?
    if(mode==nullptr)
      return;

    //special cases for modes who has special render methods
    if(mode->get_name()=="effects") {
      DisplayEffect *effect = get_active_effect_();
      if(effect==nullptr)
        return;
      effect->apply(*&*_display);
      return;
    }

    mode->draw(*&*_display);
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

void ZilloScope::service_mode(std::string name) {
  uint32_t mode_index = get_mode_index(name);
  if(mode_index==0) {
    ESP_LOGE(TAG, "Unknown mode %s", name.c_str());
    return;
  }
  ESP_LOGD(TAG, "Entering mode %s", name.c_str());
  start_mode_(mode_index);
}

void ZilloScope::service_effect_start(std::string name) {
  uint32_t effect_index = get_effect_index(name);
  if(effect_index==0) {
    ESP_LOGE(TAG, "Unknown effect %s", name.c_str());
    return;
  }
  ESP_LOGD(TAG, "Starting effect %s (#%d)", name.c_str(), effect_index);
  start_effect_(effect_index);
}

void ZilloScope::service_effect_stop() {
  stop_effect_();
}

uint32_t ZilloScope::get_effect_index(std::string name) {
  for (uint32_t i=0;i<effects_.size();i++) {
    if(effects_[i]->get_name()==name) {
      return i+1;
    }
  }
  return 0;
}

DisplayEffect *ZilloScope::get_active_effect_() {
 if (this->active_effect_index_ == 0)
    return nullptr;
  else
    return this->effects_[this->active_effect_index_ - 1];
}

//modes

uint32_t ZilloScope::get_mode_index(std::string name) {
  for (uint32_t i=0;i<modes_.size();i++) {
    if(modes_[i]->get_name()==name) {
      return i+1;
    }
  }
  return 0;
}

Mode *ZilloScope::get_active_mode_() {
 if (this->active_mode_index_ == 0)
    return nullptr;
  else
    return this->modes_[this->active_mode_index_ - 1];
}


void ZilloScope::start_mode_(uint32_t mode_index) {
  auto *curmode = this->get_active_mode_();
  if(curmode!=nullptr)
    curmode->stop();
  if (mode_index == 0)
    return;
  this->last_mode_index_=this->active_mode_index_;
  this->active_mode_index_ = mode_index;
  auto *mode = this->get_active_mode_();
  mode->start_internal();
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
  start_mode_(_default_mode_index);
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
  if (effect != nullptr)
    effect->stop();
  this->active_effect_index_ = 0;
}

void ZilloScope::add_effects(const std::vector<DisplayEffect *> effects) {
  this->effects_.reserve(this->effects_.size() + effects.size());
  for (auto *effect : effects) {
    this->effects_.push_back(effect);
  }
}

void ZilloScope::add_modes(const std::vector<Mode *> modes) {
  this->modes_.reserve(this->modes_.size() + modes.size());
  for (auto *mode : modes) {
    this->modes_.push_back(mode);
  }
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
  uint32_t mode_index = get_mode_index(value);
  if(mode_index==0) {
    ESP_LOGE(TAG, "Unknown mode %s in config, defaulting to first mode set", value.c_str());
    //todo choose wich one?
    _default_mode_index = 1;
  } else {
    _default_mode_index = mode_index;
  }
}

void ZilloScope::set_config_use_splash(bool value) {
  _config_use_splash=value;
}

} // namespace zilloscope

}  // namespace esphome