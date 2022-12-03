#pragma once
#include "esphome/core/automation.h"
#include "esphome/core/component.h"
#include "esphome/core/color.h"
#include "esphome/components/light/addressable_light.h"
#include "esphome/components/time/real_time_clock.h"

#define NUM_CHARS 4
#define NUM_LEDS_PER_SEGMENT 9
#define NUM_SEGMENTS_PER_CHAR 7
#define NUM_LEDS_PER_CHAR (NUM_LEDS_PER_SEGMENT * NUM_SEGMENTS_PER_CHAR)
#define NUM_LEDS_TOTAL (NUM_LEDS_PER_CHAR*NUM_CHARS)

static const uint8_t font_table[] PROGMEM =  {
  0b01110111, //0 segments 1 2 3 5 6 7
  0b00010001, //1 segments 1 5
  0b01101011, //2 segments 1 2 4 6 7
  0b00111011, //3 segments 1 2 4 5 6
  0b00011101, //4 segments 1 3 4 5
  0b00111110, //5 segments 2 3 4 5 6
  0b01111110, //6 segments 2 3 4 5 6 7
  0b00010011, //7 segments 1 2 5
  0b01111111, //8 segments 1 2 3 4 5 6 7
  0b00111111, //9 segments 1 2 3 4 5 6
  0b00000000, // empty
};

#define MATRIX_WIDTH 80
#define MATRIX_HEIGHT 21
struct pixelcoord {
  uint8_t x;
  uint8_t y;
};

