#ifndef MATRIX_WEB_H
#define MATRIX_WEB_H
#include "MD_Parola.h"
#include "MatrixWebServer.h"
#include "config.h"
#include "wifiManager.h"
#include <AsyncTCP.h>
#include <WiFi.h>
#include "webinterface.h"

#ifndef COOKIENAME
#define COOKIENAME "CineMatrix_ID"
#endif

#ifndef COOKIEMAXAGE
#define COOKIEMAXAGE 7200
#endif

class MatrixConfig; // Forward declaration

class WebInterface {
public:
  WebInterface(MatrixConfig *, const char *username = "",
               const char *password = "");
  ~WebInterface();

  void setupWebSrv(WiFiManager *);
  void setConfigPortalPages();
  void unsetConfigPortalPages();

  MatrixWebServer *server;
  MatrixConfig *myConfig;
 
  bool _waitingForClientAction = false;
  void InitPages();

private:
  String _username;
  String _password;
  bool _authRequired = false;
  // HTTPUpdateServer *httpUpdater;
  void handleNotFound(AsyncWebServerRequest *);
  void handleIndex(AsyncWebServerRequest *);
  void handleRoot(AsyncWebServerRequest *);
  void handleReset(AsyncWebServerRequest *);
  void handleSubmission(AsyncWebServerRequest *);
  void handleFile(AsyncWebServerRequest *, const char *, const unsigned char *,
                  const size_t);
  void handleScan(AsyncWebServerRequest *);
  bool captivePortal(AsyncWebServerRequest *);
  void handleCaptivePortal(AsyncWebServerRequest *);
  void handleRestart(AsyncWebServerRequest *);
  void handleLogout(AsyncWebServerRequest *);
  void handleLogin(AsyncWebServerRequest *);
  void handleConfigConfig(AsyncWebServerRequest *);
  void handleNetworkSetup(AsyncWebServerRequest *);

  unsigned long remainingPortaltime();
  unsigned long _configPortalInterfaceStart = 0;
  bool isIp(const String &);
};

const char htmlHeader[] =
    "<!DOCTYPE HTML><html><head><meta name=\"viewport\" "
    "content=\"width=device-width,initial-scale=1.0\"/><link "
    "rel=\"stylesheet\" type=\"text/css\" href=\"CineMatrix.css\" "
    "media=\"all\"/></head><body>";
const char htmlFooter[] = "</body></html>";

struct EffectStruct_t {
  textEffect_t effect;
  String label;
};

struct PositionStruct_t {
  textPosition_t position;
  String label;
  String tooltip;
};

#endif
