#include "config.h"
#include "debug.h"
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <StreamUtils.h>

MatrixConfig::MatrixConfig() {
  LOGDEBUG0("MatrixConfig constructor")
  strncpy(webPass, DEFAULTPASS,WEBPASS_BUFF_SIZE);

  for (int i = 0; i < MAXTEXTELEMENTS; i++) {
    strcpy(element[i].text, "");
    element[i].effect = PA_SCROLL_LEFT;
    element[i].position = PA_RIGHT;
    element[i].speed = DEFAULT_SPEED;
    element[i].repeat = 1;
  }

  for (int i = 0; i < NUM_WIFI_CREDENTIALS; i++) {
    *WM_config.WiFi_Creds[i].wifi_ssid = '\0';
    *WM_config.WiFi_Creds[i].wifi_pw = '\0';
  }
  WM_AP_IPconfig._ap_static_ip = {192, 168, 100, 1};
  WM_AP_IPconfig._ap_static_gw = {192, 168, 100, 1};
  WM_AP_IPconfig._ap_static_sn = {255, 255, 255, 0};

  // Default
  strcpy(element[0].text, "");
  // strcat(element[0].text, WIFI_SSID);
  strcpy(element[1].text, "");

  strcpy(element[2].text, "");

  LOGINFO(element[0].text);
}

bool MatrixConfig::prepareFS() {
  LOGDEBUG0("MatrixConfig::PrepareFS")
  if (!LittleFS.begin(false /* false: Do not format if mount failed */)) {
    Serial.println("Failed to mount LittleFS");
    if (!LittleFS.begin(true /* true: format */)) {
      LOGERROR0("Failed to format LittleFS");
      return false;
    } else {
      LOGINFO0("LittleFS formatted successfully");
      return true;
    }
  } else { // Initial mount success
    return true;
  }
}

bool MatrixConfig::loadConfig() {
  File configFile = LittleFS.open("/config.json", "r");
  if (!configFile) {
    LOGERROR0("Failed to open config file");
    return false;
  }

  StaticJsonDocument<CONFIG_BUF_SIZE> jsonDocument;
  ReadLoggingStream loggingStream(configFile, Serial);

  DeserializationError parsingError =
      deserializeJson(jsonDocument, loggingStream);

  if (_MatrixRSS_LOGLEVEL_ > 2) { // at DEBUG level
    serializeJsonPretty(jsonDocument, Serial);
  }

  if (parsingError) {

    LOGERROR0("Failed to deserialize json config file");
    LOGERROR0(parsingError.c_str());
    return false;
  }

  for (int i = 0; i < 4; i++) {
    // itterate over the IP4 tupples
    if (jsonDocument["ap_static_ip"][i])
      WM_AP_IPconfig._ap_static_ip[i] = jsonDocument["ap_static_ip"][i];
    if (jsonDocument["ap_static_gw"][i])
      WM_AP_IPconfig._ap_static_gw[i] = jsonDocument["ap_static_gw"][i];
    if (jsonDocument["ap_static_gw"][i])
      WM_AP_IPconfig._ap_static_sn[i] = jsonDocument["ap_static_gw"][i];
    if (jsonDocument["sta_static_ip"][i])
      WM_STA_IPconfig._sta_static_ip[i] = jsonDocument["sta_static_ip"][i];
    if (jsonDocument["sta_static_gw"][i])
      WM_STA_IPconfig._sta_static_gw[i] = jsonDocument["sta_static_gw"][i];
    if (jsonDocument["sta_static_sn"][i])
      WM_STA_IPconfig._sta_static_sn[i] = jsonDocument["sta_static_sn"][i];
#if USE_CONFIGURABLE_DNS
    WM_STA_IPconfig._sta_static_dns1[i] = jsonDocument["sta_static_dns1"][i];
    WM_STA_IPconfig._sta_static_dns2[i] = jsonDocument["sta_static_dns2"][i];
#endif
  }
  if (jsonDocument["WifiCredential_ssid"]) {
    int i = 0;
    JsonArray j_ssid = jsonDocument["WifiCredential_ssid"];
    // using C++11 syntax (preferred):
    for (JsonVariant value : j_ssid) {
      strcpy(WM_config.WiFi_Creds[i].wifi_ssid, value.as<const char *>());
      i++;
      if (i == NUM_WIFI_CREDENTIALS)
        break;
    }
  }
  if (jsonDocument["WifiCredential_pw"]) {
    int i = 0;
    JsonArray j_ssid = jsonDocument["WifiCredential_pw"];
    // using C++11 syntax (preferred):
    for (JsonVariant value : j_ssid) {
      strcpy(WM_config.WiFi_Creds[i].wifi_pw, value.as<const char *>());
      i++;
      if (i == NUM_WIFI_CREDENTIALS)
        break;
    }
  }
  if (jsonDocument["webpass"])
   strncpy(webPass,   jsonDocument["webpass"],WEBPASS_BUFF_SIZE);
    
  if (!strcmp(webPass,DEFAULTPASS)) // Comparing variable to define
  {
    LOGINFO1("Web Password not set",webPass)

    // Only use texts if the default password is not set.
    int cntr = 0;
    for (JsonObject el : jsonDocument["elements"].as<JsonArray>()) {
      // we should check for corruption, but alas.
      if (cntr < MAXTEXTELEMENTS) {

        element[cntr].effect = (textEffect_t)el["effect"].as<int>();
        element[cntr].position = (textPosition_t)el["position"].as<int>();
        element[cntr].repeat = el["repeat"].as<int>();
        element[cntr].speed = el["speed"].as<int>();
        strncpy(element[cntr].text, el["text"].as<const char *>(), TEXTLENGTH);
      }
      cntr++;
    }

    strcpy(element[0].text, "Connect to the server to change your password ");
  }
  return true;
}

