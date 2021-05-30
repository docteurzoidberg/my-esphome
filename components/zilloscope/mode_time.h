#pragma once

#include "esphome/core/component.h"
#include "esphome/components/display/display_buffer.h"
#include "zilloscope_mode.h"

namespace esphome {
namespace zilloscope {

 class ModeTime: public Mode  {
    public:
      ModeTime(const std::string &name,
                          const std::function<bool(display::DisplayBuffer &, uint32_t frame, bool initial_run)> &f,
                          uint32_t update_interval)
          : Mode(name,f,update_interval) {}
  };

}
}
