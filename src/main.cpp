// Contains code from
// https://microcontrollerslab.com/led-dot-matrix-display-esp32-max7219/

#include "config.h"
#include "debug.h"
#include "defaults.h"
#include "state.h"
#include "webInterface.h"
#include "wifiManager.h"
#include <Arduino.h>
#include <MD_MAX72xx.h>
#include <MD_Parola.h>
#include <SPI.h>
#include <WiFi.h>

// Sprite Definitions
const uint8_t F_MOVIE = 1;
const uint8_t W_MOVIE = 100;
const uint8_t PROGMEM movie[F_MOVIE * W_MOVIE] = // gobbling pacman animation
    {
        0x81, 0x3c, 0xBD, 0x3C, 0xBD, 0x3c, 0xBD, 0x3C, 0xBD, 0x3C, 0x81, 0x3c,
        0xBD, 0x3C, 0xBD, 0x3c, 0xBD, 0x3C, 0xBD, 0x3C, 0x81, 0x3c, 0xBD, 0x3C,
        0xBD, 0x3c, 0xBD, 0x3C, 0xBD, 0x3C, 0x81, 0x3c, 0xBD, 0x3C, 0xBD, 0x3c,
        0xBD, 0x3C, 0xBD, 0x3C, 0x81, 0x3c, 0xBD, 0x3C, 0xBD, 0x3c, 0xBD, 0x3C,
        0xBD, 0x3C, 0x81, 0x3c, 0xBD, 0x3C, 0xBD, 0x3c, 0xBD, 0x3C, 0xBD, 0x3C,
        0x81, 0x3c, 0xBD, 0x3C, 0xBD, 0x3c, 0xBD, 0x3C, 0xBD, 0x3C, 0x81, 0x3c,
        0xBD, 0x3C, 0xBD, 0x3c, 0xBD, 0x3C, 0xBD, 0x3C, 0x81, 0x3c, 0xBD, 0x3C,
        0xBD, 0x3c, 0xBD, 0x3C, 0xBD, 0x3C, 0x81, 0x3c, 0xBD, 0x3C, 0xBD, 0x3c,
        0xBD, 0x3C, 0xBD, 0x3C,

};

const uint8_t F_EMPTY = 1;
const uint8_t W_EMPTY = 1;
const uint8_t PROGMEM empty[F_EMPTY * W_EMPTY] = // gobbling pacman animation
    {0};

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
// #define HARDWARE_TYPE MD_MAX72XX::GENERIC_H

// Globals
MD_Parola Display =
    MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

MatrixConfig myConfig;
MatrixState myState(&myConfig);
webInterface myInterface(&myConfig);
WiFiManager wifiMngr(&myInterface);
char firmwareString[50];




void setup() {

  Serial.begin(115200);
  //     Need to print something to get the serial monitor setled
  for (int i = 0; i < 5; i++) {
    LOGERROR2("Initializing (", i, ")");
    delay(100);
  }
  LOGINFO1("Starting with firmwareversion: ", FIRMWAREVERSION);
  
  

  Display.begin();
  Display.setIntensity(0);
  Display.displayClear();
  Display.setSpriteData(movie, W_MOVIE, F_MOVIE, empty, W_EMPTY, F_EMPTY);
  Display.print(FIRMWAREVERSION);

 if (!myConfig.prepareFS())
    {
        LOGWARN0("Failed to mount LittleFS !");
         Display.print("Possiple Deffect, mounting LittleFS");
        delay(60*10000);
       ESP.restart();
    }
    else
    {
        LOGINFO0("Mounted.");
    }


    myConfig.loadConfig();

    strcpy(firmwareString, "Version ");
    strcat(firmwareString, FIRMWAREVERSION);

    
    LOGINFO0(firmwareString);
  

  
  myInterface.setupWebSrv(&wifiMngr);

}

void loop() {

  if (Display.displayAnimate()) {
    myState.stateChange();
    Display.displayClear();
    Display.displayScroll(myState.getText(), myState.getPosition(),
                          myState.getEffect(), myState.getSpeed());
  }
}