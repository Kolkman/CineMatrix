// Wrapper for some authentication functions
#include <Arduino.h>
#include "MatrixWebServer.h"
#include <ESPAsyncWebServer.h>
#include "debug.h"
#include "sha1.h"




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
    LOGINFO0("authenticate for:" + request->url());
    if (authRequired)
    {
        if (!request->authenticate(this->getUsername(), this->getPassword()))
        {
            return request->requestAuthentication();
        }
    }
}


// Check if header is present and correct
bool MatrixWebServer::is_authenticated(AsyncWebServerRequest *request)
{
    LOGDEBUG0("Enter is_authenticated");
    if (request->hasHeader("Cookie"))
    {
        String cookie = request->header("Cookie");
        LOGINFO1("Found cookie: ", cookie);

        String token = sha1(String(username) + ":" +
                            String(password) + ":" +
                            request->client()->remoteIP().toString());
        //  token = sha1(token);
        String ck = (String)COOKIENAME + "=" + token;
        if (cookie.indexOf(ck) != -1)
        {
            LOGINFO("    Authentication Successful");
            return true;
        }
        LOGINFO1("Cookiename:", COOKIENAME);
        LOGINFO1("Authentication Failed against:", ck);
    }
    else
    {
        LOGINFO0("    No cookie in header.");
    }
    return false;
}
