#pragma once

#include "esphome/core/automation.h"
#include "esphome/core/component.h"
#include <string>

#define STATE_BOOTING 0
#define STATE_DISPLAY 1

namespace esphome {
  namespace minitel {
    class Minitel : public Component {
    public:

      void setup() override;
      void loop() override;
      void dump_config() override;

      void add_on_ready_callback(std::function<void()> callback) {
        this->on_ready_callback_.add(std::move(callback));
      }

      uint8_t get_state();

    protected:
      CallbackManager<void()> on_ready_callback_;
    };

    class ReadyTrigger : public Trigger<> {
    public:
      explicit ReadyTrigger(Minitel *parent) {
        parent->add_on_ready_callback([this]() { this->trigger(); });
      }
    };
  }  // namespace minitel
}  // namespace esphome