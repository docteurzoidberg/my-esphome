#pragma once
#include "esphome/core/hal.h"
#include "esphome/core/component.h"
#include "esphome/components/display/display_buffer.h"
#include "zilloscope_mode.h"

namespace esphome {
namespace zilloscope {
  class ModeMeteo: public Mode  {
    public:
      const char *TAG = "zilloscope.modemeteo";
      ModeMeteo(const std::string &name, const std::function<bool(display::DisplayBuffer &, uint32_t frame, bool initial_run)> &f,
                          uint32_t update_interval)
          : Mode(name,"ModeMeteo",f,update_interval) {}

      void set_sensor_inside_temp() {

      }
      void set_sensor_meteo_current_temp() {

      }
      void set_sensor_meteo_min_temp() {

      }
      void set_sensor_meteo_max_temp() {

      }
      void set_sensor_meteo_rain() {

      }
      void set_sensor_meteo_icon() {

      }
  };
}
}
