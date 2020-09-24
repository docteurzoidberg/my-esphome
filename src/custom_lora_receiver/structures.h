#ifndef __STRUCTURES_H
#define __STRUCTURES_H

struct DeviceJ7DataStruct {
	char 				header[3];
	uint64_t		timer;
	float     	deviceBattVoltage;
	uint8_t     deviceBattPercent;
	bool        devicePirState;
	bool        deviceAlarmActive;
	bool        deviceLowBatt;
};

#endif