#include <TFT_eSPI.h>//LCD 라이브러리(Bodmer)
#include <SPI.h>//LCD에서 사용하는 SPI 라이브러리(아두이노)

#include <SimgSprite.h>//simg형식의 이미지 출력 라이브러리(Attic Electronics 방구석전자)


#include <ScrollElement.h>//화면 스크롤 라이브러리(Attic Electronics 방구석전자)
#include <ScrollContainer.h>//화면 스크롤 라이브러리(Attic Electronics 방구석전자)
#include <NotoSans.h>
#include <TextToSprite.h>//한글출력 라이브러리(Attic Electronics 방구석전자)

#include <WiFi.h>//WIFI 라이브러리(아두이노)
#include <WiFiClient.h>//WIFI 클라이언트 라이브러리(아두이노)

#include <I2SAudioReceiver.h>//I2S 오디오 받기 라이브러리 (Attic Electronics 방구석전자)
#include <I2SAudioSender.h>//I2S 오디오 보내기 라이브러리 (Attic Electronics 방구석전자)

#include <Button.h>//버튼 라이브러리

#define VERTICAL_STEP 16 //글씨 크기이자 세로 이동값 
#define BG_COLOR 0x000000 //배경색깔
#define START_Y 440 //기준점 Y

Button testbutton(9);//버튼 포트지정

WiFiClient client;
I2SAudioSender sendVoice;
I2SAudioReceiver recvVoice;

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite **sprites;
ScrollContainer container(&tft);

void setup(void)
{
    Serial.begin(115200);
  
    WiFi.begin("본인이 사용할 WIFI 이름", "본인이 사용할 WIFI 비밀번호");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
    }

    sendVoice.setWifiClient(client);
    sendVoice.setServerAddr("본인이 연결할 PC IP", 33823); // String ip, int port
    sendVoice.setI2sBus(1);                          // 0 or 1
    sendVoice.setAudioQulity(32000, 16, 1);          // int sample_rate, int sample_size, int channels(only 1 tested)
    sendVoice.setI2sPin(8, 19, 20);                  // int sck, int sd, int ws
    sendVoice.setDmaBuf(1024, 6);                    // int len(only 1024 tested), int count
    sendVoice.i2sBegin();

    recvVoice.setWifiClient(client);
    recvVoice.setServerAddr("본인이 연결할 PC IP", 33819); // String ip, int port
    recvVoice.setI2sBus(0);                  // 0 or 1
    recvVoice.setAudioQuality(16000, 16, 1); // int sample_rate, int sample_size(only 16), int channels(only 1)
    recvVoice.setI2sPin(18, 17, 16);         // int sck, int sd, int ws
    recvVoice.setDmaBuf(1024, 6);            // int len(only 1024 tested), int count
    recvVoice.i2sBegin();
    
    tft.init();
    tft.setRotation(2);
    tft.fillScreen(BG_COLOR);     // 배경색상 흰색 지정
    tft.loadFont(NotoSansBold15); // NotoSansd에 정의됨
    tft.setTextColor(TFT_WHITE, BG_COLOR);

    sprites = new TFT_eSprite *[9];

    SimgSprite simg(&tft);
    sprites[0] = simg.load("/DS_MAIN.simg");//처음 화면 아이콘
    sprites[1] = simg.load("/JS_MAIN.simg");//처음 화면 아이콘
    sprites[2] = simg.load("/DS_ICON.simg");//채팅 사용자 아이콘
    sprites[3] = simg.load("/JS_ICON.simg");//채팅 AI 아이콘
    sprites[4] = simg.load("/ICON.simg");//처음화면 가운데 아이콘
    sprites[5] = simg.load("/MAIN_TEXT.simg");//처음 화면 안내글씨
    sprites[6] = simg.load("/TALK.simg");//대화창
    sprites[7] = simg.load("/KEY_OFF.simg");//키 안눌림
    sprites[8] = simg.load("/KEY_ON.simg");//키 눌림

    container.setBackgroundColor(BG_COLOR);
    //최초 화면 구성
    ScrollElement *DS_MAIN = new ScrollElement(0, 0, sprites[0]->width(), sprites[0]->height(), sprites[0]);
    ScrollElement *JS_MAIN = new ScrollElement(180, 0, sprites[1]->width(), sprites[1]->height(), sprites[1]);
    ScrollElement *ICON = new ScrollElement(50, 160, sprites[4]->width(), sprites[4]->height(), sprites[4]);
    ScrollElement *maintext = new ScrollElement(0, 310, sprites[5]->width(), sprites[5]->height(), sprites[5]);

    container.addElement(DS_MAIN);
    container.addElement(JS_MAIN);
    container.addElement(ICON);
    container.addElement(maintext);
    container.drawElements();
    footerUI(0);

}
void loop()
{
    int state = testbutton.checkState();
    if (state == 2)
    {
        sendVoice.writeData();
        Serial.println("홀드");
    }
    else if (state == 1)
    {
        footerUI(1);//듣고 있음을 나타내는 상태
        sendVoice.openFile();
        Serial.println("눌림");
    }
    else if (state == 3)
    {
        Serial.println("땜");
        footerUI(2);
        String whisper = sendVoice.closeFile();
        ScrollElement *JS_ICON = new ScrollElement(0, START_Y-20, sprites[3]->width(), sprites[3]->height(), sprites[3]);
        container.addElement(JS_ICON);
        TextToSprite *ttsprites = new TextToSprite(&tft, whisper, 16 * 17, 1);//한 화면 17자 제한
        ttsprites->setBackgroundColor(BG_COLOR);
        while (true)
        {
            TFT_eSprite *sprite = ttsprites->getNextSprite(80, START_Y-20);
            if (sprite != nullptr)
            {
                container.addElement(new ScrollElement(80, START_Y-20, sprite->width(), sprite->height(), sprite, 1));
                container.updateAndDraw(VERTICAL_STEP);
            }
            else
            {
                ESP_LOGD("TextToSprite", "sprite is null");
                delete ttsprites; // TextToSprite 객체 메모리 해제
                break;
            }
        }
        
        container.updateAndDraw(VERTICAL_STEP * 3);

        delay(500);
        String gptmsg = recvVoice.startSteam();
        delay(5000);
        ScrollElement *DS_ICON = new ScrollElement(0, START_Y-20, sprites[2]->width(), sprites[2]->height(), sprites[2]);
        container.addElement(DS_ICON);
        TextToSprite *ttsprites4 = new TextToSprite(&tft, gptmsg, 16 * 17, 1);
        ttsprites4->setBackgroundColor(BG_COLOR);
        while (true)
        {
            TFT_eSprite *sprite = ttsprites4->getNextSprite(80, START_Y-20);
            if (sprite != nullptr)
            {
                container.addElement(new ScrollElement(80, START_Y-20, sprite->width(), sprite->height(), sprite, 1));
                container.updateAndDraw(VERTICAL_STEP);
            }
            else
            {
                ESP_LOGD("TextToSprite", "sprite is null");
                delete ttsprites4; // TextToSprite 객체 메모리 해제
                break;
            }
        }

        int err = recvVoice.playStreamData();
    }
}

