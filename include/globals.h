#ifndef GLOBALS
#define GLOBALS

#include <Arduino.h>
#include <WiFi.h>
#include <NTPClient.h>
#include "FS.h"
#include "SD.h"
#include <SPI.h>
#include "config.h"
#include "enums.h"
#include <Preferences.h>

/**
 * Object used to add, remove and update value into device permanent memory.
 */
Preferences vPreferences;

/**
 * Type which contains all device information stored in permanant memory.
 */
DeviceInfo vDeviceInfo;

/**
 * Mutex used to manage access to globat state and hardwware resources.
 */
Mutexs xMutexs;

/**
 * Task handlers for all our tasks
 */
TaskHandlers vTaskHanders;

/**
 * Keeps track of current system time in unit/epoch format.
 */
Timers vTimers;

/**
 * Create a new sensor instance for CT
 */
Sensor EnergySensor = {
    "Energy",
    "SCT013",
    "Living Room", 
    "",
    ""
};

/**
 * Instance of firebase realtime database object.
 */
Firebase_t vFirebase_t;

/**
 * Instance of State class, allow ability to check, set and clear system states.
 */
State vState;

/**
 * Device states to be used with the SystemState instance.
 */
#define STATE_STORAGE_READY 0x1
#define STATE_PREFERENCES_READY 0x2
#define STATE_WIFI_READY 0x4
#define STATE_NTP_READY 0x8
#define STATE_FIREBASE_READY 0x10
#define STATE_SYSTEM_READY 0x20
#define STATE_OPERATING 0x40
#define STATE_SYSTEM_SYNCING 0x60
#define STATE_ENERGY_SYNCING 0x80

#endif