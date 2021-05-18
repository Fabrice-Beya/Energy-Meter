#ifndef WIFI
#define WIFI

#include <Arduino.h>
#include "globals.h"
#include "config.h"
#include "enums.h"
#include <WiFi.h>
#include "tasks/timer.h"

static void Task_WiFi_Management(void *pvParameters)
{
  
  System_Log("WiFi Management Task started.");

  while(1){

    if(WiFi.status() == WL_CONNECTED){
      vState.set(STATE_WIFI_READY);
      vTaskDelay(10000 / portTICK_PERIOD_MS);
      continue;
    }
    
    // Check if we have wifi details configured.
    if (vDeviceInfo.wifi_ssid == "" || vDeviceInfo.wifi_password == "")
    {
      vState.clear(STATE_WIFI_READY);
      System_Log("\nNo values saved for ssid or password");
    }
    else
    {

      // Hold task start timestamp.
      long start = millis();

      // Configure wire for station mode.
      WiFi.mode(WIFI_STA);
      
      // Use wifi details from flash memory to set the host name.
      WiFi.setHostname(vDeviceInfo.name.c_str());

      // Attempted to connect to WiFi.
      WiFi.begin(vDeviceInfo.wifi_ssid.c_str(), vDeviceInfo.wifi_password.c_str());

      // Get wifi connection start time.
      unsigned long startAttemptTime = millis();

      // Print a dot every 0.5 seconds whilst waiting for connection.  
      while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < WIFI_TIMEOUT)
      {
        Serial.print(".");
        vTaskDelay(500 / portTICK_PERIOD_MS);
      }

      // If connection is not achievhed after 20 seconds put to task in sleep mode for another 20 seconds before trying to connect again.
      if(WiFi.status() != WL_CONNECTED){
        vState.clear(STATE_WIFI_READY);
       System_Log("Connecting to WiFi failed.");
        vTaskDelay(WIFI_RECOVER_TIME_MS / portTICK_PERIOD_MS);

      } else {

        // Connection is successful, Log IP address in system logs
        vState.set(STATE_WIFI_READY);
        String eventMessage = "Wifi connected with IP:" + WiFi.localIP().toString();
        System_Log(eventMessage);

        if(TASK_MONITORING){
          // Compute task execution duration in ms
          long TaskDuration = millis() - start;
          Serial.printf("\nTask_WiFi_Management Execution Duration: l%d ms\n", TaskDuration);
        }
      }
    }
  }
}

#endif