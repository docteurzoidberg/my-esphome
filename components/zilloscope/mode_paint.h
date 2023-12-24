#pragma once

#include "esphome/core/component.h"
#include "esphome/components/display/display_buffer.h"
#include "esphome/components/web_server_base/web_server_base.h"
#include "zilloscope_mode.h"

namespace esphome {
namespace zilloscope {

struct UrlMatch {
  std::string domain;  ///< The domain of the component, for example "sensor"
  std::string id;      ///< The id of the device that's being accessed, for example "living_room_fan"
  std::string method;  ///< The method that's being called, for example "turn_on"
  bool valid;          ///< Whether this match is valid
};

class ModePaint: public Mode, public Component {
  public:
    const char *TAG = "zilloscope.modepaint";
    ModePaint(const std::string &name)
    : Mode(name,"ModePaint") {
      ESP_LOGD(TAG, "mode paint constructor");
      //resize buffer from (width * height *4)
      //todo get width and height from config
       width = 16;
       height = 16;
       init_internal_(width*height*4);
      //try{
        //buffer_.resize(width*height*4, 0);
        //ESP_LOGD(TAG, "vector size: %d", buffer_.size());
      //}
      //catch(std::bad_alloc&) {
        //ESP_LOGE(TAG, "bad alloc for pain buffer");
      //}
    }

    void setup() {
    }

    void loop() {
    }


    void HOT init_internal_(uint32_t buffer_length) {
      ExternalRAMAllocator<uint8_t> allocator(ExternalRAMAllocator<uint8_t>::ALLOW_FAILURE);
      this->buffer_ = allocator.allocate(buffer_length);
      if (this->buffer_ == nullptr) {
        ESP_LOGE(TAG, "Could not allocate buffer for display!");
        return;
      }
      for(int i=0;i<buffer_length;i++) {
        buffer_[i]=0;
      }
      buffer_size_=buffer_length;
    }

    virtual void draw(display::Display &it) override {
      if(is_filling) 
        return;
      
      int w = it.get_width();
      int h = it.get_height();
      //Image::Image img = new Image::Image(buffer_, w, h, ImageType::IMAGE_TYPE_TRANSPARENT_BINARY );

      for(int x=0;x<w;x++) {
        for(int y=0;y<h;y++) {
          //pixel index in 32 bit
          int index = (y*h + x)*4;
          //ESP_LOGD(TAG, "index: %d", index);
          Color color32 = Color();
          color32.r = buffer_[index+0];
          color32.g = buffer_[index+1];
          color32.b = buffer_[index+2];
          color32.w = 0;
          it.draw_pixel_at(x,y,color32);
          //it.draw_pixel_at(x,y,Color::random_color());
        }
      }
    }

    void fill_buffer_start() {
      ESP_LOGD(TAG, "Start filling buffer");
      is_filling = true;
    } 
    
    void fill_buffer_end() {
      ESP_LOGD(TAG, "End filling buffer");
      is_filling = false;
    }

    void fill_uint8_buffer(size_t from, size_t len, uint8_t *data) {
      ESP_LOGD(TAG, "Fill buffer, from: %d, len: %d", from, len);
      if((from+len)>buffer_size_){
        ESP_LOGE(TAG, "buffer overflow, cancelling");
        return;
      }
      for(int i=0; i<len; i++) {
        //ESP_LOGV(TAG, "copying %02x to %d", data[i], from + i);
        buffer_[from + i] = data[i];
      }
    }

  protected:
    bool is_filling = false;
    //std::vector<uint8_t> buffer_;
    uint8_t *buffer_{nullptr};
    uint32_t buffer_size_ = 0;
    uint8_t width = 0;
    uint8_t height = 0;
  };
}
}

