#include "config.h"
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <StreamUtils.h>

MatrixConfig::MatrixConfig()
{
    defaultPASS = true;
    strcpy(wifiSSID, WIFI_SSID);
    strcpy(wifiPASS, WIFI_PASS);
    for (int i = 0; i < MAXTEXTELEMENTS; i++)
    {
        strcpy(element[i].text, "");
        element[i].effect = PA_SCROLL_LEFT;
        element[i].position = PA_RIGHT;
        element[i].speed = DEFAULT_SPEED;
        element[i].repeat = 1;
    }
    // Default
    strcpy(element[0].text, "Connect to Wifi network ");
    strcat(element[0].text, WIFI_SSID);
    strcpy(element[1].text, "The WIFI Password is: ");
    strcat(element[1].text, WIFI_PASS);

    strcpy(element[2].text, "Browse to http:://");
    strcat(element[2].text, WEB_IP);
    strcat(element[2].text,"/");


    Serial.println(element[0].text);
}
bool MatrixConfig::prepareFS()
{
    if (!LittleFS.begin(false /* false: Do not format if mount failed */))
    {
        Serial.println("Failed to mount LittleFS");
        if (!LittleFS.begin(true /* true: format */))
        {
            Serial.println("Failed to format LittleFS");
            return false;
        }
        else
        {
            Serial.println("LittleFS formatted successfully");
            return true;
        }
    }
    else
    { // Initial mount success
        return true;
    }
}

bool MatrixConfig::loadConfig()
{
    File configFile = LittleFS.open("/config.json", "r");
    if (!configFile)
    {
        Serial.println("Failed to open config file");
        return false;
    }

    StaticJsonDocument<CONFIG_BUF_SIZE> jsonDocument;
    ReadLoggingStream loggingStream(configFile, Serial);

    DeserializationError parsingError = deserializeJson(jsonDocument, loggingStream);
    Serial.println("");
    if (parsingError)
    {

        Serial.println("Failed to deserialize json config file");
        Serial.println(parsingError.c_str());
        return false;
    }

    {
        strncpy(wifiSSID, jsonDocument["wifiSSID"], 32);
        wifiSSID[33] = '\0';
    }
    if (jsonDocument["wifiPASS"])
    {
        Serial.print("Config file WifiPass:");

        strncpy(wifiPASS, jsonDocument["wifiPASS"], 32);
        wifiPASS[33] = '\0';
        Serial.println(wifiPASS);
    }
    if (strcmp(wifiPASS, WIFI_PASS)) // Comparing variable to define
    {

        defaultPASS = false;

        // Only use texts if the default password is not set.
        int cntr = 0;
        for (JsonObject el : jsonDocument["elements"].as<JsonArray>())
        {
            // we should check for corruption, but alas.
            if (cntr < MAXTEXTELEMENTS)
            {

                element[cntr].effect = (textEffect_t)el["effect"].as<int>();
                element[cntr].position = (textPosition_t)el["position"].as<int>();
                element[cntr].repeat = el["repeat"].as<int>();
                element[cntr].speed = el["speed"].as<int>();
                strncpy(element[cntr].text, el["text"].as<const char *>(), TEXTLENGTH);
            }
            cntr++;
        }

        if (!strncmp(element[0].text, "Please connect to the following network: ", 41))
        {
            strcpy(element[0].text, "Connect to the network you configured");
        }
        if (!strncmp(element[1].text, "The WIFI Password is: ", 21))
        {
            strcpy(element[1].text, "");
        }
    }
    else
    {

        Serial.println("DEFAULT WIFI PASSWORD");
        defaultPASS = true;
    }

    return true;
}

bool MatrixConfig::saveConfig()
{
    DynamicJsonDocument jsonDocument(CONFIG_BUF_SIZE);
    JsonObject root = jsonDocument.to<JsonObject>();
    root["wifiSSID"] = wifiSSID;
    root["wifiPASS"] = wifiPASS;

    JsonArray elements = root.createNestedArray("elements");
    for (int i = 0; i < MAXTEXTELEMENTS; i++)
    {
        JsonObject elmnt = elements.createNestedObject();
        elmnt["text"] = element[i].text;
        elmnt["effect"] = element[i].effect;
        elmnt["position"] = element[i].position;
        elmnt["speed"] = element[i].speed;
        elmnt["repeat"] = element[i].repeat;
    }

    File configFile = LittleFS.open("/config.json", "w", true);
    if (!configFile)
    {
        Serial.println("Failed to open config file for writing");
        return false;
    }

    WriteLoggingStream loggedFile(configFile, Serial);

    size_t writtenBytes = serializeJson(jsonDocument, loggedFile);

    if (writtenBytes == 0)
    {
        Serial.println(F("Failed to write to file"));
        return false;
    }
    Serial.println("Bytes written: " + String(writtenBytes));

    configFile.close();
    return true;
}