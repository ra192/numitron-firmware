#include <reqHandlers.h>

#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <prefs.h>

void reqRoot(AsyncWebServerRequest *request)
{
    request->redirect("/index.html");
}

void reqGetNetworkSettings(AsyncWebServerRequest *request)
{
    StaticJsonDocument<1024> doc;

    doc["wifi_mode"] = prefs.wifiMode;
    doc["ssid"] = prefs.ssid;
    doc["hostname"] = prefs.hostname;

    JsonArray ssidList = doc.createNestedArray("ssid_list");
    uint8_t netCnt = WiFi.scanNetworks();
    for (int i = 0; i < netCnt; i++)
    {
        ssidList.add(WiFi.SSID(i));
    }

    String jsonStr;
    serializeJson(doc, jsonStr);

    request->send(200, "application/json", jsonStr);
}

void reqSaveNetworkSettings(AsyncWebServerRequest *request)
{
    if (request->hasParam("wifi_mode", true) && request->hasParam("ssid", true) && request->hasParam("password", true) && request->hasParam("hostname", true))
    {
        uint8_t wifiMode = request->getParam("wifi_mode", true)->value().toInt();
        String ssid = request->getParam("ssid", true)->value();
        String password = request->getParam("password", true)->value();
        String hostname = request->getParam("hostname", true)->value();

        prefs.wifiMode = wifiMode;
        strlcpy(prefs.ssid, ssid.c_str(), sizeof(prefs.ssid));
        strlcpy(prefs.password, password.c_str(), sizeof(prefs.password));
        strlcpy(prefs.hostname, hostname.c_str(), sizeof(prefs.hostname));

        savePreferences(prefs);

        request->send(200, "application/json", "{\"status\":\"success\"}");
    }
    else
    {
        request->send(400, "application/json", "{\"status\":\"error\", \"message\":\"Missing parameters\"}");
    }
}

void reqGetTime(AsyncWebServerRequest *request)
{
    StaticJsonDocument<256> doc;
    doc["timezone"] = prefs.timezone;

    String jsonStr;
    serializeJson(doc, jsonStr);

    request->send(200, "application/json", jsonStr);
}

void reqSaveTime(AsyncWebServerRequest *request)
{
    if (request->hasParam("timezone", true))
    {
        String timezone = request->getParam("timezone", true)->value();
        strlcpy(prefs.timezone, timezone.c_str(), sizeof(prefs.timezone));

        savePreferences(prefs);

        request->send(200, "application/json", "{\"status\":\"success\"}");
    }
    else
    {
        request->send(400, "application/json", "{\"status\":\"error\", \"message\":\"Missing parameters\"}");
    }
}