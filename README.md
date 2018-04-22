# ESP8266SDUpdater
This is the ESP8266 Prequel to the [M5Stack-SD-Updater](https://github.com/tobozo/M5Stack-SD-Updater).

ESP8266Updater is an Arduino library to package you apps on a SD card and load them from different contexts such as a menu app or MQTT message.


<img src="https://github.com/tobozo/ESP8266SDUpdater/raw/master/examples/OLEDiFOO/OLEDiESP.jpg">



Minimal Requirements
--------------------
- ESP8266 (4M and more)
- Micro SD Card reader

Usage
-----
Include this as early as possible in your sketch:

    #include "ESP8266SDUpdater.h"

Use this from anywhere in your code to trigger the update:

    if(digitalRead(BUTTON_A_PIN) == 0) {
      Serial.println("Will Load menu binary");
      updateFromSD(); // defaults to "MENU.BIN"
      ESP.restart();
    }

Also works with filenames:

    updateFromSD("/BATMAN.BIN");
    ESP.restart(); // don't forget to restart!

The [OLEDiFOO example](https://github.com/tobozo/ESP8266SDUpdater/tree/master/examples/OLEDiFOO) should have its binary renamed to MENU.BIN and copied onto the SD Card.
**The current OLEDiFOO sketch is only a rudimentary example and will crash if more than 5 files are added on the SD Card**.

This will be addressed if practical uses are found for the OLEDiESP build, but other menus should be added for other platforms suche as [OLEDiCADE](https://www.tindie.com/products/PhoenixCNC/oledicade-tiny-pac-man-game-console/), [Espresso Lite](http://www.espressolite.com/) or [Lameboy](https://www.tindie.com/products/davedarko/lameboy-esp8266-handheld/?pt=ac_prod_search).


Credits
-------
- [Hayri](https://www.tindie.com/stores/PhoenixCNC/) for providing the [OLEDiESP](https://www.tindie.com/products/PhoenixCNC/olediesp-a-tiny-cube-with-esp07--esp12--oled-iot/) to play with.
- [Reaper7](https://github.com/reaper7/) for providing inspiration with hir [SPIFFS Update Firmware](https://github.com/reaper7/ESP8266SPIFFSUpdateFirmware) project.