bool MatrixConfig::saveConfig() {
  DynamicJsonDocument jsonDocument(CONFIG_BUF_SIZE);
  JsonObject root = jsonDocument.to<JsonObject>();

  root["webpass"] = webPass;

  JsonArray elements = root.createNestedArray("elements");
  for (int i = 0; i < MAXTEXTELEMENTS; i++) {
    JsonObject elmnt = elements.createNestedObject();
    elmnt["text"] = element[i].text;
    elmnt["effect"] = element[i].effect;
    elmnt["position"] = element[i].position;
    elmnt["speed"] = element[i].speed;
    elmnt["repeat"] = element[i].repeat;
  }

  JsonArray ssid = jsonDocument.createNestedArray("WifiCredential_ssid");
  JsonArray pw = jsonDocument.createNestedArray("WifiCredential_pw");

  for (int i = 0; i < NUM_WIFI_CREDENTIALS; i++) {
    // Weed out duplicate credentials
    bool SSIDhasDuplicate = false;
    for (int j = 0; j < i; j++) {
      if (strcmp(WM_config.WiFi_Creds[i].wifi_ssid,
                 WM_config.WiFi_Creds[j].wifi_ssid) == 0) {

        WM_config.WiFi_Creds[i].wifi_pw[0] = '\0';
        WM_config.WiFi_Creds[i].wifi_ssid[0] = '\0';
      }
    }

    ssid.add(WM_config.WiFi_Creds[i].wifi_ssid);
    pw.add(WM_config.WiFi_Creds[i].wifi_pw);
  }

  File configFile = LittleFS.open("/config.json", "w", true);
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    return false;
  }

  WriteLoggingStream loggedFile(configFile, Serial);

  size_t writtenBytes = serializeJson(jsonDocument, loggedFile);

  if (writtenBytes == 0) {
    Serial.println(F("Failed to write to file"));
    return false;
  }
  Serial.println("Bytes written: " + String(writtenBytes));

  configFile.close();
  return true;
}

String MatrixConfig::passForSSID(String _SSID) {
  String pass;
  pass = "";
  for (int i = 0; i < NUM_WIFI_CREDENTIALS; i++) {
    if (String(WM_config.WiFi_Creds[i].wifi_ssid) == _SSID) {
      pass = String(WM_config.WiFi_Creds[i].wifi_pw);
    };
  }
  return (pass);
}