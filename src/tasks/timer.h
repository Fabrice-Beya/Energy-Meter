#ifndef TIMER
#define TIMER

#include <Arduino.h>
#include "globals.h"
#include "config.h"
#include "enums.h"
#include <WiFi.h>
#include <NTPClient.h>
#include "functions/utils.h"
#include "tasks/logger.h"

/**
 * Define NTP Client to get time
 */
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_SERVER, NTP_OFFSET_SECONDS, NTP_UPDATE_INTERVAL_MS);

static void Task_Timer(void *pvParameters)
{
  System_Log("NTP Timer Task started.");

  while (1)
  {
    // Hold task start timestamp.
    long start = millis();

    // Check that WiFi is connected before requesting time update.
    if(!WiFi.isConnected()){
      vState.clear(STATE_NTP_READY);
      vTaskDelay(10*1000 / portTICK_PERIOD_MS);
      continue;
    }

    // Attempt to update the timer, else force an timer udpate.
    while (!timeClient.update())
    {
      timeClient.forceUpdate();
    }
   
    vTimers.timestamp = timeClient.getEpochTime();
    // Serial.printf("\nUnix timeStamp: %d", timeStamp);

    vTimers.formatedTime = timeClient.getFormattedTime();
    // Serial.println("\nFormated timeStamp: " + formatedTime);

    vState.set(STATE_NTP_READY);

    if (TASK_MONITORING) {
      // Compute task execution duration in ms
      long TaskDuration = millis() - start;
      Serial.printf("\nTask_Timer Execution Duration: l%d ms\n", TaskDuration);
    }

    // Sleep for a period(see NTP_UPDATE_INTERVAL_MS) before checking again
    vTaskDelay(NTP_UPDATE_INTERVAL_MS / portTICK_PERIOD_MS);
  }
}

static void Task_Elapsed_Timer(void *pvParameters)
{
  System_Log("Elapse Task started.");

  while (1)
  {
    // Hold task start timestamp.
    long start = millis();

    reset_wdt();
    vTimers.elapseTime = millis() - vTimers.initialTime;
    // Serial.printf("\nElapsed time: %d ms\n", elapseTime);

  if (TASK_MONITORING) {
    // Compute task execution duration in ms
    long TaskDuration = millis() - start;
    Serial.printf("\nTask_Elapsed_Timer Execution Duration: l%d ms\n", TaskDuration);
  }
  }
}

#endif