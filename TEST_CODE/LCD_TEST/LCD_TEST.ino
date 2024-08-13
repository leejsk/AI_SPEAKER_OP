#include <TFT_eSPI.h> 
TFT_eSPI tft = TFT_eSPI();
void setup(void) {
  tft.init();
  tft.fillScreen(TFT_RED);
  delay(500);
}
void loop(){}
