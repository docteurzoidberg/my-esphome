#include "esphome/core/log.h"
#include "plantiquette.h"

namespace esphome {
namespace plantiquette {

static const char *TAG = "plantiquette.component";

static uint8_t _state=STATE_BOOTING;
static bool _has_state=false;

static homeassistant::HomeassistantTextSensor *_tag1=NULL;
static homeassistant::HomeassistantTextSensor *_tag2=NULL;

// component
void Plantiquette::setup() {

}

void Plantiquette::loop() {
  if(_state==STATE_BOOTING){
    if(!_has_state) {
      _has_state = _tag1->has_state() && _tag2->has_state();
      if(_has_state) {
        _state = STATE_DISPLAY;
        this->on_ready_callback_.call();
      }
      else {
        ESP_LOGD(TAG, "waiting sensors from ha");
      }
    }
  }
}

void Plantiquette::dump_config(){
  ESP_LOGCONFIG(TAG, "Plantiquette component");
}

uint8_t Plantiquette::get_state() {
  return _state;
}

void Plantiquette::set_tag1(homeassistant::HomeassistantTextSensor *hatext) {
  _tag1=hatext;
}

void Plantiquette::set_tag2(homeassistant::HomeassistantTextSensor *hatext) {
  _tag2=hatext;
}

}

}  // namespace esphome