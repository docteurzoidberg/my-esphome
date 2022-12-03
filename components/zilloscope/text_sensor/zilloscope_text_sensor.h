#pragma once 

#include "esphome/components/zilloscope/zilloscope.h"
#include "esphome/components/text_sensor/text_sensor.h"

namespace esphome {
namespace zilloscope {

class ZilloscopeTextSensor : public text_sensor::TextSensor, public Component {
 public:
  ZilloscopeTextSensor() {};
 protected:
};

}  // namespace keypad
}  // namespace esphome

