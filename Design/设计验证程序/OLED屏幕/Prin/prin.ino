//Arduino OLED Example
#include <Arduino.h>
#include <U8g2lib.h>
#include <stdlib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#define OLED_SCL 5
#define OLED_SDA 18

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, OLED_SCL,OLED_SDA, U8X8_PIN_NONE);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  u8g2.begin();
  u8g2.setFont(u8g2_font_ncenB08_tr);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.printf("OK\n");
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(10,10,"HELLOW WORLD!");
  u8g2.sendBuffer();
  delay(1000);
  //读取 H X Y Z Phi Sigma Alpha Beta
    char data[20] = {};

    u8g2.clearBuffer();

    sprintf(data,"Manual Operation");
    u8g2.drawStr(15,20,data);

    sprintf(data,"Automatic Operation");
    u8g2.drawStr(15,50,data);

    sprintf(data,">");
    u8g2.drawStr(0,50,data);
    
    u8g2.sendBuffer();
  delay(10000);
}
