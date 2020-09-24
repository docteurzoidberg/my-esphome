#ifndef __MY_OLED_DISPLAY_H_
#define __MY_OLED_DISPLAY_H_

#include <Wire.h>
#include "SSD1306Wire.h"
#include "structures.h"
#include "images.h"

class MyOledDisplay : public SSD1306Wire{
    public:
        MyOledDisplay(uint8_t address,uint8_t sdapin,uint8_t sclpin);
        void drawLogoFrame();
        void drawWifiConnectFrame();
        void drawWifiConnectProgressFrame(uint8_t wifiCounter);
        void drawWifiConnectedFrame(String localIp);
        void drawMainFrame(DeviceJ7DataStruct* state, uint16_t packetCounter, bool wifiConnected, ulong lastPacketReceived);
};

#endif
