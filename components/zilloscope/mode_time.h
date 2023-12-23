#pragma once

#include "esphome/core/component.h"
#include "esphome/components/display/display_buffer.h"
#include "esphome/components/time/real_time_clock.h"
#include "zilloscope_mode.h"

namespace esphome {
namespace zilloscope {
  class ModeTime: public Mode  {
    public:
      const char *TAG = "zilloscope.modetime";
      
      ModeTime(const std::string &name,
                          const std::function<bool(display::DisplayBuffer &, uint32_t frame, bool initial_run)> &f, 
                          time::RealTimeClock * time,
                          uint32_t update_interval)
          : Mode(name,"ModeTime",f,update_interval) {
        _time=time;
      }

      virtual bool is_ready() override {
        if(!_has_time && _time->now().is_valid()) {
          _has_time = true;
        }
        return _has_time;
      }

    private:
      bool _has_time{false};
      time::RealTimeClock *_time=nullptr;
  };
}
}
