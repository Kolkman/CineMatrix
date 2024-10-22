
// Code fragments from
// https://github.com/lbernstone/asyncUpdate/bb/master/AsyncUpdate.ino

#include "webInterfaceAPI.h"
#include "debug.h"
#include "pages/configDone.html.h"
#include <ESPAsyncWebServer.h>
webInterfaceAPI webAPI;

webInterfaceAPI::webInterfaceAPI() {
  server = nullptr;
  content_len = 0;
  mustAuthenticate = true;
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
  LOGINFO1("API SET with", request->url());
  char message[2048]; // This is sufficiently big to store all key:val
                      // combinations
  if (!server->is_authenticated(request) && mustAuthenticate) {
    LOGINFO("API not authenticed")
    strcpy(message, "{\"authenticated\": false}");
    request->send(200, "application/json", message);
    return;
  }

  strcpy(message, "{");

  for (int z = 0; z < NUM_WIFI_CREDENTIALS; z++) {
    myConfig->WM_config.WiFi_Creds[z].config_change = false; // initiate
  }
  for (uint8_t i = 0; i < request->args(); i++) {
    if (request->argName(i).startsWith("wifi_ssid")) {
      LOGINFO("wifi_ssid Found");
      for (int z = 0; z < NUM_WIFI_CREDENTIALS; z++) {

        if (request->argName(i).equals("wifi_ssid" + String(z)) &&
            request->arg(i) != "") {
          myConfig->WM_config.WiFi_Creds[z].config_change =
              true; // administer change has happened.
          LOGINFO("--- wifi_ssid" + String(z));
          addjson(message, firstarg, "wifi_ssid" + String(z), request->arg(i));
          strlcpy(myConfig->WM_config.WiFi_Creds[z].wifi_ssid,
                  request->arg(i).c_str(), SSID_MAX_LEN);
          reconf = true;
        }
      }
    } else if (request->argName(i).startsWith("wifi_pw")) {
      LOGINFO("wifi_pw Found");
      for (int z = 0; z < NUM_WIFI_CREDENTIALS; z++) {

        if (request->argName(i).equals("wifi_pw" + String(z)) &&
            request->arg(i) != "") {
          myConfig->WM_config.WiFi_Creds[z].config_change =
              false; // administer pw has changed
          LOGINFO("--- wifi_pw" + String(z));
          addjson(message, firstarg, "wifi_pw" + String(z), request->arg(i));
          strlcpy(myConfig->WM_config.WiFi_Creds[z].wifi_pw,
                  request->arg(i).c_str(), PASS_MAX_LEN);
          reconf = true;
        }
      }
    } else if (request->argName(i) == "safeandrestart" || request->arg(i)) {
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
  if (!server->is_authenticated(request) && mustAuthenticate) {
    strcpy(message, "{\"authenticated\":false}");
  } else {
    strcpy(message, "{\"authenticated\":true}");
  }
  request->send(200, "application/json", message);
}

void webInterfaceAPI::requireAuthorization(bool require) {
  mustAuthenticate = require;
  return;
}