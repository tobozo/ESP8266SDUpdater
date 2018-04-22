
/*
 *
 * ESP8266 SD Menu Example
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
 * This example is based on an OLEDiESP from Hayri and is using a XPad from Radomir Dopieralski:
 *  https://www.tindie.com/products/PhoenixCNC/olediesp-a-tiny-cube-with-esp07--esp12--oled-iot/
 *  https://www.tindie.com/products/deshipu/x-pad-20-buttons-shield-for-d1-mini/
 *
 * It shouldn't be difficult to replicate with the most generic builds
 * as long as the default pins are used for the SD:
 * 
 * GPIO  4 SDA
 * GPIO  5 SCK/SCL for i2c
 * GPIO 12 MISO
 * GPIO 13 MOSI
 * GPIO 14 CLK
 * GPIO 15 CS 
 *
 *
 * This build is using the following hardware on the default pins:
 *  - ESP12-E
 *  - I2C SSD1306
 *  - I2C 8 buttons XPad
 *  - SPI Micro SDCard Reader
 *  - CP2102 UART
 *  
 */

#include <ESP8266SDUpdater.h>
#include "xpad8buttons.h"
#include "SSD1306Wire.h"

#define LEDPIN      BUILTIN_LED
#define MAX_FILES   5 // set more and you overflow the display, no pagination so far :p

SSD1306Wire display = SSD1306Wire(0x3c, SDA, SCL); // for 0.96" OLED
File root;

/* filenames cache structure */
struct FileInfo {
  String fileName;  // the binary name
  size_t fileSize;
};

FileInfo fileInfo[MAX_FILES];
uint16_t appsCount = 0;
bool toggle = false;
int16_t virtualPos = 0;


void getFileInfo(File &file) {
  String fileName = String(file.name());
  size_t fileSize = file.size();
  fileInfo[appsCount].fileName = fileName;
  fileInfo[appsCount].fileSize = fileSize;
}


void getDirectory(File dir, int numTabs) {
  int colcnt =0;
  while(true) {
    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    if (numTabs > 0) {
      for (uint8_t i=0; i<=numTabs; i++) {
         Serial.print('\t');
      }
    }
    Serial.print(entry.name());

    getFileInfo(entry);
    appsCount++;
    
    if (entry.isDirectory()) {
       Serial.println("/");
       getDirectory(entry, numTabs+1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}


void getSDRoot() {
  root = SD.open("/");
  root.rewindDirectory();
  getDirectory(root, 0); //Display the card contents
  root.close();
  Serial.print("Files count : ");
  Serial.println( appsCount );
}


void renderUi() {
  display.clear();
  display.setColor(WHITE);
  display.drawString(0, virtualPos*12, "*");
  for(uint16_t i=0;i<appsCount;i++) {
    display.drawString(10, i*12, fileInfo[i].fileName);
  }
  display.display();
}


void setup() {
  Serial.begin(115200);
  Serial.println("Setting up");

  display.init();
  display.setContrast(255);
  //display.flipScreenVertically();
  
  while(!SD.begin()) {
    // TODO: make a more fancy animation
    Serial.println("Insert SD");
    display.clear();
    display.display();
    delay(300);
    display.drawString(10, 20, "INSERT SD");
    display.display();
  }

  Serial.println("SD Detected");
  display.clear();
  display.drawString(10, 20, "SD Detected");
  display.display();
  delay(500);
  display.clear();
  display.display();

  buttonsPadInit();
  getSDRoot();
  renderUi();
}


void loop() {
  handleButtonsPad();

  if(padButtons[0].pushed()==true) {
    Serial.println("Up");
    if(virtualPos+1<appsCount) {
      virtualPos++;
      delay(500);
    }
    renderUi();
  }
  
  if(padButtons[3].pushed()==true) {
    Serial.println("Down");
    if(virtualPos-1>=0) {
      virtualPos--;
      delay(500);
    }
    renderUi();
  }

  if(padButtons[6].pushed()==true) {
    Serial.print("Will load ");
    Serial.println(fileInfo[virtualPos].fileName);
    delay(1000);
    updateFromSD(fileInfo[virtualPos].fileName);
    ESP.restart();
  }
}


