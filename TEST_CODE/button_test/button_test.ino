#include <Button.h>

Button testbutton(9);

void setup(void)
{
 Serial.begin(115200);  
}
void loop()
{
    int state = testbutton.checkState();
    if (state == BUTTON_PRESSED)
    {
      Serial.println("버튼 눌림");
    }
    else if (state == BUTTON_RELEASED)
    {
      Serial.println("버튼 땜");
    }
    else if (state == BUTTON_HOLD)
    {
      Serial.println("버튼 홀드");
    }
}
