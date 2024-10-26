#include "webInterface.h"
#include "ESPAsyncWebServer.h"
#include "config.h"
#include "debug.h"
#include "defaults.h"
#include "pgmspace.h"
#include "sha1.h"
#include "webInterfaceAPI.h"
#include <Arduino.h>

#include "pages/WebLogin.html.h"
#include "pages/captivePortal.html.h"
#include "pages/helpers.js.h"
#include "pages/networkConfigPage.js.h"
#include "pages/networkSetup.html.h"
#include "pages/redCircleCrossed.svg.h"

#include "pages/CineMatrix.css.h"
#ifdef ELEGANT_OTA
#include <AsyncElegantOTA.h>
#else
#include "webInterfaceOTAUpdate.h"
#endif

webInterface::webInterface(MatrixConfig *config, const char *password) {
  LOGDEBUG0("Webinterfce Constructor");
  _password = password;
  myConfig = config;
  _authRequired = false;
  server = new MatrixWebServer(80, password);
}

webInterface::~webInterface() { LOGDEBUG0("Webinterfce Destructor"); }


extern MD_Parola Display;
void webInterface::setupWebSrv(WiFiManager *wifiMngr) {
 
  LOGINFO0("Setting up Webserver");

  // We set this for later. Wnen there are no credentials set we want to keep
  // the captive portal open - ad infinitum
  _IPnotSetYet = false;

  // Check if we have WIFI confuration
  for (int i = 0; i < NUM_WIFI_CREDENTIALS; i++) {
    if (strlen(myConfig->WM_config.WiFi_Creds[i].wifi_ssid) > 0) {
      _IPnotSetYet = false;
    }
  }

  if (_IPnotSetYet || ALWAYS_START_WITH_PORTAL) {
    LOGINFO0("NO WiFi NEtworks set, we'll later keep the captive portal open");
    wifiMngr->setupWiFiAp(&(myConfig->WM_AP_IPconfig));
    server->reset();
    setConfigPortalPages();
    server->begin(); /// Webserver is now running....
    LOGINFO0("Wifi Manager done, following up with WebSrv");
    wifiMngr
        ->loopPortal(); /// Wait the configuration to be finished or timed out.
  }

  myConfig->loadConfig(); // To make sure we are in sync
  wifiMngr->connectMultiWiFi(myConfig);
  LOGDEBUG0("Resetting the Webserver");
  server->reset();
  // closing the API for unauthorized
  webAPI.requireAuthorization(true);
  //  setupWebSrv(wifiMngr);
  LOGDEBUG0("Starting the Webserver");
  //server->begin(); /// Webserver is now running....

  if (server == nullptr) {
    LOGERROR0("WEBinterface: Server NULLPTR - halting execution");
    while (true) {
      // Hang around infinitly...
      delay(1);
    }
  }
  server->onNotFound(
      std::bind(&webInterface::handleNotFound, this, std::placeholders::_1));
  server->on("/", HTTP_GET,
             std::bind(&webInterface::handleRoot, this, std::placeholders::_1));
  server->on(
      "/reset", HTTP_GET,
      std::bind(&webInterface::handleReset, this, std::placeholders::_1));
  server->on(
      "/index.html", HTTP_GET,
      std::bind(&webInterface::handleIndex, this, std::placeholders::_1));
  server->on(
      "/submit.html", HTTP_GET,
      std::bind(&webInterface::handleSubmission, this, std::placeholders::_1));
  // respond to GET requests on URL /heap
  server->on("/heap", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", String(ESP.getFreeHeap()));
  });

  InitPages(); // sets some default pages.

  webAPI.begin(server, myConfig);
  webAPI.requireAuthorization(true);
  // Static pages
  // from the various include/pages/*.h files come the following definitions.
  DEF_HANDLE_CineMatrix_css;
  DEF_HANDLE_helpers_js;

#ifdef ELEGANT_OTA
  AsyncElegantOTA.begin(server);
#else
  webOTAUpdate.begin(server);
