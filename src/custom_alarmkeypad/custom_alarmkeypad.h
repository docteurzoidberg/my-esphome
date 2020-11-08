#include "esphome.h"

using namespace esphome;

static const char *TAG = "AlarmKeypad";

char displaybuffer[9] = {'T', 'O', 'T', 'O', 'T', 'I', 'T', 'I', 0};

void mydisplay();
void sequence_init();

void sequence_init() {
  strcpy(displaybuffer, "-       "); mydisplay(); vTaskDelay(100);
  strcpy(displaybuffer, "--      "); mydisplay(); vTaskDelay(100);
  strcpy(displaybuffer, "---     "); mydisplay(); vTaskDelay(100);
  strcpy(displaybuffer, "----    "); mydisplay(); vTaskDelay(100);
  strcpy(displaybuffer, "-----   "); mydisplay(); vTaskDelay(100);
  strcpy(displaybuffer, "------  "); mydisplay(); vTaskDelay(100);
  strcpy(displaybuffer, "------- "); mydisplay(); vTaskDelay(100);
  strcpy(displaybuffer, "--------"); mydisplay(); vTaskDelay(100);
  strcpy(displaybuffer, " -------"); mydisplay(); vTaskDelay(100);
  strcpy(displaybuffer, "  ------"); mydisplay(); vTaskDelay(100);
  strcpy(displaybuffer, "   -----"); mydisplay(); vTaskDelay(100);
  strcpy(displaybuffer, "    ----"); mydisplay(); vTaskDelay(100);
  strcpy(displaybuffer, "     ---"); mydisplay(); vTaskDelay(100);
  strcpy(displaybuffer, "      --"); mydisplay(); vTaskDelay(100);
  strcpy(displaybuffer, "       -"); mydisplay(); vTaskDelay(100);
  strcpy(displaybuffer, "        "); mydisplay(); vTaskDelay(200);
  strcpy(displaybuffer, "-       "); mydisplay(); vTaskDelay(100);
  strcpy(displaybuffer, "--      "); mydisplay(); vTaskDelay(100);
  strcpy(displaybuffer, "---     "); mydisplay(); vTaskDelay(100);
  strcpy(displaybuffer, "----    "); mydisplay(); vTaskDelay(100);
  strcpy(displaybuffer, "-----   "); mydisplay(); vTaskDelay(100);
  strcpy(displaybuffer, "------  "); mydisplay(); vTaskDelay(100);
  strcpy(displaybuffer, "------- "); mydisplay(); vTaskDelay(100);
  strcpy(displaybuffer, "--------"); mydisplay(); vTaskDelay(100);
  strcpy(displaybuffer, " -------"); mydisplay(); vTaskDelay(100);
  strcpy(displaybuffer, "  ------"); mydisplay(); vTaskDelay(100);
  strcpy(displaybuffer, "   -----"); mydisplay(); vTaskDelay(100);
  strcpy(displaybuffer, "    ----"); mydisplay(); vTaskDelay(100);
  strcpy(displaybuffer, "     ---"); mydisplay(); vTaskDelay(100);
  strcpy(displaybuffer, "      --"); mydisplay(); vTaskDelay(100);
  strcpy(displaybuffer, "       -"); mydisplay(); vTaskDelay(100);
  strcpy(displaybuffer, "        "); mydisplay(); vTaskDelay(2200);

  strcpy(displaybuffer, " SYSTEM "); mydisplay(); vTaskDelay(900);
  strcpy(displaybuffer, " READY  "); mydisplay(); vTaskDelay(3900);
}

void mydisplay() {
  /*
  alpha1.writeDigitAscii(0, displaybuffer[0]);
  alpha1.writeDigitAscii(1, displaybuffer[1]);
  alpha1.writeDigitAscii(2, displaybuffer[2]);
  alpha1.writeDigitAscii(3, displaybuffer[3]);
  alpha2.writeDigitAscii(0, displaybuffer[4]);
  alpha2.writeDigitAscii(1, displaybuffer[5]);
  alpha2.writeDigitAscii(2, displaybuffer[6]);
  alpha2.writeDigitAscii(3, displaybuffer[7]);
  alpha1.writeDisplay();
  alpha2.writeDisplay();
  */

  //id(alpha1).printf("%.4s",displaybuffer);
  //id(alpha1).printf("%c%c%c%c",displaybuffer[0],displaybuffer[1],displaybuffer[2],displaybuffer[3]);
  //id(alpha1).display_();
  //id(alpha2).printf("%s",std::string(displaybuffer).substr(4,4));
  //id(alpha2).display_();

}

class AlarmKeypad : public Component, CustomAPIDevice  {
  private:

  public:
    AlarmKeypad() : Component() {
    }
    void on_hello_world() {
      ESP_LOGD(TAG, "Hello World!");
    }
    void setup() override {
      ESP_LOGCONFIG(TAG, "Setting up ...");
      register_service(&AlarmKeypad::on_hello_world, "hello_world");
      /*
      id(alpha1).print("TOTO");
      id(alpha1).display_();
      id(alpha2).print("TITI");
      id(alpha2).display_();
      sequence_init();
      */
    }
    void loop() override {
      //if not connected, scan and try to connect
      //mydisplay();
      //delay(20);
    }
};