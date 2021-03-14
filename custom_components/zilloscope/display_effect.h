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

// Thanks to Liemmerle for the fire effect algorythm <3
  class DisplayFireEffect : public DisplayEffect {
    public:
      float max_flame_height = 1;
      float max_heat_spot = 1;
      float min_x_attenuation = 0.2;
      float init_flame_height = 0;
      float init_heat_spots = 0;
      float init_x_attenuation = 2;
      float speed = 100;
      float starting_speed = 1;

      explicit DisplayFireEffect(const std::string &name) : DisplayEffect(name) {

      }
/*
      float rnd(int x, int y) {
        int X = x ^ 6428;
        int Y = y ^ 6356;
        return ((
          X * 7151
          + Y * 1357)
          ^ 35135) / 137.0f % 1.0f;
      }
*/
      float rnd(int x, int y) {
        int X = x ^ 6428;
        int Y = y ^ 6356;
        float f=1;

        return ((
            (X * 7151 + Y * 1357)
            ^ 35135
          ) % 256) / 256.0f;

        /*
        return fmod(
          (
            (X * 7151 + Y * 1357)
            ^ 35135
          )
          / 137.0f
        ,f);
        */
        //return esp_random()/UINT32_MAX;
      }

      float noise(int X, int Y, int T, float flame_height, float heat_spots, float x_attenuation) {

        int x = X;
        float attenuation = (height_ - Y) / flame_height / height_ + pow(1 - (X + 1) * (width_ - X) * 4.0f / (width_ + 2) / (width_ + 2), 1 / x_attenuation);
        float n = 0;
        float sum_coeff = 0;

        for(int i = 8 ; i > 4 ; i >>= 1) {
          if(attenuation > 1)
            continue;
          int y = Y + T * 8 / i;

          float rnd_00 = pow(rnd(x / i, y / i), 1 / heat_spots / (attenuation + 1));
          float rnd_01 = pow(rnd(x / i, y / i + 1), 1 / heat_spots / (attenuation + 1));
          float rnd_10 = pow(rnd(x / i + 1, y / i), 1 / heat_spots / (attenuation + 1));
          float rnd_11 = pow(rnd(x / i + 1, y / i + 1), 1 / heat_spots / (attenuation + 1));

          float dx = x * 1.0f / i - x / i;
          float dy = y * 1.0f / i - y / i;

          float coeff = i;

          n += ((rnd_00 * (1 - dx) + rnd_10 * dx) * (1 - dy)
            + (rnd_01 * (1 - dx) + rnd_11 * dx) * dy) * coeff;
          sum_coeff += coeff;
        }

        return max(0.0f, n / sum_coeff - attenuation);
      }

      uint32_t heat_color(float value) {
        int r = (int) min(((int)(value * 255) * 3), 255);
        int g = (int) max(0, min((int)((value - 0.33) * 255 * 3), 255));
        int b = (int) max(0, min((int)((value - 0.66) * 255 * 3), 255));
        return (r << 16) + (g << 8) + b; // là t'as p't'être une fonction toute faite qui te donne une couleur pour le triplet r, g, b correspondant a utiliser plutôt que ça
        //return r + (r << 8) + (r << 16);
      }

      void start() override {

      }
      // code exécuté a chaque frame
      void apply(display::DisplayBuffer &it) override {
        unsigned long timer = millis();
        float flame_height = min(init_flame_height + starting_speed * timer / 1000.0f, max_flame_height);
        float heat_spots = min(init_heat_spots + starting_speed * timer / 1000.0f, max_heat_spot);
        float x_attenuation = max(init_x_attenuation - starting_speed * timer / 1000.0f, min_x_attenuation);
        for(int x = 0 ; x < width_; x ++) {
          for(int y = 0 ; y < height_; y ++) {
            uint32_t heat = heat_color(noise(x, y, (int) (timer * speed / 1000), flame_height, heat_spots, x_attenuation));
            //fill(red(heat), green(heat), blue(heat));
            //rect(x * 16, y * 16, 16, 16);
            it.draw_pixel_at(x,y,Color(heat));
          }
        }
      }
      void set_speed(uint32_t speed) { this->speed_ = speed; }
      void set_width(uint16_t width) { this->width_ = width; }
      void set_height(uint16_t height) { this->height_ = height; }
    protected:
      uint32_t speed_{10};
      uint16_t width_{35};
      uint16_t height_{25};
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