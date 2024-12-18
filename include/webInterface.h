#ifndef MATRIX_WEB_H
#define MATRIX_WEB_H
#include "MD_Parola.h"
#include "MatrixWebServer.h"
#include "config.h"
#include "webinterface.h"
#include "wifiManager.h"
#include <AsyncTCP.h>
#include <WiFi.h>

#ifndef COOKIENAME
#define COOKIENAME "CineMatrix_ID"
#endif

#ifndef COOKIEMAXAGE
#define COOKIEMAXAGE 604800   // 1 week
#endif

class MatrixConfig; // Forward declaration

class webInterface {
public:
  webInterface(MatrixConfig *, const char *password = "");
  ~webInterface();

  void setupWebSrv(WiFiManager *);
  void setConfigPortalPages();
  void unsetConfigPortalPages();
  void requireAuthorization(bool);
  MatrixWebServer *server;
  MatrixConfig *myConfig;

  bool _waitingForClientAction = false;
  bool _exitConfig = false;
  bool _IPnotSetYet = false;
  void InitPages();

private:
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
  void asyncpause(const unsigned long);
};

const char htmlHeader[] =
    "<!DOCTYPE HTML><html><head><meta name=\"viewport\" "
    "content=\"width=device-width,initial-scale=1.0\"/><link "
    "rel=\"stylesheet\" type=\"text/css\" href=\"CineMatrix.css\" "
    "media=\"all\"/>"
    "<link rel=\"preconnect\" href=\"https://fonts.googleapis.com\">"
    "<link rel=\"preconnect\" href=\"https://fonts.gstatic.com\" crossorigin>"
    "|<link "
    "href=\"https://fonts.googleapis.com/"
    "css2?family=Sixtyfour+Convergence&display=swap\" rel=\"stylesheet\">"
    " <script src=\"helpers.js\"></script>"
    "</head> <body> ";
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
