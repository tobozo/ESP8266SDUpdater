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
Put this as early as possible in your sketch:

    #include "ESP8266SDUpdater.h"
    SDUpdater sdUpdater;

Use this from anywhere in your code to trigger the update:

    if(digitalRead(BUTTON_A_PIN) == 0) {
      Serial.println("Will Load menu binary");
      sdUpdater.updateFromSD(); // defaults to "MENU.BIN"
      ESP.restart();
    }

Also works with filenames:

    sdUpdater.updateFromSD("/BATMAN.BIN");
    ESP.restart(); // don't forget to restart!

The [OLEDiFOO example](https://github.com/tobozo/ESP8266SDUpdater/tree/master/examples/OLEDiFOO) should have its binary renamed to MENU.BIN and copied onto the SD Card.

This will be addressed if practical uses are found for the OLEDiESP build, but other menus should be added for other platforms such as [OLEDiCADE](https://www.tindie.com/products/PhoenixCNC/oledicade-tiny-pac-man-game-console/), [Espresso Lite](http://www.espressolite.com/) or [Lameboy](https://www.tindie.com/products/davedarko/lameboy-esp8266-handheld/?pt=ac_prod_search).

Troubleshooting
---------------
Using this library will create issues in your existing code. This is caused by [SPIFFS and SD fighting for the legitimacy of FS namespace](https://github.com/esp8266/Arduino/issues/2281). ESP8266SDUpdater needs to use both, and this comes with a treat.

<br><br><br><br>

**Compiler error**: `#error Architecture or board not supported`


```
    In file included from ~/Arduino/libraries/SD/src/utility/Sd2Card.h:26:0,
        from ~/Arduino/libraries/SD/src/utility/SdFat.h:29,
        from ~/Arduino/libraries/SD/src/SD.h:20,
        from ~/Arduino/libraries/ESP8266SDUpdater/ESP8266SDUpdater.h:61,
        from ~/MySketch/MySketch.ino:xx:~/Arduino/libraries/SD/src/utility/Sd2PinMap.h:510:2: error: #error Architecture or board not supported.
        #error Architecture or board not supported.
```

  - Make sure you include the `ESP8266SDUpdater.h` library first. If you can't, make sure you insert `#define FS_NO_GLOBALS` before every `#include "FS.h"`. 

  - Downgrade the SD library to version 1.1.1
    <img src=https://raw.githubusercontent.com/wiki/tobozo/ESP8266SDUpdater/img/library-manager.jpg>

  - If you still get the dreadful `#error Architecture or board not supported`:

    - Double check with the compile logs where it says: 
    ```
        Multiple libraries were found for "SD.h"
          Used: ~/Arduino/libraries/SD
          Not used: ~/Arduino/packages/esp8266/hardware/esp8266/2.3.0/libraries/SD
    ```
    
    - Copy the path to the offending SD library folder from the error message.<br>In the error message example the path is `~/Arduino/libraries/SD/` but the path may vary depending on the user prefs.<br>
    If you only see one path, and it's leading to the esp8266 folder, don't follow the next step and [open an issue](https://github.com/tobozo/ESP8266SDUpdater/issues/new).
    
    - Delete the folder `rm -Rf ~/Arduino/libraries/SD/`. By doing this, the sdk will use the built-in ESP8266 SD library instead of the updated version.

  Don't worry, you can still get the other SD.h back by performing a simple update check through the library manager.


<br><br><br><br>

**Compiler error**: `error XXXX was not declared in this scope` for one of the following objects:

  `FS, File, Dir, SeekMode, SeekSet, SeekCur, SeekEnd, FSInfo`

  - **Solution**: prefix all FS object calls with `fs::` (i.e. `FS` => `fs::FS`, `File` => `fs::File`, `FSInfo` => `fs::FSInfo`) until no further error is found.


Credits
-------
- [Hayri](https://www.tindie.com/stores/PhoenixCNC/) for providing the [OLEDiESP](https://www.tindie.com/products/PhoenixCNC/olediesp-a-tiny-cube-with-esp07--esp12--oled-iot/) to play with.
- [Reaper7](https://github.com/reaper7/) for providing inspiration with his [SPIFFS Update Firmware](https://github.com/reaper7/ESP8266SPIFFSUpdateFirmware) project.



