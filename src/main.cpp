#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <LittleFS.h>

#include <prefs.h>
#include <numitron.h>


#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include <reqHandlers.h>

#include <RTClib.h>
#include <FastLED.h>

#define LED_PIN 12
#define NUM_LEDS 4

#define WIFI_CONNECTION_TIMEOUT 10000 // 10 seconds

#define TIME_UPDATE_INTERVAL 1000
#define TIME_SYNC_INTERVAL 60000
#define LEDS_UPDATE_INTERVAL 20

typedef enum
{
  CLOCK_MODE_TIME,
  CLOCK_MODE_DATE
} ClockMode;

static AsyncWebServer server(80);

static ClockMode clockMode = CLOCK_MODE_TIME;

RTC_DS3231 rtc;

CRGB leds[NUM_LEDS];

void displayTime();

void displayDate();

void syncTimeWithRTC();

void updateLEDs();

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

  server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");

  server.on("/get-network-settings", HTTP_GET, reqGetNetworkSettings);
  server.on("/save-network-settings", HTTP_POST, reqSaveNetworkSettings);

  server.on("/get-time", HTTP_GET, reqGetTime);
  server.on("/save-time", HTTP_POST, reqSaveTime);

  server.begin();

  wifiNetworksCount = WiFi.scanNetworks();

  rtc.begin();

  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
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

  syncTimeWithRTC();
  updateLEDs();
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

void syncTimeWithRTC()
{
  static unsigned long lastTimeSync = 0;

  unsigned long ms = millis();

  if (ms - lastTimeSync < TIME_SYNC_INTERVAL)
    return;

  lastTimeSync = ms;

  DateTime now = rtc.now();
  struct tm t;
  t.tm_sec = now.second();
  t.tm_min = now.minute();
  t.tm_hour = now.hour();
  t.tm_mday = now.day();
  t.tm_mon = now.month() - 1; // Note: tm_mon is 0-11, not 1-12
  t.tm_year = now.year() - 1900; // Note: tm_year is years since 1900
  t.tm_isdst = -1; // Not considering Daylight Saving Time

  time_t timeSinceEpoch = mktime(&t);
  struct timeval tv = { .tv_sec = timeSinceEpoch, .tv_usec = 0 };
  settimeofday(&tv, nullptr);
}

void updateLEDs()
{
  static unsigned long lastLEDsUpdate = 0;
  unsigned long ms = millis();

  if (ms - lastLEDsUpdate < LEDS_UPDATE_INTERVAL)
    return;

  lastLEDsUpdate = ms;

  static uint8_t hue = 0;
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CHSV(hue + (i * 10), 255, 50);
  }
  FastLED.show();
  hue++;
}