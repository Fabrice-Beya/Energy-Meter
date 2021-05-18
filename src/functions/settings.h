#ifndef SETTINGS
#define SETTINGS

#include <Arduino.h>
#include "globals.h"
#include "config.h"
#include "enums.h"
#include "functions/utils.h"

void print_settings();

void load_vPreferences()
{
    vPreferences.begin("device", false);
    vDeviceInfo.name = vPreferences.getString("name", "");
    vDeviceInfo.domain = vPreferences.getString("domain", "");
    vPreferences.end();

    vPreferences.begin("firebase", false);
    vDeviceInfo.uid = vPreferences.getString("uid", "");
    vDeviceInfo.api_key = vPreferences.getString("api", "");
    vDeviceInfo.db_url = vPreferences.getString("db_url", "");
    vDeviceInfo.storage_bucket = vPreferences.getString("storage_bucket", "");
    vDeviceInfo.email = vPreferences.getString("email", "");
    vDeviceInfo.firebase_password = vPreferences.getString("password", "");
    vPreferences.end();

    vPreferences.begin("credentials", false);
    vDeviceInfo.wifi_ssid = vPreferences.getString("ssid", "");
    vDeviceInfo.wifi_password = vPreferences.getString("password", "");
    vPreferences.end();

    vState.set(STATE_PREFERENCES_READY);

    print_settings();
}

void print_settings()
{
    Serial.printf("\nName: ");
    Serial.println(vDeviceInfo.name);
    Serial.printf("Domain: ");
    Serial.println(vDeviceInfo.domain);
    Serial.printf("API KEY: ");
    Serial.println(vDeviceInfo.api_key);
    Serial.printf("DB URL: ");
    Serial.println(vDeviceInfo.db_url);
    Serial.printf("Storage Bucket: ");
    Serial.println(vDeviceInfo.storage_bucket);
    Serial.printf("Email: ");
    Serial.println(vDeviceInfo.email);
    Serial.printf("Password: ");
    Serial.println(vDeviceInfo.firebase_password);
    Serial.printf("UID: ");
    Serial.println(vDeviceInfo.uid);
    Serial.printf("SSID: ");
    Serial.println(vDeviceInfo.wifi_ssid);
    Serial.printf("WIFI Password: ");
    Serial.println(vDeviceInfo.wifi_password + "\n");

}

#endif