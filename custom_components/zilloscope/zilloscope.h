#pragma once

#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/components/display/display_buffer.h"

#include <string>
#include <queue>

namespace esphome {
  namespace zilloscope {

    using display_writer_t = std::function<bool(display::DisplayBuffer &, uint32_t)>;

    //states of the zilloscope
    enum state
    {
      booting,
      time,
      notify,
      ota,
      shutdown
    };

    enum NotificationType
    {
      INFO,
      WARNING,
      ALERT,
      OTHER
    };

    class Notification {
    public:
      uint8_t _type;
      std::string _text;
      unsigned long _timeout;

      Notification(uint8_t type, std::string text, unsigned long timeout) {
        _type = type;
        _text = text;
        _timeout = timeout;
      }
      bool is_started() {
        return _started>0;
      }
      bool is_finished() {
        return _finished;
      }
      bool is_timed_out() {
        if(_started==0 || _timeout==0)
          return false;
        return (millis()-_started) > _timeout;
      }

      uint8_t get_type() {
        return _type;
      }
      std::string get_text() {
        return _text;
      }
      unsigned long get_timeout() {
        return _timeout;
      }
      unsigned long get_started() {
        return _started;
      }
      void start() {
        _started=millis();
      }
      void end() {
        _finished=true;
      }

    private:
      bool _finished=false;
      unsigned long _started=0;   //ts when set to current
      uint32_t _framecounter=0;   //ellapsed frames
    };

    class ZilloScope : public Component {
    public:

    //component
      void setup() override;
      void loop() override;
      void dump_config() override;
    //getter/setters
      state get_state();
      std::string get_notification_text();
      void set_state(state state);
      void set_display(display::DisplayBuffer * it);
      void set_font(display::Font * font);
      void set_time(time::RealTimeClock * time);
      void set_render_boot(display_writer_t  &&render_boot_f) { this->render_boot_f_ = render_boot_f; }
      void set_render_time(display_writer_t  &&render_time_f) { this->render_time_f_ = render_time_f; }
      void set_render_notification(display_writer_t  &&render_notification_f) { this->render_notification_f_ = render_notification_f; }
      void set_render_ota(display_writer_t  &&render_ota_f) { this->render_ota_f_ = render_ota_f; }
      void set_render_shutdown(display_writer_t &&render_shutdown_f) { this->render_shutdown_f_ = render_shutdown_f; }
      void add_on_ready_callback(std::function<void()> callback) {this->on_ready_callback_.add(std::move(callback));}

      void next_notification();
      void end_notification();

    //display
      void display_lambdacall(display::DisplayBuffer & it);

    //events
      void on_boot();
      void on_ota();
      void on_shutdown();

    //services
      void service_notify(int type, std::string text, unsigned long timeout);

    protected:

      //triggers
      CallbackManager<void()> on_ready_callback_;

      //display lambdas
      display_writer_t render_boot_f_;
      display_writer_t render_time_f_;
      display_writer_t render_notification_f_;
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