
// Code fragments from
// https://github.com/lbernstone/asyncUpdate/bb/master/AsyncUpdate.ino

#include "webInterfaceAPI.h"
#include "debug.h"
#include "pages/configDone.html.h"
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>

webInterfaceAPI webAPI;

webInterfaceAPI::webInterfaceAPI() {
  server = nullptr;
  content_len = 0;
  _authRequired = true;
}

void webInterfaceAPI::begin(MatrixWebServer *s, MatrixConfig *conf) {
  server = s;
  myConfig = conf;

  server->on("/api/v1/authenticated", HTTP_GET,
             std::bind(&webInterfaceAPI::handleIsAuthenticated, this,
                       std::placeholders::_1));
  server->on(
      "/api/v1/firmware", HTTP_GET,
      std::bind(&webInterfaceAPI::handleFirmware, this, std::placeholders::_1));
  server->on(
      "/api/v1/set", HTTP_GET,
      std::bind(&webInterfaceAPI::handleSet, this, std::placeholders::_1));
  server->on("/api/v1/config", HTTP_GET,
             std::bind(&webInterfaceAPI::handleConfigFile, this,
                       std::placeholders::_1));

  server->on("/api/v1/resetPassword", HTTP_GET,
             std::bind(&webInterfaceAPI::handlePasswordReset, this,
                       std::placeholders::_1));
  server->on("/api/v1/resetConfig", HTTP_GET,
             std::bind(&webInterfaceAPI::handleConfigReset, this,
                       std::placeholders::_1));
}

void webInterfaceAPI::handleFirmware(AsyncWebServerRequest *request) {
  String message = "{\"version\": \"" + String(FIRMWAREVERSION) + "-" +
                   String(F(__DATE__)) + ":" + String(F(__TIME__)) + "\"}";
  request->send(200, "application/json", message);
}

void addjson(char *msg, bool &firstarg, String argument, String value) {
  if (!firstarg)
    strcat(msg, ",");
  else
    firstarg = false;
  char addition[50];
  ("\"" + argument + "\":" + value).toCharArray(addition, 50);
  strcat(msg, addition);
  return;
}

void webInterfaceAPI::handleSet(AsyncWebServerRequest *request) {
  bool safeandrestart = false;
  bool reconf = false;
  bool firstarg = true;
  LOGDEBUG1("API SET with", request->url());
  char message[2048]; // This is sufficiently big to store all key:val
                      // combinations
  if (!server->is_authenticated(request) && _authRequired) {
    LOGINFO0("API not authenticed")

    return;
  }

  int params = request->params();
  for (int i = 0; i < params; i++) {
    const AsyncWebParameter *p = request->getParam(i);
    if (p->isFile()) { // p->isPost() is also true
      String msg =
          "FILE[" + p->name() + "]: " + p->value() + "(" + p->size() + ")";
      LOGDEBUG0(msg);
    } else if (p->isPost()) {
      String msg = "POST[" + p->name() + "]: " + p->value();
      LOGDEBUG0(msg);

    } else {

      String msg = "GET[" + p->name() + "]: " + p->value();
      LOGDEBUG0(msg);
    }
  }

  strcpy(message, "{");

  for (int z = 0; z < NUM_WIFI_CREDENTIALS; z++) {
    myConfig->WM_config.WiFi_Creds[z].config_change = false; // initiate
  }
  for (uint8_t i = 0; i < request->args(); i++) {
    const AsyncWebParameter *p = request->getParam(i);
    if (p->name().startsWith("wifi_ssid")) {
      LOGDEBUG0("wifi_ssid Found");
      for (int z = 0; z < NUM_WIFI_CREDENTIALS; z++) {

        if (p->name().equals("wifi_ssid" + String(z)) && p->value() != "") {
          myConfig->WM_config.WiFi_Creds[z].config_change =
              true; // administer change has happened.
          LOGDEBUG0("--- wifi_ssid" + String(z) + ":" + p->value());
          addjson(message, firstarg, "wifi_ssid" + String(z), p->value());
          strlcpy(myConfig->WM_config.WiFi_Creds[z].wifi_ssid,
                  p->value().c_str(), SSID_MAX_LEN);
          reconf = true;
        }
      }
    } else if (p->name().startsWith("wifi_pw")) {
      LOGDEBUG0("wifi_pw Found");
      for (int z = 0; z < NUM_WIFI_CREDENTIALS; z++) {

        if (p->name().equals("wifi_pw" + String(z)) &&
            p->value() != "") {
          myConfig->WM_config.WiFi_Creds[z].config_change =
              false; // administer pw has changed
          LOGDEBUG0("--- wifi_pw" + String(z));
          addjson(message, firstarg, "wifi_pw" + String(z), p->value());
          strlcpy(myConfig->WM_config.WiFi_Creds[z].wifi_pw,
                  p->value().c_str(), PASS_MAX_LEN);
          reconf = true;
        }
      }
    } else if (p->name() == "safeandrestart" || p->value()) {
      safeandrestart = true;
    }
  }
  if (reconf) {
    // if ssid has changed but password didn't then set pw to ""
    for (int z = 0; z < NUM_WIFI_CREDENTIALS; z++) {
      if (myConfig->WM_config.WiFi_Creds[z].config_change) {
        myConfig->WM_config.WiFi_Creds[z].wifi_pw[0] = '\0'; //
      }
    }
  }

  if (safeandrestart) {
    myConfig->saveConfig();

    AsyncWebServerResponse *response = request->beginResponse(
        200, "text/html;charset=UTF-8", configDone_html, configDone_html_len);
    response->addHeader("Content-Encoding", "gzip");
    response->addHeader("Access-Control-Allow-Origin",
                        "WM_HTTP_CORS_ALLOW_ALL");
    request->send(response);

    delay(2000);
    ESP.restart();
  }

  strcat(message, "}");
  request->send(200, "application/json", message);
}

