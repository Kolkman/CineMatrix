#pragma once
#ifndef wifiManager_h
#define wifiManager_h

#include <ESPAsyncDNSServer.h>
#include "defaults.h"
#include "WiFiMulti.h"
#include <MD_Parola.h>
// #include "webinterface.h"

#define NUM_WIFI_CREDENTIALS 2
// Assuming max 49 c§ars
#define TZNAME_MAX_LEN 50
#define TIMEZONE_MAX_LEN 50
#define SSID_MAX_LEN 32
#define PASS_MAX_LEN 64

#define AP_PASSWORD "E32" // HEX will be added

#define MIN_AP_PASSWORD_SIZE 8
#define DNS_PORT 53
#define RFC952_HOSTNAME_MAXLEN  63 // HOSTNAME can be up to 255 chars, but we'll take DNS label length.
     // (longer than  in original code)
#ifndef RFC952_HOSTNAME
#define RFC952_HOSTNAME "CineMatrix"
#endif


#define MAX_WIFI_CHANNEL 13
#define ALWAYS_START_WITH_PORTAL true

#ifndef CONFIGPORTAL_TIMEOUT
#define CONFIGPORTAL_TIMEOUT 60 * 1000
#endif

#define WIFI_MULTI_CONNECT_WAITING_MS                                          \
  10 * 1000 // MultiWifi reconnects after 10 seconds.
#define WIFI_MULTI_1ST_CONNECT_WAITING_MS 3 * 1000

#include <esp_wifi.h>
#define ESP_getChipId() ((uint32_t)ESP.getEfuseMac())

#ifndef CONFIG_NETWORK_PASSWORD
#define CONFIG_NETWORK_PASSWORD "GerritMatrix"
#endif

#if !(defined(ESP32))
#error This code is intended to run on the ESP32 platform! Please check your Tools->Board setting.
#endif

// WifiManager related config

typedef struct {
  IPAddress _ap_static_ip;
  IPAddress _ap_static_gw;
  IPAddress _ap_static_sn;
} WiFi_AP_IPConfig;

typedef struct {
  IPAddress _sta_static_ip;
  IPAddress _sta_static_gw;
  IPAddress _sta_static_sn;
  IPAddress _sta_static_dns1;
  IPAddress _sta_static_dns2;
} WiFi_STA_IPConfig;

typedef struct {
  char wifi_ssid[SSID_MAX_LEN];
  char wifi_pw[PASS_MAX_LEN];
  bool config_change; // used to see if during configuration the wifi changes
                      // without a password (it should then be deleted)
} WiFi_Credentials;

typedef struct {
  String wifi_ssid;
  String wifi_pw;
} WiFi_Credentials_String;
typedef struct {
  WiFi_Credentials WiFi_Creds[NUM_WIFI_CREDENTIALS];
  char TZ_Name[TZNAME_MAX_LEN]; // "America/Toronto"
  char TZ[TIMEZONE_MAX_LEN];    // "EST5EDT,M3.2.0,M11.1.0"
  uint16_t checksum;
} WM_Config;

class MatrixConfig; // Forward declaration

class webInterface; // Forward declaration

class WiFiManager : public WiFiMulti {
public:
  WiFiManager(webInterface *);
  void setupWiFiAp(WiFi_AP_IPConfig *);
  void loopPortal();
  uint8_t connectMultiWiFi(MatrixConfig *);

  AsyncDNSServer *dnsServer;

  // SSID and PW for Config Portal

private:
  webInterface *myInterface;
  String ApSSID;
  String ApPass;
  bool connect;
  char *getRFC952_hostname(const char *);
  char RFC952_hostname[RFC952_HOSTNAME_MAXLEN + 1];
  unsigned long _configPortalStart = 0;
};

#endif
