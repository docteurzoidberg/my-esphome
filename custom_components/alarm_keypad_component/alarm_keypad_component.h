#pragma once

#include "esphome/core/component.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/light/addressable_light.h"
#include "esphome/components/homeassistant/text_sensor/homeassistant_text_sensor.h"
#include "esphome/components/ht16k33_alpha/ht16k33_display.h"
#include <string>

//TODO: enum
#define STATE_BOOTING 0
#define STATE_ALARM_STATUS_DISPLAY 1
#define STATE_TYPING 2
#define STATE_ARMING_HOME 3
#define STATE_ARMING_AWAY 4
#define STATE_DISARMING 5
#define STATE_FAILED 6
#define STATE_DISCONNECTED 8
#define STATE_SHUTDOWN 9


namespace esphome {
  namespace alarm_keypad_component {
    class AlarmKeypadComponent : public Component {
    public:

      void setup() override;
      void loop() override;
      void dump_config() override;

      uint8_t get_state();
      char get_arm_key();
      void set_arm_key(char key);
      void set_display1(ht16k33_alpha::HT16K33AlphaDisplay * it);
      void set_display2(ht16k33_alpha::HT16K33AlphaDisplay * it);
      void set_alarmstatusentity(homeassistant::HomeassistantTextSensor *hatext);
      void set_keypadtext(text_sensor::TextSensor *kptext);
      void set_leds_keypad(light::AddressableLight *it);

      //ht16k33 displays
      void display1_lambdacall(ht16k33_alpha::HT16K33AlphaDisplay & it, std::string text);
      void display2_lambdacall(ht16k33_alpha::HT16K33AlphaDisplay & it, std::string text);

      //leds
      void leds_keypad_lambdacall(light::AddressableLight & it);
      void leds_case_lambdacall(light::AddressableLight & it);
      void leds_rfid_lambdacall(light::AddressableLight & it);

      void start_typing();
      void typing_timeout();

      //config triggers
      void on_keypad_progress(std::string x);
      void on_keypad_value(std::string x);
      void on_boot();
      void on_shutdown();
    };


  }  // namespace alarm_keypad_component
}  // namespace esphome