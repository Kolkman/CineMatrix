#include "webInterface.h"
#include "config.h"
#include <Arduino.h>

#include "pages/CineMatrix.css.h"

WebInterface::WebInterface(const char *username, const char *password)
{
  Serial.println("Webinterfce Constructor");
  _username = username;
  _password = password;
  server = new MatrixWebServer(80, username, password);
}

WebInterface::~WebInterface()
{
  Serial.println("Webinterfce Destructor");
}

void WebInterface::setupWebSrv()
{
  Serial.println("Setting up  Webserver");

  if (server == nullptr)
  {
    Serial.println("WEBinterface: Server NULLPTR - halting execution");
    while (true)
    {
      // Hang around infinitly...
      delay(1);
    }
  }
  server->onNotFound(std::bind(&WebInterface::handleNotFound, this, std::placeholders::_1));
  server->on("/", HTTP_GET, std::bind(&WebInterface::handleRoot, this, std::placeholders::_1));
  server->on("/reset", HTTP_GET, std::bind(&WebInterface::handleReset, this, std::placeholders::_1));
  server->on("/index.html", HTTP_GET, std::bind(&WebInterface::handleIndex, this, std::placeholders::_1));


  // respond to GET requests on URL /heap
  server->on("/heap", HTTP_GET, [](AsyncWebServerRequest *request)
             { request->send(200, "text/plain", String(ESP.getFreeHeap())); });

  /*
  #ifdef ELEGANT_OTA
    AsyncElegantOTA.begin(server);
  #else
    webOTAUpdate.begin(server);
  #endif
  */

  // Static pages
  // from the various include/pages/*.h files come the following definitions.
  DEF_HANDLE_CineMatrix_css;



  server->begin();
  Serial.println("HTTP server started");
}

void WebInterface::handleNotFound(AsyncWebServerRequest *request)
{
  String message = htmlHeader;
  message += "<H1>Error 400 <br/> File Not Found</H1>\n\n";
  message += "<div id=\"notFoundInfo\"><div id=\"notFoundURI\">URI: <span id=\"notFoundURL\">";
  message += request->url();
  message += "</span></div>\n<div id=\"notFoundMethod\">Method: ";
  message += (request->method() == HTTP_GET) ? "GET" : "POST";
  message += "</div>\n<div id=\"notFoundArguments\">Arguments: ";
  message += request->args();
  message += "</div>\n";

  for (uint8_t i = 0; i < request->args(); i++)
  {
    message += "<div class=\"notFoundArgument\"><span class=\"notFoundargName\">" + request->argName(i) + "</span>:<span class=\"notFoundarg\"> " + request->arg(i) + "</span></div>\n";
  }
  message += "</div>";
  message += htmlFooter;
  request->send(404, "text/html", message);
}

void WebInterface::handleReset(AsyncWebServerRequest *request)
{
  String message = "<head><meta http-equiv=\"refresh\" content=\"2;url=/\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>CineMatrix</title></head>";
  message += "<h1> Reseting Device ! </h1>";
  request->send(200, "text/html", message);
  ESP.restart();
}

void WebInterface::handleRoot(AsyncWebServerRequest *request)
{
  request->redirect("/index.html");
}

void WebInterface::handleIndex(AsyncWebServerRequest *request)
{
  String message = htmlHeader;
  message += "<H1>Gerrit Lichtkrant</H1>";

  message += htmlFooter;
  request->send(200, "text/html", message);
}


void WebInterface::handleFile(AsyncWebServerRequest *request, const char *mimetype, const unsigned char *compressedData, const size_t compressedDataLen)
{
  AsyncWebServerResponse *response = request->beginResponse_P(200, mimetype, compressedData, compressedDataLen);
  response->addHeader("Server", "ESP Async Web Server");
  response->addHeader("Content-Encoding", "gzip");
  request->send(response);
}
