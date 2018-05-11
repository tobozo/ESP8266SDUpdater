#ifndef __ESP8266UPDATER_H
#define __ESP8266UPDATER_H
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
 * 
 * This code is used by the menu but must also be included in
 * any app that will be compiled and copied the sd card.
 * 
 * Just add this in as early as possible in your sketch:
 * 
 *   #include "ESP8266SDUpdater.h"
 *   SDUpdater sdUpdater;
 *  
 * Insert this in your setup:
 * 
 *   if(digitalRead(BUTTON_A_PIN) == 0) {
 *     Serial.println("Will Load menu binary");
 *     sdUpdater.run(); // expecting a "MENU.BIN" file on the SD card
 *     ESP.restart();
 *   }
 * 
 * And do whatever you need to do (button init, timers)
 * in the setup and the loop. Your app will be ready 
 * to run normally except at boot if the Button A is 
 * pressed, it will load the MENU_BIN from the sd card
 * 
 * Arbitrary apps can be loaded as follows:
 * 
 *   sdUpdater.updateFromSD("MY_APP.BIN");
 * 
 * Note: It is assumed the SD Card is using the default pins for CS, 
 * MISO, MOSI and CLK. Only the CS pin can be changed using:
 * 
 *   #define SD_CS [pin-number]
 * 
 */

#include <SPI.h>
// Depending on the SDK version this may require a small patch in "FS.h", see
//     https://github.com/esp8266/Arduino/search?utf8=%E2%9C%93&q=FS_NO_GLOBALS
// If you also get namespace conflicts you may need to patch you app too:
//     https://github.com/esp8266/Arduino/issues/2281#issuecomment-258706478
#define FS_NO_GLOBALS // avoid namespace conflicts between SPIFFS and SD
#include <SD.h> // downgrade to version 1.1.1 of the SD.h library if necessary

static String MENU_BIN = "/MENU.BIN";

class SDUpdater {
  public: 
    void run();
    bool updateFromSD(String fileName = MENU_BIN );

  private:
    bool performUpdate(Stream &updateSource, size_t updateSize, String fileName);
};

/* don't break older versions of the SD Updater */
static void updateFromSD(String fileName = MENU_BIN ) {
  SDUpdater sdUpdater;
  sdUpdater.updateFromSD(fileName);
}

#endif 
