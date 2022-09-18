// Contains code from https://microcontrollerslab.com/led-dot-matrix-display-esp32-max7219/

#include <Arduino.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include "defaults.h"
#include "config.h"
#include "state.h"
#include "webInterface.h"
#include <WiFi.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
//#define HARDWARE_TYPE MD_MAX72XX::GENERIC_H

// Globals
MD_Parola Display = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);
MatrixState myState;
WebInterface myInterface;

void setup()
{

    Serial.begin(115200);
    //     Need to print something to get the serial monitor setled
    for (int i = 0; i < 5; i++)
    {
        Serial.print("Initializing (");
        Serial.print(i);
        delay(20);
        Serial.println(")");
        delay(100);
    }

    Serial.println(WiFi.localIP());

    Display.begin();
    Display.setIntensity(0);
    Display.displayClear();

    if (!myState.init())
    {
        Display.displayScroll("Init Error: filesystem problems", PA_RIGHT, PA_SCROLL_LEFT, 50);
    }
    else
    {
        Display.displayScroll(myState.getText(), myState.getPosition(), myState.getEffect(), myState.getSpeed());
        // State setup not needed, done by the innitiator
    }

    {
        IPAddress ip, netmask;
        ip.fromString(WEB_IP);
        netmask.fromString(WEB_NETMASK);
        WiFi.softAPConfig(ip, ip, netmask);
    }
    
  
    // Print ESP Local IP Address
    Serial.print("Setting up WIFI with SSID:");
    Serial.print(myState.getWifiSSID());
    Serial.print(" and password:");
    Serial.println(myState.getWifiPass());
    WiFi.softAP(myState.getWifiSSID(), myState.getWifiPass());
    myInterface.setupWebSrv();
  
}

void loop()
{

    if (Display.displayAnimate())
    {
        myState.stateChange();
        Display.displayClear();
        Display.displayScroll(myState.getText(), myState.getPosition(), myState.getEffect(), myState.getSpeed());
    }
}