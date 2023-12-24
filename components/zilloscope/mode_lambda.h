#pragma once

#include "esphome/core/component.h"
#include "esphome/components/display/display_buffer.h"
#include "zilloscope_mode.h"

namespace esphome {
namespace zilloscope {
  class ModeLambda: public Mode  {
    public:
      const char *TAG = "zilloscope.modelambda";
      ModeLambda(const std::string &name,
                          const std::function<bool(display::Display &, uint32_t frame, bool initial_run)> &f,
                          uint32_t update_interval)
          : Mode(name,"ModeLambda",f,update_interval) {}
  };
}
}
