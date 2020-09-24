#include "esphome.h"

#include <SPI.h>
#include <LoRa.h>

#include <dzb/CRC.h>
#include <dzb/Packet.h>
#include <dzb/PacketType.h>
#include <dzb/PacketWriter.h>
#include <dzb/PacketReader.h>
#include <dzb/Serialization.h>

#include "myoleddisplay.h"
#include "structures.h"


using namespace esphome;

static const char *TAG = "sensor.lora_receiver";
static const char* host = "lora-station1";

#define  PIN_SCK    5
#define  PIN_MISO   19
#define  PIN_MOSI   27
#define  PIN_SS     18
#define  PIN_RST    14
#define  PIN_DI00   26
#define  BAND       433E6
#define  PABOOST    true

MyOledDisplay 	display(0x3c, 4, 15);

uint16_t 		packetCounter=0;

volatile uint32_t lastPacketReceived=0;
volatile uint32_t lastScreenUpdate=0;

static DeviceJ7DataStruct state = {
  .header="J7",
  .timer=0,
  .deviceBattVoltage=0.0f,
  .deviceBattPercent=0,
  .devicePirState=false,
  .deviceAlarmActive=false,
  .deviceLowBatt=false
};

class LoraReceiver : public Component {
  private:

    void parse_packet(dzb::Packet const& packet) {

      if (!packet.is_crc_valid()) {
        // TODO: handle error
        Serial.println("[PACKET] CRC error");
        return;
      }

      dzb::PacketReader reader(packet);

      char devicestring[3] = "  ";
      //HELP :D

      //ONE DEVICE: J7
      if(packet.is_from_device("J7")) {

        if(reader.get_value(dzb::PacketType::TIMESTAMP, 0, state.timer)) {

        }
        else {
          Serial.printf("[PACKET][FROM:%s][ERROR] Could not get TIMESTAMP\n", devicestring);
        }

        if(reader.get_value(dzb::PacketType::PRESENCE, 0, state.devicePirState)) {
          device_J7_pir_state->publish_state(state.devicePirState);
        }
        else {
          Serial.printf("[PACKET][FROM:%s][ERROR] Could not get PRESENCE\n", devicestring);
        }

        if(reader.get_value(dzb::PacketType::ALARM, 0, state.deviceAlarmActive)) {
          device_J7_alarm_isactive->publish_state(state.deviceAlarmActive);
        }
        else {
          Serial.printf("[PACKET][FROM:%s][ERROR] Could not get ALARM\n", devicestring);
        }

        if(reader.get_value(dzb::PacketType::BATT_LOW, 0, state.deviceLowBatt)) {
          device_J7_batt_islow->publish_state(state.deviceLowBatt);
        }
        else {
          Serial.printf("[PACKET][FROM:%s][ERROR] Could not get BATT_LOW\n" , devicestring);
        }

        if(reader.get_value(dzb::PacketType::BATT_PERCENT, 0, state.deviceBattPercent)) {
          device_J7_batt_percent->publish_state(state.deviceBattPercent);
        }
        else {
          Serial.printf("[PACKET][FROM:%s][ERROR] Could not get BATT_PERCENT\n", devicestring);
        }

        if(reader.get_value(dzb::PacketType::BATT_VOLTAGE, 0, state.deviceBattVoltage)) {
          device_J7_batt_voltage->publish_state(state.deviceBattVoltage);
        }
        else {
          Serial.printf("[PACKET][FROM:%s][ERROR] Could not get BATT_VOLTAGE\n", devicestring);
        }

        Serial.printf("[PACKET][FROM:%s][DATA] Alarm=%d|LowBatt=%d|BattVoltage=%0.2f|BattPercent=%d|Presence=%d\n", devicestring, state.deviceAlarmActive, state.deviceLowBatt, state.deviceBattVoltage, state.deviceBattPercent, state.devicePirState);
      }
      //ANOTHER DEVICE
      else if(packet.is_from_device("XX")) {
        //another device
        Serial.printf("[PACKET][FROM:%s][DATA] data...\n", devicestring);
      }
      //UNKNOWN DEVICE
      else {
        Serial.printf("[PACKET][FROM:%s][ERROR] Unknown device %s\n", devicestring, devicestring);
      }

    }

    void on_lora_packet_received(int size) {

      std::vector<uint8_t> buffer(size);

      int readsize = LoRa.readBytes(buffer.data(), size);
      if(size!=readsize) {
        Serial.println("[LORA] Received packet size mismatch");
        return;
      }

      packetCounter++;
      lastPacketReceived=millis();
      //print_buffer(buffer.data(), size);

      Serial.printf("[LORA] Received packet %d of %d bytes with RSSI %d\n", packetCounter, size, LoRa.packetRssi());

      auto packet = dzb::Packet::deconstruct(buffer.data(), size);
      parse_packet(packet);
    }

  public:
    sensor::Sensor *device_J7_batt_voltage = new sensor::Sensor();
    sensor::Sensor *device_J7_batt_percent = new sensor::Sensor();
    sensor::Sensor *device_J7_batt_islow = new sensor::Sensor();
    sensor::Sensor *device_J7_pir_state = new sensor::Sensor();
    sensor::Sensor *device_J7_alarm_isactive = new sensor::Sensor();

    LoraReceiver() : Component() {
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
        ESP_LOGCONFIG(TAG, "[LORA] Starting LoRa failed!");
        while (1) {
          delay(1000);
        }
      }

      ESP_LOGCONFIG(TAG, "[LORA] Started!");

      dzb::init_packet_type_meta();
      dzb::init_crc_table();

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
      device_J7_batt_voltage->clear_filters();
      device_J7_batt_percent->clear_filters();
      device_J7_batt_islow->clear_filters();
      device_J7_pir_state->clear_filters();
      device_J7_alarm_isactive->clear_filters();

      //LoRa.onReceive(on_lora_packet_received);
      //LoRa.receive();
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
      int packetSize = LoRa.parsePacket();
      if(packetSize){
        on_lora_packet_received(packetSize);
      }
    }
};