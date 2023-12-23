#pragma once
#include "esphome.h"
#include "esphome/core/automation.h"
#include "esphome/core/component.h"
#include "esphome/core/color.h"

#include "esphome/components/time/real_time_clock.h"
#include <driver/gpio.h>
#include <driver/dac.h>
#include <soc/rtc.h>
#include <soc/sens_reg.h>
#include "DataTable.h"


//Variables

namespace esphome {
  namespace oscilloclock {

    //consts
    static const char *TAG = "oscilloclock.component";

    enum state
    {
      booting,
      time,
      ota,
      shutdown
    };

    //milliseconds, you should twick this
    //to get a better accuracy  int           lastx,lasty;
    int lastx,lasty;
    unsigned long currentMillis  = 0;
    unsigned long previousMillis = 0;
    int           Timeout        = 20;
    const    long interval       = 990;


  inline void Dot(int x, int y)
  {
      if (lastx!=x){
        lastx=x;
        dac_output_voltage(DAC_CHANNEL_1, x);
      }
      if (lasty!=y){
        lasty=y;
        dac_output_voltage(DAC_CHANNEL_2, y);
      }
  }

  // Bresenham's Algorithm implementation optimized
  // also known as a DDA - digital differential analyzer
  void Line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
  {
      int acc;
      // for speed, there are 8 DDA's, one for each octant
      if (y1 < y2) { // quadrant 1 or 2
          uint8_t dy = y2 - y1;
          if (x1 < x2) { // quadrant 1
              uint8_t dx = x2 - x1;
              if (dx > dy) { // < 45
                  acc = (dx >> 1);
                  for (; x1 <= x2; x1++) {
                      Dot(x1, y1);
                      acc -= dy;
                      if (acc < 0) {
                          y1++;
                          acc += dx;
                      }
                  }
              }
              else {   // > 45
                  acc = dy >> 1;
                  for (; y1 <= y2; y1++) {
                      Dot(x1, y1);
                      acc -= dx;
                      if (acc < 0) {
                          x1++;
                          acc += dy;
                      }
                  }
              }
          }
          else {  // quadrant 2
              uint8_t dx = x1 - x2;
              if (dx > dy) { // < 45
                  acc = dx >> 1;
                  for (; x1 >= x2; x1--) {
                      Dot(x1, y1);
                      acc -= dy;
                      if (acc < 0) {
                          y1++;
                          acc += dx;
                      }
                  }
              }
              else {  // > 45
                  acc = dy >> 1;
                  for (; y1 <= y2; y1++) {
                      Dot(x1, y1);
                      acc -= dx;
                      if (acc < 0) {
                          x1--;
                          acc += dy;
                      }
                  }
              }
          }
      }
      else { // quadrant 3 or 4
          uint8_t dy = y1 - y2;
          if (x1 < x2) { // quadrant 4
              uint8_t dx = x2 - x1;
              if (dx > dy) {  // < 45
                  acc = dx >> 1;
                  for (; x1 <= x2; x1++) {
                      Dot(x1, y1);
                      acc -= dy;
                      if (acc < 0) {
                          y1--;
                          acc += dx;
                      }
                  }

              }
              else {  // > 45
                  acc = dy >> 1;
                  for (; y1 >= y2; y1--) {
                      Dot(x1, y1);
                      acc -= dx;
                      if (acc < 0) {
                          x1++;
                          acc += dy;
                      }
                  }

              }
          }
          else {  // quadrant 3
              uint8_t dx = x1 - x2;
              if (dx > dy) { // < 45
                  acc = dx >> 1;
                  for (; x1 >= x2; x1--) {
                      Dot(x1, y1);
                      acc -= dy;
                      if (acc < 0) {
                          y1--;
                          acc += dx;
                      }
                  }

              }
              else {  // > 45
                  acc = dy >> 1;
                  for (; y1 >= y2; y1--) {
                      Dot(x1, y1);
                      acc -= dx;
                      if (acc < 0) {
                          x1--;
                          acc += dy;
                      }
                  }
              }
          }

      }
  }

  void PlotTable(uint8_t *SubTable, int SubTableSize, int skip, int opt, int offset)
  {
    int i=offset;
    while (i<SubTableSize){
      if (SubTable[i+2]==skip){
        i=i+3;
        if (opt==1) if (SubTable[i]==skip) i++;
      }
      Line(SubTable[i],SubTable[i+1],SubTable[i+2],SubTable[i+3]);
      if (opt==2){
        Line(SubTable[i+2],SubTable[i+3],SubTable[i],SubTable[i+1]);
      }
      i=i+2;
      if (SubTable[i+2]==0xFF) break;
    }
  }



    class OscilloClock : public Component {
      public:

        void setup() override {
          dac_output_enable(DAC_CHANNEL_1);
          dac_output_enable(DAC_CHANNEL_2);
        }

        void loop() override{
          //Waiting until we got a valid time to enter state::time after boot
          if(!_has_time && _time->now().is_valid()) {
            _has_time=true;
            on_ready();
            return;
          }
          if(_state==state::time) {
            render_time();
          }
          else if (_state == state::booting) {
            render_boot();
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

        state get_state() {
          return _state;
        }

        void set_state(state state) {
          _state=state;
        }

        void render_boot() {

        }

        void render_time() {
          if (esphome::millis() - previousMillis < interval) {
            return;
          }
          esphome::time::ESPTime now = _time->now();
          PlotTable(DialData,sizeof(DialData),0x00,1,0);      //2 to back trace
          PlotTable(DialDigits12,sizeof(DialDigits12),0x00,1,0);//2 to back trace
          PlotTable(HrPtrData, sizeof(HrPtrData), 0xFF,0,9*now.hour);  // 9*h
          PlotTable(MinPtrData,sizeof(MinPtrData),0xFF,0,9*now.minute);  // 9*m
          PlotTable(SecPtrData,sizeof(SecPtrData),0xFF,0,5*now.second);  // 5*s
          //draw_number(it, 0, now.minute<10 ? now.minute : now.minute % 10);
          //draw_number(it, 1, now.minute<10 ? 0 : now.minute / 10);
          //draw_number(it, 2, now.hour<10 ?  now.hour : now.hour % 10);
          //draw_number(it, 3, now.hour<10 ? 0 : now.hour / 10);
          previousMillis = esphome::millis();
        }

        void set_time(time::RealTimeClock *time) {
          _time=time;
        }

      protected:
        CallbackManager<void()> _on_ready_callback_;
        bool _has_time{false};
        state _state{state::booting};
        time::RealTimeClock *_time;
    };

    class ReadyTrigger : public Trigger<> {
      public:
        explicit ReadyTrigger(OscilloClock *parent) {
          parent->add_on_ready_callback([this]() { this->trigger(); });
        }
    };
  }
}