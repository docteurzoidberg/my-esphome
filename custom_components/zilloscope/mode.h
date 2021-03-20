#pragma once

#include "esphome/core/component.h"
#include "esphome/components/display/display_buffer.h"

namespace esphome {
namespace zilloscope {
  class ZilloMode {
    public:
      ZilloMode(const std::string &name) : name_(name) {

      }
      virtual void start() override { this->initial_run_ = true; }
      void draw(display::DisplayBuffer &it) {
        const uint32_t now = millis();
        if (now - this->last_run_ >= this->update_interval_) {
          this->last_run_ = now;
          this->f_(it, this->initial_run_);
          this->initial_run_ = false;
        }
      }
    protected:
      std::string name_;
      std::function<void(display::DisplayBuffer &, bool initial_run)> f_;
      uint32_t update_interval_;
      uint32_t last_run_{0};
      bool initial_run_;
  };

  class ZilloModeTime: public ZilloMode {
      ZilloModeTime(const std::string &name,
                          const std::function<void(display::DisplayBuffer &, bool initial_run)> &f,
                          uint32_t update_interval)
          : ZilloMode(name), f_(f), update_interval_(update_interval) {}
  };

  class ZilloModeEffects: public ZilloMode {
      ZilloModeEffects(const std::string &name)
          : ZilloMode(name) {}
  };
}
}