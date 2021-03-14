#pragma once

#include "esphome/core/component.h"
#include "esphome/components/display/display_buffer.h"
#include <math.h>

namespace esphome {
namespace zilloscope {

  class DisplayEffect {
    public:
      explicit DisplayEffect( const std::string &name) : name_(name) {}
      const std::string &get_name() { return this->name_; }

      /// Initialize this LightEffect. Will be called once after creation.
      virtual void start() {}
      virtual void start_internal() { this->start(); }
      /// Called when this effect is about to be removed
      virtual void stop() {}
      /// Apply this effect. Use the provided state for starting transitions, ...
      virtual void apply(display::DisplayBuffer &it) = 0;
      /// Internal method called by the LightState when this light effect is registered in it.
      virtual void init() {}
      void init_internal(display::DisplayBuffer *display) {
        this->display_ = display;
        this->init();
      }
    protected:
      display::DisplayBuffer *get_display_() const { return (display::DisplayBuffer *) this->display_; }
      display::DisplayBuffer *display_{nullptr};
      std::string name_;
  };

  class DisplayLambdaEffect : public DisplayEffect {
    public:
      DisplayLambdaEffect(const std::string &name,
                          const std::function<void(display::DisplayBuffer &, bool initial_run)> &f,
                          uint32_t update_interval)
          : DisplayEffect(name), f_(f), update_interval_(update_interval) {}

      void start() override { this->initial_run_ = true; }
      void stop() override {}
      void apply(display::DisplayBuffer &it) override {
        const uint32_t now = millis();
        if (now - this->last_run_ >= this->update_interval_) {
          this->last_run_ = now;
          this->f_(it, this->initial_run_);
          this->initial_run_ = false;
        }
      }
    protected:
      std::function<void(display::DisplayBuffer &, bool initial_run)> f_;
      uint32_t update_interval_;
      uint32_t last_run_{0};
      bool initial_run_;
  };

  class DisplayFireEffect : public DisplayEffect {
    public:
      uint8_t counter = 0;
      float cur[35][25];
      float prev[35][25];
      float v = 0.5f;
      float a = 0.01f;

      explicit DisplayFireEffect(const std::string &name) : DisplayEffect(name) {
        //todo: fill arrays?
      }
      uint32_t heat_color(float value) {
        int r = (int) min(((int)(value * 255) * 3), 255);
        int g = (int) max(0, min((int)((value - 0.33) * 255 * 3), 255));
        int b = (int) max(0, min((int)((value - 0.66) * 255 * 3), 255));
        return (r << 16) + (g << 8) + b; // là t'as p't'être une fonction toute faite qui te donne une couleur pour le triplet r, g, b correspondant a utiliser plutôt que ça
        //return r + (r << 8) + (r << 16);
      }
      void start() override {
         for(int a=0;a<35;a++) {
          for(int b=0;b<25;b++) {
            prev[a][b]=cur[a][b]=0;
          }
        }
      }
      // code exécuté a chaque frame
      void apply(display::DisplayBuffer &it) override {

        for(int x = 0 ; x < it.get_width() ; x++)
          for(int y = 0 ; y < it.get_height() ; y++) {
            //=> ici, donc, les 0.1, 0.1, 0.1 et 0.7 servent a orienter la flamme
            //la somme des 4 valeurs doit faire 1
            //float tmp = prev[max(x-1, 0)][y] * 0.1 + prev[x][max(y-1, 0)] * 0.1 + prev[min(x+1, it.get_width() - 1)][y] * 0.1 + prev[x][min(y+1, it.get_height() - 1)] * 0.7;
            float tmp = prev[max(x-1, 0)][y] * 0.01 + prev[x][max(y-1, 0)] * 0.01 + prev[min(x+1, it.get_width() - 1)][y] * 0.01 + prev[x][min(y+1, it.get_height() - 1)] * 0.97;

            //cur[x][y] = tmp * 0.1 + prev[x][y] * 0.89;
            cur[x][y] = tmp * v + prev[x][y] * (1 - v - a);
            it.draw_pixel_at(x, y, Color(heat_color(cur[x][y])));
          }

        //cur[0][0]=1;
        for(int x = 0 ; x < it.get_width() ; x ++)
          if((esp_random() & 31) == 0)
            cur[x][it.get_height() - 1] = 1;

        //prev = cur;
        for(int a=0;a<it.get_width();a++) {
          for(int b=0;b<it.get_height();b++) {
            prev[a][b]=cur[a][b];
          }
        }
        counter++;
      }

      void set_speed(uint32_t speed) { this->speed_ = speed; }
      void set_width(uint16_t width) { this->width_ = width; }
      void set_height(uint16_t height) { this->height_ = height; }

    protected:
      uint32_t speed_{10};
      uint16_t width_{8};
      uint16_t height_{8};
  };

  class DisplayRainbowEffect : public DisplayEffect {
    public:
      explicit DisplayRainbowEffect(const std::string &name) : DisplayEffect(name) {}

      void apply(display::DisplayBuffer &it) override {
        //TODO
      }
      void set_speed(uint32_t speed) { this->speed_ = speed; }
      void set_width(uint16_t width) { this->width_ = width; }

    protected:
      uint32_t speed_{10};
      uint16_t width_{50};
  };

}
}