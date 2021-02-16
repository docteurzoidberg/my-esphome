#pragma once

#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/components/display/display_buffer.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/homeassistant/text_sensor/homeassistant_text_sensor.h"
#include <string>

namespace esphome {
  namespace zilloscope {

    using display_writer_t = std::function<void(display::DisplayBuffer &)>;

    //states of the zilloscope
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

    //component
      void setup() override;
      void loop() override;
      void dump_config() override;

    //getter/setters
      state get_state();
      void set_state(state state);
      void set_display(display::DisplayBuffer * it);
      void set_font(display::Font * font);
      void set_time(time::RealTimeClock * time);
      void set_render_boot(display_writer_t  &&render_boot_f) { this->render_boot_f_ = render_boot_f; }
      void set_render_time(display_writer_t  &&render_time_f) { this->render_time_f_ = render_time_f; }
      void set_render_ota(display_writer_t  &&render_ota_f) { this->render_ota_f_ = render_ota_f; }
      void set_render_shutdown(display_writer_t &&render_shutdown_f) { this->render_shutdown_f_ = render_shutdown_f; }
      void add_on_ready_callback(std::function<void()> callback) {this->on_ready_callback_.add(std::move(callback));}

    //display
      void display_lambdacall(display::DisplayBuffer & it);

    //events
      void on_boot();
      void on_ota();
      void on_shutdown();

    protected:
      //triggers
      CallbackManager<void()> on_ready_callback_;

      //display lambdas
      display_writer_t render_boot_f_;
      display_writer_t render_time_f_;
      display_writer_t render_ota_f_;
      display_writer_t render_shutdown_f_;
    };

    class ReadyTrigger : public Trigger<> {
    public:
      explicit ReadyTrigger(ZilloScope *parent) {
        parent->add_on_ready_callback([this]() { this->trigger(); });
      }
    };
  }  // namespace alarm_keypad_component
}  // namespace esphome