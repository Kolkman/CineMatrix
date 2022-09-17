// Contains code from https://microcontrollerslab.com/led-dot-matrix-display-esp32-max7219/

#include <Arduino.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include "config.h"
#include "state.h"


#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
//#define HARDWARE_TYPE MD_MAX72XX::GENERIC_H


// Globals
MD_Parola Display = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

MatrixState myState;


void setup()
{
  
    Display.begin();
    Display.setIntensity(0);
    Display.displayClear();
  

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
    if(!myState.init()){
        Display.displayScroll("Init Error: filesystem problems",PA_RIGHT, PA_SCROLL_LEFT, 50);
    }else{
        Display.displayScroll(myState.getText(),myState.getPosition(),myState.getEffect(),myState.getSpeed());
    //State setup not needed, done by the innitiator
    }
}

void loop()
{


    if (Display.displayAnimate())
    {
        myState.stateChange();
        Display.displayClear();
        Display.displayScroll(myState.getText(),myState.getPosition(),myState.getEffect(),myState.getSpeed());

    }
}