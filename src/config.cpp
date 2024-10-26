#include "config.h"
#include "debug.h"
#include "defaults.h"
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <StreamUtils.h>

MatrixConfig::MatrixConfig() {
  LOGDEBUG0("MatrixConfig constructor")
  strncpy(webPass, DEFAULTPASS, WEBPASS_BUFF_SIZE);

  setDefaults();

  for (int i = 0; i < NUM_WIFI_CREDENTIALS; i++) {
    *WM_config.WiFi_Creds[i].wifi_ssid = '\0';
    *WM_config.WiFi_Creds[i].wifi_pw = '\0';
  }
  WM_AP_IPconfig._ap_static_ip = {192, 168, 100, 1};
  WM_AP_IPconfig._ap_static_gw = {192, 168, 100, 1};
  WM_AP_IPconfig._ap_static_sn = {255, 255, 255, 0};

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

void MatrixConfig::setDefaults() {
  LOGDEBUG0("Setting Defaults");
  for (int i = 0; i < MAXTEXTELEMENTS; i++) {
    strcpy(element[i].text, "");
    element[i].effect = PA_SCROLL_LEFT;
    element[i].position = PA_RIGHT;
    element[i].speed = DEFAULT_SPEED;
    element[i].repeat = 1;
  }
  strncpy(element[0].text, DEFAULTLINE0, TEXTLENGTH);
  strncpy(element[1].text, DEFAULTLINE1, TEXTLENGTH);
  strncpy(element[2].text, DEFAULTLINE2, TEXTLENGTH);
  strncpy(element[0].field, DEFAULTFIELD0, FIELDVALUELENGTH);
  strncpy(element[1].field, DEFAULTFIELD1, FIELDVALUELENGTH);
  strncpy(element[2].field, DEFAULTFIELD2, FIELDVALUELENGTH);
  strncpy(element[0].value, DEFAULTVALUE0, FIELDVALUELENGTH);
  strncpy(element[1].value, DEFAULTVALUE1, FIELDVALUELENGTH);
  strncpy(element[2].value, DEFAULTVALUE2, FIELDVALUELENGTH);

  for (int i = 3; i < MAXTEXTELEMENTS; i++) {
    strncpy(element[i].text, "", TEXTLENGTH);
    strncpy(element[i].field, "", FIELDVALUELENGTH);
    strncpy(element[i].value, "", FIELDVALUELENGTH);
  }
  for (int i = 0; i < MAXTEXTELEMENTS; i++) {
    createtMatrixText(&element[i]);
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

  int cntr = 0;
  for (JsonObject el : jsonDocument["elements"].as<JsonArray>()) {
    // we should check for corruption, but alas.
    if (cntr < MAXTEXTELEMENTS) {

      element[cntr].effect = (textEffect_t)el["effect"].as<int>();
      element[cntr].position = (textPosition_t)el["position"].as<int>();
      element[cntr].repeat = el["repeat"].as<int>();
      element[cntr].speed = el["speed"].as<int>();
      if (el["text"]) {
        strncpy(element[cntr].text, el["text"].as<const char *>(), TEXTLENGTH);
      }
      if (el["field"]) {
        strncpy(element[cntr].field, el["field"].as<const char *>(),
                FIELDVALUELENGTH);
      }
      if (el["value"]) {
        strncpy(element[cntr].value, el["value"].as<const char *>(),
                FIELDVALUELENGTH);
      }
      /// Here we  do a bunch magic to create the actual displayd text
      createtMatrixText(&element[cntr]);
    }
    cntr++;
  }
  if (jsonDocument["webpass"])
    strncpy(webPass, jsonDocument["webpass"], WEBPASS_BUFF_SIZE);
  if (!strcmp(webPass, DEFAULTPASS)) {
    LOGDEBUG("Config contains Default Pass");
    strncpy(element[0].matrixtext,
           "Verander eerst het passsword van de CineMatrix server",MATRIXTEXTLENGTH);
    for (int i = 1; i < MAXTEXTELEMENTS; i++) {
      strcpy(element[i].matrixtext, "");
    }
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
    elmnt["field"] = element[i].field;
    elmnt["value"] = element[i].value;
    elmnt["effect"] = element[i].effect;
    elmnt["position"] = element[i].position;
    elmnt["speed"] = element[i].speed;
    elmnt["repeat"] = element[i].repeat;

    for (int i = 0; i < MAXTEXTELEMENTS; i++) {
      createtMatrixText(&element[i]);
    }
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

void MatrixConfig::createtMatrixText(textelements *el) {
  if (!strcmp(el->text, "")) {
    strcpy(el->matrixtext, "");
    return;
  }
  String MatrixString = String(el->text);

  for (int i = 0; i < MAXTEXTELEMENTS; i++) {
    MatrixString.replace("[[" + String(element[i].field) + "]]",
                         element[i].value);
  }
  strncpy(el->matrixtext, MatrixString.c_str(), MATRIXTEXTLENGTH);
}
