/*
 * 
 * I2C Xpad Driver for ESP8266
 * 
 * Copyright 2018 tobozo http://github.com/tobozo
 *
 * Permission is hereby granted, free of charge, to any person 
 * obtaining a copy of this software and associated documentation 
 * files ("I2C Xpad Driver"), to deal in the Software without 
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
 * I2C XPad is a project from Radomir Dopieralski @ Tindie
 * https://www.tindie.com/products/deshipu/x-pad-20-buttons-shield-for-d1-mini/
 * 
 *  PAD Bits assignment (from right to left)
 *   _______________________     
 *  |                       |
 *  |    3 :  ____¬ :   4   |
 *  | 2    : ]    [ :       |
 *  |        ]    [   6   5 |
 *  |    0 : ]____[ :       |
 *  | 1    :  ¬¬    :   7   |
 *  |_______________________|
 *
 *
 */
#ifndef __XPAD8BUTTONS_H
#define __XPAD8BUTTONS_H

#include <Wire.h>

unsigned long xpaddebounce = 300;
unsigned long xpadcheckdelay = 100;
unsigned long xpadlastcheck = millis();


struct PadButton {
  uint8_t bitPosition; /* 0-7 */
  bool state; /* true = pushed, false = inert */
  unsigned long lastpush;
  unsigned long lastrelease;
  bool pushed() {
    bool ispushed = this->state;
    this->state = false;
    return ispushed;
  };
};


PadButton BUTTON_0;
PadButton BUTTON_1;
PadButton BUTTON_2;
PadButton BUTTON_3;
PadButton PAD_UP;
PadButton PAD_RIGHT;
PadButton PAD_LEFT;
PadButton PAD_DOWN;


PadButton padButtons[8] = {
  BUTTON_0,
  BUTTON_1,
  BUTTON_2,
  BUTTON_3,
  PAD_UP,
  PAD_RIGHT,
  PAD_LEFT,
  PAD_DOWN
};


void buttonsPadInit() {
  xpadlastcheck = millis();
}


void handleButtonsPad() {
  unsigned long now = millis();
  if(now-xpadcheckdelay < xpadlastcheck) return;
  Wire.begin();
  Wire.requestFrom(0x20, 1);
  uint8_t ret = Wire.read();
  xpadlastcheck = millis();

  for(uint8_t i=0;i<8;i++) {
    if((ret >> i)&1) {
      if(padButtons[i].state && padButtons[i].lastpush + xpaddebounce < now) {
        // button released
        padButtons[i].state = false;
        padButtons[i].lastrelease = now;
      }
    } else {
      if(!padButtons[i].state && padButtons[i].lastpush + xpaddebounce < now) {
        // button pushed
        padButtons[i].state = true;
        padButtons[i].lastpush = now;
      }
    }
  }
}




#endif
