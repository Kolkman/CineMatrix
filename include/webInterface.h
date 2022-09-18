#ifndef MATRIX_WEB_H
#define MATRIX_WEB_H
#include <WiFi.h>
#include <AsyncTCP.h>
#include "MatrixWebServer.h"

class WebInterface
{
public:
    WebInterface(const char *username = "", const char *password = "");
    ~WebInterface();
    void setupWebSrv();
    MatrixWebServer *server;

private:
    String _username;
    String _password;
    bool _authRequired = false;
    // HTTPUpdateServer *httpUpdater;
    void handleNotFound(AsyncWebServerRequest *);
    void handleIndex(AsyncWebServerRequest *);
    void handleRoot(AsyncWebServerRequest *);
    void handleReset(AsyncWebServerRequest *);
    void handleFile(AsyncWebServerRequest *, const char *, const unsigned char *, const size_t);
};

const char htmlHeader[] = "<!DOCTYPE HTML><html><head><meta name=\"viewport\" content=\"width=device-width,initial-scale=1.0\"/><link rel=\"stylesheet\" type=\"text/css\" href=\"CineMatrix.css\" media=\"all\"/><link rel=\"icon\" href=\"data:;base64,iVBORw0KGgo=\"></head><body>";
const char htmlFooter[] = "</body></html>";

#endif