// 하단 UI 변경
void footerUI(int state)
{
    if (state == 0)
    {
        sprites[6]->pushSprite(0, START_Y);
        sprites[7]->pushSprite(280, START_Y);
        TextToSprite *ttsprites = new TextToSprite(&tft, "버튼을 누르고 말해주세요", 16 * 11, 0);
      while (true)
        {
            TFT_eSprite *sprite = ttsprites->getNextSprite(82, START_Y);
            if (sprite != nullptr)
            {
                sprite->pushSprite(40,START_Y+10);
                delete sprite;
            }
            else
            {
                delete ttsprites; // TextToSprite 객체 메모리 해제
                break;
            }
        }
    }
    else if (state == 1)
    {
        sprites[6]->pushSprite(0, START_Y);                     // 하단 문자영역
        sprites[8]->pushSprite(280, START_Y);
        TextToSprite *ttsprites = new TextToSprite(&tft, "듣고 있습니다 말씀하세요", 16 * 11, 0);
      while (true)
        {
            TFT_eSprite *sprite = ttsprites->getNextSprite(82, START_Y);
            if (sprite != nullptr)
            {
                sprite->pushSprite(40,START_Y+10);
                delete sprite;
            }
            else
            {
                delete ttsprites; // TextToSprite 객체 메모리 해제
                break;
            }
        }
    }
    else if (state == 2)
    {
        sprites[6]->pushSprite(0, START_Y);                             // 하단 문자영역
        sprites[7]->pushSprite(280, START_Y);
        TextToSprite *ttsprites = new TextToSprite(&tft, "버튼을 누르고 말해주세요", 16 * 11, 0);
      while (true)
        {
            TFT_eSprite *sprite = ttsprites->getNextSprite(82, START_Y);
            if (sprite != nullptr)
            {
                sprite->pushSprite(40,START_Y+10);
                delete sprite;
            }
            else
            {
                delete ttsprites; // TextToSprite 객체 메모리 해제
                break;
            }
        }
    }
}
