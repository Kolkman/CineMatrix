#ifndef MatrixCONFIG_H
#define MatrixCONFIG_H

#include"defaults.h"
#define CURRENTFIRMWARE "CineMatrix-1.0.0"
#define CONFIG_BUF_SIZE 2048
#define FORMAT_SPIFFS_IF_FAILED true
#ifndef DEFAULT_SPEED
#define DEFAULT_SPEED 100
#define ELEGANT_OTA false
#endif



#include <MD_Parola.h>

// WifiManager related config
/*
typedef struct
{
    char wifi_ssid[33];
    char wifi_pw[33];
} WiFi_Credentials;
*/
typedef struct
// Eenumps from 
{
    char text[TEXTLENGTH+1];
    // Enumerations 
    textEffect_t effect;
    textPosition_t position;
    uint16_t speed;
    uint16_t repeat;
} textelements;


// Class to handle all things that have to do with configuration
class MatrixConfig
{
public:
    MatrixConfig();
    char wifiSSID[33];
    char wifiPASS[33];
    bool defaultPASS;
    bool prepareFS();
    bool loadConfig();  // load current config to disk (limited set of vallues)
    bool saveConfig();  // save current config to disk (limited set of vallues)
    void resetConfig(); // resetConfig to values that were programmed by default
    textelements element[MAXTEXTELEMENTS];
};

#endif