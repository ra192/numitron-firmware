#include <prefs.h>
#include <LittleFS.h>

void loadPreferences(Preferences &prefs)
{
    File file = LittleFS.open("/prefs.json", "r");
    if (!file)
    {
        Serial.println("Failed to open preferences file");
        return;
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, file);
    if (error)
    {
        Serial.println("Failed to parse preferences file");
        return;
    }

    prefs.wifiMode = doc["wifiMode"] | 1;
    strlcpy(prefs.hostname, doc["hostname"] | "pulsar-clock", sizeof(prefs.hostname));
    strlcpy(prefs.ssid, doc["ssid"] | "", sizeof(prefs.ssid));
    strlcpy(prefs.password, doc["password"] | "12345678", sizeof(prefs.password));
    strlcpy(prefs.timezone, doc["timezone"] | "GMT0", sizeof(prefs.timezone));

    file.close();
}

void savePreferences(const Preferences &prefs)
{
    File file = LittleFS.open("/prefs.json", "w");
    if (!file)
    {
        Serial.println("Failed to open preferences file for writing");
        return;
    }

    JsonDocument doc;
    doc["ssid"] = prefs.ssid;
    doc["password"] = prefs.password;
    doc["timezone"] = prefs.timezone;
    doc["wifiMode"] = prefs.wifiMode;

    if (serializeJson(doc, file) == 0)
    {
        Serial.println("Failed to write preferences to file");
    }

    file.close();
}

Preferences prefs;