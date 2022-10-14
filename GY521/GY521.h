#ifndef _GY521_H_
#define _GY521_H_

#ifndef G
#define G 0.97
#endif
#ifndef PI
#define PI 3.141592
#endif

float GY521_Rad_to_Angle(float rad);


float GY521_Rad_to_Angle(float rad){
    return rad * 180 / PI;
}

void Get_Alphi(){
    sensor.read();
    float alpha;
    float y = sensor.getAccelY();
    float z = sensor.getAccelZ();
    alpha = GY521_Rad_to_Angle( acos(z/G) );
    if(isnan(alpha)){
        alpha = 0;  
    }
    if( y>0 ){
        // 抬头 
        alpha += 90; 
    }else{
        alpha = 90 - alpha;
    }

    if( alpha > 180 || alpha < 0 ) Alpha = 45 ; else 
    if ( pow( ( Alpha - alpha) , 2 ) > 9 ) Alpha = alpha ;
    Serial.printf("Get_Alpha OK (Alpha: %f\n)",Alpha);

}

#endif 

