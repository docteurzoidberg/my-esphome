#include "esphome.h"

#include <SPI.h>
#include <LoRa.h>

#include "myoleddisplay.h"
#include "structures.h"

using namespace esphome;

static const char *TAG = "sensor.ids_lora_receiver";
static const char* host = "j7-ids";

#define  PIN_SCK    5
#define  PIN_MISO   19
#define  PIN_MOSI   27
#define  PIN_SS     18
#define  PIN_RST    14
#define  PIN_DI00   26
#define  BAND       433E6
#define  PABOOST    true

MyOledDisplay 	display(0x3c, 4, 15);

uint16_t packetCounter=0;
uint32_t lastPacketReceived=0;
uint32_t lastGatewayAliveReport = 0;
uint32_t lastScreenUpdate=0;


class IDSLoraReceiver : public Component, public Sensor {

  private:

    DeviceDataStruct state = {
      .header="J7",
      .timer=0,
      .deviceBattVoltage=0.0f,
      .deviceBattPercent=0,
      .devicePirState=false,
      .deviceAlarmActive=false,
      .deviceLowBatt=false
    };

    DeviceDataStruct newState;

  public:

    Sensor *ids_device_batt_voltage = new Sensor();
    Sensor *ids_device_batt_percent = new Sensor();
    Sensor *ids_device_batt_islow = new Sensor();
    Sensor *ids_device_pir_state = new Sensor();
    Sensor *ids_device_alarm_isactive = new Sensor();

    IDSLoraReceiver() : Component() {
        //this->pin = new GPIOPin(pin, INPUT);
        //this->factor = 3600000.0f / pulses_per_kwh;
    }

    void handleRadioData(){

      // read packet(s)
      if (!LoRa.available()) {
        return;
      }

      int readBytes = LoRa.readBytes((uint8_t*) &newState, sizeof(DeviceDataStruct));
      if(readBytes!=sizeof(DeviceDataStruct)){
        ESP_LOGD(TAG, "Invalid packet size: %d", readBytes);
        return;
      }
      if(strcmp(newState.header,"J7")!=0){
        ESP_LOGD(TAG, "Invalid packet header: %s", newState.header);
        return;
      }

      state=newState;
      packetCounter++;
      lastPacketReceived=millis();

      ESP_LOGD(TAG, "Received packet %d with RSSI %d", packetCounter, LoRa.packetRssi());

      ids_device_batt_voltage->publish_state(state.deviceBattVoltage);
      ids_device_batt_percent->publish_state(state.deviceBattPercent);
      ids_device_batt_islow->publish_state(state.deviceLowBatt);
      ids_device_pir_state->publish_state(state.devicePirState);
      ids_device_alarm_isactive->publish_state(state.deviceAlarmActive);

      //TODO: setstate with all sensor values
      //stateRequest();
    }

    void setup() override {

      ESP_LOGCONFIG(TAG, "Setting up ...");

      pinMode(16,OUTPUT);
      pinMode(25,OUTPUT);

      digitalWrite(16, LOW);

      SPI.begin(PIN_SCK,PIN_MISO,PIN_MOSI,PIN_SS);
      LoRa.setPins(PIN_SS,PIN_RST,PIN_DI00);

      if (!LoRa.begin(BAND))
      {
        ESP_LOGCONFIG(TAG, "Starting LoRa failed!");
        while (1) {
          delay(1000);
        }
      }

      ESP_LOGCONFIG(TAG, "LoRa initial success!");

      delay(50);
      digitalWrite(16, HIGH);
      display.init();
      display.flipScreenVertically();
      display.setFont(ArialMT_Plain_10);
      display.drawLogoFrame();
      display.display();
      delay(1500);
      display.drawMainFrame(&state, 0, true, 0);
      display.display();

      //clear filters???
      ids_device_batt_voltage->clear_filters();
      ids_device_batt_percent->clear_filters();
      ids_device_batt_islow->clear_filters();
      ids_device_pir_state->clear_filters();
      ids_device_alarm_isactive->clear_filters();
    }

    void loop() override {

      //display buffer update

      //TODO: better way to get wifi connection status from esphome?
      display.drawMainFrame(&state, packetCounter, WiFi.status()==WL_CONNECTED, lastPacketReceived);

      //send 'gateway is alive' every minute
      //if(((millis()-lastGatewayAliveReport)>(60*1000)) || (lastGatewayAliveReport==0)) {
      //  //TODO: setstate with timestamp to see current status of the gateway sensor
      //  lastGatewayAliveReport=millis();
      //  return;
      //}

      //refresh screen @24 fps
      if((millis()-lastScreenUpdate)>(1000/24)){
        display.display();
        lastScreenUpdate=millis();
        return;
      }

      //check incoming lora message
      int packetSize = LoRa.parsePacket(sizeof(DeviceDataStruct));
      if(packetSize){
        handleRadioData();
      }
    }
};