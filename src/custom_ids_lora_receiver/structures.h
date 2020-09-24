#ifndef __STRUCTURES_H
#define __STRUCTURES_H

struct DeviceDataStruct {
	char 		header[3];
	uint64_t	timer;
	float     	deviceBattVoltage;
	uint8_t     deviceBattPercent;
	bool        devicePirState;
	bool        deviceAlarmActive;
	bool        deviceLowBatt;
};

struct StateRequestStruct {
	uint8_t		rssi;
	uint64_t	timerGateway;
	uint64_t	timerDevice;
	float    	deviceBattVoltage;
	uint8_t     deviceBattPercent;
	uint8_t     devicePirState;
	uint8_t     deviceAlarmActive;
	uint8_t     deviceLowBatt;
}__attribute__((packed));

#endif