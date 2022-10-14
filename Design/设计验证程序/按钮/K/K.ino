#include "Arduino.h"
volatile boolean RunBuzzer = true;
const int led = 18;
const int button = 35;

// 中断函数
void warning()
{
  RunBuzzer = !RunBuzzer;
}

void setup()
{
  Serial.begin(9600);
  Serial.println("OKOK");
  // 初始化外部中断、pin
  // 当按键按下时，引脚 34 输入的电平由高变低,触发中断函数warning
  pinMode(led, OUTPUT);
  pinMode(button, INPUT);
  attachInterrupt(button, warning, FALLING);    // 设置外部中断
}

void loop()
{
  if (RunBuzzer)
  {
    Serial.printf("%d\n",RunBuzzer);
    digitalWrite(led, HIGH);
  }
  else
  {
    Serial.printf("%d\n",RunBuzzer);
    digitalWrite(led, LOW);
  }
}
