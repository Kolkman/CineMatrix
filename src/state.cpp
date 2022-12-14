#include "state.h"
MatrixState::MatrixState()
{
    // Empty
    elementID = 0;
    elementItterator = 0;
}

bool MatrixState::init()
{

    if (!myConfig.prepareFS())
    {
        Serial.println("Failed to mount LittleFS !");
        return false;
    }
    else
    {
        Serial.println("Mounted.");
    }

    if (!myConfig.loadConfig())
    {
        Serial.println("Failed to load config. Using default values and creating config...");
        if (!myConfig.saveConfig())
        {
            Serial.println("Failed to save config");
            return false;
        }
        else
        {
            Serial.println("Config saved");
            return true;
        }
    }
    else
    {
        Serial.println("Config loaded");
        return true;
    }
}

bool MatrixState::stateChange()
{
    if (elementItterator < myConfig.element[elementID].repeat ){
        elementItterator++;
    }else{
        elementItterator=1;
        if (elementID<MAXTEXTELEMENTS){
            elementID++;
        }else{
            elementID=0;
        }
    }
    return true;    
}



char *MatrixState::getText()
{
    return myConfig.element[elementID].text;
}

textPosition_t MatrixState::getPosition()
{
    return myConfig.element[elementID].position;
}

textEffect_t MatrixState::getEffect()
{
    return myConfig.element[elementID].effect;
}

uint16_t MatrixState::getSpeed()
{
    return myConfig.element[elementID].speed;
}

char * MatrixState::getWifiPass()
{
    return myConfig.wifiPASS;
}

char * MatrixState::getWifiSSID(){
    return myConfig.wifiSSID;
}