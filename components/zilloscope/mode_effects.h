#pragma once

#include "esphome/core/component.h"
#include "esphome/components/display/display_buffer.h"
#include "display_effect.h"
#include "zilloscope_mode.h"

namespace esphome {
namespace zilloscope {
  class ModeEffects: public zilloscope::Mode {
    public:

      ModeEffects(const std::string &name)
          : Mode(name, "ModeEffects") {}
    /*
    TODO: handle effect's interval instal of global 'effects' one
    */

      void add_effects(const std::vector<DisplayEffect *> effects) {
        this->effects_.reserve(this->effects_.size() + effects.size());
        for (auto *effect : effects) {
          this->effects_.push_back(effect);
        }
      }

      void set_default_effect(const std::string &default_effect) {
        uint32_t default_effect_index = get_effect_index(default_effect);
        if(default_effect_index==0) {
          ESP_LOGE("TOTO", "Unknown default effect %s", default_effect.c_str());
          return;
        }
        default_effect_index_=default_effect_index;
      }

      uint32_t get_effect_index(std::string name) {
        for (uint32_t i=0;i<effects_.size();i++) {
          if(effects_[i]->get_name()==name) {
            return i+1;
          }
        }
        return 0;
      }

      DisplayEffect * get_active_effect_() {
      if (this->active_effect_index_ == 0)
          return nullptr;
        else
          return this->effects_[this->active_effect_index_ - 1];
      }

      std::string get_effect_name() {
        if (this->active_effect_index_ > 0)
          return this->effects_[this->active_effect_index_ - 1]->get_name();
        else
          return "None";
      }

      virtual void  start_internal() override {
        ESP_LOGD("TOTO", "ModeEffects %s's StartInternal", get_name().c_str());
        //ESP_LOGD("TOTO", "active effect index %d", active_effect_index_);
        if(active_effect_index_==0) {
          if(default_effect_index_>0) {
            ESP_LOGD("TOTO", "defaulting to default effect index %d", active_effect_index_);
            active_effect_index_ = default_effect_index_;
          } else {
            ESP_LOGD("TOTO", "defaulting to first effect");
            active_effect_index_=1;
          }
        }
        start_effect_(active_effect_index_);
      }

      virtual void draw(display::DisplayBuffer &it) override {
        //ESP_LOGD("TOTO", "ModeEffects %s's Draw", get_name().c_str());
        DisplayEffect *effect = get_active_effect_();
        if(effect==nullptr)
          return;
        //use effect's apply method
        effect->apply(it);
      }

      void start_effect_(uint32_t effect_index) {
        this->stop_effect_();
        if (effect_index == 0)
          return;
        this->active_effect_index_ = effect_index;
        auto *effect = this->get_active_effect_();
        effect->start_internal();
      }

      void stop_effect_() {
        auto *effect = this->get_active_effect_();
        if (effect != nullptr)
          effect->stop();
        this->active_effect_index_ = 0;
      }

      void next_effect() {
        active_effect_index_++;
        if(active_effect_index_>effects_.size()) {
          active_effect_index_=1;
        }
        auto effect = effects_[active_effect_index_-1];
        ESP_LOGD("ModeEffects", "Starting effect %s (#%d)", effect->get_name().c_str(), active_effect_index_);
        start_effect_(active_effect_index_);
      }

      void prev_effect() {
        active_effect_index_--;
        if(active_effect_index_<=0) {
          active_effect_index_=effects_.size();
        }
        auto effect = effects_[active_effect_index_-1];
        ESP_LOGD("ModeEffects", "Starting effect %s (#%d)", effect->get_name().c_str(), active_effect_index_);
        start_effect_(active_effect_index_);
      }



    protected:

      optional<uint32_t> effect_;
      uint32_t default_effect_index_{0};
      uint32_t active_effect_index_{0};

      std::vector<DisplayEffect *> effects_;

      bool has_effect_() {
        return this->effect_.has_value();
      }
  };

}
}