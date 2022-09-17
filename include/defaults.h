#ifndef MATRIXDEFAULT_H
#define MATRIXDEFAULT_H

// HARDWARE
#define MAX_DEVICES 12 // Number of 8x8 Matrices


#define DATA_PIN 25
#define CS_PIN 26
#define CLK_PIN 27


#define DEFAULT_SPEED 75

#define WIFI_SSID "ESP32_MATRIX" // Not longer than 32 characters
#define WIFI_PASS "Elvis_in_Gerrit"   // / Not longer than 32 characters

#define WEB_IP "192.168.122.1"
#define WEB_NET "192.168.122.0" // should be the same NET as WEB_IP is in
                                // Hardcoded as /24

#define TEXTLENGTH 256 // Maximum length of text to be displayed

#define MAXTEXTS 5 // Amount of different sentences to display

#endif