#endif
  server->begin();
  LOGINFO0("HTTP server started");

}

void webInterface::InitPages() {
  server->onNotFound(
      std::bind(&webInterface::handleNotFound, this, std::placeholders::_1));
  server->on(
      "/login", HTTP_POST,
      std::bind(&webInterface::handleLogin, this, std::placeholders::_1));
  server->on(
      "/logout", HTTP_GET,
      std::bind(&webInterface::handleLogout, this, std::placeholders::_1));

  DEF_HANDLE_WebLogin_html;
  DEF_HANDLE_CineMatrix_css;
  DEF_HANDLE_redCircleCrossed_svg;
  DEF_HANDLE_helpers_js;

}

void webInterface::handleNotFound(AsyncWebServerRequest *request) {

  String message = htmlHeader;
  message += "<H1>Error 400 <br/> File Not Found</H1>\n\n";
  message += "<div id=\"notFoundInfo\"><div id=\"notFoundURI\">URI: <span "
             "id=\"notFoundURL\">";
  message += request->url();
  message += "</span></div>\n<div id=\"notFoundMethod\">Method: ";
  message += (request->method() == HTTP_GET) ? "GET" : "POST";
  message += "</div>\n<div id=\"notFoundArguments\">Arguments: ";
  message += request->args();
  message += "</div>\n";

  for (uint8_t i = 0; i < request->args(); i++) {
    message +=
        "<div class=\"notFoundArgument\"><span class=\"notFoundargName\">" +
        request->argName(i) + "</span>:<span class=\"notFoundarg\"> " +
        request->arg(i) + "</span></div>\n";
  }
  message += "</div>";
  message += htmlFooter;
  request->send(404, "text/html", message);
}

void webInterface::handleReset(AsyncWebServerRequest *request) {
  String message =
      "<head><meta http-equiv=\"refresh\" content=\"2;url=/\">\n<meta "
      "name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" "
      "/><title>CineMatrix</title></head>";
  message += "<h1> Reseting Device ! </h1>";
  request->send(200, "text/html", message);
  ESP.restart();
}

void webInterface::handleRoot(AsyncWebServerRequest *request) {
  request->redirect("/index.html");
}

