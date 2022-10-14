#include "GY521.h"
#include <math.h>

#define G 0.97
#define PI 3.1415

GY521 sensor(0x68);


uint32_t counter = 0;



float Rad_to_Angle(float rad){
    return rad * 180 / PI;
}

void setup()
{
  Serial.begin(115200);
  Serial.println(__FILE__);

  Wire.begin(18,5);

  delay(100);
  while (sensor.wakeup() == false)
  {
    Serial.print(millis());
    Serial.println("\tCould not connect to GY521");
    delay(1000);
  }
  sensor.setAccelSensitivity(0);  // 8g
  sensor.setGyroSensitivity(1);   // 500 degrees/s

  sensor.setThrottle();
  Serial.println("start...");

  // set calibration values from calibration sketch.
  sensor.axe = 0;
  sensor.aye = 0;
  sensor.aze = 0;
  sensor.gxe = 0;
  sensor.gye = 0;
  sensor.gze = 0;
}


void loop()
{
  sensor.read();
  float x = sensor.getAccelX(); 
  float z = sensor.getAccelZ();
  float y = sensor.getAccelY();
  float alpha = Rad_to_Angle( acos(z/G) );
  //当无解时为0 
  if(isnan(alpha)){
    alpha = 0;  
  }
  // 抬头Y为+ 低头Y为-
  if( y>0 ){
    // 抬头 
    alpha +=90; 
   }else{
    alpha = 90 - alpha;
    }
  
  
  
  Serial.printf("%f\t %f\t %f\t %f\t ",x,y,z,alpha);
  Serial.println();

  counter++;
}


// -- END OF FILE --
