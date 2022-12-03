#pragma once

#include "esphome/core/automation.h"
#include "esphome/core/component.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/homeassistant/text_sensor/homeassistant_text_sensor.h"
#include <string>

#define STATE_BOOTING 0
#define STATE_DISPLAY 1

namespace esphome {
  namespace plantiquette {
    class Plantiquette : public Component {
    public:

      void setup() override;
      void loop() override;
      void dump_config() override;

      void add_on_ready_callback(std::function<void()> callback) {
        this->on_ready_callback_.add(std::move(callback));
      }

      uint8_t get_state();
      void set_tag1(homeassistant::HomeassistantTextSensor *tag1);
      void set_tag2(homeassistant::HomeassistantTextSensor *tag2);

    protected:
      CallbackManager<void()> on_ready_callback_;
    };

    class ReadyTrigger : public Trigger<> {
    public:
      explicit ReadyTrigger(Plantiquette *parent) {
        parent->add_on_ready_callback([this]() { this->trigger(); });
      }
    };
  }  // namespace plantiquette
}  // namespace esphome