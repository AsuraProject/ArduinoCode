#include <Asura.h>

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
  
  asuraMain.begin();
  
  screenController.begin();
  screenController.clear();

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
      screenController.firstPage();
      do {
        draw(x, y, pixelPositionsSize, xString, yString, stringSerial, stringPositionsSizeBack);
      } while (screenController.nextPage());

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
  if (currentMillis - previousAccelMillis > 100){
    previousAccelMillis = currentMillis;
    if(asuraMain.accel('z') <= 145){
      previousAccelStartMillis = previousAccelStartMillis + (millis() - currentMillis);
    }
  }

  if(previousAccelStartMillis < 18 && previousAccelStartMillis != 0){
    if(asuraMain.accel('z') == 180){
      Serial1.println(0); // Up
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
      Serial1.println(2); // Left
      previousAccelRightMillis = 0;
    }
  }else{
    if(asuraMain.accel('y') <= 271){
      previousAccelRightMillis = 0;
    }
  }

  if(previousAccelLeftMillis < 200 && previousAccelLeftMillis != 0 ){ 
    if(asuraMain.accel('y') >= 271){
      Serial1.println(3); // Right
      previousAccelLeftMillis = 0;
    }else{
      if(asuraMain.accel('y') >= 271){
        previousAccelLeftMillis = 0;
      }
    }
  }
    
  if(previousAccelUpMillis < 200 && previousAccelUpMillis != 0 ){ 
    if(asuraMain.accel('z') == 180){
      Serial1.println(1); // Down
      previousAccelUpMillis = 0;
    }else{
      if(asuraMain.accel('z') == 180){
        previousAccelUpMillis = 0;
      }  
    }
  }      
}
