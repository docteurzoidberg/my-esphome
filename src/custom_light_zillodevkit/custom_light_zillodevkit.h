#include "esphome.h"

class MyCustomLightOutput : public Component, public LightOutput {
 public:
  void setup() override {
    // This will be called by App.setup()
    //pinMode(5, INPUT);
  }
  LightTraits get_traits() override {
    // return the traits this light supports
    auto traits = LightTraits();
    traits.set_supports_brightness(true);
    traits.set_supports_rgb(true);
    traits.set_supports_rgb_white_value(false);
    traits.set_supports_color_temperature(false);
    return traits;
  }

  void write_state(LightState *state) override {
    // This will be called by the light to get a new state to be written.
    float red, green, blue;
    bool state_is_on;
    // use any of the provided current_values methods
    state->current_values_as_rgb(&red, &green, &blue);
    state->current_values_as_binary(&state_is_on);
    // Write red, green and blue to HW
    // ...
  }
};