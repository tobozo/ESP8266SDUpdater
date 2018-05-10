
/*

   ESP8266 SD Menu Example
   Project Page: https://github.com/tobozo/ESP8266SDUpdater

   Copyright 2018 tobozo http://github.com/tobozo

   Permission is hereby granted, free of charge, to any person
   obtaining a copy of this software and associated documentation
   files ("ESP8266 SD Updater"), to deal in the Software without
   restriction, including without limitation the rights to use,
   copy, modify, merge, publish, distribute, sublicense, and/or
   sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following
   conditions:

   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
   OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
   HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
   OTHER DEALINGS IN THE SOFTWARE.


   This example is based on an OLEDiESP from Hayri and is using a XPad from Radomir Dopieralski:
    https://www.tindie.com/products/PhoenixCNC/olediesp-a-tiny-cube-with-esp07--esp12--oled-iot/
    https://www.tindie.com/products/deshipu/x-pad-20-buttons-shield-for-d1-mini/

   It shouldn't be difficult to replicate with the most generic builds
   as long as the default pins are used for the SD:

   GPIO  4 SDA
   GPIO  5 SCK/SCL for i2c
   GPIO 12 MISO
   GPIO 13 MOSI
   GPIO 14 CLK
   GPIO 15 CS


   This build is using the following hardware on the default pins:
    - ESP12-E
    - I2C SSD1306
    - I2C 8 buttons XPad
    - SPI Micro SDCard Reader
    - CP2102 UART

*/

#include <ESP8266SDUpdater.h>
#include <FS.h>
#include "xpad8buttons.h"
#include "SSD1306Wire.h"

#define LEDPIN      BUILTIN_LED
#define MAX_FILES   5 // set more and you overflow the display, no pagination so far :p
#define MENU_ITEM_HEIGHT 12 // pixel height for one menu item

SSD1306Wire display = SSD1306Wire(0x3c, SDA, SCL); // for 0.96" OLED
File root;
SDUpdater sdUpdater;

/* filenames cache structure */
struct FileInfo {
  String fileName;  // the binary name
  size_t fileSize;
};

FileInfo fileInfo[MAX_FILES];
uint16_t appsCount = 0;
bool toggle = false;
int16_t virtualPos = 0;


void setFileInfo(File &file, uint8_t appsCount) {
  String fileName = String(file.name());
  size_t fileSize = file.size();
  fileInfo[appsCount].fileName = fileName;
  fileInfo[appsCount].fileSize = fileSize;
}


void getDirectory(File dir, int numTabs) {
  int colcnt = 0;
  while (true) {
    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    if (numTabs > 0) {
      for (uint8_t i = 0; i <= numTabs; i++) {
        Serial.print('\t');
      }
    }
    Serial.print(entry.name());

    if (entry.isDirectory()) {
      Serial.println("/");
      getDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t");
      Serial.println(entry.size(), DEC);
      setFileInfo(entry, appsCount);
      appsCount++;
    }
    entry.close();
  }
}


void getSDRoot() {
  root = SD.open("/");
  root.rewindDirectory();
  getDirectory(root, 0); //Display the card contents
  root.close();
  Serial.print(F("Files count : "));
  Serial.println( appsCount );
}




