#ifndef __ESP8266UPDATER_H
#define __ESP8266UPDATER_H
/*
 *
 * ESP8266 SD Updater
 * Project Page: https://github.com/tobozo/ESP8266-SD-Updater
 * 
 * Copyright 2018 tobozo http://github.com/tobozo
 *
 * Permission is hereby granted, free of charge, to any person 
 * obtaining a copy of this software and associated documentation 
 * files ("ESP8266 SD Updater"), to deal in the Software without 
 * restriction, including without limitation the rights to use, 
 * copy, modify, merge, publish, distribute, sublicense, and/or 
 * sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following 
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be 
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES 
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR 
 * OTHER DEALINGS IN THE SOFTWARE.
 * 
 * 
 * 
 * This code is used by the menu but must also be included in
 * any app that will be compiled and copied the sd card.
 * 
 * 
 *  Just add this in your sketch:
 * 
 *  #include "ESP8266Updater.h"
 *  
 * 
 *   if(digitalRead(BUTTON_A_PIN) == 0) {
 *     Serial.println("Will Load menu binary");
 *     updateFromFS(SD);
 *     ESP.restart();
 *   }
 * 
 * And do whatever you need to do (button init, timers)
 * in the setup and the loop. Your app will be ready 
 * to run normally except at boot if the Button A is 
 * pressed, it will load the MENU_BIN from the sd card
 * 
 */

#include <SPI.h>
//#define FS_NO_GLOBALS // depending on the SDK version this may require a small patch
//#include <FS.h> // see https://github.com/esp8266/Arduino/search?utf8=%E2%9C%93&q=FS_NO_GLOBALS
#include <SD.h> // if you get namespace conflicts, patch your FS.h!

String MENU_BIN = "MENU.BIN";

void displayUpdateUI(String label) {
  /*
  display.setBrightness(100);
  display.fillScreen(BLACK);
  display.setCursor(10, 10);
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.printf(label.c_str());
  display.drawRect(16, 20, 100, 24, WHITE);
  */
}


void ESP8266SDMenuProgress(int state, int size) {
  int percent = (state*100) / size;
  Serial.printf("percent = %d\n", percent);
  if (percent > 0) {
    //display.drawRect(16, 20, percent, 24, BLACK);
  }
}

// perform the actual update from a given stream
void performUpdate(Stream &updateSource, size_t updateSize, String fileName) {
   displayUpdateUI("LOADING " + fileName);
   //Update.onProgress(ESP8266SDMenuProgress);
   if (Update.begin(updateSize)) {      
      size_t written = Update.writeStream(updateSource);
      if (written == updateSize) {
         Serial.println("Written : " + String(written) + " successfully");
      } else {
         Serial.println("Written only : " + String(written) + "/" + String(updateSize) + ". Retry?");
      }
      if (Update.end()) {
         Serial.println("FOTA done!");
         if (Update.isFinished()) {
            Serial.println("Update successfully completed. Rebooting.");
         } else {
            Serial.println("Update not finished? Something went wrong!");
         }
      } else {
         Serial.println("Error Occurred. Error #: " + String(Update.getError()));
      }
   } else {
      Serial.println("Not enough space to begin FOTA");
   }
}

// check given FS for valid MENU_BIN and perform update if available
void updateFromSD(String fileName = MENU_BIN ) {
  SD.begin();
  File root = SD.open("/");

  File updateBin = SD.open(fileName);

  size_t updateSize = updateBin.size();
  if (updateSize > 0) {
    // TODO: check max flashable size (no limit found so far)
    Serial.println("Try to start update");
    performUpdate(updateBin, updateSize, fileName);
  } else {
     Serial.println("Error, file is empty");
  }
  updateBin.close();

}
#endif 
