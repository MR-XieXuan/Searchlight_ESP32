/**************************************                              
*                                     *
*    Searchlight.h                    *        
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

#ifndef _SEARCHLIGHT_H_
#define _SEARCHLIGHT_H_


#include "Light/Light.h"        //探照灯
#include "U8g2lib.h"            //OLED
#include "GY521.h"              //GY521
#include <Wire.h>              //IIC库
#include <LSM303.h>             //GY511
#include "soc/rtc_wdt.h"    //看门狗库
#include <SPIFFS.h>

#ifndef G
#define G 0.97
#endif
#ifndef PI
#define PI 3.141592
#endif

// 引脚定义
#define BOTTOM_PIN     21
#define HEAD_PIN       19
#define IIC_SCL        5
#define IIC_SDA        18
#define BACK_KEY    2      
#define UP_KEY      4  
#define OK_KEY      22
#define LIFT_KEY    36
#define DOWN_KEY    39
#define RIGHT_KEY   32


//常用变量
/***
 * NowPrint[0] 现在在哪一个页面
 *  * 0 在运行部分 显示灯高 人的位置 灯的角度 人的角度
 *  * 1 在中断 1 内选择设定模式
 *  * 2 在直接输入内
 *  * 3 在自动校准内
 *  * 4 在设定 偏移角内
 * 
 *  * 0 在运行部分 显示灯高 人的位置 灯的角度 人的角度
 *  * 1 在中断 1 内选择设定模式
        * 0 选中直接输入
        * 1 选中自动校准
        * 2 设定偏移角
 *  * 2 在直接输入内
 *      * 0 选中H
 *      * 1 选中X
 *      * 2 选中Y
 *      * 3 选中Z
 *      * 4 选中L
 *          * 0 个位
 *          * 1 十位
 *          * 2 百位
  *         * 3 十分位
 *          * 4 百分位
 *          * 5 千分位
 *  * 3 在自校准内
 *      * 0 选中无墙
 *      * 1 选中有墙
 *  * 4 在设定偏移角内
 *  * 5 有墙自动校准
 *      * 0 地面1
 *      * 1 地面2
 *      * 2 墙体
 *  * 6 无墙自动校准
 *      * 0 地面1
 *      * 1 地面2
*/
uint8_t NowPrinter[10] = {0}; 
#define NowPrinter_To0() for(int i = 0 ;i < 10 ; i++) NowPrinter[i] = 0


float Angle_For_N = 0 ; //读取的距北的角度
float Alpha,Beta;      //计算后的角度
uint8_t Now_Key;        //现在要被处理的按键
float My_H;
float My_X;
float My_Y;
float My_Z;
float My_L;
float My_Excursion;
float My_Sigma_1 = 45, My_Phi_1 = 45 ;
float My_Alpha_1, My_Beta_1;
float My_Sigma_2 = 90, My_Phi_2 = 80;
float My_Alpha_2, My_Beta_2;


//设备对象定义
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2 ( U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ IIC_SCL, /* data=*/ IIC_SDA ) ; // 定义OLED屏幕对象
Light PTZ(BOTTOM_PIN,HEAD_PIN);           //云台对象 
LSM303 compass;                 //定义 GY521对象
GY521 sensor(0x68);             // GY521对象

#include "OLED/OLED.h" //OLED 操作库
#include "GY521/GY521.h"    // GY521 操作库
#include "KEY\KEY.h"        // 按钮操作库

float St_To_Fl(char *st,float *fl); // 定义一个函数 将字符串改成float形式的数字 /*程序未使用*/
float Angle_Standardizing(float *angle);

void Facility_Init(){   // 设备初始化的实现
    // 初始化串口
    Serial.begin(9600);
    rtc_wdt_protect_off(); //关闭看门狗保护
    rtc_wdt_enable();       //使能看门狗
    rtc_wdt_feed();         // 喂狗
    rtc_wdt_set_time(RTC_WDT_STAGE0, 7000); // 设置狗 
    //初始化Searchlight
    
    //OLED  初始化
    OLED_Init();        // 启动OLED屏幕
    Serial.printf("OLED_Init OK\n");    // OLED屏幕打印 OLED——Init OK

    //GY-511初始化
    Wire.begin(IIC_SDA,IIC_SCL);    // GY511 定义引脚
    compass.init();                 // GY511 启动代码
    compass.enableDefault();           
    compass.m_min = (LSM303::vector<int16_t>){-473,-584,-494};  // 自己测量的适用值
    compass.m_max = (LSM303::vector<int16_t>){+522,+328,+193};  // 自己测量的适用值
    compass.read();              // 读取 GY511传感器数据
    Angle_For_N = compass.heading((LSM303::vector<int>){1, 0, 0});       // 读取出来的数据以X轴为基准轴保存在 Angle——For——N中
    Serial.printf("GY-511 OK\n");   // 串口打印 GY511 OK

    //GY-521初始化
     while (sensor.wakeup() == false){  // 启动GY521 没有启动成功就循环下面的操作 每次停1秒防止触发看门狗
        Serial.print(millis()); // 打印现在的计算机时间
        Serial.println("\tCould not connect to GY521"); // 打印 GY521启动失败
        delay(1000);
    }
    // 下面是GY521的配置代码
    sensor.setAccelSensitivity(0);  // 8g
    sensor.setGyroSensitivity(1);   // 500 degrees/s
    sensor.setThrottle();
    sensor.read();  // 读取 GY521 传感器的数据

    //按钮初始化
    // 将所有按钮的引脚模式都设定为上拉电阻读取模式
    pinMode( BACK_KEY, INPUT_PULLUP);
    pinMode( UP_KEY,   INPUT_PULLUP);
    pinMode( OK_KEY,   INPUT_PULLUP);
    pinMode( LIFT_KEY, INPUT_PULLUP);
    pinMode( DOWN_KEY, INPUT_PULLUP);
    pinMode( RIGHT_KEY,INPUT_PULLUP);
    Now_Key = KEY_NONE ; // 现在要操作的按钮为 “没有”

    // 数据初始化 、*未使用*、
    SPIFFS.begin();
    
} 


