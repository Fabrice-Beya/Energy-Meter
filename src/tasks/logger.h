#ifndef LOGGER
#define LOGGER

#include <Arduino.h>
#include "globals.h"
#include "config.h"
#include "enums.h"
#include "FS.h"
#include "SD.h"
#include <SPI.h>

/**
 * Task to upload system log files to firebase storage every period = SYSTEM_LOGS_SYNC_INTERVAL
 */
static void Task_System_Sync(void *pvParameters)
{
  
  while (1)
  {
    // Prevent upload when system initialisation is not ready, and request access to system file mutex before proceeding.
    while(!vState.check(STATE_SYSTEM_READY)){
       vTaskDelay(TASK_LOCK_PERIOD / portTICK_PERIOD_MS);
    }

    // Wait [ENERGY_LOGS_SYNC_INTERVAL] seconds before uploading energy logs
    vTaskDelay(SYSTEM_LOGS_SYNC_INTERVAL / portTICK_PERIOD_MS);
    
    // Hold the current time.
    long start = millis();

    if (xSemaphoreTake(xMutexs.xMutex_firebase, portMAX_DELAY) == pdTRUE){
      vState.set(STATE_SYSTEM_SYNCING);
      // Serial.printf("\nTask_System_Sync holding the firebase Mutex...\n");

      if (Firebase.Storage.upload(&vFirebase_t.fbdo, vDeviceInfo.storage_bucket.c_str(), SYSTETM_FILE, mem_storage_type_sd , "system.txt" , "text/plain"))
        {
          Serial.printf("\nUpload successful...\n");
          vPreferences.begin("firebase");
          vPreferences.putString("system_log_url", vFirebase_t.fbdo.downloadURL().c_str());
          vPreferences.end();
        }
        else
        {
          Serial.println("Upload failed: " + vFirebase_t.fbdo.errorReason());
        }

        xSemaphoreGive(xMutexs.xMutex_firebase);
        vState.clear(STATE_ENERGY_SYNCING);

    } 

    if(TASK_MONITORING){
      // Compute task execution duration in ms
      long taskDuration = millis() - start;
      Serial.printf("\nSystem Sync Task Execution Duration: %ld ms\n", taskDuration);
    }
  }
}

/**
 * Task to upload energy log files to firebase storage every period = SYSTEM_LOGS_SYNC_INTERVAL
 */
static void Task_Energy_Sync(void *pvParameters)
{
  
  while (1)
  {
    // Prevent upload when system initialisation is not ready, and request access to system file mutex before proceeding.
    while(!vState.check(STATE_SYSTEM_READY)){
      vTaskDelay(TASK_LOCK_PERIOD / portTICK_PERIOD_MS);
    }

    // Wait [ENERGY_LOGS_SYNC_INTERVAL] seconds before uploading energy logs
    vTaskDelay(ENERGY_LOGS_SYNC_INTERVAL / portTICK_PERIOD_MS);
    
    // Hold task start timestamp.
    long start = millis();

    if (xSemaphoreTake(xMutexs.xMutex_firebase, portMAX_DELAY) == pdTRUE){
      vState.set(STATE_ENERGY_SYNCING);

      // Serial.printf("\nTask_Energy_Sync holding the firebase Mutex...\n");

      if (Firebase.Storage.upload(&vFirebase_t.fbdo, vDeviceInfo.storage_bucket.c_str(), ENERGY_FILE, mem_storage_type_sd , "energy.txt" , "text/plain"))
        {
          Serial.printf("\nUpload successful...\n");
          vPreferences.begin("firebase");
          vPreferences.putString("energy_log_url", vFirebase_t.fbdo.downloadURL().c_str());
          vPreferences.end();
        }
        else
        {
          Serial.println("Upload failed: " + vFirebase_t.fbdo.errorReason());
        }

        xSemaphoreGive(xMutexs.xMutex_firebase);
        vState.clear(STATE_ENERGY_SYNCING);
        // Serial.println("Task_Energy_Sync give firebase mutex");
    } 

   

   if(TASK_MONITORING){
      // Compute task execution duration in ms
      long taskDuration = millis() - start;
      Serial.printf("\nEnergy Task Sync Execution Duration: %ld ms\n", taskDuration);
   }
  }
}

