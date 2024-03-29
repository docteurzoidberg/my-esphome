#pragma once

#include "esphome/core/automation.h"
#include "esphome/core/component.h"
#include "esphome/core/color.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/light/addressable_light.h"
#include "esphome/components/homeassistant/text_sensor/homeassistant_text_sensor.h"
#include "esphome/components/ht16k33_alpha/ht16k33_display.h"
#include <string>

//TODO: enum
#define STATE_BOOTING 0
#define STATE_ALARM_STATUS_DISPLAY 1
#define STATE_TYPING 2
#define STATE_SERVICE_WAIT 3
#define STATE_SHUTDOWN 9


namespace esphome {
  namespace alarm_keypad {
    class AlarmKeypad : public Component {
    public:

      void setup() override;
      void loop() override;
      void dump_config() override;

      void add_on_ready_callback(std::function<void()> callback) {
        this->on_ready_callback_.add(std::move(callback));
      }
      void add_on_code_check_callback(std::function<void()> callback) {
        this->on_code_check_callback_.add(std::move(callback));
      }
      void add_on_code_ok_callback(std::function<void()> callback) {
        this->on_code_ok_callback_.add(std::move(callback));
      }
      void add_on_code_ko_callback(std::function<void()> callback) {
        this->on_code_ko_callback_.add(std::move(callback));
      }

      uint8_t get_state();
      char get_arm_key();
      void set_arm_key(char key);

      void set_display(ht16k33_alpha::HT16K33AlphaDisplay * it);
      void set_alarmstatusentity(homeassistant::HomeassistantTextSensor *hatext);
      void set_keypadtext(text_sensor::TextSensor *kptext);

      //ht16k33 display
      void display_lambdacall(ht16k33_alpha::HT16K33AlphaDisplay & it, std::string text);

      //leds
      void leds_keypad_lambdacall(light::AddressableLight & it);
      void leds_case_lambdacall(light::AddressableLight & it);
      void leds_rfid_lambdacall(light::AddressableLight & it);

      void start_typing();
      void typing_timeout();
      void start_service();
      void service_timeout();

      //config triggers
      void on_keypad_progress(std::string x);
      void on_keypad_value(std::string x);
      void on_boot();
      void on_shutdown();
    protected:
      CallbackManager<void()> on_ready_callback_;
      CallbackManager<void()> on_code_check_callback_;
      CallbackManager<void()> on_code_ok_callback_;
      CallbackManager<void()> on_code_ko_callback_;
    };

    class ReadyTrigger : public Trigger<> {
    public:
      explicit ReadyTrigger(AlarmKeypad *parent) {
        parent->add_on_ready_callback([this]() { this->trigger(); });
      }
    };

    class CodeCheckTrigger : public Trigger<> {
    public:
      explicit CodeCheckTrigger(AlarmKeypad *parent) {
        parent->add_on_code_check_callback([this]() { this->trigger(); });
      }
    };

    class CodeOKTrigger : public Trigger<> {
    public:
      explicit CodeOKTrigger(AlarmKeypad *parent) {
        parent->add_on_code_ok_callback([this]() { this->trigger(); });
      }
    };

    class CodeKOTrigger : public Trigger<> {
    public:
      explicit CodeKOTrigger(AlarmKeypad *parent) {
        parent->add_on_code_ko_callback([this]() { this->trigger(); });
      }
    };


  }  // namespace alarm_keypad_component
}  // namespace esphome