/**
 * Created by Fabrice Beya
 * 
 * Email: beyafabrice@gmail.com
 * 
 * Github: https://github.com/Fabrice-Beya/Energy-Meter
 * 
 * License: Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
 *
*/

#include <Arduino.h>
#include "config.h"
#include "enums.h"
#include "globals.h"
#include "tasks/firebase.h"
#include "tasks/logger.h"
#include "tasks/measure_energy.h"
#include "tasks/timer.h"
#include "tasks/wifi.h"
#include "functions/settings.h"
#include "functions/utils.h"

void setup()
{
  // Set the intial device time.
  vTimers.initialTime = millis();

  Serial.begin(115200);

  // Create Mutex's
  xMutexs.xMutex_firebase = xSemaphoreCreateMutex();
  // xMutex_system_file = xSemaphoreCreateMutex();
  // xMutex_energy_file = xSemaphoreCreateMutex();

  // Wait file system to load.
  while(!SD_Init()){}

  System_Log("Device powered on, task initialisation in progress...");

  // Load device information
  load_vPreferences();

  /**
   * TASK: Keep track of elapsed time from device power on.
   */
  xTaskCreate(Task_Elapsed_Timer, "Elapsed Timer", 4096, NULL, 1, &vTaskHanders.TaskHandle_Elapsed_Timer);

  /**
   * TASK: Connect to WiFi and keep the connection alive.
   */
  xTaskCreate(Task_WiFi_Management, "Wifi Task", 5000, NULL, 1, &vTaskHanders.TaskHandle_WiFi_Management);

  /**
   * TASK: Use wifi ntp server to keep track of real world time.
   */
  xTaskCreate(Task_Timer, "Timer Task", 4096, NULL, 1, &vTaskHanders.TaskHandle_Timer);

  // Update device configuration
  // config_vPreferences();

  /**
   * TASK: Firebase management.
   */
  xTaskCreate(Task_Firebase_Management, "Firebase Task", 10000, NULL, 1, &vTaskHanders.TaskHandle_Firebase);
  
  /**
   * TASK: Measure electrical energy consumed.
   */
  xTaskCreate(Task_Energy, "Energy Task", 15000, NULL, 1, &vTaskHanders.TaskHandle_Energy);

  /**
   * TASK: System logs synchronisation with server
   */
  xTaskCreate(Task_System_Sync, "System Log Sync", 20000, NULL, 1, &vTaskHanders.TaskHandle_System_Sync);

  /**
   * TASK: Energy logs synchronisation with server
   */
  xTaskCreate(Task_Energy_Sync, "Energy Log Sync", 20000, NULL, 1, &vTaskHanders.TaskHandle_Energy_Sync);

  // Wait for [SYSTEM_INIT_PERIOD] seconds for all initialisation to be done.
  vTaskDelay(SYSTEM_INIT_PERIOD / portTICK_PERIOD_MS);

  // Update system ready status
  vState.set(STATE_SYSTEM_READY);

  System_Log("System intialisation completed. ");

  // Delete the setup and loop tasks.
  vTaskDelete(NULL);
}

void loop()
{
}