/*
    Scan SPIFFS for binaries and move them onto the SD Card
    TODO: create an app manager for the SD Card
*/
void scanDataFolder() {

  if (!SPIFFS.begin()) {
    Serial.println(F("SPIFFS Mounting failed"));
    return;
  } else {
    Serial.println(F("SPIFFS Mounting success"));
    fs::Dir dir = SPIFFS.openDir("/");

    if (!dir.next()) {
      Serial.println("Nothing to copy");
      return;
    }

    fs::File spiffsFile = dir.openFile("r");

    static String fileName = spiffsFile.name();
    size_t fileSize = spiffsFile.size();

    if (!fileName.endsWith(".BIN")) {
      Serial.println(F("Nothing to move"));
      return;
    }

    if (SD.exists((char *)fileName.c_str())) {
      // remove file first to prevent file_append (bug?)
      SD.remove( (char *)fileName.c_str() );
      // lazy file management: delete SD file if SPIFFS file size is zero
      if (fileSize == 0) {
        Serial.println("Will DELETE zero-length file " + fileName);
        SPIFFS.remove( fileName );
        return;
      }
    }

    Serial.println("Will move " + fileName + " / " + String(fileSize));
    File sdFile = SD.open(fileName, FILE_WRITE);

    if (!sdFile) {
      Serial.println("SD Write Failed");
    } else {
      sdFile.seek(0);
      #define SD_SPIFFS_BUFSIZE 256
      static uint8_t buf[SD_SPIFFS_BUFSIZE];
      size_t packets = 0;
      int lastpercent = 0;

      while (spiffsFile.available()) {
        size_t packetsCount = spiffsFile.readBytes(buf, SD_SPIFFS_BUFSIZE);
        sdFile.write(buf, packetsCount);
        packets += packetsCount;
        int percent = (packets * 100) / fileSize;
        if (percent != lastpercent) {
          Serial.println();
          lastpercent = percent;
          display.clear();
          display.drawString(0, 20, "Moving " + String(fileName));
          display.drawString(0, 40, String(percent) + "%");
          display.display();
          Serial.print("Moving " + fileName + ": " + String(percent) + "% ");
        }
      }

      sdFile.close();
      Serial.println();
      Serial.println(F("Copy successful, cleaning up..."));
      display.clear();
      display.drawString(0, 20, "Copy successful");
      display.drawString(0, 40, "Cleaning up...");
      display.display();
      SPIFFS.remove( fileName );
      display.clear();
      display.display();
      Serial.println(F("Will restart"));
      delay(500);
      ESP.restart();
    }
  }
}



void renderUi() {
  display.clear();
  display.setColor(WHITE);
  uint16_t istart = 0;
  uint16_t iend   = appsCount;
  uint16_t vpos   = virtualPos * MENU_ITEM_HEIGHT;

  // lazy pagination
  while (vpos > display.height() - MENU_ITEM_HEIGHT) {
    istart++;
    vpos -= MENU_ITEM_HEIGHT;
  }

  display.drawString(0, vpos, "*");
  for (uint16_t i = istart, j = 0; i < iend; i++, j++) {
    uint16_t hpos = j * MENU_ITEM_HEIGHT;
    if (hpos > display.height()) continue;
    if (vpos == hpos) {
      display.fillRect(10, hpos, 118, MENU_ITEM_HEIGHT);
      display.setColor(BLACK);
      display.drawString(10, hpos, fileInfo[i].fileName);
      display.setColor(WHITE);
    } else {
      display.drawString(10, hpos, fileInfo[i].fileName);
    }
  }

  display.display();
}


void setup() {
  Serial.begin(115200);
  Serial.println(F("Setting up"));

  display.init();
  display.setContrast(255);
  //display.flipScreenVertically();

  while (!SD.begin()) {
    // TODO: make a more fancy animation
    Serial.println(F("Insert SD"));
    display.clear();
    display.display();
    delay(300);
    display.drawString(10, 20, "INSERT SD");
    display.display();
  }

  Serial.println(F("SD Detected"));
  display.clear();
  display.drawString(10, 20, "SD Detected");
  display.display();
  delay(500);
  display.clear();
  display.display();

  buttonsPadInit();
  scanDataFolder();
  getSDRoot();
  renderUi();
}


void loop() {
  handleButtonsPad();

  if (padButtons[0].pushed() == true) {
    Serial.println("Up");
    if (virtualPos + 1 < appsCount) {
      virtualPos++;
      delay(500);
    }
    renderUi();
  }

  if (padButtons[3].pushed() == true) {
    Serial.println("Down");
    if (virtualPos - 1 >= 0) {
      virtualPos--;
      delay(500);
    }
    renderUi();
  }

  if (padButtons[6].pushed() == true) {
    Serial.print(F("Will load "));
    Serial.println(fileInfo[virtualPos].fileName);
    display.clear();
    display.drawString(10, 20, "LOADING");
    display.drawString(10, 35, fileInfo[virtualPos].fileName);
    display.display();
    delay(1000);
    sdUpdater.updateFromSD(fileInfo[virtualPos].fileName);
    display.clear();
    display.display();
    ESP.restart();
  }
}
