#ifndef MatrixMach_WEB_API_H
#define MatrixMach_WEB_API_H
#include <Arduino.h>
#include "MatrixWebServer.h"
#include "wifiManager.h"


class webInterfaceAPI
{
public:
    void begin(MatrixWebServer *, MatrixConfig *) ;
    void requireAuthorization(bool);
    webInterfaceAPI();

private:
    MatrixWebServer *server;
    MatrixConfig * myConfig;

    bool _authRequired;
    size_t content_len;
    void handleStatus(AsyncWebServerRequest *);
    void handleFirmware(AsyncWebServerRequest *);
    void handleSet(AsyncWebServerRequest *);
    void handleStats(AsyncWebServerRequest *);
    void handleConfigFile(AsyncWebServerRequest *);
    void handleNetwork(AsyncWebServerRequest *);
    void handleIsAuthenticated(AsyncWebServerRequest *);
};

extern webInterfaceAPI webAPI;

#endif