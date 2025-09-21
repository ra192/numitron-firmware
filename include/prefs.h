#ifndef _PREFS_H
#define _PREFS_H

#include <Arduino.h>
#include <ArduinoJson.h>

struct Preferences {
    uint8_t wifiMode; // 0 for STA, 1 for AP
    char hostname[32];
    char ssid[32];
    char password[64];
    char timezone[64];
};

extern uint8_t wifiNetworksCount;

void loadPreferences(Preferences &prefs);

void savePreferences(const Preferences &prefs);

extern Preferences prefs;

#endif