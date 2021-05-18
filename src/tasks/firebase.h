#ifndef FIREBASE
#define FIREBASE

#include <Arduino.h>
#include "globals.h"
#include "config.h"
#include "enums.h"
#include "functions/utils.h"
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include "timer.h"

/**
 * Update firebase realtime database
 */
bool update_firebase(FirebaseJson jsonObject, String databasePath)
{
  Serial.println("Uploading to firebase....");

  if (!vFirebase_t.isAuthenticated)
  {
    System_Log("Firebase upload failed due to no authentication.");
    return false;
  }

  if (!Firebase.RTDB.setJSON(&vFirebase_t.fbdo, databasePath.c_str(), &jsonObject))
  {
    Serial.println(vFirebase_t.fbdo.errorReason());
    return false;
  }
  else
  {
    Serial.println("Uploading successful.");
    return true;
  }
}

/**
 * Register device by uploading device information to firebase realtime database
 */
bool Register_Device()
{
  FirebaseJson jsonObject;

  // Initialise json object with device and sensor meta-data.
  jsonObject.add("deviceuid", vDeviceInfo.uid);
  jsonObject.add("email", vDeviceInfo.email);
  jsonObject.add("name", vDeviceInfo.name);
  jsonObject.add("type", EnergySensor.type);
  jsonObject.add("sensor", EnergySensor.name);
  jsonObject.add("location", EnergySensor.location);
  jsonObject.add("version", "0.02");

  String path = vDeviceInfo.name + "-" + EnergySensor.name;

  if (!Firebase.RTDB.setJSON(&vFirebase_t.fbdo, path.c_str(), &jsonObject))
  {
    Serial.println("Error Message:" + vFirebase_t.fbdo.errorReason());
    return false;
  }

  return true;
}

/**
 * Initialise the firebase service.
 * 1. Check is done for a local firebase unique id to confirm if this is a new or old device.
 * 2. If the device is new, signup is done using the device email and password preconfig and stored in flash memory. 
 *    The returned firebase unique ID is stored in flash memory.
 * 3. If the device is old, signin is done using the device email and password preconfig and stored in flash memory.
 */
void Task_Firebase_Management(void *pvParameters)
{
  System_Log("Firebase Management Task started");

  while (1)
  {

    // Hold task start timestamp.
    long start = millis();

    // Check if we have WiFi else wait 1 second
    while (WiFi.status() != WL_CONNECTED)
    {
      vTaskDelay(TASK_LOCK_PERIOD / portTICK_PERIOD_MS);
    }

    // Check if we are already authenticated else wait 10 seconds
    while (vFirebase_t.isAuthenticated)
    {
      vTaskDelay(10000 / portTICK_PERIOD_MS);
    }

    System_Log("Configuring Firebase...");

    // Check if api keys and db url are stored in flash aka device has been configured/
    if (vDeviceInfo.api_key == "" || vDeviceInfo.db_url == "")
    {
      System_Log("No values saved for api key and db url");
    }
    else
    {
      // Add firebase API Key from device flash to config object
      vFirebase_t.config.api_key = vDeviceInfo.api_key.c_str();

      // Add firebase database url from device flash to config object
      vFirebase_t.config.database_url = vDeviceInfo.db_url.c_str();

      // Enable WiFi reconnection
      Firebase.reconnectWiFi(true);

      // Check if this device has signed into firebase already. UID will be empty for new devices.
      if (vDeviceInfo.uid == "")
      {
        System_Log("Signing up new user...");

        // Sign up new device using preconfigured email and password stored in flash memory
        if (Firebase.signUp(&vFirebase_t.config, &vFirebase_t.auth, vDeviceInfo.email.c_str(), vDeviceInfo.firebase_password.c_str()))
        {
          System_Log("Sign up Successful");
          // Get the firebase user ID and store it in permanent flash memory.
          vDeviceInfo.uid = vFirebase_t.auth.token.uid.c_str();
          vPreferences.begin("firebase", false);
          vPreferences.putString("uid", vDeviceInfo.uid);
          vPreferences.end();

          // Update device authentication status
          vFirebase_t.isAuthenticated = true;
        }
        else
        {
          vState.clear(STATE_FIREBASE_READY);
          String eventMessage = "Failed - " + String(vFirebase_t.config.signer.signupError.message.c_str());
          System_Log(eventMessage);
          vFirebase_t.isAuthenticated = false;
        }
      }
      else
      {
        System_Log("Signing in existing user...");

        // Configure user credentials using email and passwords stored in flash memory.
        vFirebase_t.auth.user.email = vDeviceInfo.email.c_str();
        vFirebase_t.auth.user.password = vDeviceInfo.firebase_password.c_str();

        // Update device authentication status
        vFirebase_t.isAuthenticated = true;
      }

      // Assign the callback function for the long running token generation task, see addons/TokenHelper.h
      vFirebase_t.config.token_status_callback = tokenStatusCallback;

      // Initialise firebase service
      Firebase.begin(&vFirebase_t.config, &vFirebase_t.auth);

      //Set the size of HTTP response buffers in the case where we want to work with large data.
      vFirebase_t.fbdo.setResponseSize(1024);

      // Wait [FIREBASE_TOKEN_READY_PERIOD] for token to be ready and then Register device in database
      vTaskDelay(FIREBASE_TOKEN_READY_PERIOD / portTICK_PERIOD_MS);

      // Register_Device();
      if (Register_Device())
      {
        System_Log("Device registration completed.");
      }
      else
      {
        System_Log("Device registration failed.");
      };

      // Log system event
      String eventMessage = "Connected to Firebase with UID - " + vDeviceInfo.uid;
      System_Log(eventMessage);

      vState.set(STATE_FIREBASE_READY);

      if (TASK_MONITORING)
      {
        // Compute task execution duration in ms
        long TaskDuration = millis() - start;
        Serial.printf("\nTask_Firebase_Management Execution Duration: l%d ms\n", TaskDuration);
      }
    }
  }
}

#endif