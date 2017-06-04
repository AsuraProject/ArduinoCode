#include <Asura.h>
#include "U8glib.h"

/* Principal */
Asura asuraMain;

bool started = false;
int layout = 0;

/* Gyro Millis */
long previousGyroMillis;
long previousGyroStartMillis;
long previousGyroRightMillis;
long previousGyroLeftMillis;
long previousGyroUpMillis;

/* Screen */
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);

char *arraySerial = (char*) calloc(0, sizeof(char));
int *x = (int*) calloc(0, sizeof(int));
int *y = (int*) calloc(0, sizeof(int));
int *pixelPositionsSize = (int*) calloc(0, sizeof(int));

char **stringSerial = (char**) calloc(0, sizeof(char));
int *stringPositionsSize = (int*) calloc(0, sizeof(int));
int *xString = NULL;
int *yString = NULL;

String stringBase = "";
bool bluetoothStart = false;  

/* Bluetooth Functions */

void getBluetoothResults(char *arraySerial, int *pixelPositionsSize, int *xString, int *yString, int *stringPositionsSize, int stringPositionsSizeBack, char **stringSerial, String stringBase) {
  char caractereSerial;
  bool stringLevel = false;
  int pixelsSize;
  int counter = 0;  
  int temp = *stringPositionsSize;
  String stringSerialBase = "";
  String pixelSerialBase = "";

  while(stringBase[counter] != '`'){
    caractereSerial = stringBase[counter];
    if (stringLevel == true) {
        stringSerialBase.concat(caractereSerial);         
    } else {
      if (caractereSerial != '#') {
        pixelSerialBase.concat(caractereSerial);          
      } else {
        stringLevel = true;                    
      }
    }
    counter++;      
  }
  
  if(stringSerialBase != ""){
    temp++;
    stringSerial[stringPositionsSizeBack] = (char*) calloc(stringSerialBase.length(), sizeof(char));
    stringSerialBase.toCharArray(stringSerial[stringPositionsSizeBack], stringSerialBase.length() + 1);
    *stringPositionsSize = temp;
  }

  if(pixelSerialBase != ""){
    pixelSerialBase.toCharArray(arraySerial, pixelSerialBase.length() + 1);
    (*pixelPositionsSize)++;
  }
}

/* Screen Functions */

void draw(int *x, int *y, int *pixelPositionsSize, int *xString, int *yString, char **stringSerial, int stringPositionsSizeBack) {
  u8g.setFont(u8g_font_04b_03);
  
  int counter = 0;
  String drawString;
  
  while (counter != *pixelPositionsSize) {
    u8g.drawPixel(x[counter], y[counter]);
    counter++;
  }

  counter = 0;
  while(counter != stringPositionsSizeBack){   
    u8g.drawStr(xString[counter], yString[counter], stringSerial[counter]);
    counter++;
  }
}

void getPixelPositions(char *arraySerial, int *x, int *y, int pixelPositionsSizeBack) {
  String positionString;

  positionString = String(String(arraySerial[0]) + String(arraySerial[1]));
  x[pixelPositionsSizeBack] = positionString.toInt();
  positionString = String(String(arraySerial[2]) + String(arraySerial[3]));
  y[pixelPositionsSizeBack] = positionString.toInt();

  free(arraySerial);
}

void getStringPositions(char **stringSerial, int *xString, int *yString, int stringPositionsSizeBack){    
  String intString;
  int counter = 0;

  while(counter != stringPositionsSizeBack){
    intString = String(String(stringSerial[counter][0]) + String(stringSerial[counter][1]));
      xString[counter] = intString.toInt();
      intString = String(String(stringSerial[counter][2]) + String(stringSerial[counter][3]));
      yString[counter] = intString.toInt();
      intString = String(stringSerial[counter]); 
        
      intString.remove(0, 4);
      stringSerial[counter] = (char*) calloc(intString.length() + 1, sizeof(char));
      intString.toCharArray(stringSerial[counter], intString.length() + 1);
      counter++;
  }
}


/* Start */

void setup(){
  Serial.begin(57600);
  Serial1.begin(57600);
  asuraMain.gyroStart();

  free(x);
  free(y);
  free(arraySerial);
  arraySerial = (char*) calloc(0, sizeof(char));
  x = (int*) calloc(0, sizeof(int));
  y = (int*) calloc(0, sizeof(int));
  *pixelPositionsSize = 0;
              
  free(xString);
  free(yString);
  free(stringSerial);
  *stringPositionsSize = 0;
  stringSerial = (char**) calloc(0, sizeof(char));
  xString = (int*) calloc(0, sizeof(int));
  yString = (int*) calloc(0, sizeof(int));
}

void loop(){

  /* Screen */

  if(Serial1.available()){
    char caractereSerial;
    while (Serial1.available() > 0) {
      caractereSerial = Serial1.read(); 
      stringBase.concat(caractereSerial);
      if((int) caractereSerial == 96){
        bluetoothStart = true;
      }
    }
  } 

  if (bluetoothStart == true) {
    int stringPositionsSizeBack = *stringPositionsSize;
    int pixelPositionsSizeBack = *pixelPositionsSize;
    
    stringSerial = (char**) realloc(stringSerial, (stringPositionsSizeBack + 1) * sizeof(char*));
    arraySerial = (char*) calloc(4, sizeof(char));
    
    getBluetoothResults(arraySerial, pixelPositionsSize, xString, yString, stringPositionsSize, stringPositionsSizeBack, stringSerial, stringBase);
    stringBase = "";
    bluetoothStart = false;
    x = (int*) realloc(x, (pixelPositionsSizeBack + 1) * sizeof(int));
    y = (int*) realloc(y, (pixelPositionsSizeBack + 1) * sizeof(int));    

    if (strcmp (stringSerial[stringPositionsSizeBack], "ended") != 0) { 
      if (x == NULL || y == NULL) {
        Serial.println("Memory Error");            
      }else{    
        getPixelPositions(arraySerial, x, y, pixelPositionsSizeBack);
      }
    } else {
      xString = (int*) calloc(stringPositionsSizeBack, sizeof(int));
      yString = (int*) calloc(stringPositionsSizeBack, sizeof(int));
      getStringPositions(stringSerial, xString, yString, stringPositionsSizeBack);
      u8g.firstPage();
      do {
        draw(x, y, pixelPositionsSize, xString, yString, stringSerial, stringPositionsSizeBack);
      } while (u8g.nextPage());

      free(x);
      free(y);
      free(arraySerial);
      arraySerial = (char*) calloc(0, sizeof(char));
      x = (int*) calloc(0, sizeof(int));
      y = (int*) calloc(0, sizeof(int));
      *pixelPositionsSize = 0;
                  
      free(xString);
      free(yString);
      free(stringSerial);
      *stringPositionsSize = 0;
      stringSerial = (char**) calloc(0, sizeof(char));
      xString = (int*) calloc(0, sizeof(int));
      yString = (int*) calloc(0, sizeof(int));

    }   
  }

  /* Accelerometer */
  
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
