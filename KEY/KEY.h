#ifndef _KEY_H_
#define _KEY_H_


// 按钮宏
#define KEY_NONE    0
#define KEY_BACK    1
#define KEY_UP      2
#define KEY_OK      3
#define KEY_LIFT    4
#define KEY_DOWN    5
#define KEY_RIGHT   6

/*
    switch( Now_Key ){
        case KEY_BACK: 
            break;
        case KEY_UP: 
            break;
        case KEY_OK: 
            break;
        case KEY_LIFT: 
            break;
        case KEY_DOWN:
            break;
        case KEY_RIGHT: 
            break;
    }
*/

// 按钮的识别函数

void Key_Back (){
    while( digitalRead(BACK_KEY) == LOW )  {
        Now_Key = KEY_BACK ;
        rtc_wdt_feed();     //喂狗
        Serial.printf("BACK_KEY\n");
        delay(100);
    }
    
}

void Key_Up   (){
    while( digitalRead(UP_KEY) == LOW )   {
        Now_Key = KEY_UP;
        rtc_wdt_feed(); 
        Serial.printf("UP_KEY\n");
        delay(100);
    }
}

void Key_Ok   (){
    while( digitalRead(OK_KEY) == LOW )  {
        Now_Key = KEY_OK;
        rtc_wdt_feed(); 
        Serial.printf("OK_KEY\n");
        delay(100);
    }

}

void Key_Lift (){
    while( digitalRead(LIFT_KEY) == LOW )  {
        Now_Key = KEY_LIFT;
        rtc_wdt_feed(); 
        Serial.printf("LIFT_KEY\n");
        delay(100);
    }

}

void Key_Down (){
    while( digitalRead(DOWN_KEY) == LOW )    {
        Now_Key = KEY_DOWN;
        rtc_wdt_feed(); 
        Serial.printf("DOWN_KEY\n");
        delay(100);
    }

}

void Key_Right(){
    while( digitalRead(RIGHT_KEY) == LOW )  {
        Now_Key = KEY_RIGHT;
        rtc_wdt_feed(); 
        Serial.printf("RIGHT_KEY\n");
        delay(100);
    }

}

void NowPrint_Change(); //主要函数
void Running_Do();  //运行中按按钮
void Set_Now_Do();  //选则界面按按钮
void Inter_Now_Do();  //直接输入内按按钮
    void Change_Number_Do(float * num , uint8_t set , uint8_t add_down ); //用来对某个数加减
    void Up_Data_Do();  //讲输入数据上传给舵机
void Auto_Set_Do();  //在自动校准选择节目按按按钮 
    void Auto_Set_No_Wall_Do(); //在无墙的自动校准
    void Auto_Set_Have_Wall_Do();   // 在有墙的自动校准
void Set_Angle_Do();    //在输入偏移角内按按钮
    void Up_Data_N_Do();            //上传偏移角的数据


void NowPrint_Change(){
    Key_Back();
    Key_Up();
    Key_Ok();
    Key_Lift();
    Key_Down();
    Key_Right();
    // 若没有按键按下则直接退出
    if( Now_Key == 0) return;

    switch(NowPrinter[0]){//现在所在的页面
        case 0 :
            Running_Do(); //在运行部分 显示灯高 人的位置 灯的角度 人的角度
            break;
        case 1 :
            Set_Now_Do(); //在中断 1 内选择设定模式
            break;
        case 2 :
            Inter_Now_Do();   //在直接输入内
            break;
        case 3 :
            Auto_Set_Do();     //在自动校准内
            break;
        case 4 :
            Set_Angle_Do(); //在设定 偏移角内
            break;
        case 5 : 
            Auto_Set_Have_Wall_Do();  //在设定有墙的自动校准内
            break;
        case 6 :
            Auto_Set_No_Wall_Do(); // 在设定无墙的自动校准内
            break;
    }

//Serial.printf(" NowPrint_Change(%d) Over *\n",Now_Key);

}

