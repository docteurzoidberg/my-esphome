#include "esphome.h"
#include "BoostHub.h"
#include "Lpf2HubConst.h"

using namespace esphome;

static const char *TAG = "r2d2";

BoostHub myBoostHub;

class R2D2 : public Component, CustomAPIDevice  {
  private:

  public:
    R2D2() : Component() {
    }
    void on_nonon() {

      ESP_LOGD("r2d2", "Nonon");
      ESP_LOGD("r2d2", "IsConnecting: %d", myBoostHub.isConnecting());
      ESP_LOGD("r2d2", "IsConnected: %d", myBoostHub.isConnected());
      if(!myBoostHub.isConnected()) {
        ESP_LOGD("r2d2", "Not connected");
        myBoostHub.init(5);
        ESP_LOGD("r2d2", "IsConnecting: %d", myBoostHub.isConnecting());
        if(myBoostHub.isConnecting()) {
          ESP_LOGD("r2d2", "Connect hub");
          myBoostHub.connectHub();
        }
        ESP_LOGD("r2d2", "IsConnected: %d", myBoostHub.isConnected());
      }

      //wait to be connected?
      if (myBoostHub.isConnected()) {
        char hubName[] = "DrZoid";
        myBoostHub.setHubName(hubName);
        myBoostHub.setLedColor(LpfLedColor::RED);
        myBoostHub.rotateLeft(90);
        //myBoostHub.setMotorSpeed(BoostHub::Port::D  , 100);
        ///delay(3000);
        myBoostHub.rotateRight(180);
        myBoostHub.rotateLeft(90);
        //myBoostHub.setMotorSpeed(BoostHub::Port::D , 0);
        //myBoostHub.shutDownHub();
      } else {
         ESP_LOGD("r2d2", "still not connected");
      }
    }
    void on_hello_world() {

      ESP_LOGD("r2d2", "Hello World!");

      ESP_LOGD("r2d2", "IsConnecting: %d", myBoostHub.isConnecting());
      ESP_LOGD("r2d2", "IsConnected: %d", myBoostHub.isConnected());

      if(!myBoostHub.isConnected()) {
        ESP_LOGD("r2d2", "Not connected");
        myBoostHub.init(5);
        ESP_LOGD("r2d2", "IsConnecting: %d", myBoostHub.isConnecting());
        if(myBoostHub.isConnecting()) {
          ESP_LOGD("r2d2", "Connect hub");
          myBoostHub.connectHub();
        }
        ESP_LOGD("r2d2", "IsConnected: %d", myBoostHub.isConnected());
      }

      //wait to be connected?
      if (myBoostHub.isConnected()) {
        char hubName[] = "DrZoid";
        myBoostHub.setHubName(hubName);
        myBoostHub.setLedColor(LpfLedColor::BLUE);
        myBoostHub.setMotorSpeed(BoostHub::Port::D  , 100);
        delay(3000);
        myBoostHub.setMotorSpeed(BoostHub::Port::D , 0);
        //myBoostHub.shutDownHub();
      } else {
         ESP_LOGD("r2d2", "still not connected");
      }
    }
    void setup() override {
      ESP_LOGCONFIG(TAG, "Setting up ...");
      register_service(&R2D2::on_hello_world, "hello_world");
      register_service(&R2D2::on_nonon, "non_non");
    }
    void loop() override {
      //if not connected, scan and try to connect
    }
};