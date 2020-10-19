#include "esphome.h"
#include "config.h"
#include <SD.h>
#include <SPIFFS.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Ticker.h>

#include <ESP8266SAM.h>
#include <AudioOutputI2S.h>


using namespace esphome;

static const char *TAG = "twatch";

class TWatch : public Component, CustomAPIDevice  {
  private:
    AudioOutputI2S *out = NULL;
    ESP8266SAM *sam = NULL;
  public:
    TWatch() : Component() {
    }
    void on_hello_world() {
      ESP_LOGD("TWatch", "Hello World!");
      sam = new ESP8266SAM;
      sam->SetVoice(sam->VOICE_SAM);
      sam->Say(out, "Hello world!");
      delay(500);
      delete sam;
    }
    void setup() override {
      ESP_LOGCONFIG(TAG, "Setting up ...");
      out = new AudioOutputI2S();
      out->SetPinout( 26, 25, 33 );
      out->SetGain(3.5f);
      out->begin();
      register_service(&TWatch::on_hello_world, "hello_world");
    }
    void loop() override {
      //if not connected, scan and try to connect
    }
};