//在运行部分 显示灯高 人的位置 灯的角度 人的角度 
void Running_Do(){
    // 按键可调节云台的角度 通过直接修改人和灯塔的相对位置实现
    switch( Now_Key ){
        case KEY_BACK: 
            NowPrinter[0] ++;
            Now_Key = KEY_NONE;
            break;
        case KEY_UP: 
            PTZ.Z += 3;
            PTZ.Y += 3;
            Now_Key = KEY_NONE;
            break;
        case KEY_OK: 
            NowPrinter[0] ++;
            Now_Key = KEY_NONE;
            break;
        case KEY_LIFT: 
            PTZ.X += 3;
            Now_Key = KEY_NONE;
            break;
        case KEY_DOWN:
            PTZ.Y -= 2;
            Now_Key = KEY_NONE;
            break;
        case KEY_RIGHT:
            PTZ.X -= 2; 
            Now_Key = KEY_NONE;
            break;
    }
}

void Set_Now_Do(){

    //按键可选当前模式 
    switch( Now_Key ){
        case KEY_BACK: 
            NowPrinter_To0();
            Now_Key = KEY_NONE;
            break;

        case KEY_OK: 
            if (NowPrinter[1] == 0)  NowPrinter[0] = 2 ;
            else if(NowPrinter[1] == 1  )NowPrinter[0] = 3;
            else NowPrinter[0] = 4;
            // 无用位清零
            NowPrinter[1] = 0 ;
            Now_Key = KEY_NONE;
            break;

        case KEY_UP: 
        case KEY_RIGHT:
            //更改选中数据
            if(NowPrinter[1] == 0){
            }else {
                NowPrinter[1] -- ;
            }
            Now_Key = KEY_NONE;
            break;

        case KEY_DOWN:
        case KEY_LIFT:
            if(NowPrinter[1] == 2){
            }else {
                NowPrinter[1] ++ ;
            }
            Now_Key = KEY_NONE;
            break;
    }

}

void Inter_Now_Do(){
    //按键可调整数据
    switch( Now_Key ){
        case KEY_BACK:
            
            if ( NowPrinter[1] == 0 && NowPrinter[2] == 0 ) { // 已选中最前面 直接退出
                NowPrinter_To0(); 
            }
            else if( NowPrinter[2] == 0 ) {
                NowPrinter[2] = 5;
                NowPrinter[1] --;
            }
            else {
                NowPrinter[2] -- ;
            }

            Now_Key = KEY_NONE;
            break;

        case KEY_OK: 
            if ( NowPrinter[1] == 4 && NowPrinter[2] == 5 ) { // 已选中最后面 设置完成
                NowPrinter_To0(); 
                // 设置完成
                Up_Data_Do();

            }
            else if( NowPrinter[2] == 5 ) {
                NowPrinter[2] = 0;
                NowPrinter[1] ++;
            }
            else {
                NowPrinter[2] ++ ;
            }

            Now_Key = KEY_NONE;
            break;

        case KEY_UP: 
        case KEY_RIGHT:
            //更改选中数据
            switch(NowPrinter[1]){
                case 0 :
                    Change_Number_Do(&My_H , NowPrinter[2], 1);
                    Now_Key = KEY_NONE;
                    break;
                case 1 :
                    Change_Number_Do(&My_X, NowPrinter[2], 1);
                    Now_Key = KEY_NONE;
                    break;
                case 2 :
                    Change_Number_Do(&My_Y, NowPrinter[2], 1);
                    Now_Key = KEY_NONE;
                    break;
                case 3 :
                    Change_Number_Do(&My_Z, NowPrinter[2], 1);
                    Now_Key = KEY_NONE;
                    break;
                case 4 :
                    Change_Number_Do(&My_L, NowPrinter[2], 1);
                    Now_Key = KEY_NONE;
                    break;
            }
            break;

        case KEY_DOWN:
        case KEY_LIFT:
            //更改选中数据
            switch(NowPrinter[1]){
                case 0 :
                    Change_Number_Do(&My_H , NowPrinter[2], 0);
                    Now_Key = KEY_NONE;
                    break;
                case 1 :
                    Change_Number_Do(&My_X, NowPrinter[2], 0);
                    Now_Key = KEY_NONE;
                    break;
                case 2 :
                    Change_Number_Do(&My_Y, NowPrinter[2], 0);
                    Now_Key = KEY_NONE;
                    break;
                case 3 :
                    Change_Number_Do(&My_Z, NowPrinter[2], 0);
                    Now_Key = KEY_NONE;
                    break;
                case 4 :
                    Change_Number_Do(&My_L, NowPrinter[2], 0);
                    Now_Key = KEY_NONE;
                    break;
            }
            break;
    }
    
}

