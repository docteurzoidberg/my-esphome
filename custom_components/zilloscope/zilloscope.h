#pragma once

#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/components/display/display_buffer.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/homeassistant/text_sensor/homeassistant_text_sensor.h"
#include <string>

namespace esphome {
  namespace zilloscope {

    enum state
    {
      booting,
      time,
      notify,
      ota,
      shutdown
    };

    class ZilloScope : public Component {
    public:
      void setup() override;
      void loop() override;
      void dump_config() override;

      state get_state();
      char get_arm_key();

      void set_state(state state);
      void set_arm_key(char key);
      void set_display(display::DisplayBuffer * it);
      void set_font(display::Font * font);
      void set_time(time::RealTimeClock * time);

      void add_on_ready_callback(std::function<void()> callback) {
        this->on_ready_callback_.add(std::move(callback));
      }

      void display_lambdacall(display::DisplayBuffer & it);
      void display_boot();
      void display_time();
      void display_ota();
      void display_shutdown();

      void on_boot();
      void on_ota();
      void on_shutdown();

    protected:
      CallbackManager<void()> on_ready_callback_;
    };

    class ReadyTrigger : public Trigger<> {
    public:
      explicit ReadyTrigger(ZilloScope *parent) {
        parent->add_on_ready_callback([this]() { this->trigger(); });
      }
    };
  }  // namespace alarm_keypad_component
}  // namespace esphome