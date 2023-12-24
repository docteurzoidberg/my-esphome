#pragma once
#include "esphome/core/hal.h"
#include "esphome/core/log.h"
#include "esphome/core/component.h"
#include "esphome/components/display/display_buffer.h"
#include <math.h>

#define LOGPERFS 0

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
      virtual void apply(display::Display &it) = 0;
      /// Internal method called by the LightState when this light effect is registered in it.
      virtual void init() {}
      void init_internal(display::Display *display) {
        this->display_ = display;
        this->init();
      }
    protected:
      display::Display *get_display_() const { return (display::Display *) this->display_; }
      display::Display *display_{nullptr};
      std::string name_;
  };

  class DisplayLambdaEffect : public DisplayEffect {
    public:
      DisplayLambdaEffect(const std::string &name,
                          const std::function<void(display::Display &, uint32_t frame, bool initial_run)> &f,
                          uint32_t update_interval)
          : DisplayEffect(name), f_(f), update_interval_(update_interval) {}

      void start() override { this->initial_run_ = true; }
      void stop() override {}
      void apply(display::Display &it) override {
        const uint32_t now = esphome::millis();
        if (now - this->last_run_ >= this->update_interval_) {
          this->last_run_ = now;
          this->f_(it, this->frame_counter_,this->initial_run_);
          this->frame_counter_++;
          this->initial_run_ = false;
        }
      }
    protected:
      std::function<void(display::Display &, uint32_t frame, bool initial_run)> f_;
      uint32_t update_interval_;
      uint32_t frame_counter_{0};
      uint32_t last_run_{0};
      bool initial_run_;
  };

  // Thanks to Liemmerle for the fire effect algorythm <3
  class DisplayFireEffect : public DisplayEffect {
    public:
      const char *TAG = "zilloscope.displayeffect";

      int max_flame_height = 1000;
      int max_heat_spots = 1000;
      int min_x_attenuation = 500;

      int min_flame_height = 300;
      int min_heat_spots = 300;
      int max_x_attenuation = 500;

      int init_flame_height = 1;
      int init_heat_spots = 1000;
      int init_x_attenuation = 5000;

      int speed = 15000;

      int starting_speed = 1000;

      int periodicity = 7200000;
      // pour constater la présence d'oscillations dans le feu, tu pourra diviser cette valeur par 1000 ou 1000000, ici 7200000 ça correspond a une période de 2h

      explicit DisplayFireEffect(const std::string &name) : DisplayEffect(name) {

      }

      int apow(int a, int b) {
        return 1000 + (a - 1000) * b / 1000;
      }

      int int_lerp(int a, int b, int c) {
        if(c <= 0)
          return a;
        if(c >= 1000)
          return b;
        return (a * (1000 - c) + b * c) / 1000;
      }

      unsigned int rnd(int x, int y) {
        int X = x ^ 64228;
        int Y = y ^ 61356;
        return ((
          X * 71521
          + Y * 13547
          ^ 35135) % 1000 + 1000) % 1000;
      }

      int noise(int X, int Y, int T, int flame_height, int heat_spots, int x_attenuation) {
        int x = X;
        int n = 0;

        int attenuation = (height_ - Y) * 1000 / height_ * 1000 / flame_height
          + (x_attenuation == 0 ? 0
          : std::max(0, apow(1000 - (X + 1) * (width_- X) * 4000 / ((width_ + 2) * (width_+ 2)), 1000000 / x_attenuation)));

        int sum_coeff = 0;

        for(int i = 8 ; i > 0 ; i >>= 1) {
          int y = Y + T * 8 / i;

          int rnd_00 = rnd(x / i, y / i);
          int rnd_01 = rnd(x / i, y / i + 1);
          int rnd_10 = rnd(x / i + 1, y / i);
          int rnd_11 = rnd(x / i + 1, y / i + 1);

          int coeff = i;

          int dx = x % i;
          int dy = y % i;

          n += ((rnd_00 * (i - dx) + rnd_10 * dx) * (i - dy)
              + (rnd_01 * (i - dx) + rnd_11 * dx) * dy)
            * coeff / (i * i);
          sum_coeff += coeff;
        }
        return std::max(0, apow(n / sum_coeff, 1000000 / heat_spots * 1000 / (attenuation + 1000)) - attenuation);
      }

      uint32_t heat_color(int heat) {
        int r = std::min(255, (int) (heat * 255 / 333));
        int g = std::min(255, std::max(0, (int) ((heat - 333) * 255 / 333)));
        int b = std::min(255, std::max(0, (int) ((heat - 667) * 255 / 333)));
        return (r << 16) | (g << 8) | b;
      }

      void start() override {

      }

      //each frame
      void apply(display::Display &it) override {
        unsigned long timer = millis();
        int begin_time = timer > starting_speed ? 1000 : (int) (timer * 1000 / starting_speed);

        // must be signed int
        int periodic_time = (int) (timer % periodicity);
        periodic_time = periodic_time * 1000 / periodicity;
        if(periodic_time > 500)
          periodic_time = 1000 - periodic_time;

        int flame_height = int_lerp(int_lerp(init_flame_height, max_flame_height, begin_time), min_flame_height, periodic_time);
        int heat_spots = int_lerp(int_lerp(init_heat_spots, max_heat_spots, begin_time), min_heat_spots, periodic_time);
        int x_attenuation = int_lerp(int_lerp(init_x_attenuation, min_x_attenuation, begin_time), max_x_attenuation, periodic_time);

        for(int x = 0 ; x < width_; x ++) {
          for(int y = 0 ; y < height_; y ++) {
            int heat = heat_color(noise(x, y, (int) (timer * speed_ / 1000), flame_height, heat_spots, x_attenuation));
            it.draw_pixel_at(x,y,Color(heat));
          }
        }
         //unsigned long timer2 = millis();
         //ESP_LOGD(TAG, "draw time: %lu" , (timer2-timer));
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

      void apply(display::Display &it) override {
        //TODO
      }
      void set_speed(uint32_t speed) { this->speed_ = speed; }
      void set_width(uint16_t width) { this->width_ = width; }

    protected:
      uint32_t speed_{10};
      uint16_t width_{50};
  };

  // Thanks to Liemmerle for the bubble effect algorythm <3
  class DisplayBubblesEffect : public DisplayEffect {
    public:
      const char *TAG = "zilloscope.displaybubbleseffect";
      explicit DisplayBubblesEffect(const std::string &name) : DisplayEffect(name) {}

      int rnd(int x, int y, int z) {
          int X = x ^ 1273419445;
          int Y = y ^ 897982756;
          int Z = z ^ 453454122;
          int tmp = ((
            X * 315132157
            + Y * 1325782542
            + Z * 351213151)
            ^ 351356521);
          return tmp ^ (tmp >> 8) ^ (tmp << 8);
        }

      int apow(int a, int b) {
          return 1000 + (a - 1000) * b / 1000;
        }

      int repeat(int X, int r) {
          int tmp = X % (2 * r);
          if(tmp > r) {
            return 2 * r - tmp;
          }
          return tmp;
        }

      int sqr_dist(int x0, int y0, int x1, int y1) {
          return (x0 - x1) * (x0 - x1) + (y0 - y1) * (y0 - y1);
        }

      uint32_t color_bubble (int x, int y, int nx, int ny, int radius) {
          int dist = 0;
          for(int x0 = x-1 ; x0 <= x+1 ; x0++){
            for(int y0 = y-1 ; y0 <= y+1 ; y0++){
              dist += sqr_dist(x0, y0, nx, ny);
            }
          }
          dist /= 9;
          int coeff = dist * 1024 / (radius == 0 ? 1 : radius * radius);
          return (uint32_t) (std::min(255, std::max(0, apow(coeff, 1200) / 3))) * 0x010101;
        }

      int noise(int X, int Y, int T) {
          uint32_t n = background_;
          for(int i = biggest_bubble_ ; i >= smallest_bubble_ ; i -= 4) {
            if(i == 0)
              i = 1;

            bool bkg = false;

            int nx = 0;
            int ny = 0;
            int bdist = 1 << 30;
            int y = Y + T * i / 1000;
            int x = X + repeat(T / 1000 + i, 3) - 1;
            int xi = x - (x % i);
            int yi = y - (y % i);
            int crad = i / 2;
            if(crad == 0)
              crad = 1;

            int rad = i / 2;

            for(int x0 = xi - i ; x0 <= xi + i ; x0 += i)
              for(int y0 = yi - i ; y0 <= yi + i ; y0 += i)
                if(x0 / i % 2 == 0 && y0 / i % 2 == 0){
                int xp = (rnd(x0, y0, 0) % crad) + x0;
                int yp = (rnd(x0, y0, 1) % crad) + y0;
                int b = rnd(x0, y0, 3512);
                int trad = i / 2 + ((b & 4) >> 2);
                int dist = sqr_dist(xp, yp, x, y);
                if((b & (128 | 64 | 16 | 512 |2048)) == 0 && dist < bdist && dist <= trad * trad) {
                  bdist = dist;
                  nx = xp;
                  ny = yp;
                  rad = trad;
                  bkg = true;
                }
              }
            n |= !bkg ? 0 : color_bubble(x, y, nx, ny, rad);
          }
          return n | (n >> 8) | (n >> 16);
        }

      void apply(display::Display &it) override {
        unsigned long timer = esphome::millis();
        for(int x = 0 ; x < width_ ; x ++) {
          for(int y = 0 ; y < height_ ; y ++) {
            uint32_t color = (noise(x, y, (int) (timer * speed_ / 10)));
            it.draw_pixel_at(x,y,Color(color));
          }
        }
        //unsigned long timer2 = millis();
        //ESP_LOGD(TAG, "draw time: %lu" , (timer2-timer));
      }

      void set_speed(uint32_t speed) { this->speed_ = speed; }
      void set_width(uint16_t width) { this->width_ = width; }
      void set_height(uint16_t height) { this->height_ = height; }
      void set_background_color(uint32_t background) { this->background_ = background; }
      void set_min_bubble_size(uint8_t smallest_bubble) { this->smallest_bubble_ = smallest_bubble; }
      void set_max_bubble_size(uint8_t biggest_bubble) { this->biggest_bubble_ = biggest_bubble; }

    protected:
      uint32_t background_{0x193291};
      uint32_t speed_{15};
      uint16_t width_{32};
      uint16_t height_{32};
      uint8_t smallest_bubble_{0};
      uint8_t biggest_bubble_{16};
  };

  class DisplaySnowEffect : public DisplayEffect {
    public:
      const char *TAG = "zilloscope.displaysnoweffect";
      explicit DisplaySnowEffect(const std::string &name) : DisplayEffect(name) {}

      uint32_t santa_claus_colors[8] = {0xffffff, 0xc92464, 0x393457, 0xf7e476, 0xf7b69e, 0xf99252, 0x9b9c82};
      int santa_x = 100000;
      int santa_y = 100000;
      int santa[9][11]= {
        {-1,-1,-1, 0, 1,-1,-1,-1,-1,-1},
        {-1,-1,-1,-1, 1, 1,-1,-1,-1,-1},
        {-1, 3, 3,-1, 4, 4,-1,-1,-1,-1},
        { 3, 1, 1, 5, 1, 2, 4, 3, 3,-1},
        { 5, 2, 1, 5, 1, 2, 3, 1, 2, 5},
        {-1, 5, 2, 1, 5, 5, 1, 2, 5,-1},
        {-1,-1, 2, 2, 1, 1, 2, 5,-1,-1},
        {-1,-1,-1, 2, 2, 2, 2,-1,-1,-1}
      };
      int reindeer[9][9]  = {
        {-1,-1,-1, 1,-1, 1, 1,-1},
        {-1,-1,-1,-1, 1,-1, 1,-1},
        {-1,-1,-1,-1,-1, 5, 5,-1},
        {-1,-1,-1,-1, 4, 4, 6, 1},
        { 5, 5, 5, 5, 4, 6,-1,-1},
        { 6, 6, 6, 6, 6,-1,-1,-1},
        { 6,-1,-1,-1, 6,-1,-1,-1},
        {-1, 6,-1, 6,-1,-1,-1,-1}
      };

      int rnd(int x, int y, int z) {
          int X = x ^ 1273419445;
          int Y = y ^ 897982756;
          int Z = z ^ 453454122;
          int tmp = ((
            X * 315132157
            + Y * 1325782542
            + Z * 351213151)
            ^ 351356521);
          return tmp ^ (tmp >> 8) ^ (tmp << 8);
        }

      int apow(int a, int b) {
          return 1000 + (a - 1000) * b / 1000;
        }

      int repeat(int X, int r) {
          int tmp = X % (2 * r);
          if(tmp > r) {
            return 2 * r - tmp;
          }
          return tmp;
        }

      int sqr_dist(int x0, int y0, int x1, int y1) {
          return (x0 - x1) * (x0 - x1) + (y0 - y1) * (y0 - y1);
        }

      uint32_t color_snowflake (int x, int y, int nx, int ny, int radius, int variant) {
        int coeff = 0;

        int dx = abs(x - nx);
        int dy = abs(y - ny);

        if((variant & 1) != 0) {
          dx ^= dy;
          dy ^= dx;
          dx ^= dy;
        }

        if((radius <= 1 || dy < radius)
          && ( dx == 0 || dx - 1 == dy))
          coeff = 255;

        return (uint32_t) coeff * 0x010101;
      }

      int noise(int X, int Y, int T) {
        uint32_t n = background_;
        for(int i = biggest_snowflake_ ; i >= smallest_snowflake_ ; i -= 2) {
          if(i == 0)
            i = 1;

          bool bkg = false;
          int variant = 0;
          int nx = 0;
          int ny = 0;
          int bdist = 1 << 30;
          int y = Y - T * i / 1000;  //modif snow
          int x = X + repeat(T / 1000 + i, 3) - 1;
          int xi = x - (x % i);
          int yi = y - (y % i);
          int crad = i / 2;
          if(crad == 0)
            crad = 1;
          int rad = i / 2;

          for(int x0 = xi - i ; x0 <= xi + i ; x0 += i)
            for(int y0 = yi - i ; y0 <= yi + i ; y0 += i)
              if(x0 / i % 2 == 0 && y0 / i % 2 == 0){
              int xp = (rnd(x0, y0, 0) % crad) + x0;
              int yp = (rnd(x0, y0, 1) % crad) + y0;
              int b = rnd(x0, y0, 3512);
              int trad = i / 2 + ((b & 4) >> 2);
              int dist = sqr_dist(xp, yp, x, y);
              if((b & (128 | 64 | 16 | 512 |2048)) == 0 && dist < bdist && dist <= trad * trad) {
                variant = (b & 1024) >> 10;
                bdist = dist;
                nx = xp;
                ny = yp;
                rad = trad;
                bkg = true;
              }
            }
          n |= !bkg ? 0 : color_snowflake(x, y, nx, ny, rad, variant);
        }
        return n | (n >> 8) | (n >> 16);
      }

      void apply(display::Display &it) override {

        unsigned long timer = esphome::millis();

        //Draw the snowflakes
        for(int x = 0 ; x < width_ ; x ++) {
          for(int y = 0 ; y < height_ ; y ++) {
            uint32_t color = (noise(x, y, (int) (timer * speed_ / 10)));
            it.draw_pixel_at(x,y,Color(color));
          }
        }

        #ifdef LOGPERF
          unsigned long timer2 = millis();
          ESP_LOGD(TAG, "draw time flakes: %lu" , (timer2-timer));
        #endif

        //Calculate santa pos
        santa_x += santa_speed_ * 1000 / 60;
        if (santa_x > width_ * 1000) {
          santa_x = -1000 * (reindeer_number_ * 6 + 10) - 50 - (int) (random_uint32() % (10000 - 0) + 0);
          santa_y = (int) (random_uint32() % height_);
        }

        //Draw reindeers
        for(int n = 0 ; n < reindeer_number_ ; n ++) {
          //for(int y = 0 ; y < reindeer.length ; y ++)
          for(int y = 0 ; y < 8 ; y ++) {
            //for(int x = 0 ; x < reindeer[y].length ; x ++) {
            for(int x = 0 ; x < 8; x ++) {
              int X = x + santa_x / 1000 + 4 + reindeer_number_ * 6 - n * 6;
              int Y = y + santa_y + n % 2;
              if(X >= 0 && X < width_ && Y >= 0 && Y < height_ && reindeer[y][x] != -1) {
                uint32_t col = santa_claus_colors[reindeer[y][x]];
                it.draw_pixel_at(X,Y, Color(col));
              }
            }
          }
        }

        //Draw Santa
        //for(int y = 0 ; y < santa.length ; y ++)
        for(int y = 0 ; y < 8 ; y ++) {
          //for(int x = 0 ; x < santa[y].length ; x ++) {
          for(int x = 0 ; x < 10 ; x ++) {
            int X = x + santa_x / 1000;
            int Y = y + santa_y;
            if(X >= 0 && X < width_ && Y >= 0 && Y < height_ && santa[y][x] != -1) {
              uint32_t col = santa_claus_colors[santa[y][x]];
              it.draw_pixel_at(X,Y, Color(col));
            }
          }
        }
        #ifdef LOGPERF
          unsigned long timer3 = millis();
          ESP_LOGD(TAG, "draw time santa+deers: %lu" , (timer3-timer2));
        #endif
      }

      void set_snow_speed(uint16_t speed) { this->speed_ = speed; }
      void set_santa_speed(uint16_t speed) { this->santa_speed_ = speed; }
      void set_reindeer_number(uint16_t number) { this->reindeer_number_ = number; }
      void set_width(uint16_t width) { this->width_ = width; }
      void set_height(uint16_t height) { this->height_ = height; }
      void set_background_color(uint32_t background) { this->background_ = background; }
      void set_min_snowflake_size(uint16_t smallest_snowflake) { this->smallest_snowflake_ = smallest_snowflake; }
      void set_max_snowflake_size(uint16_t biggest_snowflake) { this->biggest_snowflake_ = biggest_snowflake; }

    protected:
      uint32_t background_{0x090f2f};
      uint16_t reindeer_number_{3};
      uint16_t santa_speed_{15};
      uint16_t speed_{15};
      uint16_t width_{32};
      uint16_t height_{32};
      uint16_t smallest_snowflake_{1};
      uint16_t biggest_snowflake_{6};
  };

  // Thanks to Liemmerle for the matrix animation code
  class DisplayMatrixEffect : public DisplayEffect
  {
  public:
    const char *TAG = "zilloscope.displayeffect";
    explicit DisplayMatrixEffect(const std::string &name) : DisplayEffect(name) {}

    int rnd(int x, int y, int z)
    {
      int X = x ^ 1273419445;
      int Y = y ^ 897982756;
      int Z = z ^ 165135135;
      int tmp = ((
                     X * 315132157 + Y * 1325782542 + Z * 315132189) ^
                 351356521);
      if (tmp < 0)
        tmp = -tmp;
      return tmp ^ (tmp >> 8) ^ (tmp << 8);
    }

    int apow(int a, int b)
    {
      return 1000 + (a - 1000) * b / 1000;
    }

    int noise(int X, int Y, int T)
    {

      int n = background_;

      Y *= -1;

      for (int i = smallest_line_; i <= biggest_line_; i += line_iterator_)
      {
        if (i == 0)
          i = 1;

        int ny = 0;
        bool bkg = true;

        int y = Y + T * i / 1000;
        int x = X;

        int yi = y - (y % i);

        for (int y0 = yi; y0 <= yi + i * 2; y0 += i)
        {
          int yp = (rnd(x, y0, 0) % i) + y0;

          int random = rnd(x, y0, 1) & (64 | 2048 | 4096);

          if (random == 0 && yp >= y && yp - y < i)
          {
            ny = yp;
            bkg = false;
            break;
          }
        }

        if (!bkg)
          n = color_line(y, ny, i);
      }

      return n;
    }

  void apply(display::Display &it) override {
    unsigned long timer = millis();
    for(int x = 0 ; x < width_ ; x ++) {
      for(int y = 0 ; y < height_ ; y ++) {

        uint32_t color = (noise(x, y, (int) (timer * speed_ / 10)));
        it.draw_pixel_at(x,y,Color(color));
      }
    }
    //unsigned long timer2 = millis();
    //ESP_LOGD(TAG, "draw time: %lu" , (timer2-timer));
  }

  uint32_t color_line(int y, int ny, int size)
  {
    int dist = ny - y;

    int r = std::max(0, std::min(255, 200 * apow(2716, (dist - size) * 1000 / size) / 1000));
    int g = std::max(0, std::min(255, 255 * apow(2716, (dist - size) * 500 / size) / 1000));
    int b = std::max(0, std::min(255, 200 * apow(2716, (dist - size) * 1000 / size) / 1000));

    return (r << 16) + (g << 8) + b;
    }

    void set_speed(uint32_t speed) { this->speed_ = speed; }
    void set_width(uint16_t width) { this->width_ = width; }
    void set_height(uint16_t height) { this->height_ = height; }
    void set_background(uint32_t background) { this->background_ = background; }
    void set_smallest_line(uint8_t smallest_line) { this->smallest_line_ = smallest_line; }
    void set_biggest_line(uint8_t biggest_line) { this->biggest_line_ = biggest_line; }

  protected:
    uint32_t background_{0x011902};
    uint32_t speed_{15};
    uint16_t width_{32};
    uint16_t height_{32};
    uint8_t smallest_line_{8};
    uint8_t biggest_line_{32};
    uint8_t line_iterator_{8};
  };

  class DisplayTiledPuzzleEffect : public DisplayEffect {
    public:
      const char *TAG = "zilloscope.displaytilepuzzleseffect";
      explicit DisplayTiledPuzzleEffect(const std::string &name) : DisplayEffect(name) {
      }
      long random(long howbig)
      {
          if(howbig == 0) {
              return 0;
          }
          return esphome::random_uint32() % howbig;
      }
      //random uint8_t from 0 to 128
      uint8_t rnd128() {
        return random(128);
        //return fast_random_8() >> 1;
      }

      void start() override {

        //fast_random_set_seed(random_uint32());
        for(x_pos = 0 ; x_pos < width_ ; x_pos+=tile_size_) {
          for(y_pos = 0 ; y_pos < height_ ; y_pos+=tile_size_) {
            uint8_t border_red=rnd128()+127;
            uint8_t border_green=rnd128()+127;
            uint8_t border_blue=rnd128()+127;
            uint8_t fill_red=border_red-50;
            uint8_t fill_green=border_green-50;
            uint8_t fill_blue=border_blue-50;

            fill_color= (fill_red << 16) | (fill_green << 8) | fill_blue;
            border_color= (border_red << 16) | (border_green << 8) | border_blue;

            display_->filled_rectangle(x_pos, y_pos, x_pos+tile_size_-1, y_pos+tile_size_-1, Color(fill_color));

            //if the tile is sized 3 or more place a border around
            if(tile_size_ > 2) {
              display_->rectangle(x_pos, y_pos, x_pos+tile_size_-1, y_pos+tile_size_-1, Color(border_color));
            }
          }
        }

        //set the end and actual position to the same value to get into tile select at the first run
        x_end = x_pos;
        y_end = y_pos;
        x_old = tile_size_ * random(width_/tile_size_-1);
        y_old = tile_size_ * random(height_/tile_size_-1);

        //draw black rectangle to erase one tile
        display_->filled_rectangle(x_old, y_old, x_old+tile_size_-1, y_old+tile_size_-1, Color(0x000000));
      }

      void newtile() {
        while(x_pos == x_end && y_pos == y_end) {
          x_pos = x_old;
          y_pos = y_old;
          uint8_t rnd_3=random(3);
          if(rnd_3 == 0) {
            if(x_old > 0) {
              x_pos=x_old-tile_size_;
            }
            else {
              x_pos=x_old+tile_size_;
            }
          }
          else if(rnd_3 == 1){
            if(x_old < width_-tile_size_) {
              x_pos=x_old+tile_size_;
            }
            else {
              x_pos=x_old-tile_size_;
            }
          }
          else if(rnd_3 == 2) {
            if(y_old > 0) {
              y_pos=y_old-tile_size_;
            }
            else {
              y_pos=y_old+tile_size_;
            }
          }
          else if(rnd_3 == 3) {
            if(y_old < height_-tile_size_) {
              y_pos=y_old+tile_size_;
            }
            else{
              y_pos=y_old-tile_size_;
            }
          }
        }

        x_end = x_old;
        y_end = y_old;
        x_old = x_pos;
        y_old = y_pos;

        //#get color of the tile we want to move
        uint8_t border_red=0;
        uint8_t border_green=0;
        uint8_t border_blue=0;

        //pget(x_pos, y_pos, border_red, border_green, border_blue);
        //TODO: how to read the displaybuffer ?

        uint8_t fill_red=border_red-50;
        uint8_t fill_green=border_green-50;
        uint8_t fill_blue=border_blue-50;

        fill_color= (fill_red << 16) | (fill_green << 8) | fill_blue;
        border_color= (border_red << 16) | (border_green << 8) | border_blue;
      }

      void apply(display::Display &it) override {
        unsigned long timer = millis();

        //select new tile to move
        if((x_pos == x_end) && (y_pos == y_end)) {
          newtile();
        }

        //lets move the tile

        //delete the moved part on the old position
        if(x_pos < x_end) {
          it.line(x_pos, y_pos, x_pos, y_pos+tile_size_-1, Color(0x000000));
          x_pos=x_pos+1;
        }
        else if(x_pos > x_end) {
          it.line(x_pos+tile_size_-1, y_pos, x_pos+tile_size_-1, y_pos+tile_size_-1, Color(0x000000));
          x_pos=x_pos-1;
        }

        if(y_pos < y_end) {
          it.line(x_pos, y_pos, x_pos+tile_size_-1, y_pos, Color(0x000000));
          y_pos++;
        }
        else if(y_pos > y_end) {
          it.line(x_pos, y_pos+tile_size_-1, x_pos+tile_size_-1, y_pos+tile_size_-1, Color(0x000000));
          y_pos--;
        }

        //and draw the tile on the new position
        it.filled_rectangle(x_pos, y_pos, x_pos+tile_size_-1, y_pos+tile_size_-1, Color(fill_color));

        //if the tile is sized 3 or more place a corner around
        if(tile_size_ > 2) {
          it.rectangle(x_pos, y_pos, x_pos+tile_size_-1, y_pos+tile_size_-1, Color(border_color));
        }

        unsigned long timer2 = millis();
        ESP_LOGD(TAG, "draw time: %lu" , (timer2-timer));
      }

      void set_tile_size(uint32_t tile_size) { this->tile_size_ = tile_size; }
      void set_speed(uint32_t speed) { this->speed_ = speed; }
      void set_width(uint16_t width) { this->width_ = width; }
      void set_height(uint16_t height) { this->height_ = height; }

    protected:
      uint32_t x_pos=0;
      uint32_t y_pos=0;
      uint32_t x_old=0;
      uint32_t y_old=0;
      uint32_t x_end=0;
      uint32_t y_end=0;
      uint32_t fill_color=0;
      uint32_t border_color=0;

      uint32_t tile_size_{5};
      uint32_t speed_{15};
      uint16_t width_{32};
      uint16_t height_{32};
  };

}
}

