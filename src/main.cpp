#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include <LittleFS.h>

#include <prefs.h>
#include <numitron.h>
#include <reqHandlers.h>

#define LED_PIN 12

#define WIFI_CONNECTION_TIMEOUT 10000 // 10 seconds

#define TIME_UPDATE_INTERVAL 1000

typedef enum
{
  CLOCK_MODE_TIME,
  CLOCK_MODE_DATE
} ClockMode;

static AsyncWebServer server(80);

static ClockMode clockMode = CLOCK_MODE_TIME;

void displayTime();

void displayDate();

void setup()
{
  Serial.begin(115200);

  numitron_init();
  numitron_setBrightness(64);

  LittleFS.begin();
  loadPreferences(prefs);

  if (prefs.wifiMode == WIFI_AP)
  {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(prefs.hostname, prefs.password);
    Serial.println("WiFi Access Point started.");
  }
  else
  {
    WiFi.mode(WIFI_STA);
    WiFi.begin(prefs.ssid, prefs.password);
    Serial.println("WiFi Station mode started.");

    if (WiFi.waitForConnectResult(WIFI_CONNECTION_TIMEOUT) != WL_CONNECTED)
    {
      Serial.println("Couldn't connect to wifi network");
      WiFi.mode(WIFI_AP);
      WiFi.softAP(prefs.hostname, prefs.password);
    }
    else
    {
      Serial.println("\nWiFi connected.");
    }
  }

  configTzTime(prefs.timezone, "pool.ntp.org", "time.nist.gov");

  server.on("/", HTTP_GET, reqRoot);
  server.serveStatic("/index.html", LittleFS, "/index.html");

  server.on("/get-network-settings", HTTP_GET, reqGetNetworkSettings);
  server.on("/save-network-settings", HTTP_POST, reqSaveNetworkSettings);

  server.on("/get-time", HTTP_GET, reqGetTime);
  server.on("/save-time", HTTP_POST, reqSaveTime);

  server.begin();
}

tm tmInfo;
uint8_t digs[NUMITRON_DIGIT_COUNT];

void loop()
{
  switch (clockMode)
  {
  case CLOCK_MODE_TIME:
    displayTime();
    break;
  
  case CLOCK_MODE_DATE:
    displayDate();
    break; 

  default:
    break;
  }
}

void displayTime()
{
  static unsigned long lastTimeUpdate = 0;
  unsigned long ms = millis();

  if (ms - lastTimeUpdate < TIME_UPDATE_INTERVAL)
    return;

  lastTimeUpdate = ms;

  getLocalTime(&tmInfo, 10);

  digs[0] = (uint8_t)(tmInfo.tm_hour / 10);
  digs[1] = (uint8_t)(tmInfo.tm_hour % 10);
  digs[2] = (uint8_t)(tmInfo.tm_min / 10);
  digs[3] = (uint8_t)(tmInfo.tm_min % 10);
  digs[4] = (uint8_t)(tmInfo.tm_sec / 10);
  digs[5] = (uint8_t)(tmInfo.tm_sec % 10);

  numitron_display(digs);
}

void displayDate()
{
  static unsigned long lastDateUpdate = 0;
  unsigned long ms = millis();

  if (ms - lastDateUpdate < TIME_UPDATE_INTERVAL)
    return;

  lastDateUpdate = ms;

  getLocalTime(&tmInfo, 10);

  digs[0] = (uint8_t)((tmInfo.tm_mday) / 10);
  digs[1] = (uint8_t)((tmInfo.tm_mday) % 10);
  digs[2] = (uint8_t)((tmInfo.tm_mon + 1) / 10);
  digs[3] = (uint8_t)((tmInfo.tm_mon + 1) % 10);
  digs[4] = (uint8_t)((tmInfo.tm_year + 1900) / 10 % 10);
  digs[5] = (uint8_t)((tmInfo.tm_year + 1900) % 10);

  numitron_display(digs);
}
