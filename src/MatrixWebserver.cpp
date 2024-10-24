// Wrapper for some authentication functions
#include "MatrixWebServer.h"
#include "debug.h"
#include "sha1.h"
#include <Arduino.h>
#include <ESPAsyncWebServer.h>

MatrixWebServer::MatrixWebServer(uint16_t port, const char *password)
    : AsyncWebServer(port) {

  setPassword(password);
  if (strlen(password) > 0) {
    authRequired = true;
  } else {
    authRequired = false;
  }
}

char *MatrixWebServer::getPassword() { return password; }

void MatrixWebServer::setPassword(const char *p = "") {
  memset(password, '\0', CREDENTIAL_LENGTH);
  strncpy(password, p, std::max(strlen(p), (size_t)(CREDENTIAL_LENGTH - 1)));
}

bool MatrixWebServer::authenticate(AsyncWebServerRequest *request) {
  {      
     LOGINFO1("authenticate for:", String(request->url())); 
        if (is_authenticated(request))
        {

            LOGINFO("Authentication Successful");
            return true;
        }
        LOGINFO0("Authentication Failed");
        AsyncWebServerResponse *response = request->beginResponse(301); // Sends 301 redirect
        response->addHeader("Location", "/WebLogin.html?msg=Authentication Failed, you must log in.");
        response->addHeader("Cache-Control", "no-cache");
        request->send(response);
        return false;
    }
}

// Check if header is present and correct
bool MatrixWebServer::is_authenticated(AsyncWebServerRequest *request) {
  LOGDEBUG0("Enter is_authenticated");
  if (request->hasHeader("Cookie")) {
    String cookie = request->header("Cookie");
    LOGINFO1("Found cookie: ", cookie);

    String token =
        sha1(String(password) + ":" + request->client()->remoteIP().toString());
    //  token = sha1(token);
    String ck = (String)COOKIENAME + "=" + token;
    if (cookie.indexOf(ck) != -1) {
      LOGINFO0("    Authentication Successful");
      return true;
    }
    LOGINFO1("Cookiename:", COOKIENAME);
    LOGINFO1("Authentication Failed against:", ck);
  } else {
    LOGINFO0("    No cookie in header.");
  }
  return false;
}