void webInterface::handleIndex(AsyncWebServerRequest *request) {

  // Following are Constructed from  MD_Parola.h
  struct PositionStruct_t PositionEntry[] = {
      {PA_LEFT, "LEFT"}, {PA_CENTER, "CENTER"}, {PA_RIGHT, "RIGHT"}};
  struct EffectStruct_t EffectEntry[] = {
    {PA_NO_EFFECT,
     "No Effect"},       // "Used as a place filler executes no operation"
    {PA_PRINT, "Print"}, //  "Text just appears (printed)"
    {PA_SCROLL_UP, "Scroll Up"},     //  "Text scrolls up through the display"
    {PA_SCROLL_DOWN, "Scroll Down"}, //  "Text scrolls down through the display"
    {PA_SCROLL_LEFT,
     "Scroll Left"}, //  "Text scrolls right to left on the display"
    {PA_SCROLL_RIGHT,
     "Scroll Right"}, //  "Text scrolls left to right on the display"
#if ENA_SPRITE
    {PA_SPRITE, "Movie"}, //  "Text enters and exits using user defined sprite"
#endif
#if ENA_MISC
    {PA_SLICE, "Slice"}, //  "Text enters and exits a slice (column) at a time
                         //  from the right"
    {PA_MESH, "Mesh"},   //  "Text enters and exits in columns moving in
                         //  alternate direction (U/D)"
    {PA_FADE, "Fade"},   //  "Text enters and exits by fading from/to 0 and
                         //  intensity setting"
    {PA_DISSOLVE, "Dissolve"},  //  "Text dissolves from one display to another"
    {PA_BLINDS, "Blinds"},      //  "Text is replaced behind vertical blinds"
    {PA_RANDOM, "Random dots"}, //  "Text enters and exits as random dots"
#endif                          // ENA_MISC
#if ENA_WIPE
    {PA_WIPE, "Wipe"}, // "Text appears disappears one column at a time, looks
                       // like it is wiped on and off"
    {PA_WIPE_CURSOR,
     "Wipe Cursor"}, //  "WIPE with a light bar ahead of the change"
#endif               // ENA_WIPES
#if ENA_SCAN
    {PA_SCAN_HORIZ,
     "Scan Horizontal led"}, //  "Scan the LED column one at a time then
                             //  appears/disappear at end"
    {PA_SCAN_HORIZX,
     "Scan Horizontal blank"}, //  "Scan a blank column through the text one
                               //  column at a time then appears/disappear at
                               //  end"
    {PA_SCAN_VERT, "Scan Vertical led"}, //  "Scan the LED row one at a time
                                         //  then appears/disappear at end"
    {PA_SCAN_VERTX,
     "Scan Vertical blank"}, //  "Scan a blank row through the text one row at
                             //  a time then appears/disappear at end"
#endif                       // ENA_SCAN
#if ENA_OPNCLS
    {PA_OPENING, "Opening"}, // "Appear and disappear from the center of the
                             // display},  towards the ends"
    {PA_OPENING_CURSOR,
     "Opening Cursor"},      //  "OPENING with light bars ahead of the change"
    {PA_CLOSING, "Closing"}, // "Appear and disappear from the ends of the
                             // display}, towards the middle"
    {PA_CLOSING_CURSOR,
     "Closing Cursor"}, //  "CLOSING with light bars ahead of the change"
#endif                  // ENA_OPNCLS
#if ENA_SCR_DIA
    {PA_SCROLL_UP_LEFT, "Scroll Up&Left"},  //  "Text moves in/out in a diagonal
                                            //  path up and left (North East)"
    {PA_SCROLL_UP_RIGHT, "Scrol Up&Right"}, //  "Text moves in/out in a diagonal
                                            //  path up and right (North West)"
    {PA_SCROLL_DOWN_LEFT,
     "Scrol Down&Left"}, //  "Text moves in/out in a diagonal path down and
                         //  left (South East)"
    {PA_SCROLL_DOWN_RIGHT,
     "Scroll Down&Right"}, //  "Text moves in/out in a diagonal path down and
                           //  right (North West)"
#endif                     // ENA_SCR_DIA
#if ENA_GROW
    {PA_GROW_UP, "Grow Up"},    //  "Text grows from the bottom up and shrinks
                                //  from the top down"
    {PA_GROW_DOWN, "Grow Down"} // "Text grows from the top down and and
                                // shrinks from the bottom up"}
#endif                          // ENA_
  };

  bool advancedReq = false;
  for (uint8_t i = 0; i < request->args(); i++) {
    if (request->argName(i) == "advanced")
      advancedReq = true;
  }

  String message = htmlHeader;
  message += "<H1>CineMatrix</H1>";

  if (strcmp(myConfig->webPass, DEFAULTPASS)) {
    LOGDEBUG0("Loading index.html while password is not the default")

    requireAuthorization(true);

    if (_authRequired && !server->authenticate(request)) {
      return;
    }

    message += "<div class=\"matrixform\"> \n";
    message += "<form action=\"/submit.html\"> \n";
    if (!advancedReq) {
      for (int i = 0; i < MAXTEXTELEMENTS; i++) {
        if (strcmp(myConfig->element[i].field, "")) {
          message += "<div class=\"valueentryblock\" >\n";
          message += "\t<div class=\"valueentry\"> <label for=\"valueentry" +
                     String(i) + "\">" + String(myConfig->element[i].field) +
                     ":</label>\n";
          message +=
              "\t<input class=\"valueentry\" type=\"text\" id=\"valueentry" +
              String(i) + "\" name=\"valueentry" + String(i) + "\"";
          message += " maxlength=" + String(FIELDVALUELENGTH);
          message += " value=\"" + String(myConfig->element[i].value) + "\"";

          message += "/>";
          message += "</div>\n";
          message += "</div>\n";
        }
      }
    } else {
      message += "<div class=\"fieldform\">";
      message +=
          "<div class=\"fieldformheader\">Provide individual fieldnames</div>";
      message += "<div class=\"fieldformexplanation\">These are the names of "
                 "the fields that can be used in the texts below and for which "
                 "you can enter values on the home page.</div>";
      for (int i = 0; i < MAXTEXTELEMENTS; i++) {
        message += "<div class=\"fieldentryblock\" >\n";
        message += "\t<div class=\"fieldentry\"> <label for=\"fieldentry" +
                   String(i) + "\"> Field " + String(i) + ":</label>\n";
        message += "\t<input class=\"fieldentry\" id=\"fieldentry" + String(i) +
                   "\" name=\"fieldentry" + String(i) + "\"";
        message += " maxlength=" + String(FIELDVALUELENGTH);
        message += " value=\"" + String(myConfig->element[i].field) + "\"";

        message += "/>";

        message += "</div>\n";
        message += "</div>\n";
      }
      message += "</div>";

      message += "<div class=\"matrixentryform\">";
      message += "<div class=\"matrixentryformheader\">Provide individual "
                 "texts</div>";
      message += "<div class=\"matrixentryexplanation\">"
                 "These are the texts that appear on the Matrix display. Use a "
                 "fieldname between a set of two square brackets, "
                 "[[likeThis]], so that the vallue that you have entered on "
                 "the first page will be substituted."
                 "</div>";
      for (int i = 0; i < MAXTEXTELEMENTS; i++) {
        message += "<div class=\"matrixentry\" >\n";
        message += "\t<div class=\"textentry\"> <label for=\"textentry" +
                   String(i) + "\">Text:</label>\n";
        message += "\t<textarea id=\"textentry" + String(i) +
                   "\" name=\"textentry" + String(i) + "\"";
        message += " maxlength=" + String(TEXTLENGTH) + ">";
        message += String(myConfig->element[i].text) + "</textarea></div>\n";

        message += "<div class=\"inputblock\" >\n";
        message +=
            "\t\t<label for=\"effects" + String(i) + "\"> Effect: </label>\n";
        message += "\t\t<select name=\"effect" + String(i) + "\" id=\"effects" +
                   String(i) + "\">\n";

        for (EffectStruct_t e : EffectEntry) {
          message += "\t\t\t<option value=" + String(e.effect);
          if (e.effect == myConfig->element[i].effect) {
            message += " selected";
          }

          message += " >" + e.label + "</option>\n";
        }
        message += "\t</select>\n";
        message += "</div><!-- class=inputblock-->\n";
        message += "<div class=\"inputblock\" >\n";
        message += "\t\t<label for=\"positions" + String(i) +
                   "\"> Position: </label>\n";
        message += "\t\t<select name=\"position" + String(i) +
                   "\" id=\"positions" + String(i) + "\">\n";

        for (PositionStruct_t p : PositionEntry) {
          message += "\t\t\t<option value=" + String(p.position);
          if (p.position == myConfig->element[i].position) {
            message += " selected";
          }

          message += +" >" + p.label + "</option>\n";
        }
        message += "\t</select>\n\n";
        message += "</div><!-- class=inputblock-->\n";
        message += "<div class=\"inputblock\" >\n";
        message += "\t<label for=\"speed" + String(i) + "\">Speed:</label>\n";
        message += "\t<input id=\"speed" + String(i) + "\" name=\"speed" +
                   String(i) + "\" style=\"width:4em;\"";
        message += " value=" + String(myConfig->element[i].speed) +
                   " type=\"number\" minlength=1 maxlength=4 >";
        message += "</div><!-- class=inputblock-->\n";
        message += "<div class=\"inputblock\" >\n";
        message += "\t<label for=\"repeat" + String(i) + "\">Repeat:</label>\n";
        message += "\t<input id=\"repeat" + String(i) + "\" name=\"repeat" +
                   String(i) + "\" style=\"width:3em;\"";
        message += " value=" + String(myConfig->element[i].repeat) +
                   " type=\"number\" minlength=1 maxlength=2 >";
        message += "</div><!-- class=inputblock-->\n";

        message += "</div>\n";
      }
      message += "</div>\n";
    } // End advancedReq

    message += " <div class=\"submitbutton\"> <input type=\"submit\" "
               "value=\"Submit\"> </div>\n";
    message += "</form></div>\n";

    if (advancedReq) {

      message += "<div class=\"scriptbutton\" id=\"backbutton\"> <button "
                 "onclick=\"backtoindexButton()\">"
                 "Back </button><script>function backtoindexButton() {  "
                 "location.replace(\"/index.html\")}</script></div>";

      message += "<div class=\"scriptbutton\" id=\"updatebutton\"> <button "
                 "onclick=\"updateButton()\"> "
                 "Update Firmware </button><script>function updateButton() {  "
                 "location.replace(\"/update.html\")}</script></div>";

    } else {
      message += "<div class=\"scriptbutton\" id=\"advancedbutton\"> <button "
                 "onclick=\"advancedButton()\">Advanced "
                 "Configuration</button><script>function advancedButton() {  "
                 "location.replace(\"/index.html?advanced\")}</script></div>";
    }
    message += "<div class=\"scriptbutton\" id=\"logoutbutton\"> <button "
               "onclick=\"logout()\">Log "
               "Out</button><script>function logout() {  "
               "location.replace(\"/logout\")}</script></div>";

  } else {
    // we really want authorization off when changing the default password.
    requireAuthorization(false);
    message += "<div class=\"warning\"> You must change the WEB "
               "password!</div> <!-- div class=warning--> ";

    message += "<div class=\"webpassform\"> \n";
    message += "<form action=\"/submit.html\" onsubmit=\"return "
               "CompareFields(this)\"> \n";
    message += "<div class=\"webpassentry\">";
    message += "<div class=\"webpassinput\">";
    message += "<div class=\"webpasslabel\"> <label for=\"webpass\">password"
               ":</label></div> <!-- div class=webpasslabel -->\n";
    message += "<div class=\"webpassfield\"> <input id=\"field1\" "
               "name=\"webpass1\" maxlength=32   ></div> "
               "<!-- div class=webpassfield -->\n";
    message += "</div> <!-- div  class=webpassinput -->\n";
    message += "<div class=\"webpassinput\">";
    message += "<div class=\"webpasslabel\"> <label for=\"pass\"> repeat "
               "password:</label></div> <!-- div class=webpasslabel -->\n";
    message +=
        "<div class=\"webpassfield\"><input id=\"field2\" name=\"webpass2\" "
        "maxlength=32 required >"
        "</div><!-- div class=webpassfield -->\n";
    message += "</div> <!-- div  class=webpassinput -->\n";
    message += "</div><!--div class=webpassentry -->\n";
    message += " <div class=\"submitbutton\"> <input type=\"submit\" "
               "value=\"Submit\"> </div>\n";
    message += "</form></div>\n";

    message +=
        "<div class=\"scriptbutton\" id=\"advancedbutton\"> <button "
        "onclick=\"updateButton()\">Update</button><script>function "
        "updateButton() {  location.replace(\"/update\")}</script></div>";

    message += "<div class=\"scriptbutton\"id=\"advancedbutton\"> <button "
               "onclick=\"indexButton()\">Back</button><script>function "
               "indexButton() "
               "{  location.replace(\"/index.html\")}</script></div>";
  }

  if (!advancedReq) { // display preview of the matrix text
    message +=
        "<div class=\"sixtyfour-convergence-matrixrss\" id=\"matrixpreview\">";
    for (int i = 0; i < MAXTEXTELEMENTS; i++) {
      if (strcmp(myConfig->element[i].matrixtext, "")) {

        message += "<div class=\"matrixpreviewline\">";
        message += String(myConfig->element[i].matrixtext);
        message += "</div>";
      }
    }

    message += "</div>";
  }

  message +=
      "<div class=\"firmware\"> Firmware version: " + String(FIRMWAREVERSION) +
      " - " + String(F(__DATE__)) + ":" + String(F(__TIME__)) + "</div>";

  message += htmlFooter;
  request->send(200, "text/html", message);
}

