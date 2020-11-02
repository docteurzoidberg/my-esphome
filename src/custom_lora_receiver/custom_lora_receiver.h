#include "esphome.h"

#include <SPI.h>
#include <LoRa.h>

#include <dzb/CRC.h>
#include <dzb/Packet.h>
#include <dzb/PacketType.h>
#include <dzb/PacketWriter.h>
#include <dzb/PacketReader.h>
#include <dzb/Serialization.h>

#include "structures.h"

using namespace esphome;

static const char *TAG = "[LORA]";
static const char* host = "lora-station1";

#define  PIN_SCK    5
#define  PIN_MISO   19
#define  PIN_MOSI   27
#define  PIN_SS     18
#define  PIN_RST    14
#define  PIN_DI00   26
#define  BAND       433E6
#define  PABOOST    true

//MyOledDisplay 	display(0x3c, 4, 15);

uint16_t packetCounter=0;

//timers
volatile uint32_t lastPacketReceived=0;
//volatile uint32_t lastScreenUpdate=0;

static DeviceJ7DataStruct state = {
  .header="J7",
  .timer=0,
  .deviceBattVoltage=0.0f,
  .deviceBattPercent=0,
  .devicePirState=false,
  .deviceAlarmActive=false,
  .deviceLowBatt=false
};

static DevicePTDataStruct pt_state = {
  .deviceBattVoltage=0.0f,
  .deviceBattPercent=0,
  .deviceWaterLevel=0,
  .deviceLowBatt=false
};

class LoraPacketSensor : public Component {
  public:
    TextSensor *last_packet_received = new TextSensor();
};

class LoraReceiver : public Component {
  private:

    char const hex[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

    std::string byte_2_str(unsigned char* bytes, int size) {
      std::string str;
      for (int i = 0; i < size; ++i) {
        const unsigned char ch = bytes[i];
        str.append(&hex[(ch  & 0xF0) >> 4], 1);
        str.append(&hex[ch & 0xF], 1);
      }
      return str;
    }

    void parse_packet(dzb::Packet const& packet) {

      if (!packet.is_crc_valid()) {
        // TODO: handle error
        ESP_LOGD(TAG, "[PACKET] CRC error");
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
          ESP_LOGD(TAG, "[PACKET][FROM:%s][ERROR] Could not get TIMESTAMP\n", devicestring);
        }

        if(reader.get_value(dzb::PacketType::PRESENCE, 0, state.devicePirState)) {
          device_J7_pir_state->publish_state(state.devicePirState);
        }
        else {
          ESP_LOGD(TAG, "[PACKET][FROM:%s][ERROR] Could not get PRESENCE\n", devicestring);
        }

        if(reader.get_value(dzb::PacketType::ALARM, 0, state.deviceAlarmActive)) {
          device_J7_alarm_isactive->publish_state(state.deviceAlarmActive);
        }
        else {
          ESP_LOGD(TAG, "[PACKET][FROM:%s][ERROR] Could not get ALARM\n", devicestring);
        }

        if(reader.get_value(dzb::PacketType::BATT_LOW, 0, state.deviceLowBatt)) {
          device_J7_batt_islow->publish_state(state.deviceLowBatt);
        }
        else {
          ESP_LOGD(TAG, "[PACKET][FROM:%s][ERROR] Could not get BATT_LOW\n" , devicestring);
        }

        if(reader.get_value(dzb::PacketType::BATT_PERCENT, 0, state.deviceBattPercent)) {
          device_J7_batt_percent->publish_state(state.deviceBattPercent);
        }
        else {
          ESP_LOGD(TAG, "[PACKET][FROM:%s][ERROR] Could not get BATT_PERCENT\n", devicestring);
        }

        if(reader.get_value(dzb::PacketType::BATT_VOLTAGE, 0, state.deviceBattVoltage)) {
          device_J7_batt_voltage->publish_state(state.deviceBattVoltage);
        }
        else {
          ESP_LOGD(TAG, "[PACKET][FROM:%s][ERROR] Could not get BATT_VOLTAGE\n", devicestring);
        }

        ESP_LOGD(TAG, "[PACKET][FROM:%s][DATA] Alarm=%d|LowBatt=%d|BattVoltage=%0.2f|BattPercent=%d|Presence=%d\n", devicestring, state.deviceAlarmActive, state.deviceLowBatt, state.deviceBattVoltage, state.deviceBattPercent, state.devicePirState);
      }
      //ANOTHER DEVICE
      else if(packet.is_from_device("PT")) {
        //another device
        ESP_LOGD(TAG, "[PACKET][FROM:%s][DATA] data...\n", devicestring);

        if(reader.get_value(dzb::PacketType::BATT_LOW, 0, pt_state.deviceLowBatt)) {
          device_PT_batt_islow->publish_state(pt_state.deviceLowBatt);
        }
        else {
          ESP_LOGD(TAG, "[PACKET][FROM:%s][ERROR] Could not get BATT_LOW\n" , devicestring);
        }

        if(reader.get_value(dzb::PacketType::BATT_PERCENT, 0, pt_state.deviceBattPercent)) {
          device_PT_batt_percent->publish_state(pt_state.deviceBattPercent);
        }
        else {
          ESP_LOGD(TAG, "[PACKET][FROM:%s][ERROR] Could not get BATT_PERCENT\n", devicestring);
        }

        if(reader.get_value(dzb::PacketType::BATT_VOLTAGE, 0, pt_state.deviceBattVoltage)) {
          device_PT_batt_voltage->publish_state(pt_state.deviceBattVoltage);
        }
        else {
          ESP_LOGD(TAG, "[PACKET][FROM:%s][ERROR] Could not get BATT_VOLTAGE\n", devicestring);
        }

        if(reader.get_value(dzb::PacketType::DISTANCE, 0, pt_state.deviceWaterLevel)) {
          device_PT_water_level->publish_state(pt_state.deviceWaterLevel);
        }
        else {
          ESP_LOGD(TAG, "[PACKET][FROM:%s][ERROR] Could not get WATER LEVEL\n", devicestring);
        }

      }
      //UNKNOWN DEVICE
      else {
        ESP_LOGE(TAG, "[PACKET][FROM:%s][ERROR] Unknown device %s\n", devicestring, devicestring);
      }

    }