void Change_Number_Do(float * num , uint8_t set , uint8_t add_down ){
    // num 需要改的数
    // set  
        //  0 个位
        //  1 十位
        //  2 百位       
        //  3 十分位
        //  4 百分位
        //  5 千分位
    //Serial.printf("num:%f,set:%d,add_down:%d \n",*num, set, add_down);
    if( add_down == 1 ){    //加
        if( set < 3 ){
        	set = pow(10,set);
            if( (int)( *num / set ) % 10 != 9 )
                *num += set; 
        }
        else {
            float k = pow(0.1,set-2);
            if( (int)( *num / k ) % 10 != 9 )
                *num += k;
        }
    }else {                 //减
        if( set < 3 ){
        	set = pow(10,set);
			if( (int)( *num / set ) % 10 != 0 )
			    *num -= set; 
        }
        else {
        	float k = pow(0.1,set-2);
            if( (int)( *num / k ) % 10 != 0 )
                *num -= k ;
        }
    }
   // Serial.printf("num:%f,set:%d,add_down:%d \n",*num, set, add_down);
}

void Up_Data_Do(  ){
    PTZ.Init(My_H,My_X,My_Y,My_Z,My_L);
}

void Auto_Set_Do(){
    
    // 按键可选当前是否有墙 
    switch( Now_Key ){
        case KEY_BACK: 
            NowPrinter_To0();
            Now_Key = KEY_NONE;
            break;

        case KEY_OK: 
            if (NowPrinter[1] == 0)  NowPrinter[0] = 5 ;
            else if(NowPrinter[1] == 1  )NowPrinter[0] = 6;
            NowPrinter[1] = 0;
            Now_Key = KEY_NONE;
            break;

        case KEY_UP: 
        case KEY_RIGHT:
            //更改选中数据
            if(NowPrinter[1] == 0){
            }else {
                NowPrinter[1] -- ;
            }
            Now_Key = KEY_NONE;
            break;

        case KEY_DOWN:
        case KEY_LIFT:
            if(NowPrinter[1] == 1){
            }else {
                NowPrinter[1] ++ ;
            }
            Now_Key = KEY_NONE;
            break;
    }

}

void Set_Angle_Do(){
    //设定偏移角
    switch( Now_Key ){
        case KEY_BACK:
            
            if ( NowPrinter[1] == 0 ) { // 已选中最前面 直接退出
                NowPrinter_To0(); 
            }
            else {
                NowPrinter[1] -- ;
            }

            Now_Key = KEY_NONE;
            break;

        case KEY_OK: 
            if (NowPrinter[1] == 5 ) { // 已选中最后面 设置完成
                NowPrinter_To0(); 
                // 设置完成
                Up_Data_N_Do();

            }
            else {
                NowPrinter[1] ++ ;
            }

            Now_Key = KEY_NONE;
            break;

        case KEY_UP: 
        case KEY_RIGHT:
            //更改选中数据
            Change_Number_Do(&My_Excursion, NowPrinter[1], 1);
            Now_Key = KEY_NONE;
            break;

        case KEY_DOWN:
        case KEY_LIFT:
            //更改选中数据
            Change_Number_Do(&My_Excursion , NowPrinter[1], 0);
            Now_Key = KEY_NONE;
            break;
    }

}

