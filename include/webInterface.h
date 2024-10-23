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
#define COOKIEMAXAGE 7200
#endif

class MatrixConfig; // Forward declaration

class WebInterface {
public:
  WebInterface(MatrixConfig *, const char *password = "");
  ~WebInterface();

  void setupWebSrv(WiFiManager *);
  void setConfigPortalPages();
  void unsetConfigPortalPages();

  MatrixWebServer *server;
  MatrixConfig *myConfig;

  bool _waitingForClientAction = false;
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
};

const char htmlHeader[] =
    "<!DOCTYPE HTML><html><head><meta name=\"viewport\" "
    "content=\"width=device-width,initial-scale=1.0\"/><link "
    "rel=\"stylesheet\" type=\"text/css\" href=\"CineMatrix.css\" "
    "media=\"all\"/>"
    "<script type =\"text/javascript\">"
    "function    CompareFields(f1, f2, caseinsensitive) {"
    "var val1 = document.getElementById(f1).value;"
    "var val2 = document.getElementById(f2).value;"
    "if (caseinsensitive) {"
    "  val1 = val1.toUpperCase();"
    "  val2 = val2.toUpperCase();"
    "}"
    "val1 = val1.replace(/ ^\\s * /, "
    ");"
    "val1 = val1.replace(/\\s * $ /, "
    ");"
    "if (val1.length == 0) {"
    "  return;"
    "}"
    "val2 = val2.replace(/ ^\\s * /, "
    ");"
    "val2 = val2.replace(/\\s * $ /, "
    ");"
    "if (val2.length == 0) {"
    "  return;"
    "}"
    "if (val1 == val2) {"
    "return;"
    "  }"
    "   alert(\"The form fields need to be identical.\");"
    "}"
    "</script></head> <body> ";
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