void webInterface::handleSubmission(AsyncWebServerRequest *request) {
  if (_authRequired && !server->authenticate(request)) {
    LOGDEBUG1("Not Authenticated", request->url())
    return;
  }
  bool reconf = false;
  for (uint8_t i = 0; i < request->args(); i++) {

    const AsyncWebParameter *p = request->getParam(i);
    LOGDEBUG3("sumbit.html parsing", p->name(), "=", p->value());
    if (p->name() == "webpass1") {
      // check on consistency
      for (uint8_t j = 0; j < request->args(); j++) {
        const AsyncWebParameter *r = request->getParam(j);
        if ((r->name() == "webpass2") && p->value() == r->value()) {

          if (!p->value().equals(myConfig->webPass)) {
            strncpy(myConfig->webPass, p->value().c_str(), WEBPASS_BUFF_SIZE);
            LOGINFO1("Changing web password to:", (myConfig->webPass));
         
            reconf = true;
          }
        }
      }
    } else {
      for (int j = 0; j < MAXTEXTELEMENTS; j++) {
        if (p->name() == "textentry" + String(j)) {
          p->value().toCharArray(myConfig->element[j].text, TEXTLENGTH);
          reconf = true;
        }

        if (p->name() == "fieldentry" + String(j)) {
          p->value().toCharArray(myConfig->element[j].field, FIELDVALUELENGTH);
          reconf = true;
        }

        if (p->name() == "valueentry" + String(j)) {
          p->value().toCharArray(myConfig->element[j].value, FIELDVALUELENGTH);
          reconf = true;
        }

        else if (p->name() == "effect" + String(j)) {
          // We should be checking this input against the allowed values
          // But the effect is simply that nothing is displayed
          myConfig->element[j].effect = (textEffect_t)p->value().toInt();
          reconf = true;
        }

        else if (p->name() == "position" + String(j)) {

          myConfig->element[j].position = (textPosition_t)p->value().toInt();
          reconf = true;
        }

        else if (p->name() == "speed" + String(j)) {

          myConfig->element[j].speed = p->value().toInt();
          reconf = true;
        } else if (p->name() == "repeat" + String(j)) {
          myConfig->element[j].repeat = p->value().toInt();
          reconf = true;
        }
      }
    }
  }

  if (reconf) {
    LOGDEBUG("Saving CONFIG")
    myConfig->saveConfig();
  }
  request->redirect("/");
};

