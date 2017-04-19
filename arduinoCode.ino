#include <Asura.h>

/* Principal */
bool started = false;
int layout = 0;

/* Gyro */
Asura asuraMain;

/* Gyro Millis */
long previousGyroMillis;
long previousGyroStartMillis;
long previousGyroRightMillis;
long previousGyroLeftMillis;
long previousGyroUpMillis;
  
/* Start */

void setup(){
  asuraMain.gyroStart();
  Serial.begin(9600);
}

void loop(){
  unsigned long currentMillis = millis();
  if (currentMillis - previousGyroMillis > 100){
    previousGyroMillis = currentMillis;
    if(asuraMain.gyro('z') <= 145){
      previousGyroStartMillis = previousGyroStartMillis + (millis() - currentMillis);
    }
  }

  if(previousGyroStartMillis < 18 && previousGyroStartMillis != 0 ){
     if(asuraMain.gyro('z') == 180){
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
    if(asuraMain.gyro('z') == 180){
      previousGyroStartMillis = 0;
    }
  }

  if(started == true){
    currentMillis = millis();
    if(asuraMain.gyro('y') >= 317){
      previousGyroRightMillis = previousGyroRightMillis + (millis() - currentMillis);      
    }

    currentMillis = millis();
    int gyroY = asuraMain.gyro('y');
    if(gyroY >= 216 && gyroY <= 219){
      previousGyroLeftMillis = previousGyroLeftMillis + (millis() - currentMillis);  
    }

    currentMillis = millis();
    if(asuraMain.gyro('z') <= 200 && asuraMain.gyro('z') >= 195){
      previousGyroUpMillis = previousGyroUpMillis + (millis() - currentMillis);  
    }
        
    if(previousGyroRightMillis < 200 && previousGyroRightMillis != 0 ){ 
      if(asuraMain.gyro('y') <= 271){
        Serial.println("Direita");
        previousGyroRightMillis = 0;
        layout++;
      }
    }else{
      if(asuraMain.gyro('y') <= 271){
        previousGyroRightMillis = 0;
      }
    }

    if(previousGyroLeftMillis < 200 && previousGyroLeftMillis != 0 ){ 
      if(asuraMain.gyro('y') >= 271){
        Serial.println("Esquerda");
        previousGyroLeftMillis = 0;
        if(layout != 0){
          layout--;
        }
      }
    }else{
      if(asuraMain.gyro('y') >= 271){
        previousGyroLeftMillis = 0;
      }
    }

    if(previousGyroUpMillis < 200 && previousGyroUpMillis != 0 ){ 
      if(asuraMain.gyro('z') == 180){
        Serial.println("Cima");
        previousGyroUpMillis = 0;
      }
    }else{
      if(asuraMain.gyro('z') == 180){
        previousGyroUpMillis = 0;
      }
    }
    
  }
}