static esphome::light::ESPHSVColor matrix_buffer[MATRIX_WIDTH][MATRIX_HEIGHT];
static const std::array<pixelcoord, NUM_LEDS_TOTAL> matrix_setup PROGMEM = {
  (pixelcoord) {0,11},
  {0,12},
  {0,13},
  {0,14},
  {0,15},
  {0,16},
  {0,17},
  {0,18},
  {0,19},
  {1,20},
  {2,20},
  {3,20},
  {4,20},
  {5,20},
  {6,20},
  {7,20},
  {8,20},
  {9,20},
  {10,19},
  {10,18},
  {10,17},
  {10,16},
  {10,15},
  {10,14},
  {10,13},
  {10,12},
  {10,11},
  {9,10},
  {8,10},
  {7,10},
  {6,10},
  {5,10},
  {4,10},
  {3,10},
  {2,10},
  {1,10},
  {0,9},
  {0,8},
  {0,7},
  {0,6},
  {0,5},
  {0,4},
  {0,3},
  {0,2},
  {0,1},
  {1,0},
  {2,0},
  {3,0},
  {4,0},
  {5,0},
  {6,0},
  {7,0},
  {8,0},
  {9,0},
  {10,1},
  {10,2},
  {10,3},
  {10,4},
  {10,5},
  {10,6},
  {10,7},
  {10,8},
  {10,9},
  {19,11},
  {19,12},
  {19,13},
  {19,14},
  {19,15},
  {19,16},
  {19,17},
  {19,18},
  {19,19},
  {20,20},
  {21,20},
  {22,20},
  {23,20},
  {24,20},
  {25,20},
  {26,20},
  {27,20},
  {28,20},
  {29,19},
  {29,18},
  {29,17},
  {29,16},
  {29,15},
  {29,14},
  {29,13},
  {29,12},
  {29,11},
  {28,10},
  {27,10},
  {26,10},
  {25,10},
  {24,10},
  {23,10},
  {22,10},
  {21,10},
  {20,10},
  {19,9},
  {19,8},
  {19,7},
  {19,6},
  {19,5},
  {19,4},
  {19,3},
  {19,2},
  {19,1},
  {20,0},
  {21,0},
  {22,0},
  {23,0},
  {24,0},
  {25,0},
  {26,0},
  {27,0},
  {28,0},
  {29,1},
  {29,2},
  {29,3},
  {29,4},
  {29,5},
  {29,6},
  {29,7},
  {29,8},
  {29,9},
  {39,11},
  {39,12},
  {39,13},
  {39,14},
  {39,15},
  {39,16},
  {39,17},
  {39,18},
  {39,19},
  {40,20},
  {41,20},
  {42,20},
  {43,20},
  {44,20},
  {45,20},
  {46,20},
  {47,20},
  {48,20},
  {49,19},
  {49,18},
  {49,17},
  {49,16},
  {49,15},
  {49,14},
  {49,13},
  {49,12},
  {49,11},
  {48,10},
  {47,10},
  {46,10},
  {45,10},
  {44,10},
  {43,10},
  {42,10},
  {41,10},
  {40,10},
  {39,9},
  {39,8},
  {39,7},
  {39,6},
  {39,5},
  {39,4},
  {39,3},
  {39,2},
  {39,1},
  {40,0},
  {41,0},
  {42,0},
  {43,0},
  {44,0},
  {45,0},
  {46,0},
  {47,0},
  {48,0},
  {49,1},
  {49,2},
  {49,3},
  {49,4},
  {49,5},
  {49,6},
  {49,7},
  {49,8},
  {49,9},
  {59,11},
  {59,12},
  {59,13},
  {59,14},
  {59,15},
  {59,16},
  {59,17},
  {59,18},
  {59,19},
  {60,20},
  {61,20},
  {62,20},
  {63,20},
  {64,20},
  {65,20},
  {66,20},
  {67,20},
  {68,20},
  {69,19},
  {69,18},
  {69,17},
  {69,16},
  {69,15},
  {69,14},
  {69,13},
  {69,12},
  {69,11},
  {68,10},
  {67,10},
  {66,10},
  {65,10},
  {64,10},
  {63,10},
  {62,10},
  {61,10},
  {60,10},
  {59,9},
  {59,8},
  {59,7},
  {59,6},
  {59,5},
  {59,4},
  {59,3},
  {59,2},
  {59,1},
  {60,0},
  {61,0},
  {62,0},
  {63,0},
  {64,0},
  {65,0},
  {66,0},
  {67,0},
  {68,0},
  {69,1},
  {69,2},
  {69,3},
  {69,4},
  {69,5},
  {69,6},
  {69,7},
  {69,8},
  {69,9}
};

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

        void add_on_ready_callback(std::function<void()> callback) {
          this->_on_ready_callback_.add(std::move(callback));
        }

        void on_ready() {
          _state = state::time;
          ESP_LOGD(TAG, "ready");
          this->_on_ready_callback_.call();
        }
        // @param bit = 0 - 7
        bool get_bit(char c, int n)
        {
          return (c >> n) & 1;
        }

        void draw_number(light::AddressableLight  & it, uint8_t char_index, uint8_t number, Color color=Color::WHITE) {

          //Calculate starting led index
          uint8_t led_start = char_index * NUM_LEDS_PER_CHAR;

          //turn off all leds from the char
          it.range(led_start,led_start+NUM_LEDS_PER_CHAR) = Color::BLACK;

          if(number>10)
            return;

          //get segments to lit on
          uint8_t segments = font_table[number];

          //lit segments
          for(uint8_t i=0;i<NUM_LEDS_PER_CHAR;i++){
            uint8_t led_segment_index = i / NUM_LEDS_PER_SEGMENT;
            bool segment_should_be_lit = get_bit(segments,led_segment_index);
            if(!segment_should_be_lit)
              continue;
            pixelcoord led_coords=matrix_setup[led_start+i];
            it[led_start+i]=matrix_buffer[led_coords.x][led_coords.y];
          }
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

          //rainbooooooooowwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
          light::ESPHSVColor hsv;
          hsv.value = 255;
          hsv.saturation = 240;
          uint16_t hue = (millis() * 10) % 0xFFFF;
          const uint16_t add = 0xFFFF / (MATRIX_WIDTH*MATRIX_HEIGHT);

          for(uint8_t x=0;x<MATRIX_WIDTH;x++){
            for(uint8_t y=0;y<MATRIX_HEIGHT;y++){
              hsv.hue = hue >> 8;
              matrix_buffer[x][y]= hsv;
              hue += add;
            }
          }

          esphome::time::ESPTime now = _time->now();
          draw_number(it, 0, now.minute<10 ? now.minute : now.minute % 10);
          draw_number(it, 1, now.minute<10 ? 0 : now.minute / 10);
          draw_number(it, 2, now.hour<10 ?  now.hour : now.hour % 10);
          draw_number(it, 3, now.hour<10 ? 0 : now.hour / 10);
        }

        void set_time(time::RealTimeClock *time) {
          _time=time;
        }

        void set_clock_addressable_lights(light::LightState *it) {
          _clock_lights=it;
        }

      protected:
        CallbackManager<void()> _on_ready_callback_;
        bool _has_time{false};
        state _state{state::booting};
        light::LightState *_clock_lights;
        time::RealTimeClock *_time;
    };

    class ReadyTrigger : public Trigger<> {
      public:
        explicit ReadyTrigger(TimeMasheen *parent) {
          parent->add_on_ready_callback([this]() { this->trigger(); });
        }
    };
  }
}