#ifndef MatrixCONFIG_H
#define MatrixCONFIG_H

#include "defaults.h"
#define CONFIG_BUF_SIZE 2048
#define FORMAT_SPIFFS_IF_FAILED true
#ifndef DEFAULT_SPEED
#define DEFAULT_SPEED 75
#endif
#define DEFAULTPASS "1b3e6984034bb"
#ifndef CONFIGFILENAME
#define CONFIGFILENAME "/configv2.json"
#endif

#define WEBPASS_BUFF_SIZE 32

#define COOKIENAME "CineMatrix_ID"
#define MATRIXTEXTLENGTH                                                       \
  TEXTLENGTH + 4 * FIELDVALUELENGTH +                                          \
      1 // This 'should' suffice for having the full text

#define FIELDVALUELENGTH 32

#include "wifiManager.h"
#include <MD_Parola.h>

typedef struct
// Eenumps from
{
  char text[TEXTLENGTH + 1];
  char field[FIELDVALUELENGTH + 1];
  char value[FIELDVALUELENGTH + 1];
  char matrixtext[MATRIXTEXTLENGTH + 1];
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
  char webPass[WEBPASS_BUFF_SIZE + 1];
  bool MustConfigureUsingPortal;
  bool prepareFS();
  void setDefaults();
  bool loadConfig();  // load current config to disk (limited set of vallues)
  bool saveConfig();  // save current config to disk (limited set of vallues)
  void resetConfig(); // resetConfig to values that were programmed by default
  void createtMatrixText(textelements *);


  textelements element[MAXTEXTELEMENTS + 1];

  WiFi_AP_IPConfig WM_AP_IPconfig; // WifiManager Configuration
  WiFi_STA_IPConfig WM_STA_IPconfig;
  WM_Config WM_config;
};

#endif