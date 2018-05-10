/*
 *
 * ESP8266 SD Updater
 * Project Page: https://github.com/tobozo/ESP8266SDUpdater
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
 */

#include "ESP8266SDUpdater.h"

bool SDUpdater::performUpdate(Stream &updateSource, size_t updateSize, String fileName) {
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
        return true;
      } else {
        Serial.println("Update not finished? Something went wrong!");
      }
    } else {
      Serial.println("Error Occurred. Error #: " + String(Update.getError()));
    }
  } else {
    Serial.println("Not enough space to begin FOTA");
  }
  return false;
}


bool SDUpdater::updateFromSD(String fileName) {
  File root = SD.open("/");
  File updateBin = SD.open(fileName);
  size_t updateSize = updateBin.size();
  bool ret = false;
  if (updateSize > 0) {
    Serial.println("Try to start update");
    ret = performUpdate(updateBin, updateSize, fileName);
  } else {
    Serial.println("Error, file is empty, did you SD.begin()?");
  }
  updateBin.close();
  return ret;
}


void SDUpdater::run() {
  #ifdef SD_CS
    SD.begin(SD_CS);
  #else
   SD.begin();
  #endif
  Serial.println("Will load MENU");
  updateFromSD(MENU_BIN);
  ESP.restart();
}