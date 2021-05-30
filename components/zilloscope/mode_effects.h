#pragma once

#include "esphome/core/component.h"
#include "esphome/components/display/display_buffer.h"
#include "zilloscope_mode.h"

namespace esphome {
namespace zilloscope {

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