    void on_lora_packet_received(int size) {

      std::vector<uint8_t> buffer(size);

      int readsize = LoRa.readBytes(buffer.data(), size);
      unsigned char* bytes = buffer.data();

      if(size!=readsize) {
        ESP_LOGD(TAG, "Received packet size mismatch");
        return;
      }

      packetCounter++;
      lastPacketReceived=millis();

      id(last_packet_received).publish_state(byte_2_str(bytes,readsize));
      //id(mqtt_client).publish("lora/receivedpacket", byte_2_str(bytes,readsize));

      last_packet_rssi->publish_state(LoRa.packetRssi());
      packet_counter->publish_state(packetCounter);

      ESP_LOGD(TAG, "Received packet %d of %d bytes with RSSI %d\n", packetCounter, size, LoRa.packetRssi());

      auto packet = dzb::Packet::deconstruct(bytes, size);
      parse_packet(packet);
    }

  public:
    Sensor *last_packet_rssi = new Sensor();
    Sensor *packet_counter = new Sensor();

    Sensor *device_J7_batt_voltage = new Sensor();
    Sensor *device_J7_batt_percent = new Sensor();
    Sensor *device_J7_batt_islow = new Sensor();
    Sensor *device_J7_pir_state = new Sensor();
    Sensor *device_J7_alarm_isactive = new Sensor();

    Sensor *device_PT_batt_voltage = new Sensor();
    Sensor *device_PT_batt_percent = new Sensor();
    Sensor *device_PT_batt_islow = new Sensor();
    Sensor *device_PT_water_level = new Sensor();

    LoraReceiver() : Component() {
    }

    void setup() override {

      ESP_LOGCONFIG(TAG, "Setting up ...");

      //pinMode(16,OUTPUT);
      //pinMode(25,OUTPUT);
      //digitalWrite(16, LOW);

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
    }

    void loop() override {
      //check incoming lora message
      int packetSize = LoRa.parsePacket();
      if(packetSize){
        on_lora_packet_received(packetSize);
      }
    }
};