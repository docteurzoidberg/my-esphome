#pragma once
#include "esphome/core/component.h"
#include "esphome/core/color.h"
#include "esphome/components/light/addressable_light.h"
#include "esphome/components/time/real_time_clock.h"

namespace esphome {
  namespace timemasheen {

    using addressable_light_writer_t = std::function<void(light::LightState, uint32_t)>;

    //consts
    static const char *TAG = "timemasheen.component";

    enum state
    {
      booting,
      time,
      ota,
      shutdown
    };

    class TimeMasheen : public Component {
      public:

        void setup() override {
        }

        void loop() override{
          //Waiting until we got a valid time to enter state::time after boot
          if(!_has_time && _time->now().is_valid()) {
            _has_time=true;
            on_ready();
            return;
          }
        }

        void on_ready() {
          _state = state::time;
          ESP_LOGD(TAG, "ready");
        }

        void addressable_lights_lambdacall(light::AddressableLight  & it) {
          if(_state==state::booting) {
            return render_boot(it);
          }
          else if(_state==state::time) {
            return render_time(it);
          }
        }

        state get_state() {
          return _state;
        }

        void set_state(state state) {
          _state=state;
        }

        void render_boot(light::AddressableLight  & it) {
          //TODO;
          light::ESPHSVColor hsv;
          hsv.value = 255;
          hsv.saturation = 240;
          uint16_t hue = (millis() * 10) % 0xFFFF;
          const uint16_t add = 0xFFFF / it.size();
          for (auto var : it) {
            hsv.hue = hue >> 8;
            var = hsv;
            hue += add;
          }
        }

        void render_time(light::AddressableLight  & it) {
          //TODO;
        }

        void set_time(time::RealTimeClock * time) {
          _time = time;
        }

        void set_clock_addressable_lights(light::LightState *it) {
          _clock_lights=it;
        }

      protected:
        bool _has_time{false};
        state _state{state::booting};
        light::LightState *_clock_lights;
        time::RealTimeClock *_time;
    };
  }
}