void webInterface::handleFile(AsyncWebServerRequest *request,
                              const char *mimetype,
                              const unsigned char *compressedData,
                              const size_t compressedDataLen) {
  AsyncWebServerResponse *response =
      request->beginResponse(200, mimetype, compressedData, compressedDataLen);
  response->addHeader("Server", "ESP Async Web Server");
  response->addHeader("Content-Encoding", "gzip");
  request->send(response);
}

void webInterface::setConfigPortalPages() {
  LOGDEBUG0("SetConfigPortalPages");
  InitPages();
  server->on("/scan", HTTP_GET,
             std::bind(&webInterface::handleScan, this, std::placeholders::_1));

  server->on("/", HTTP_GET,
             std::bind(&webInterface::handleCaptivePortal, this,
                       std::placeholders::_1));
  server->on("/configConfig", HTTP_GET,
             std::bind(&webInterface::handleConfigConfig, this,
                       std::placeholders::_1));
  server->on(
      "/restart", HTTP_GET,
      std::bind(&webInterface::handleRestart, this, std::placeholders::_1));
  server->on("/networkSetup.html", HTTP_GET,
             std::bind(&webInterface::handleNetworkSetup, this,
                       std::placeholders::_1));

  server->on("/exitconfig", HTTP_GET, [&](AsyncWebServerRequest *request) {
    _waitingForClientAction = false;
    request->redirect("/");
  });

  DEF_HANDLE_networkConfigPage_js;
  DEF_HANDLE_captivePortal_html;
  webAPI.begin(server, myConfig);
  webAPI.requireAuthorization(
      false); // The API is wide open during the configportal phase

  return;
}

