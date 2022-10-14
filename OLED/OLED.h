/**************************************                              
*                                     *
*    OLED.h                           *        
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

/**************************
 * 这个库主要用来显示
 * 刷新显示的界面
 * ************************/

#ifndef _OLED_H_

#define _OLED_H_

#include "U8g2lib.h"

/***
 * NowPrint[0] 现在在哪一个页面
 *  * 0 在运行部分 显示灯高 人的位置 灯的角度 人的角度
 *  * 1 在中断 1 内选择设定模式
 *  * 2 在直接输入内
 *  * 3 在自动校准内
 *  * 4 在设定 偏移角内
 * 
 * NowPrint[1] 现在在哪一个页面的第几部分
 *  * 0 在运行部分 显示灯高 人的位置 灯的角度 人的角度
 *  * 1 在中断 1 内选择设定模式
        * 0 选中直接输入
        * 1 选中自动校准
 *  * 2 在直接输入内
 *      * 0 选中H
 *      * 1 选中X
 *      * 2 选中Y
 *      * 3 选中Z
 *      * 4 选中L
 *  * 3 在自动校准内
 *      * 0 选中无墙
 *      * 1 选中有墙
 *  * 4 在设定偏移角内
*/

void ShowOLED();

void Running_Show();        //运行部分 *
void Set_Now_Show();       //选择界面 *
void Inter_Now_Show();      //直接输入内    *     
void Auto_Set_Show();       //自动校准内    
void Set_Angle_Show();      //设定偏移角内
void Auto_Set_Ground_Show(); //有墙的自动校准
void Auto_Set_Wall_Show(); //无墙的自动校准

void  OLED_Init(){
    u8g2.begin();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.sendBuffer();
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(30,10,"OLED OPEN");
    u8g2.sendBuffer();
}

void  ShowOLED(){
    //判断现在应该显示的东西
    switch(NowPrinter[0]){//现在所在的页面
        case 0 :
            Running_Show(); //在运行部分 显示灯高 人的位置 灯的角度 人的角度
            break;
        case 1 :
            Set_Now_Show(); //在中断 1 内选择设定模式
            break;
        case 2 :
            Inter_Now_Show();   //在直接输入内
            break;
        case 3 :
            Auto_Set_Show();     //在自动校准内
            break;
        case 4 :
            Set_Angle_Show();   //在设定 偏移角内
            break;
        case 5 :
            if (NowPrinter[1] == 2){     
                Auto_Set_Wall_Show();   //在设定墙
            } else {      //设定地面                  
                Auto_Set_Ground_Show(); //在设定地面
            }
            break;
        case 6 :
            Auto_Set_Ground_Show();//无墙
            break;
    }

    //Serial.printf(" ShowOLED(%d) Over *\n",NowPrinter[0]);
}


void  Running_Show(){
    //读取 H X Y Z Phi Sigma Alpha Beta
    char data[20] = {}; 

    //显示 H X Y Z Phi Sigma Alpha Beta
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);

    sprintf(data," Phi : %06.2f ", PTZ.Phi );
    u8g2.drawStr(30,10,data);

    sprintf(data," Sigma : %06.2f ", PTZ.Sigma );
    u8g2.drawStr(30,20,data);

    sprintf(data," Alpha : %06.2f ", PTZ.Alpha );
    u8g2.drawStr(30,30,data);

    sprintf(data," Beta : %06.2f ", PTZ.Beta );
    u8g2.drawStr(30,40,data);

    sprintf(data," H : %06.2f ", PTZ.H );
    u8g2.drawStr(0,50,data);

    sprintf(data," X : %06.2f ", PTZ.X );
    u8g2.drawStr(60,50,data);

    sprintf(data," Y : %06.2f ", PTZ.Y );
    u8g2.drawStr(0,60,data);

    sprintf(data," Z : %06.2f ", PTZ.Z );
    u8g2.drawStr(60,60,data);

    u8g2.sendBuffer();

    //Serial.printf(" Running_Show( H:%06.2f X:%06.2f Y:%06.2f Z:%06.2f Phi:%06.2f Sigma:%06.2f Alpha:%06.2f Beta:%06.2f ) Over *\n",PTZ.H,PTZ.X,PTZ.Y,PTZ.Z,PTZ.Phi,PTZ.Sigma,PTZ.Alpha,PTZ.Beta);

}

void  Set_Now_Show(){
/*
*  * 1 在中断 1 内选择设定模式
    * 0 选中直接输入
    * 1 选中自动校准
*/
    char data[20] = {}; 

    //输出 0.自动输入 1.自动校准
    u8g2.clearBuffer();

    sprintf(data,"Manual Operation");
    u8g2.drawStr(15,10,data);

    sprintf(data,"Automatic Operation");
    u8g2.drawStr(15,35,data);

    sprintf(data,"Set The Excursion");
    u8g2.drawStr(15,60,data);

    //判断选中的模式 并在它前面加上 *

    switch(NowPrinter[1]){
        case 0:
            sprintf(data,">");
            u8g2.drawStr(0,10,data);
            break;
        case 1: 
            sprintf(data,">");
            u8g2.drawStr(0,35,data);
            break;
        case 2: 
            sprintf(data,">");
            u8g2.drawStr(0,60,data);
            break;
    }
    
    u8g2.sendBuffer();

    //Serial.printf(" Set_Now_Show(%d) Over *\n",NowPrinter[1]);
}

