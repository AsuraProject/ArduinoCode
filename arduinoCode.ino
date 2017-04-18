#include <Wire.h>

/* Principal */
bool started = false;
int layout = 0;

/* Mirillis */
long previousGyroMillis;
long gyroMillis = 100;
long previousGyroStartMillis;
long previousGyroRightMillis;
long previousGyroLeftMillis;
long previousGyroUpMillis;

/* Gyro */
#define MPU_addr 0x68
#define minVal 265
#define maxVal 402

void gyroStart(){
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  Serial.begin(9600);
}


int gyro(char direction){
  int16_t AcX,AcY,AcZ;
  double x;
  double y;
  double z;
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);
  AcX=Wire.read()<<8|Wire.read();
  AcY=Wire.read()<<8|Wire.read();
  AcZ=Wire.read()<<8|Wire.read();
  int xAng = map(AcX,minVal,maxVal,-90,90);
  int yAng = map(AcY,minVal,maxVal,-90,90);
  int zAng = map(AcZ,minVal,maxVal,-90,90);
  x= RAD_TO_DEG * (atan2(-yAng, -zAng)+PI);
  y= RAD_TO_DEG * (atan2(-xAng, -zAng)+PI);
  z= RAD_TO_DEG * (atan2(-yAng, -xAng)+PI);

  if(direction == 'y'){
    return(y);
  }

  if(direction == 'x'){
    return(x);
  }

  if(direction == 'z'){
    return(z);
  }
}

/* Start */

void setup(){
  gyroStart();
}

void loop(){
  unsigned long currentMillis = millis();
  if (currentMillis - previousGyroMillis > gyroMillis){
    previousGyroMillis = currentMillis;
    if(gyro('z') <= 145){
      previousGyroStartMillis = previousGyroStartMillis + (millis() - currentMillis);
    }
  }

  if(previousGyroStartMillis < 18 && previousGyroStartMillis != 0 ){
     if(gyro('z') == 180){
       if(started == false){
         started = true;
         Serial.println("Ligado");
         previousGyroStartMillis = 0;  
       }else{
         if(layout == 0){
          started = false;
          Serial.println("Desligado");
          previousGyroStartMillis = 0;
         }else{
          Serial.println("Abaixo");
          previousGyroStartMillis = 0;
         }
       }
     }
  }else{
    if(gyro('z') == 180){
      previousGyroStartMillis = 0;
    }
  }

  if(started == true){
    currentMillis = millis();
    if(gyro('y') >= 317){
      previousGyroRightMillis = previousGyroRightMillis + (millis() - currentMillis);      
    }

    currentMillis = millis();
    int gyroY = gyro('y');
    if(gyroY >= 216 && gyroY <= 219){
      previousGyroLeftMillis = previousGyroLeftMillis + (millis() - currentMillis);  
    }

    currentMillis = millis();
    if(gyro('z') <= 200 && gyro('z') >= 195){
      previousGyroUpMillis = previousGyroUpMillis + (millis() - currentMillis);  
    }
        
    if(previousGyroRightMillis < 200 && previousGyroRightMillis != 0 ){ 
      if(gyro('y') <= 271){
        Serial.println("Direita");
        previousGyroRightMillis = 0;
        layout++;
      }
    }else{
      if(gyro('y') <= 271){
        previousGyroRightMillis = 0;
      }
    }

    if(previousGyroLeftMillis < 200 && previousGyroLeftMillis != 0 ){ 
      if(gyro('y') >= 271){
        Serial.println("Esquerda");
        previousGyroLeftMillis = 0;
        if(layout != 0){
          layout--;
        }
      }
    }else{
      if(gyro('y') >= 271){
        previousGyroLeftMillis = 0;
      }
    }

    if(previousGyroUpMillis < 200 && previousGyroUpMillis != 0 ){ 
      if(gyro('z') == 180){
        Serial.println("Cima");
        previousGyroUpMillis = 0;
      }
    }else{
      if(gyro('z') == 180){
        previousGyroUpMillis = 0;
      }
    }
    
  }
}
