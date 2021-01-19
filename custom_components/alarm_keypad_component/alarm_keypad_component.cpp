#include "esphome/core/log.h"
#include "alarm_keypad_component.h"
#include <string>

namespace esphome {
namespace alarm_keypad_component {

static const char *TAG = "alarm_keypad_component.component";

uint16_t _typing_timer=0;
uint8_t _state=STATE_BOOTING;
bool _has_state=false;

static ht16k33_alpha::HT16K33AlphaDisplay *_display1=NULL;
static ht16k33_alpha::HT16K33AlphaDisplay *_display2=NULL;
static homeassistant::HomeassistantTextSensor *_alarmstatus=NULL;

//void register_text_sensor(text_sensor::TextSensor *obj) { this->text_sensors_.push_back(obj); }

void AlarmKeypadComponent::setup() {

}

void AlarmKeypadComponent::loop() {

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

  //handle typing timeout
  if(_state == STATE_TYPING) {
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
      if(_display1==NULL) return;

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
      else {

      }
}

void AlarmKeypadComponent::display2_lambdacall(std::string text) {
      if(_display2==NULL) return;
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
      else {

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
}

void AlarmKeypadComponent::typing_timeout() {
  ESP_LOGD(TAG, "typing timeout");
  _typing_timer=0;
  _state = STATE_ALARM_STATUS_DISPLAY;
}

void AlarmKeypadComponent::keypress() {
  //TODO: print *
  if(_state == STATE_ALARM_STATUS_DISPLAY) {
    _state=STATE_TYPING;
    start_typing();
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

}

}  // namespace esphome