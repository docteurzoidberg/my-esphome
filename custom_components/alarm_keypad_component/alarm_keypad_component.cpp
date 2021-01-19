#include "esphome/core/log.h"
#include "alarm_keypad_component.h"
#include <string>

namespace esphome {
namespace alarm_keypad_component {

static const char *TAG = "alarm_keypad_component.component";

uint16_t _typing_timer=0;
uint8_t _state=STATE_BOOTING;

//void register_text_sensor(text_sensor::TextSensor *obj) { this->text_sensors_.push_back(obj); }

void AlarmKeypadComponent::setup() {
  _state = STATE_ALARM_STATUS_DISPLAY;
}

void AlarmKeypadComponent::loop() {
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

void display1_lambdacall(ht16k33_alpha::HT16K33AlphaDisplay & it) {

      if(_state==STATE_BOOTING) {
        it.print("BOOT");
      }
      else if(_state==STATE_ALARM_STATUS_DISPLAY) {
        //if(text.length()>4) {
          //it.print(text.substr(0,4));
        //}
        //else {
        //  it.print(text);
        //}
      }
      else {

      }
}

void display2_lambdacall(ht16k33_alpha::HT16K33AlphaDisplay & it) {

      if(_state==STATE_BOOTING) {
        it.print("ING");
      }
      else if(_state==STATE_ALARM_STATUS_DISPLAY) {
        //if(text.length()>4) {
          //it.print(text.substr(4,4));
        //}
        //else {
        //  it.print("");
        //}
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


}

}  // namespace esphome