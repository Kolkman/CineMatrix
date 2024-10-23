#ifndef MatrixCONFIG_H
#define MatrixCONFIG_H


#include "defaults.h"

#define CONFIG_BUF_SIZE 2048
#define FORMAT_SPIFFS_IF_FAILED true
#ifndef DEFAULT_SPEED
#define DEFAULT_SPEED 75
#define ELEGANT_OTA true
#endif




#define COOKIENAME "CineMatrix_ID"




#include "wifiManager.h"
#include <MD_Parola.h>

typedef struct
// Eenumps from
{
  char text[TEXTLENGTH + 1];
  // Enumerations
  textEffect_t effect;
  textPosition_t position;
  uint16_t speed;
  uint16_t repeat;
} textelements;

// Class to handle all things that have to do with configuration
class MatrixConfig {
public:
  MatrixConfig();
  String passForSSID(String);

  bool prepareFS();
  bool loadConfig();  // load current config to disk (limited set of vallues)
  bool saveConfig();  // save current config to disk (limited set of vallues)
  void resetConfig(); // resetConfig to values that were programmed by default
  textelements element[MAXTEXTELEMENTS];

  WiFi_AP_IPConfig WM_AP_IPconfig; // WifiManager Configuration
  WiFi_STA_IPConfig WM_STA_IPconfig;
  WM_Config WM_config;
};

#endif