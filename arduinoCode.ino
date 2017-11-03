/* Copyright 2017 Yuri Faria

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */

#include <Asura.h>
#include <MemoryFree.h>

/* Principal */
Asura asuraMain;

/* Accelerometer Millis */
long previousAccelMillis;
long previousAccelStartMillis;
long previousAccelRightMillis;
long previousAccelLeftMillis;
long previousAccelUpMillis;

/* Screen */
U8G2_SSD1306_128X64_NONAME_1_HW_I2C screenController(U8G2_R0, 8, 13, 11);

char **pixelSerial = (char**) calloc(0, sizeof(char));
int *pixelPositionsSize = (int*) calloc(0, sizeof(int));
int *x = NULL;
int *y = NULL;

char **stringSerial = (char**) calloc(0, sizeof(char));
int *stringPositionsSize = (int*) calloc(0, sizeof(int));
int *xString = NULL;
int *yString = NULL;

String stringBase = "";
bool bluetoothStart = false;

/* Bluetooth Functions */

void getBluetoothResults(char **pixelSerial, int *pixelPositionsSize, int pixelPositionsSizeBack, int *xString, int *yString, int *stringPositionsSize, int stringPositionsSizeBack, char **stringSerial, String stringBase) {
  char caractereSerial;
  bool stringLevel = false;
  int pixelsSize;
  int counter = 0;  
  int temp = *stringPositionsSize;
  int temp2 = *pixelPositionsSize;
  String stringSerialBase = "";
  String pixelSerialBase = "";

  while(stringBase[counter] != '\n'){
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
    if(stringSerial == NULL){
        freeAll();
        Serial1.println("9");
      }
    stringSerialBase.toCharArray(stringSerial[stringPositionsSizeBack], stringSerialBase.length() + 1);
    *stringPositionsSize = temp;
  }

  if(pixelSerialBase != ""){
    temp2++;
    pixelSerial[pixelPositionsSizeBack] = (char*) calloc(pixelSerialBase.length(), sizeof(char));
    if(pixelSerial == NULL){
        freeAll();
        Serial1.println("9");
      }
    pixelSerialBase.toCharArray(pixelSerial[pixelPositionsSizeBack], pixelSerialBase.length() + 1);
    *pixelPositionsSize = temp2;
  }
}

/* Screen Functions */

void draw(int *x, int *y, int *pixelPositionsSize, int *xString, int *yString, char **stringSerial, int stringPositionsSizeBack) {
  screenController.setFont(u8g2_font_u8glib_4_tf);
  
  int counter = 0;
  String drawString;

  while (counter != *pixelPositionsSize) {
    screenController.drawPixel(32 + x[counter], 32 + y[counter]);
    counter++;
  }

  counter = 0;
  while(counter != stringPositionsSizeBack){   
    screenController.drawStr(32 + xString[counter], 32 + yString[counter], stringSerial[counter]);
    counter++;
  }
    Serial.print("Free Memory - After Draw: ");
    Serial.println(freeMemory());  
}

