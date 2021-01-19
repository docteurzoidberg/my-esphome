#include "esphome/core/log.h"
#include "alarm_keypad_component.h"


namespace esphome {
namespace alarm_keypad_component {

static const char *TAG = "alarm_keypad_component.component";

static unsigned long _typing_timer=0;
static uint8_t _typing_len=0;
static std::string _typing_progress="";
static uint8_t _state=STATE_BOOTING;
static bool _has_state=false;

static ht16k33_alpha::HT16K33AlphaDisplay *_display1=NULL;
static ht16k33_alpha::HT16K33AlphaDisplay *_display2=NULL;
static homeassistant::HomeassistantTextSensor *_alarmstatus=NULL;
static text_sensor::TextSensor *_keypadtext=NULL;

//void register_text_sensor(text_sensor::TextSensor *obj) { this->text_sensors_.push_back(obj); }

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
      return;
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
}

void AlarmKeypadComponent::dump_config(){
  ESP_LOGCONFIG(TAG, "Alarm keypad component");
}

void AlarmKeypadComponent::display1_lambdacall(std::string text) {
      if(_display1==NULL)
        return;

      if(_state==STATE_BOOTING) {
        _display1->print("BOOT");
      }
      else if(_state==STATE_ALARM_STATUS_DISPLAY) {
        if(text.length()>4) {
          _display1->print(text.substr(0,4));
        }
        else {
          _display1->print(text);
        }
      }
      else if(_state==STATE_TYPING){
        if(_keypadtext==NULL)
          return;

        //Works
        if(_typing_progress.length()==1){
          _display1->print("*___");
        }
        else if(_typing_progress.length()==2){
          _display1->print("**__");
        }
        else if(_typing_progress.length()==3){
          _display1->print("***_");
        }
        else if(_typing_progress.length()==4){
          _display1->print("****");
        }
        else {
          _display1->print("____");
        }

        //Doesn't works. because i'm dumb:
        //_display1->print(std::string('*',_typing_len).c_str());
      }
}

void AlarmKeypadComponent::display2_lambdacall(std::string text) {
      if(_display2==NULL)
        return;

      if(_state==STATE_BOOTING) {
        _display2->print("ING");
      }
      else if(_state==STATE_ALARM_STATUS_DISPLAY) {
        if(text.length()>4) {
          _display2->print(text.substr(4,4));
        }
        else {
          _display2->print("");
        }
      }
      else if(_state==STATE_TYPING){
        _display2->print("");
      }
}

void AlarmKeypadComponent::network_connected() {
  ESP_LOGD(TAG, "network connected");
}

void AlarmKeypadComponent::network_disconnected() {
  ESP_LOGD(TAG, "network disconnected");
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

void AlarmKeypadComponent::keypad_progress(std::string x) {

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

void AlarmKeypadComponent::keypad_value(std::string x) {
  ESP_LOGD(TAG,"keypad value: %s", x.c_str());
  if(_state==STATE_TYPING){
    _typing_len=0;
    _typing_progress=x;
    _state=STATE_ALARM_STATUS_DISPLAY;
    return;
  }
}

uint8_t AlarmKeypadComponent::get_state() {
  return _state;
}

void AlarmKeypadComponent::set_display1(ht16k33_alpha::HT16K33AlphaDisplay *it) {
  _display1=it;
}

void AlarmKeypadComponent::set_display2(ht16k33_alpha::HT16K33AlphaDisplay *it) {
  _display2=it;
}

void AlarmKeypadComponent::set_alarmstatusentity(homeassistant::HomeassistantTextSensor *hatext) {
  _alarmstatus=hatext;
}

void AlarmKeypadComponent::set_keypadtext(text_sensor::TextSensor *kptext) {
  _keypadtext=kptext;
}

}

}  // namespace esphome