void webInterfaceAPI::handleConfigFile(AsyncWebServerRequest *request) {
  char message[32] = ""; // This is sufficiently for any of the messages below

  if (request->args()) {
    LOGDEBUG1("config?", request->argName(0));

    if (request->argName(0) == "load") {
      if (myConfig->loadConfig()) {

        strcpy(message, "{\"load\":true}");
      } else {
        strcpy(message, "{\"load\":false}");
      }
    } else if (request->argName(0) == "save") {

      if (myConfig->saveConfig()) {
        strcpy(message, "{\"save\":true}");
      } else {
        strcpy(message, "{\"save\":false}");
      }
    }

  } else {
    strcpy(message, "{\"config\":false}");
  }

  LOGDEBUG("HIEROO4");

  request->send(200, "application/json", message);
  LOGDEBUG("HIEROO4");
}

void webInterfaceAPI::handleIsAuthenticated(AsyncWebServerRequest *request) {
  char message[32]; // This is sufficiently for any of the messages below
  if (!server->is_authenticated(request) && _authRequired) {
    strcpy(message, "{\"authenticated\":false}");
  } else {
    strcpy(message, "{\"authenticated\":true}");
  }
  request->send(200, "application/json", message);
}

void webInterfaceAPI::requireAuthorization(bool require) {
  _authRequired = require;
  return;
}


void webInterfaceAPI::handlePasswordReset(AsyncWebServerRequest *request) {
if (_authRequired && !  server->authenticate(request) ) {
    LOGINFO0("API not authenticed")

    return;
  }

  strncpy(myConfig->webPass, DEFAULTPASS, WEBPASS_BUFF_SIZE);
  myConfig->saveConfig();
  request->redirect("/");
}


void webInterfaceAPI::handleConfigReset(AsyncWebServerRequest *request) {
if (_authRequired && !  server->authenticate(request) ) {
    LOGINFO0("API not authenticed")

    return;
  }

  LittleFS.remove("./config.json");
   String message =
      "<head><meta http-equiv=\"refresh\" content=\"2;url=/\">\n<meta "
      "name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" "
      "/><title>CineMatrix</title></head>";
  message += "<h1> Reseting Device ! </h1>";
  request->send(200, "text/html", message);
  ESP.restart();

}