void Auto_Set_No_Wall_Do(){
    
    switch( Now_Key ){
        case KEY_BACK: 
            if( NowPrinter[1] == 0 )
                NowPrinter_To0();
            else NowPrinter[1] -- ;
            Now_Key = KEY_NONE;
            break;

        case KEY_OK: 
            if( NowPrinter[1] == 1 ){
                // 上传数据
                PTZ.AutoSetGround(My_Alpha_1, My_Beta_1, My_Phi_1,My_Sigma_1,My_Alpha_2, My_Beta_2, My_Phi_2,My_Sigma_2);
                NowPrinter_To0();
            }
            else NowPrinter[1] ++ ;
            Now_Key = KEY_NONE;
            break;

        case KEY_UP:  //舵机向上
            if( NowPrinter[1] != 1 ){
                My_Phi_1 += 3 ;
            } else {
                My_Phi_2 += 3 ;
            }
            Now_Key = KEY_NONE;
            break;

        case KEY_RIGHT: // 舵机向右
            if( NowPrinter[1] != 1 ){
                My_Sigma_1 += 3 ;
            } else {
                My_Sigma_2 += 3 ;
            }
            Now_Key = KEY_NONE;
            break;

        case KEY_DOWN:     // 舵机向下
            if( NowPrinter[1] != 1 ){
                My_Phi_1 -= 3 ;
            } else {
                My_Phi_2 -= 3 ;
            }
            Now_Key = KEY_NONE;
            break;

        case KEY_LIFT:      //舵机向左
            if( NowPrinter[1] != 1 ){
                My_Sigma_1 -= 3 ;
            } else {
                My_Sigma_2 -= 3 ;
            }
            Now_Key = KEY_NONE;
            break;
    }
    
}

void Auto_Set_Have_Wall_Do(){
    
    switch( Now_Key ){
        case KEY_BACK: 
            if( NowPrinter[1] != 1 )
                NowPrinter_To0();
            else NowPrinter[1] -- ;
            Now_Key = KEY_NONE;
            break;

        case KEY_OK: 
            if( NowPrinter[1] == 1 ){
                PTZ.AutoSetGround(My_Alpha_1, My_Beta_1, My_Phi_1,My_Sigma_1,My_Alpha_2, My_Beta_2, My_Phi_2,My_Sigma_2);
                NowPrinter[1] ++;
            }else if ( NowPrinter[1] == 2 ){
                PTZ.AutoSetWall(My_Alpha_1,My_Beta_1,My_Phi_1,My_Sigma_1);
                NowPrinter_To0();
            }else NowPrinter[1] ++ ;
            Now_Key = KEY_NONE;
            break;

        case KEY_UP:  //舵机向上
            if( NowPrinter[1] != 1 ){
                My_Phi_1 += 3 ;
            } else {
                My_Phi_2+= 3 ;
            }
            Now_Key = KEY_NONE;
            break;

        case KEY_RIGHT: // 舵机向右
            if( NowPrinter[1] != 1 ){
                My_Sigma_1 += 3 ;
            } else {
                My_Sigma_2 += 3 ;
            }
            Now_Key = KEY_NONE;
            break;

        case KEY_DOWN:     // 舵机向下
            if( NowPrinter[1] != 1 ){
                My_Phi_1 -= 3 ;
            } else {
                My_Phi_2 -= 3 ;
            }
            Now_Key = KEY_NONE;
            break;

        case KEY_LIFT:      //舵机向左
            if( NowPrinter[1] != 1 ){
                My_Sigma_1 -= 3 ;
            } else {
                My_Sigma_2 -= 3 ;
            }
            Now_Key = KEY_NONE;
            break;
    }
}


void Up_Data_N_Do(){
    PTZ.Excursion = My_Excursion;
}


#endif