/**
 * Append data to the SD card (DON'T MODIFY THIS FUNCTION)
 */
void appendFile(fs::FS &fs, const char *path, const char *message)
{

  File file = fs.open(path, FILE_APPEND);
  if (!file)
  {
    Serial.println("Failed to open file for appending");
    return;
  }
  if (!file.print(message))
  {
    Serial.println("Append failed");
  }

  file.close();
}

/**
 * Write to the SD card (DON'T MODIFY THIS FUNCTION)
 */
void writeFile(fs::FS &fs, const char *path, const char *message)
{
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file)
  {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message))
  {
    Serial.println("File written");
  }
  else
  {
    Serial.println("Write failed");
  }
  file.close();
}

/**
 * Add system logs to system.txt file
 */
void System_Log(String systemEvent)
{
  // if (xSemaphoreTake(xMutex_system_file, portMAX_DELAY) == pdTRUE){
    String message = "";
    if (vTimers.formatedTime == "") {
      message = systemEvent + "\r\n";   
    } else {
      message = vTimers.formatedTime + ": " + systemEvent + "\r\n";
    }

    Serial.println("\n" + message);
    appendFile(SD, SYSTETM_FILE, message.c_str());

  //   xSemaphoreGive(xMutex_system_file);
  // }
}

/**
 * Add energy measurement logs to energy.txt file.
 */
void Energy_Log(float value)
{
  // if (xSemaphoreTake(xMutex_energy_file, portMAX_DELAY) == pdTRUE){
    String message = String(vTimers.timestamp) + ";" + value + "\r\n";
    Serial.println("\n" + message);
    appendFile(SD, ENERGY_FILE, message.c_str());

  //   xSemaphoreGive(xMutex_energy_file);
  // }
}

/**
 * Initialise SD Card 
 * 1. Create two files, energy.txt and system.txt
 * 2. Get the last set of information saved.
 */
bool SD_Init()
{
  // Hold task start timestamp.
  long start = millis();

  Serial.println("\nSD Card intialisation started.");
  
  // Initialize SD Card SPI peripheral
  SD.begin(SD_CS);
  if (!SD.begin(SD_CS))
  {
    Serial.println("Card Mount Failed");
    vState.clear(STATE_STORAGE_READY);
    return false;
  }

  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE)
  {
    Serial.println("No SD card attached");
    vState.clear(STATE_STORAGE_READY);
    return false;
  }

  if (!SD.begin(SD_CS))
  {
    Serial.println("ERROR - SD card initialization failed!");
    vState.clear(STATE_STORAGE_READY);
    return false; // init failed
  }

  // Create a files to system logs, and energy measuresments
  File energyFile = SD.open(ENERGY_FILE);
  File systemFile = SD.open(SYSTETM_FILE);

  // Check if energy files already exist, if not create it and add headings to the file.
  if (!systemFile)
  {
    writeFile(SD, SYSTETM_FILE, "System logs file created successfully. \r\n");
  }
  else
  {
    System_Log("System logs file initialised successfully.");

    // TODO: Read the last line to get the last system events.
  }
  systemFile.close();

  // Check if energy files already exist, if not create it and add headings to the file.
  if (!energyFile)
  {
    writeFile(SD, ENERGY_FILE, "Timestamp;Readings \r\n");
    System_Log("Energy logs file created successfully.");
  }
  else
  {
    System_Log("Energy logs file initialised successfully.");

    // TODO: Read the last line to get the last readings saved.
  }
  energyFile.close();

  vState.set(STATE_STORAGE_READY);
  System_Log("SD Card files initialised successfully.");

  return true;
}

#endif