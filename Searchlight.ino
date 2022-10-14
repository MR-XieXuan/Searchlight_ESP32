/**************************************                              
*                                     *
*    Searchlight.ino                  *        
*                                     *
*    created 23 Apr 2022              *                      
*    By MR-XieXuan                    *                
*                                     *
*    The code is public for :         *                           
*    --ESP32                          *            
*    *  Xie-Xuan.                     *               
*    *  WanXin                        *             
*                                     *
**************************************/     

#include "Searchlight.h"

void setup() {
  //设备初始化
  Facility_Init();
  //Serial.printf("Facility_Init OK\n");
}

void loop() {
    // 
    NowPrint_Change();
    // 获取需要计算的数据
    GetTheAngle();
    //Serial.printf("GetTheAngle OK\n");
    // 计算数据
    Change_Data();
    //Serial.printf("Change_Data OK\n");
    // 打印数据
    ShowOLED();
    //Serial.printf("ShowOLED OK\n");
    // 使能设备
    Print();
    //Serial.printf("Print OK\n");
    Save_Data();

}