void webInterface::handleScan(AsyncWebServerRequest *request) {
  LOGDEBUG0("Scan Handle");
  String json = "[";
  int n = WiFi.scanComplete();
  if (n == -2) {
    LOGERROR0("Scanning no result, initiating no scan");
    WiFi.scanNetworks(true);
  } else if (n) {
    for (int i = 0; i < n; ++i) {
      if (i)
        json += ",";
      json += "{";
      json += "\"rssi\":" + String(WiFi.RSSI(i));
      json += ",\"ssid\":\"" + WiFi.SSID(i) + "\"";
      json += ",\"pass\":\"" + myConfig->passForSSID(WiFi.SSID(i)) + "\"";
      json += ",\"bssid\":\"" + WiFi.BSSIDstr(i) + "\"";
      json += ",\"channel\":" + String(WiFi.channel(i));
      json += ",\"secure\":" + String(WiFi.encryptionType(i));
      json += "}";
    }
    WiFi.scanDelete();
    if (WiFi.scanComplete() == -2) {
      WiFi.scanNetworks(true);
    }
  }
  json += "]";
  request->send(200, "application/json", json);
  json = String();
};

void webInterface::handleLogout(AsyncWebServerRequest *request) {
  Serial.println("Disconnection");
  AsyncWebServerResponse *response =
      request->beginResponse(301); // Sends 301 redirect
  response->addHeader("Location", "/WebLogin.html?msg=User disconnected");
  response->addHeader("Cache-Control", "no-cache");

  response->addHeader("Set-Cookie",
                      (String)COOKIENAME +
                          "=0 ;expires=Thu, 01 Jan 1970 00:00:00 GMT;");
  request->send(response);
  return;
}

