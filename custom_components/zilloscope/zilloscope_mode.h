#pragma once

#include "esphome/core/component.h"
#include "esphome/components/display/display_buffer.h"

namespace esphome {
namespace zilloscope {
  class Mode {
    public:
      Mode(const std::string &name, std::function<void(display::DisplayBuffer &, bool initial_run)> f, uint32_t update_interval)
        : name_(name), f_(f), update_interval_(update_interval) {}

      Mode(const std::string &name) : name_(name) {}

      virtual void start() { this->initial_run_ = true; }
      virtual void start_internal() {  }
      virtual void stop() {  }
      void draw(display::DisplayBuffer &it) {
        const uint32_t now = millis();
        if (now - this->last_run_ >= this->update_interval_) {
          this->last_run_ = now;
          this->f_(it, this->initial_run_);
          this->initial_run_ = false;
        }
      }
      std::string get_name() { return name_; }
    protected:
      std::string name_;
      std::function<void(display::DisplayBuffer &, bool initial_run)> f_;
      uint32_t update_interval_;
      uint32_t last_run_{0};
      bool initial_run_;
  };

  class ModeTime: public zilloscope::Mode {
    public:
      ModeTime(const std::string &name,
                          const std::function<void(display::DisplayBuffer &, bool initial_run)> &f,
                          uint32_t update_interval)
          : Mode(name,f,update_interval) {}
  };

  class ModeEffects: public zilloscope::Mode {
    public:
      ModeEffects(const std::string &name)
          : Mode(name) {}
    /*
    TODO: handle effect's interval instal of global 'effects' one
    */
  };
}
}