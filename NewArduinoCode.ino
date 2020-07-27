#include <U8g2lib.h>
#include <SPI.h>
#include <MemoryFree.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C screenController(U8G2_R0, 8, 13, 11);
String stringBase;
bool bluetoothStart = false;
bool isFirst = true;

void setup() {
  Serial.begin(57600);
  Serial1.begin(57600);

  screenController.begin();
}

void loop() {
  if(Serial1.available()){
    char caractereSerial;
    while (Serial1.available()) {
      caractereSerial = Serial1.read(); 
      if(caractereSerial == '\n'){
          bluetoothStart = true;
      }else{
        stringBase.concat(caractereSerial); 
      }
    }
  }

  if(bluetoothStart){
    Serial.println(stringBase);
    
    if(isFirst && !stringBase.equals("#ended")){
      screenController.clearBuffer();
      isFirst = false;
    }
    
    if(stringBase.equals("#ended")){
      screenController.sendBuffer();
      isFirst = true;    
      stringBase = "";
      bluetoothStart = false;
      Serial.println("");
      return;
    }

    if(stringBase.charAt(1) == 'f'){
      Serial.println(stringBase.substring(2));
      Serial.println(stringBase.substring(2).toInt());
      if(stringBase.substring(2) == "Freedom"){
        screenController.setFont(u8g2_font_freedoomr10_tu);
      }else{
        switch(stringBase.substring(2).toInt()){
          case 4:
            screenController.setFont(u8g2_font_u8glib_4_tf);
            break;
          case 6:
            screenController.setFont(u8g2_font_5x7_tf);
            break;
          case 8:
            screenController.setFont(u8g2_font_6x10_tf);
            Serial.println("FONT 8");
            break;
          case 12:
            screenController.setFont(u8g2_font_7x14_tf);
            break;
          default:
            screenController.setFont(u8g2_font_u8glib_4_tf);
        }
      }
      stringBase = "";
      bluetoothStart = false;
      return;
    }
    
    if(stringBase.charAt(0) == '#'){
      char stringArray[stringBase.length() - 4];
      String text = stringBase.substring(5);
      text.toCharArray(stringArray, stringBase.length() - 4);
      screenController.drawStr(32 + (stringBase.substring(1, 3)).toInt(), 32 + (stringBase.substring(3, 5)).toInt(), stringArray);
    }else{
      screenController.drawPixel(32 + (stringBase.substring(0, 2)).toInt(), (stringBase.substring(2, 4)).toInt());
    }

    stringBase = "";
    bluetoothStart = false;
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