void webInterface::handleLogin(AsyncWebServerRequest *request) {
  LOGINFO("Handle login");
  String msg;
  if (request->hasHeader("Cookie")) {
    // Print cookies
    String cookie = request->header("Cookie");
    LOGINFO1("Found cookie: ", cookie);
  }
  if (request->hasArg("password")) {
    LOGINFO0("Found parameter: ");
    if (request->arg("password") == String(myConfig->webPass)) {
      AsyncWebServerResponse *response =
          request->beginResponse(301); // Sends 301 redirect

      String RedirectURL = "/";
      if (request->hasArg("url")) {
        RedirectURL = request->arg("url");
      }
      response->addHeader("Location", RedirectURL);
      response->addHeader("Cache-Control", "no-cache");
      String token = sha1(String(_password) + ":" +
                          request->client()->remoteIP().toString());
      LOGINFO1("Token: ", token);
      response->addHeader("Set-Cookie",
                          (String)COOKIENAME + "=" + (String)token +
                              "; Max-Age=" + (String)COOKIEMAXAGE);
      request->send(response);
      LOGINFO("Log in Successful");
      return;
    }
    msg = "Wrong password! try again.";
    Serial.println("Log in Failed");
    AsyncWebServerResponse *response =
        request->beginResponse(301); // Sends 301 redirect

    response->addHeader("Location", "/WebLogin.html?msg=" + msg);
    response->addHeader("Cache-Control", "no-cache");
    request->send(response);
    return;
  }
}

// helper function for below checks whether a value is in a vectpr
using namespace std;
template <typename T>

bool contains(vector<T> vec, const T &elem) {
  bool result = false;
  if (find(vec.begin(), vec.end(), elem) != vec.end()) {
    result = true;
  }
  return result;
}