void getPixelPositions(char **pixelSerial, int *x, int *y, int pixelPositionsSizeBack) {
  String positionString;
  int counter = 0;

  while(counter != pixelPositionsSizeBack){
    positionString = String(String(pixelSerial[counter][0]) + String(pixelSerial[counter][1]));
    x[counter] = positionString.toInt();
    positionString = String(String(pixelSerial[counter][2]) + String(pixelSerial[counter][3]));
    y[counter] = positionString.toInt();

    
    counter++;
  }
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

void freeAll(){
  Serial.print("Free Memory - Before FreeAll(): ");
  Serial.println(freeMemory());
  free(x);
  free(y);
  free(pixelSerial);
  pixelSerial = NULL;
  x = NULL;
  y = NULL;
  *pixelPositionsSize = 0;
              
  free(xString);
  free(yString);
  free(stringSerial);
  *stringPositionsSize = 0;
  stringSerial = NULL;
  xString = NULL;
  yString = NULL; 

  Serial.print("Free Memory - After FreeAll(): ");
  Serial.println(freeMemory());
  Serial.println("--------------------------------------");   
}

void setup(){
  Serial.begin(57600);
  Serial1.begin(57600);

  while(!Serial){
    
  }
  
  asuraMain.begin();
  
  screenController.begin();
  screenController.clear();

  Serial.print("Free Memory - Setup: ");
  Serial.println(freeMemory());
  freeAll();
}

void loop(){
  /* Screen */

  if(Serial1.available()){
    char caractereSerial;
    while (Serial1.available()) {
      caractereSerial = Serial1.read(); 
      stringBase.concat(caractereSerial); 
      if(caractereSerial == '\n'){
        if(stringBase[0] != ' ' && stringBase.length() >= 5){
          bluetoothStart = true;
        }else{
          //Serial.println("Barrado");
          stringBase = "";
        }
      }
    }
  } 

  if (bluetoothStart) {
    Serial.print("Free Memory - Before BluetoothStart: ");
    Serial.println(freeMemory());    
    int stringPositionsSizeBack = *stringPositionsSize;
    int pixelPositionsSizeBack = *pixelPositionsSize;
    
    stringSerial = (char**) realloc(stringSerial, (stringPositionsSizeBack + 1) * sizeof(char*));
    pixelSerial = (char**) realloc(pixelSerial,(pixelPositionsSizeBack + 1) * sizeof(char*));

      if(stringSerial == NULL && pixelSerial == NULL){
        Serial1.println("9");
        freeAll();
      }

    //Serial.print(stringBase);
    
    getBluetoothResults(pixelSerial, pixelPositionsSize, pixelPositionsSizeBack, xString, yString, stringPositionsSize, stringPositionsSizeBack, stringSerial, stringBase);
    stringBase = "";
    bluetoothStart = false;

    if (strcmp (stringSerial[stringPositionsSizeBack], "ended") == 0) { 
      //Serial.println("");
      
      xString = (int*) calloc(stringPositionsSizeBack, sizeof(int));
      yString = (int*) calloc(stringPositionsSizeBack, sizeof(int));
      x = (int*) calloc(pixelPositionsSizeBack, sizeof(int));
      y = (int*) calloc(pixelPositionsSizeBack, sizeof(int));    

      if(xString == NULL || yString == NULL || x == NULL || y == NULL){
        Serial1.println("9");
        freeAll();
      }

      Serial.print("Free Memory - Before Draw: ");
      Serial.println(freeMemory());

      getStringPositions(stringSerial, xString, yString, stringPositionsSizeBack);
      getPixelPositions(pixelSerial, x, y, pixelPositionsSizeBack);
      screenController.firstPage();
      do {
        draw(x, y, pixelPositionsSize, xString, yString, stringSerial, stringPositionsSizeBack);
      } while (screenController.nextPage());

      freeAll();
    }   
  }

  /* Accelerometer */
  
  unsigned long currentMillis = millis();
  if (currentMillis - previousAccelMillis > 100){
    previousAccelMillis = currentMillis;
    if(asuraMain.accel('z') <= 145){
      previousAccelStartMillis = previousAccelStartMillis + (millis() - currentMillis);
    }
  }

  if(previousAccelStartMillis < 18 && previousAccelStartMillis != 0){
    if(asuraMain.accel('z') == 180){
      //Serial.println("Up");
      //Serial1.println(0); // Up
      previousAccelStartMillis = 0;
     }
  }else{
    if(asuraMain.accel('z') == 180){
      previousAccelStartMillis = 0;
    }
  }

  currentMillis = millis();
  if(asuraMain.accel('y') >= 317){
    previousAccelRightMillis = previousAccelRightMillis + (millis() - currentMillis);      
  }

  currentMillis = millis();
  int accelY = asuraMain.accel('y');
  if(accelY >= 216 && accelY <= 219){
    previousAccelLeftMillis = previousAccelLeftMillis + (millis() - currentMillis);  
  }

  currentMillis = millis();
  if(asuraMain.accel('z') <= 200 && asuraMain.accel('z') >= 195){
    previousAccelUpMillis = previousAccelUpMillis + (millis() - currentMillis);  
  }
        
  if(previousAccelRightMillis < 200 && previousAccelRightMillis != 0 ){ 
    if(asuraMain.accel('y') <= 271){
      //Serial.println("Left");
      //Serial1.println(2); // Left
      previousAccelRightMillis = 0;
    }
  }else{
    if(asuraMain.accel('y') <= 271){
      previousAccelRightMillis = 0;
    }
  }

  if(previousAccelLeftMillis < 200 && previousAccelLeftMillis != 0 ){ 
    if(asuraMain.accel('y') >= 271){
      //Serial.println("Right");
      //Serial1.println(3); // Right
      previousAccelLeftMillis = 0;
    }else{
      if(asuraMain.accel('y') >= 271){
        previousAccelLeftMillis = 0;
      }
    }
  }
    
  if(previousAccelUpMillis < 200 && previousAccelUpMillis != 0 ){ 
    if(asuraMain.accel('z') == 180){
      //Serial.println("Down");
      //Serial1.println(1); // Down
      previousAccelUpMillis = 0;
    }else{
      if(asuraMain.accel('z') == 180){
        previousAccelUpMillis = 0;
      }  
    }
  }

   if(previousAccelUpMillis > 500 && previousAccelUpMillis < 1000 && previousAccelUpMillis != 0 ){ 
    if(asuraMain.accel('z') == 180){
      //Serial.println("Destroy");
      //Serial1.println(4); // Destroy
      previousAccelUpMillis = 0;
    }else{
      if(asuraMain.accel('z') == 180){
        previousAccelUpMillis = 0;
      }
    }
  }

  if(Serial.available()){
      char readA = Serial.read();
      if(readA == '0'){ Serial1.println(0); /*Serial.println("Up");*/}
      if(readA == '1'){ Serial1.println(1); /*Serial.println("Down"); */}
      if(readA == '2'){ Serial1.println(2); /*Serial.println("Left"); */}
      if(readA == '3'){ Serial1.println(3); /*Serial.println("Right"); */}
      if(readA == '4'){ Serial1.println(4); /*Serial.println("Destroy"); */}
  }
}
