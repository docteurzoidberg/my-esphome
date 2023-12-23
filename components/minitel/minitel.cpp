#include "esphome/core/log.h"
#include "minitel.h"

namespace esphome {
namespace minitel {

static const char *TAG = "minitel.component";

static uint8_t _state=STATE_BOOTING;
static bool _has_state=false;

// component
void Minitel::setup() {

}

void Minitel::loop() {
  if(_state==STATE_BOOTING){
    if(!_has_state) {
      //todo: check state
      _has_state = false;
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

void Minitel::dump_config(){
  ESP_LOGCONFIG(TAG, "Minitel component");
}

uint8_t Minitel::get_state() {
  return _state;
}

}

}  // namespace esphome