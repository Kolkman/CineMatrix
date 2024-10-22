// MatrixWebserver is a AsyncWebserver with additional password management
#ifndef DEF_MYwebServer_H
#define DEF_MYwebServer_H

#include <ESPAsyncWebServer.h>
#include "config.h"
#define CREDENTIAL_LENGTH 64

#ifndef COOKIENAME
#define COOKIENAME "CineMatrix_ID"
#endif

class MatrixWebServer : public AsyncWebServer
{
public:
    MatrixWebServer(uint16_t, const char *, const char *);
    bool authRequired;

    void setPassword(const char *);
    void setUsername(const char *);

    void authenticate(AsyncWebServerRequest *);
    char *getUsername();
    char *getPassword();
    bool is_authenticated(AsyncWebServerRequest *);
  
   

  
  
private:
    char username[CREDENTIAL_LENGTH + 1];
    char password[CREDENTIAL_LENGTH + 1];
};

#endif