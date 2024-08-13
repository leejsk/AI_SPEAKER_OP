/*
  현재 LittleFS 만 사용한다.
*/
#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SimgSprite.h>

TFT_eSPI tft;
TFT_eSprite** sprites;

void setup() {
  Serial.begin(115200);
  tft.init();
  tft.fillScreen(TFT_BLACK);
  tft.setRotation(2);

  sprites = new TFT_eSprite*[9];
  SimgSprite simg(&tft);
  sprites[0] = simg.load("/DS_MAIN.simg");//
  sprites[0]->pushSprite(0,0);
  sprites[1] = simg.load("/JS_MAIN.simg");
  sprites[1]->pushSprite(180,0,simg.TRANS);
  sprites[2] = simg.load("/JS_ICON.simg");
  sprites[2]->pushSprite(0,100,simg.TRANS);
  sprites[3] = simg.load("/DS_ICON.simg");
  sprites[3]->pushSprite(100,100,simg.TRANS);
  sprites[4] = simg.load("/ICON.simg");
  sprites[4]->pushSprite(50,160,simg.TRANS);
  sprites[5] = simg.load("/MAIN_TEXT.simg");
  sprites[5]->pushSprite(0,310,simg.TRANS);
  sprites[6] = simg.load("/TALK.simg");
  sprites[6]->pushSprite(0,440,simg.TRANS);
  sprites[7] = simg.load("/KEY_OFF.simg");
  sprites[7]->pushSprite(280,440,simg.TRANS);
  sprites[8] = simg.load("/KEY_ON.simg");
  sprites[8]->pushSprite(0,400,simg.TRANS);
}
void loop() {}