void  Inter_Now_Show(){
    u8g2.clearBuffer();
    char data[20] = {}; 
    //左上角打印$
    sprintf(data,"$");
    u8g2.drawStr(0,10,data);
    //读取 H X Y Z Phi Sigma Alpha Beta
    //显示 H X Y Z Phi Sigma Alpha Beta
    
    sprintf(data," H : %07.3f ", My_H );
    //Serial.printf("My_H : %d \n",My_H);
    u8g2.drawStr(40,10,data);

    sprintf(data," X : %07.3f ", My_X );
    u8g2.drawStr(40,20,data);

    sprintf(data," Y : %07.3f ", My_Y );
    u8g2.drawStr(40,30,data);

    sprintf(data," Z : %07.3f ", My_Z );
    u8g2.drawStr(40,40,data);

    sprintf(data," L : %07.3f ", My_L );
    u8g2.drawStr(40,50,data);

    uint8_t x,y;

    //判断当前选中并在之前打印 位数
    switch(NowPrinter[2]){
        case 0 :
             x = 3; 
            break;
        case 1 :
             x = 2; 
            break;
        case 2 :
             x = 1; 
            break;
        case 3 :
             x = 4; 
            break;
        case 4 :
             x = 5 ;
            break;
        case 5 :
             x = 6; 
            break;
    }
    //现在输入的是什么
    switch(NowPrinter[1]){
        case 0 :
             y = 10; 
            break;
        case 1 :
             y= 20; 
            break;
        case 2 :
             y = 30; 
            break;
        case 3 :
             y = 40; 
            break;
        case 4 :
             y = 50; 
            break;
    }
    sprintf(data," %d > ", x );
    u8g2.drawStr(20,y,data);

    u8g2.sendBuffer();
    //Serial.printf(" Inter_Now_Show(%d) Over *\n",NowPrinter[2]);

}


void  Auto_Set_Show(){
    
    u8g2.clearBuffer();
    char data[30];
    // 选择是否有墙体
    
    sprintf(data,"Have Wall" );
    u8g2.drawStr(40,20,data);
    
    
    sprintf(data," No Wall" );
    u8g2.drawStr(40,50,data);

    // 在当前选中前打印 *
    uint8_t y;
    switch(NowPrinter[1]){
        case 0 :
             y = 20; 
            break;
        case 1 :
             y = 50; 
            break;
    }

    u8g2.drawStr(10,y,">");

    //Serial.printf(" Auto_Set_Show() Over *\n");
    u8g2.sendBuffer();
}

void  Set_Angle_Show(){
    
    u8g2.clearBuffer();
    char data[25] = {}; 

    // 读取偏移角
    //打印偏移角 
    sprintf(data,"Angle_For_N ");
    u8g2.drawStr(30,25,data);
    
    sprintf(data," %07.3f ", My_Excursion );
    u8g2.drawStr(40,35,data);

    //判断当前选中并在之前打印 位数
    uint8_t x;
    switch(NowPrinter[1]){
        case 0 :
             x = 3; 
            break;
        case 1 :
             x = 2; 
            break;
        case 2 :
             x = 1; 
            break;
        case 3 :
             x = 4; 
            break;
        case 4 :
             x = 5 ;
            break;
        case 5 :
             x = 6; 
            break;
    }

    sprintf(data," %d > ", x );
    u8g2.drawStr(3,35,data);

    u8g2.sendBuffer();
}


void Auto_Set_Ground_Show(){
    u8g2.clearBuffer();
    char data[25] = {}; 
    // 表格
    
    sprintf(data,"Auto Set The Ground" );
    u8g2.drawStr(9,9,data);

    sprintf(data," My_Sigma" );
    u8g2.drawStr(10,20,data);
    
    sprintf(data," My_Phi" );
    u8g2.drawStr(70,20,data);

    //Fist Data
    sprintf(data," %07.3f ", My_Sigma_1 );
    u8g2.drawStr(10,30,data);

    sprintf(data," %07.3f ", My_Phi_1   );
    u8g2.drawStr(70,30,data);

    sprintf(data," %07.3f ", My_Alpha_1 );
    u8g2.drawStr(10,40,data);

    sprintf(data," %07.3f ", My_Beta_1  );
    u8g2.drawStr(70,40,data);

    // Second Data
    sprintf(data," %07.3f ", My_Sigma_2 );
    u8g2.drawStr(10,52,data);

    sprintf(data," %07.3f ", My_Phi_2   );
    u8g2.drawStr(70,52,data);

    sprintf(data," %07.3f ", My_Alpha_2 );
    u8g2.drawStr(10,62,data);

    sprintf(data," %07.3f ", My_Beta_2  );
    u8g2.drawStr(70,62,data);
    
    u8g2.sendBuffer();
}

void Auto_Set_Wall_Show(){
    u8g2.clearBuffer();
    char data[25] = {}; 
    // 表格

    sprintf(data,"Auto Set The Wall" );
    u8g2.drawStr(10,10,data);

    sprintf(data," My_Sigma" );
    u8g2.drawStr(10,20,data);
    
    sprintf(data," My_Phi" );
    u8g2.drawStr(70,20,data);

    //Fist Data
    sprintf(data," %07.3f ", My_Sigma_1 );
    u8g2.drawStr(20,40,data);

    sprintf(data," %07.3f ", My_Phi_1   );
    u8g2.drawStr(70,40,data);

    sprintf(data," %07.3f ", My_Alpha_1 );
    u8g2.drawStr(20,60,data);

    sprintf(data," %07.3f ", My_Beta_1  );
    u8g2.drawStr(70,60,data);

    u8g2.sendBuffer();
}

#endif
