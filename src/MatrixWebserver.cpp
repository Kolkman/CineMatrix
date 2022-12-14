// Wrapper for some authentication functions
#include <Arduino.h>
#include "MatrixWebServer.h"
#include <ESPAsyncWebServer.h>

MatrixWebServer::MatrixWebServer(uint16_t port, const char *username, const char *password) : AsyncWebServer(port)
{
    setUsername(username);
    setPassword(password);
    if (strlen(username) > 0)
    {
        authRequired = true;
    }
    else
    {
        authRequired = false;
    }
}

char *MatrixWebServer::getUsername()
{
    return username;
}

char *MatrixWebServer::getPassword()
{
    return password;
}

void MatrixWebServer::setPassword(const char *p = "")
{
    memset(password, '\0', CREDENTIAL_LENGTH);
    strncpy(password, p, std::max(strlen(p), (size_t)(CREDENTIAL_LENGTH - 1)));
}
void MatrixWebServer::setUsername(const char *u = "")
{
    memset(username, '\0', CREDENTIAL_LENGTH);
    strncpy(username, u, std::max(strlen(u), (size_t)(CREDENTIAL_LENGTH - 1)));
}

void MatrixWebServer::authenticate(AsyncWebServerRequest *request)
{
    Serial.println("authenticate for:" + request->url());
    if (authRequired)
    {
        if (!request->authenticate(this->getUsername(), this->getPassword()))
        {
            return request->requestAuthentication();
        }
    }
}
