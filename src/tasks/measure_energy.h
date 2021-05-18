#ifndef MEASURE_ENERGY
#define MEASURE_ENERGY

#include <Arduino.h>
#include "globals.h"
#include "config.h"
#include "enums.h"
#include "functions/utils.h"
#include "EmonLib.h"
#include "tasks/logger.h"
#include "tasks/firebase.h"
#include <driver/adc.h>

// extern void energy_log();

/**
 * Array to hold energy measurements
 */
Measurement measurements[30];
short measureIndex = 0;

/**
 * Instance of the Open Energy Library
 */
EnergyMonitor emon1;

/**
 * Track current sensor initialisation
 */
bool isReady = false;

/**
 * Intialise ADC and Open Energy Library.
 */
void init()
{
  // Setup the ADC
  adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11);
  analogReadResolution(ADC_BITS);
  pinMode(ADC_INPUT, INPUT);

  // Initialize emon library
  emon1.current(ADC_INPUT, 30);

  isReady = true;
  System_Log("Energy sensor intialisattion successfull.");
}

/**
 * Compute the latest energy measurements and uploads them to firebase.
 */
static void Task_Energy(void *pvParameters)
{
  // Initialise sensor peripheral.
  init();

  while (1)
  {

    // Wait 1 seconds before taking readings
    while (!vState.check(STATE_SYSTEM_READY))
    {
      vTaskDelay(TASK_LOCK_PERIOD / portTICK_PERIOD_MS);
    }

    // Hold task start timestamp.
    long start = millis();

    // Get current measurement & compute electrical energy reading
    double amps = emon1.calcIrms(1480);
    float watts = amps * HOME_VOLTAGE;

    // Log energy reading to SD card, except when syncing with ther server.
    if (!vState.check(STATE_ENERGY_SYNCING))
    {
      Energy_Log(watts);

      FirebaseJson energy_json;

      energy_json.add("value", watts);
      energy_json.add("timestamp", vTimers.timestamp);

      // Send json data to firebase database
      String path = vDeviceInfo.name + "-" + EnergySensor.name + "/data";

      if (xSemaphoreTake(xMutexs.xMutex_firebase, portMAX_DELAY) == pdTRUE)
      {
        // Serial.printf("\nTask_Energy holding the firebase Mutex...\n");
        if (!Firebase.RTDB.setJSON(&vFirebase_t.fbdo, path.c_str(), &energy_json))
        {
          Serial.println(vFirebase_t.fbdo.errorReason());
        }

        xSemaphoreGive(xMutexs.xMutex_firebase);
        // Serial.printf("\nTask_Energy  gives firebase mutex...\n");
      }
    } else {
       Serial.printf("\nTask_Energy is paused due to syncing operation...\n");
    }

    if (TASK_MONITORING)
    {
      // Compute task execution duration in ms
      long TaskDuration = millis() - start;
      Serial.printf("\nTask_Energy Execution Duration: l%d ms\n", TaskDuration);
    }

    // Sleep for [ENERGY_MEASUREMENT_INTERVAL] seconds before taking next reading.
    vTaskDelay(ENERGY_MEASUREMENT_INTERVAL / portTICK_PERIOD_MS);
  }
}

#endif