unsigned long webInterface::remainingPortaltime() {
  return (std::max(
      (unsigned long)0,
      (_configPortalInterfaceStart + CONFIGPORTAL_TIMEOUT - millis()) / 1000));
}

void webInterface::handleConfigConfig(AsyncWebServerRequest *request) {
  vector<String> wifinets;

  String json = "{";
  json += "\"timeout\":" + String(remainingPortaltime());
  json += ",\"maxNets\":" + String(NUM_WIFI_CREDENTIALS);

  json += ",\"wifiNets\":[";
  bool firstSSID = false;
  String networklist;
  for (int i = 0; i < NUM_WIFI_CREDENTIALS; i++) {
    String tmpssid = myConfig->WM_config.WiFi_Creds[i].wifi_ssid;
    if (tmpssid && tmpssid != "") {
      if (!contains(wifinets, tmpssid)) {
        wifinets.push_back(tmpssid);
      }
    }
  }

  for (int i = 0; i < wifinets.size(); i++) {
    if (i) {
      json += ", ";
    }
    json += "\"" + wifinets[i] + "\"";
  }

  json += "]";
  json += "}";
  request->send(200, "application/json", json);
  json = String();
}

void webInterface::handleNetworkSetup(
    AsyncWebServerRequest *request) { // only used as config portal
  LOGINFO0("Blocking for finalizing input");
  _waitingForClientAction = true;

  AsyncWebServerResponse *response = request->beginResponse(
      200, "text/html;charset=UTF-8", networkSetup_html, networkSetup_html_len);
  response->addHeader("Content-Encoding", "gzip");
  response->addHeader("Access-Control-Allow-Origin", "WM_HTTP_CORS_ALLOW_ALL");
  request->send(response);
  return;
}

void webInterface::handleCaptivePortal(AsyncWebServerRequest *request) {
  LOGINFO0("CaptivePortal Hit")

  if (captivePortal(request)) {
    // If captive portal redirect instead of displaying the error page.
    return;
  }

  AsyncWebServerResponse *response =
      request->beginResponse(200, "text/html;charset=UTF-8", captivePortal_html,
                             captivePortal_html_len);
  response->addHeader("Content-Encoding", "gzip");
  response->addHeader("Access-Control-Allow-Origin", "WM_HTTP_CORS_ALLOW_ALL");
  request->send(response);

  return;
}

/**
   HTTPD redirector
   Redirect to captive portal if we got a request for another domain.
   Return true in that case so the page handler do not try to handle the
   request again.
*/
bool webInterface::captivePortal(AsyncWebServerRequest *request) {

  if (!isIp(request->host())) {
    LOGINFO1(F("Incomming request"), request->url());
    LOGINFO(F("Request redirected to captive portal"));
    LOGINFO1(F("Location http://"), (request->client()->localIP()).toString());

    AsyncWebServerResponse *response =
        request->beginResponse(302, "text/plain", "");
    response->addHeader("Location",
                        String("http://") +
                            (request->client()->localIP()).toString());

    request->send(response);

    return true;
  }

  LOGDEBUG1(F("request host IP ="), request->host());

  return false;
}

void webInterface::handleRestart(AsyncWebServerRequest *request) {
  String message =
      "<head><meta http-equiv=\"refresh\" content=\"2;url=/\">\n<meta "
      "name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" "
      "/><title>EspressIoT</title></head>";
  message += "<h1> Restarting Device ! </h1>";
  request->send(200, "text/html", message);
  delay(1000);
  ESP.restart();
}

// Is this an IP?
bool webInterface::isIp(const String &str) {
  for (unsigned int i = 0; i < str.length(); i++) {
    int c = str.charAt(i);

    if (c != '.' && c != ':' && (c < '0' || c > '9')) {
      return false;
    }
  }

  return true;
}

void webInterface::requireAuthorization(bool require) {
  _authRequired = require;
  return;
}
