#ifndef __STRUCTURES_H
#define __STRUCTURES_H

struct DeviceJ7DataStruct {
  char        header[3];
  uint64_t    timer;
  float       deviceBattVoltage;
  uint8_t     deviceBattPercent;
  bool        devicePirState;
  bool        deviceAlarmActive;
  bool        deviceLowBatt;
};

struct DevicePTDataStruct {
  float     	deviceBattVoltage;
  uint8_t     deviceBattPercent;
  uint16_t    deviceWaterLevel;
  bool        deviceLowBatt;
};

struct DeviceSoilSensorDataStruct {
  float       deviceBattVoltage;
  uint8_t     deviceBattPercent;
  uint16_t    deviceSoilMoistureLevel;
  bool        deviceLowBatt;
};

#endif