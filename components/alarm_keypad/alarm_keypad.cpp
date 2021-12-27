#include "esphome/core/log.h"
#include "esphome/core/color.h"
#include "alarm_keypad.h"

namespace esphome {
namespace alarm_keypad {

static const char *TAG = "alarm_keypad.component";

static unsigned long _typing_timer=0;
static unsigned long _service_timer=0;
static uint8_t _wait_counter=0;
static const char *_wait_chars = "-\\|/-";

static std::string _typing_progress="";
static std::string _last_alarm_status="";
static uint8_t _state=STATE_BOOTING;
static bool _has_state=false;
static char _arm_key='?';

static ht16k33_alpha::HT16K33AlphaDisplay *_display=NULL;
static homeassistant::HomeassistantTextSensor *_alarmstatus=NULL;
static text_sensor::TextSensor *_keypadtext=NULL;

// component
void AlarmKeypad::setup() {
  //fastled_base_fastledlightoutput->get_controller()->setDither(0);
}

void AlarmKeypad::loop() {

  if(_state==STATE_BOOTING){
    if(!_has_state) {
      _has_state = _alarmstatus->has_state();
      if(_has_state) {
        _state = STATE_ALARM_STATUS_DISPLAY;
        this->on_ready_callback_.call();
      }
      else {
        ESP_LOGD(TAG, "waiting alarm state");
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
          this->on_code_ok_callback_.call();
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

void AlarmKeypad::dump_config(){
  ESP_LOGCONFIG(TAG, "Alarm keypad component");
}

void AlarmKeypad::start_typing() {
  ESP_LOGD(TAG, "typing");
  _typing_timer=millis();
  _state = STATE_TYPING;
}

void AlarmKeypad::typing_timeout() {
  ESP_LOGD(TAG, "typing timeout");
  _typing_timer=millis();
  _state = STATE_ALARM_STATUS_DISPLAY;
}

void AlarmKeypad::start_service() {
  ESP_LOGD(TAG, "service");
  this->on_code_check_callback_.call();
  _service_timer=millis();
  _state = STATE_SERVICE_WAIT;
  _wait_counter=0;
  if(_alarmstatus!=NULL){
      if(_alarmstatus->has_state()) {
        _last_alarm_status=_alarmstatus->state;
      }
  }
}

void AlarmKeypad::service_timeout() {
  ESP_LOGD(TAG, "service timeout");
  this->on_code_ko_callback_.call();
  _service_timer=millis();
  _state = STATE_ALARM_STATUS_DISPLAY;
}
// lambdas

void AlarmKeypad::display_lambdacall(ht16k33_alpha::HT16K33AlphaDisplay & it, std::string text) {
  if(_state==STATE_BOOTING) {
    it.set_brightness(.1);
    std::string toprint ("  BOOT  ");
    toprint.front()=_wait_chars[_wait_counter];
    toprint.back()=_wait_chars[_wait_counter];
    it.print(toprint);
    _wait_counter++;
    if(_wait_counter>4)
      _wait_counter=0;
  }
  else if(_state==STATE_ALARM_STATUS_DISPLAY) {
    if(text=="ARMING") {
      if(((int)(millis()/1000)) % 2 == 0) {
        it.print("");
      } else {
        it.print(" ARMING ");
      }
    } else {
      it.print(text.substr(0,8));
    }
  }
  else if(_state==STATE_TYPING) {

    if(_keypadtext==NULL)
      return;

    //Works
    if(_typing_progress.length()==1){
      it.print("  *___  ");
    }
    else if(_typing_progress.length()==2){
      it.print("  **__  ");
    }
    else if(_typing_progress.length()==3){
      it.print("  ***_  ");
    }
    else if(_typing_progress.length()==4){
      it.print("  ****  ");
    }
    else {
      it.print("  ____  ");
    }

    //Doesn't works. because i'm dumb:
    //it.print(std::string('*',_typing_progress.length()).c_str());
  }
  else if(_state==STATE_SERVICE_WAIT){
    std::string toprint ("  WAIT  ");
    toprint.front()=_wait_chars[_wait_counter];
    toprint.back()=_wait_chars[_wait_counter];
    it.print(toprint);
    _wait_counter++;
    if(_wait_counter>4)
      _wait_counter=0;
  }
  else if(_state==STATE_SHUTDOWN){
      //TODO: fancier
      it.print("SHUTDOWN");
    }
}

void AlarmKeypad::leds_keypad_lambdacall(light::AddressableLight & it) {
  if(_state==STATE_BOOTING) {
    it.all() = Color(0, 0, 0);
  }
  else {
    it.all() = Color(255, 64, 0);
  }
}

void AlarmKeypad::leds_case_lambdacall(light::AddressableLight & it) {
  if(_state==STATE_BOOTING) {
    it.all() = Color(0, 0, 0);
  }
  else {
    it.all() = Color(255, 64, 0);
  }
}

void AlarmKeypad::leds_rfid_lambdacall(light::AddressableLight & it) {
  if(_state==STATE_BOOTING) {
    it.all() = Color(255, 255, 255);
  }
  else {
    it.all() = Color(0, 0, 0);
  }
}

// events

void AlarmKeypad::on_boot() {
  ESP_LOGD(TAG, "boot");
  _display->set_brightness(.2);
  _display->update();
}

void AlarmKeypad::on_shutdown() {
  ESP_LOGD(TAG, "shutdown");
  _state=STATE_SHUTDOWN;
  _display->print("OTA");
  _display->update();
}

void AlarmKeypad::on_keypad_progress(std::string x) {

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

void AlarmKeypad::on_keypad_value(std::string x) {
  ESP_LOGD(TAG,"keypad value: %s", x.c_str());
  if(_state==STATE_TYPING){
    _typing_progress=x;
    start_service();
    return;
  }
}

// getters / setters

char AlarmKeypad::get_arm_key() {
  return _arm_key;
}

void AlarmKeypad::set_arm_key(char key) {
  _arm_key=key;
}

uint8_t AlarmKeypad::get_state() {
  return _state;
}

void AlarmKeypad::set_display(ht16k33_alpha::HT16K33AlphaDisplay *it) {
  _display=it;
}

void AlarmKeypad::set_alarmstatusentity(homeassistant::HomeassistantTextSensor *hatext) {
  _alarmstatus=hatext;
}

void AlarmKeypad::set_keypadtext(text_sensor::TextSensor *kptext) {
  _keypadtext=kptext;
}

}

}  // namespace esphome