//获取 与北的夹角 和 仰角
void GetTheAngle(){
    
    switch( NowPrinter[0] ){ 
        case 0 : // 正常运行时
        case 5 :
        case 6 :
            // 直接获取与北的夹角
            compass.read();
            Angle_For_N =compass.heading((LSM303::vector<int>){1, 0, 0});
            Serial.printf("N: %f",Angle_For_N);
            //直接获取仰角  
            Get_Alphi();
            break;
        case 1:
            break;
        case 2:
            break;
        case 3 :
            break;
        case 4 :
            break;
    }
    //数据统一化    

    //Serial.printf("GetTheAngle( Angle_For_N: %f ) Over ",Angle_For_N);
}

void Change_Data(){
    //数据统一化
    switch( NowPrinter[0] ){ 
        case 0 : // 正常运行时
            float beta ; // 定义一个局部变量 beta 
            beta = Angle_For_N - PTZ.Excursion; //将传感器GY511读取到的角度减去偏移角 保存在beta里面
            
            beta = Angle_Standardizing( &beta );

            if( pow( ( Beta - beta) , 2 ) > 9 ){    // 如果 现在舵机将要旋转的角度 小于2度的话就直接不转
                Beta = beta;
            }
            Serial.printf("Beta: %f",Beta); // 打印现在舵机的横向角度
            break;
        case 1:
            break;
        case 2:
            break;
        case 3 :
            break;
        case 4 :
            break;
        case 5 :
        case 6 :    // 自动校准时
            // 读取 到 角度
            if( NowPrinter[1] != 1 ){   // 如果现在在校准地面且正在收集第二组数据时 读取到的人的角度就存在 1里面 
                My_Beta_1 = Angle_For_N - PTZ.Excursion;
                My_Beta_1 = Angle_Standardizing(&My_Beta_1);
                My_Alpha_1 = Alpha;
            } else {    // 否则就存到2里面
                My_Beta_2 = Angle_For_N - PTZ.Excursion;
                My_Beta_1 = Angle_Standardizing(&My_Beta_2);
                My_Alpha_2 = Alpha;
            }
            break;
    }

}

void Print(){
    //使能设备
    switch( NowPrinter[0] ){ 
        case 0 : // 正常运行时
            // 上传给云台后转动
            Serial.printf("Alpha: %f",Alpha);   // 打印现在舵机的仰角
            PTZ.Flush(Alpha,Beta);  // 上传人现在看的角度 并且计算出舵机要转的角度
            // 如果要转动的角度 超出标准
            if( PTZ.Sigma >= 270){
                PTZ.Sigma = 0;
            }
            PTZ.Turn();     // 舵机转动
            break;
        case 1:
            break;
        case 2:
            break;
        case 3 :
            break;
        case 4 :
            break;
        case 5 :    // 自动校准时
        case 6 :
            // 转动到 角度
            if( NowPrinter[1] != 1 ) // 如果现在在校准地面且正在收集第二组数据时 则舵机直接转到1组数据 
                PTZ.Turn(My_Phi_1,My_Sigma_1); 
            else PTZ.Turn(My_Phi_2,My_Sigma_2); // 否则转到2组数据
            break;
    }
}

float Angle_Standardizing(float *angle){ // 将角度改成0-360度中间
    while(1){
        if( *angle > 360 ){
            *angle = *angle - 360 ;
        }else if( *angle < 0  ){
            *angle = *angle + 360 ;
        }else {
            return *angle;
        }
    }

}

void Save_Data(){
    //读取数据并保存
    

}

void Read_Data(){
    File p =  SPIFFS.open("data.txt",FILE_READ); //读取文件
    char k[6][10] = {0};
    if ( !p ){ // 如果打开成功
    for(uint8_t i =0 , j = 0; i<6 ;){
        char read = (char)p.read();
        if ( read == '\n' ) {
            k[i][j] = '\0' ;
            i ++ ; 
            j = 0;
            continue;
        }
        k[i][j] = read;
        j++;
    }
    p.close();
        My_H         =  St_To_Fl(k[0], &My_H         ) ; 
        My_X         =  St_To_Fl(k[1], &My_X         ) ; 
        My_Y         =  St_To_Fl(k[2], &My_Y         ) ; 
        My_Z         =  St_To_Fl(k[3], &My_Z         ) ; 
        My_L         =  St_To_Fl(k[4], &My_L         ) ; 
        My_Excursion =  St_To_Fl(k[5], &My_Excursion ) ; 
    }
    else { //失败则全部归零
        My_H          =   0 ; 
        My_X          =   0 ; 
        My_Y          =   0 ; 
        My_Z          =   0 ; 
        My_L          =   0 ; 
        My_Excursion  =   0 ; 
    }
}

float St_To_Fl(char *st,float *fl){
    for(int i=0,j=0;st[j] != '\0'; j++){
        for(;;i++){
        	if( j>i ) {
				break;
			}else if( st[i] == '.') {
                j = i + 1 ;
                break;
            }else if( st[i] == '\0'){
                break;
            }
            *fl *= 10;
            *fl += ( st[i] - 48 ) ;
        }
        if( j < i ) break;
        *fl += ( st[j] - 48 ) * pow( 10 , i-j ) ; 
    }
    return *fl;
}

#endif 

