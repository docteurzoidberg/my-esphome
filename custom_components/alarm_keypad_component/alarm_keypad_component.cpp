#include "esphome/core/log.h"
#include "alarm_keypad_component.h"

namespace esphome {
namespace alarm_keypad_component {

static const char *TAG = "alarm_keypad_component.component";

static unsigned long _typing_timer=0;
static unsigned long _service_timer=0;

static std::string _typing_progress="";
static std::string _last_alarm_status="";
static uint8_t _state=STATE_BOOTING;
static bool _has_state=false;
static char _arm_key='?';

static ht16k33_alpha::HT16K33AlphaDisplay *_display=NULL;
static homeassistant::HomeassistantTextSensor *_alarmstatus=NULL;
static text_sensor::TextSensor *_keypadtext=NULL;

//void register_text_sensor(text_sensor::TextSensor *obj) { this->text_sensors_.push_back(obj); }

// component
void AlarmKeypadComponent::setup() {

}

void AlarmKeypadComponent::loop() {

  if(_state==STATE_BOOTING){
    if(!_has_state) {
      if(_alarmstatus==NULL)
        return;
      _has_state = _alarmstatus->has_state();
      //_has_state=false;
      if(_has_state) {
        _state = STATE_ALARM_STATUS_DISPLAY;
        this->on_ready_callback_.call();
      }
    }
  }
  //handle typing timeout
  else if(_state == STATE_TYPING) {
    //TODO: put timeout in settings
    if(millis()-_typing_timer > 5000) {
      typing_timeout();
      return;
    }
  }
  else if(_state == STATE_SERVICE_WAIT) {

    if(_alarmstatus!=NULL){
      if(_alarmstatus->has_state()) {
        if(_alarmstatus->state!=_last_alarm_status) {
          //Response from service
          _state = STATE_ALARM_STATUS_DISPLAY;
          return;
        }
      }
    }

    //TODO: put timeout in settings
    if(millis()-_service_timer > 5000) {
      service_timeout();
      return;
    }
  }
}

void AlarmKeypadComponent::dump_config(){
  ESP_LOGCONFIG(TAG, "Alarm keypad component");
}

void AlarmKeypadComponent::start_typing() {
  ESP_LOGD(TAG, "typing");
  _typing_timer=millis();
  _state = STATE_TYPING;
}

void AlarmKeypadComponent::typing_timeout() {
  ESP_LOGD(TAG, "typing timeout");
  _typing_timer=millis();
  _state = STATE_ALARM_STATUS_DISPLAY;
}

void AlarmKeypadComponent::start_service() {
  ESP_LOGD(TAG, "service");
  _service_timer=millis();
  _state = STATE_SERVICE_WAIT;
  if(_alarmstatus!=NULL){
      if(_alarmstatus->has_state()) {
        _last_alarm_status=_alarmstatus->state;
      }
  }
}

void AlarmKeypadComponent::service_timeout() {
  ESP_LOGD(TAG, "service timeout");
  _service_timer=millis();
  _state = STATE_ALARM_STATUS_DISPLAY;
}
// lambdas

void AlarmKeypadComponent::display_lambdacall(ht16k33_alpha::HT16K33AlphaDisplay & it, std::string text) {
  if(_state==STATE_BOOTING) {
    it.set_brightness(.1);
    it.print("BOOTING");
  }
  else if(_state==STATE_ALARM_STATUS_DISPLAY) {
    it.print(text.substr(0,8));
  }
  else if(_state==STATE_TYPING) {

    if(_keypadtext==NULL)
      return;

    //Works
    if(_typing_progress.length()==1){
      it.print("*___");
    }
    else if(_typing_progress.length()==2){
      it.print("**__");
    }
    else if(_typing_progress.length()==3){
      it.print("***_");
    }
    else if(_typing_progress.length()==4){
      it.print("****");
    }
    else {
      it.print("____");
    }

    //Doesn't works. because i'm dumb:
    //it.print(std::string('*',_typing_progress.length()).c_str());
  }
  else if(_state==STATE_SERVICE_WAIT){
    it.print("--------");
  }
  else if(_state==STATE_SHUTDOWN){
      //TODO: fancier
      it.print("OTA");
    }
}

void AlarmKeypadComponent::leds_keypad_lambdacall(light::AddressableLight & it) {
  if(_state==STATE_BOOTING) {
    it.all() = light::ESPColor::ESPColor::BLACK;
  }
  else {
    it.all() = light::ESPColor(255, 64, 0);
  }
}

void AlarmKeypadComponent::leds_case_lambdacall(light::AddressableLight & it) {
  if(_state==STATE_BOOTING) {
    it.all() = light::ESPColor::ESPColor::BLACK;
  }
  else {
    it.all() = light::ESPColor(255, 64, 0);
  }
}

void AlarmKeypadComponent::leds_rfid_lambdacall(light::AddressableLight & it) {
  if(_state==STATE_BOOTING) {
    it.all() = light::ESPColor::ESPColor::WHITE;
  }
  else {
    it.all() = light::ESPColor::ESPColor::BLACK;
  }
}

// events

void AlarmKeypadComponent::on_boot() {
  ESP_LOGD(TAG, "boot");

  //doesn't seems to work?
  _display->set_brightness(.2);
  _display->update();
}

void AlarmKeypadComponent::on_shutdown() {
  ESP_LOGD(TAG, "shutdown");
  _state=STATE_SHUTDOWN;
  _display->print("OTA");
  _display->update();
}

void AlarmKeypadComponent::on_keypad_progress(std::string x) {

  _typing_progress=x;

  ESP_LOGD(TAG,"keypad progress: %s", x.c_str());

  if(_state == STATE_ALARM_STATUS_DISPLAY) {
    if(x!="")
      start_typing();
  }
  else if(_state==STATE_TYPING){
    _typing_timer=millis();
  }
}

void AlarmKeypadComponent::on_keypad_value(std::string x) {
  ESP_LOGD(TAG,"keypad value: %s", x.c_str());
  if(_state==STATE_TYPING){
    _typing_progress=x;
    start_service();
    return;
  }
}

// getters / setters

char AlarmKeypadComponent::get_arm_key() {
  return _arm_key;
}

void AlarmKeypadComponent::set_arm_key(char key) {
  _arm_key=key;
}

uint8_t AlarmKeypadComponent::get_state() {
  return _state;
}

void AlarmKeypadComponent::set_display(ht16k33_alpha::HT16K33AlphaDisplay *it) {
  _display=it;
}

void AlarmKeypadComponent::set_alarmstatusentity(homeassistant::HomeassistantTextSensor *hatext) {
  _alarmstatus=hatext;
}

void AlarmKeypadComponent::set_keypadtext(text_sensor::TextSensor *kptext) {
  _keypadtext=kptext;
}

}

}  // namespace esphome