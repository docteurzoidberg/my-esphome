#include "esphome/core/log.h"
#include "zilloscope.h"
#include "mode_paint.h"
#include "mode_effects.h"
namespace esphome {
namespace zilloscope {

//consts
static const char *TAG = "zilloscope.component";
static const Color _color_blue = Color(0x0000FF);
static const Color _color_white = Color(0xFFFFFF);

//state
static uint32_t _default_mode_index = 0;     //can be overidden in config
static uint32_t _default_effect_index = 0;     //can be overidden in config

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

    //ESP_LOGD(TAG,"Before draw for %s", mode->get_name().c_str());
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

void ZilloScope::service_mode_next() {
  active_mode_index_++;
  if(active_mode_index_>modes_.size()) {
    active_mode_index_=1;
  }
  auto mode = modes_[active_mode_index_-1];
  ESP_LOGD(TAG, "Entering mode %s (#%d)", mode->get_name().c_str(), active_mode_index_);
  start_mode_(active_mode_index_);
}

void ZilloScope::service_mode_prev() {
  active_mode_index_--;
  if(active_mode_index_<=0) {
    active_mode_index_=modes_.size();
  }
  auto mode = modes_[active_mode_index_-1];
  ESP_LOGD(TAG, "Entering mode %s (#%d)", mode->get_name().c_str(), active_mode_index_);
  start_mode_(active_mode_index_);
}

void ZilloScope::service_effect_start(std::string name) {

  ModeEffects *modeeffectfound = nullptr;
  uint32_t effectindexfound = 0;

  //get ModeEffects containing the named effect
  for (uint32_t i=0;i<modes_.size();i++) {
    //test if mode is ModeEffect
    auto mode = modes_[i];
    auto modetype = modes_[i]->get_type();
    if(modetype!="ModeEffects")
      continue;
    ModeEffects *modeeffect = static_cast <ModeEffects*>(modes_[i]);
    if(modeeffect!=nullptr){
      uint32_t effectindex = modeeffect->get_effect_index(name);
      if(effectindex>0) {
        modeeffectfound=modeeffect;
        effectindexfound = effectindex;
        break;
      }
    }
  }
  if(modeeffectfound==nullptr) {
    ESP_LOGE(TAG, "Cannot get ModeEffects with this effect name %s", name.c_str());
    return;
  }
  if(effectindexfound==0) {
    ESP_LOGE(TAG, "Unknown effect %s", name.c_str());
    return;
  }
  ESP_LOGD(TAG, "Starting effect %s (#%d)", name.c_str(), effectindexfound);
  modeeffectfound->start_effect_(effectindexfound);
}

void ZilloScope::service_effect_stop() {
  for (uint32_t i=0;i<modes_.size();i++) {
    //test if mode is ModeEffect
    auto mode = modes_[i];
    auto modetype = modes_[i]->get_type();
    if(modetype!="ModeEffects")
      continue;
    ModeEffects *modeeffects= static_cast <ModeEffects*>(modes_[i]);
    if(modeeffects==nullptr) {
      ESP_LOGE(TAG, "Cannot get ModeEffects");
      return;
    }
    modeeffects->stop_effect_();
  }
}

void ZilloScope::service_effect_next() {
  auto mode = get_active_mode_();
  if(mode->get_type()=="ModeEffects") {
    ModeEffects *modeeffect = static_cast <ModeEffects*>(mode);
    modeeffect->next_effect();
  }
}

void ZilloScope::service_effect_prev() {
  auto mode = get_active_mode_();
  if(mode->get_type()=="ModeEffects") {
    ModeEffects *modeeffect = static_cast <ModeEffects*>(mode);
    modeeffect->prev_effect();
  }
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
  Mode *mode = modes_[active_mode_index_-1];
  if(mode==nullptr){
    ESP_LOGE(TAG, "Error retrieving mode index %d",active_mode_index_-1);
    return;
  }
  ESP_LOGD(TAG, "starting mode %s", mode->get_name().c_str());
  ESP_LOGD(TAG, "before start internal");
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
  ESP_LOGD(TAG, "starting mode index %d", _default_mode_index);
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