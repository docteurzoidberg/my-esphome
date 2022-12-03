#pragma once

#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/components/display/display_buffer.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "display_effect.h"
#include "zilloscope_mode.h"

#include <string>
#include <queue>

namespace esphome {
  namespace zilloscope {

    using display_writer_t = std::function<bool(display::DisplayBuffer &, uint32_t)>;
    using notification_display_writer_t = std::function<bool(display::DisplayBuffer &, uint32_t, std::string, uint32_t)>;

    //states of the zilloscope
    enum state
    {
      booting,
      splash,
      main,
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
      uint32_t _type;
      std::string _text;
      unsigned long _timeout;

      Notification(uint32_t type, std::string text, unsigned long timeout) {
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
        if(_started==0 || _timeout<=0)
          return false;
        return (millis()-_started) > _timeout;
      }

      uint32_t get_type() {
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
      uint32_t get_notification_type();
      std::string get_active_effect_name();
      std::string get_active_mode_type();
      std::string get_active_mode_name();
      uint32_t get_mode_index(std::string name);
      const std::vector<Mode *> &get_modes() const;

      void set_state(state state);
      void set_display(display::DisplayBuffer * it);
      void set_config_use_splash(bool value);
      void set_config_default_mode(std::string value);

      void add_modes(std::vector<Mode *> modes);
      void add_on_boot_callback(std::function<void()> callback) {this->on_boot_callback_.add(std::move(callback));}
      void add_on_splash_callback(std::function<void()> callback) {this->on_splash_callback_.add(std::move(callback));}
      void add_on_ready_callback(std::function<void()> callback) {this->on_ready_callback_.add(std::move(callback));}

      void next_notification();
      void end_notification();

    //display
      void set_render_boot(display_writer_t  &&render_boot_f) { this->render_boot_f_ = render_boot_f; }
      void set_render_splash(display_writer_t  &&render_splash_f) { this->render_splash_f_ = render_splash_f; }
      void set_render_notification(notification_display_writer_t  &&render_notification_f) { this->render_notification_f_ = render_notification_f; }


      void set_text_sensor_mode_name(text_sensor::TextSensor* sensor) {
        this->text_sensor_mode_name_ = sensor;
      }
      void set_text_sensor_effect_name(text_sensor::TextSensor* sensor) {
        this->text_sensor_effect_name_ = sensor;
      }

      void register_text_sensor(text_sensor::TextSensor* sensor, std::string valuetype) {
        if(valuetype=="current_mode_name") set_text_sensor_mode_name(sensor);
        if(valuetype=="current_effect_name") set_text_sensor_effect_name(sensor);
      }

      void display_lambdacall(display::DisplayBuffer & it);

    //events
      void on_boot();
      void on_splash();
      void on_ready();
      void on_ota();
      void on_shutdown();

    //services
      void service_notify(int type, std::string text, unsigned long timeout);

      void service_mode(std::string name);
      void service_mode_next();
      void service_mode_prev();
      void service_mode_back();

      void service_effect_start(std::string name);
      void service_effect_stop();
      void service_effect_back();
      void service_effect_next();
      void service_effect_prev();

      void service_start_stream();
      void service_stop_stream();

    protected:

      optional<uint32_t> mode_;

      std::vector<Mode *> modes_;

      /// Value for storing the index of the currently active effect. 0 if no effect is active
      uint32_t active_mode_index_{0};
      
      /// Value for storing the index of the last active effect. 0 if no effect previously running
      uint32_t last_mode_index_{0};

      /// Internal method to enter mode with the given index
      void start_mode_(uint32_t mode_index);

      Mode *get_active_mode_();

      //triggers
      CallbackManager<void()> on_boot_callback_;    //not used
      CallbackManager<void()> on_splash_callback_;  //not used
      CallbackManager<void()> on_ready_callback_;

      //Sensors
      text_sensor::TextSensor* text_sensor_mode_name_{nullptr};
      text_sensor::TextSensor* text_sensor_effect_name_{nullptr};

      //display lambdas
      display_writer_t render_boot_f_;
      display_writer_t render_splash_f_;
      notification_display_writer_t render_notification_f_;
    };

    //Automation triggers
    class BootTrigger : public Trigger<> {
    public:
      explicit BootTrigger(ZilloScope *parent) {
        parent->add_on_boot_callback([this]() { this->trigger(); });
      }
    };
    class SplashTrigger : public Trigger<> {
    public:
      explicit SplashTrigger(ZilloScope *parent) {
        parent->add_on_splash_callback([this]() { this->trigger(); });
      }
    };

    class ReadyTrigger : public Trigger<> {
    public:
      explicit ReadyTrigger(ZilloScope *parent) {
        parent->add_on_ready_callback([this]() { this->trigger(); });
      }
    };
  }  // namespace alarm_keypad_component
}  // namespace esphome