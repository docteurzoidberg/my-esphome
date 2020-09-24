#include "myoleddisplay.h"

//constructeur. herite de SSD1306Wire
MyOledDisplay::MyOledDisplay(uint8_t address,uint8_t sdapin,uint8_t sclpin) : SSD1306Wire(address,sdapin,sclpin) {

}

void MyOledDisplay::drawLogoFrame() {
	clear();
	drawXbm((128-logo_width)/2,(64-logo_height)/2,logo_width,logo_height,J7_profil_7x2);
}

void MyOledDisplay::drawWifiConnectFrame() {
	clear();
  drawXbm(34, 14, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);
}

void MyOledDisplay::drawWifiConnectProgressFrame(uint8_t wifiCounter) {
	clear();
 	drawXbm(34, 14, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);
	drawProgressBar(((128-64)/2), (64-12), 64, 10, wifiCounter);
}

void MyOledDisplay::drawWifiConnectedFrame(String localIp) {
	clear();
	setTextAlignment(TEXT_ALIGN_LEFT);
	setFont(ArialMT_Plain_10);
	drawString(30, 10, "Connected !");
	drawString(30, 10+12, localIp);
}

void MyOledDisplay::drawMainFrame(DeviceJ7DataStruct* state, uint16_t packetCounter, bool wifiConnected, ulong lastPacketReceived) {

	clear();

	setTextAlignment(TEXT_ALIGN_LEFT);
	setFont(ArialMT_Plain_10);

	drawXbm(20, 1, icon_voltage_width, icon_voltage_height, icon_voltage_bits);
	drawString(36, 1, String(state->deviceBattVoltage));

	drawXbm(65, 1, icon_packets_width, icon_packets_height, icon_packets_bits);
	drawString(80, 1, String(packetCounter));

	if(state->deviceAlarmActive){
		drawXbm(20, 18, alerte_width, alerte_height, alerte_bits);
	}

	if(wifiConnected){
		drawXbm(19, 42, icon_wifi_width, icon_wifi_height, icon_wifi_bits);
	}

	if((millis()-lastPacketReceived)<5000){
		drawXbm(43, 42, icon_lora_width, icon_lora_height, icon_lora_bits);
	}

	if(state->deviceLowBatt){
		drawXbm(68, 42, icon_batt_width, icon_batt_height, icon_batt_bits);
	}

	if(state->devicePirState){
		drawXbm(93, 42, icon_pir_width, icon_pir_height, icon_pir_bits);
	}
}
