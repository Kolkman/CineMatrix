#ifndef MATRIX_WEB_H
#define MATRIX_WEB_H
#include <WiFi.h>
#include <AsyncTCP.h>
#include "MatrixWebServer.h"
#include "MD_Parola.h"
#include "config.h"



class WebInterface
{
public:
    WebInterface(MatrixConfig *, const char *username = "", const char *password = "");
    ~WebInterface();
    void setupWebSrv();
    MatrixWebServer *server;
    MatrixConfig * myConfig;
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
    void handleFile(AsyncWebServerRequest *, const char *, const unsigned char *, const size_t);
};

const char htmlHeader[] = "<!DOCTYPE HTML><html><head><meta name=\"viewport\" content=\"width=device-width,initial-scale=1.0\"/><link rel=\"stylesheet\" type=\"text/css\" href=\"CineMatrix.css\" media=\"all\"/></head><body>";
const char htmlFooter[] = "</body></html>";

struct EffectStruct_t
{
    textEffect_t effect;
    String label;
};

struct PositionStruct_t
{
    textPosition_t position;
    String label;
    String tooltip;
};

#endif
