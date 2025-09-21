#ifndef _REQHANDLERS_H
#define _REQHANDLERS_H

#include <Arduino.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

void reqGetNetworkSettings(AsyncWebServerRequest *request);

void reqSaveNetworkSettings(AsyncWebServerRequest *request);

void reqGetTime(AsyncWebServerRequest *request);

void reqSaveTime(AsyncWebServerRequest *request);

#endif