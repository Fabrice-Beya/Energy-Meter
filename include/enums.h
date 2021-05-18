#ifndef ENUMS
#define ENUMS

#include <Firebase_ESP_Client.h>

struct Measurement
{
  float reading;
  int timeStamp;
};

struct DeviceInfo 
{
  String uid;
  String email;
  String firebase_password;
  String api_key;
  String db_url;
  String storage_bucket;
  String name;
  String domain;
  String wifi_ssid;
  String wifi_password;
};

struct Sensor 
{
  String type;
  String name;
  String location;
  String value;
  String timestamp;
};

class State {
public:
  bool check(uint16_t flags) { return (m_state & flags) == flags; }
  void set(uint16_t flags) { m_state |= flags; }
  void clear(uint16_t flags) { m_state &= ~flags; }
  uint16_t m_state = 0;
};

struct TaskHandlers 
{
  TaskHandle_t TaskHandle_Energy;
  TaskHandle_t TaskHandle_Timer;
  TaskHandle_t TaskHandle_WiFi_Management;
  TaskHandle_t TaskHandle_Elapsed_Timer;
  TaskHandle_t TaskHandle_Firebase;
  TaskHandle_t TaskHandle_System_Sync;
  TaskHandle_t TaskHandle_Energy_Sync;
};

struct Mutexs 
{
  // Mutex used to manage access to the system logs file. Used to prevent multiple task appending/reading data simultainously.
  SemaphoreHandle_t xMutex_system_file;

  // Mutex used to manage access to the energy measurements log file. Used to prevent multiple task appending/reading data simultainously.
  SemaphoreHandle_t xMutex_energy_file;

  // Mutex used to manage access to the firebase functionality. Used to prevent multiple task uploading data simultainously.
  SemaphoreHandle_t xMutex_firebase;
};

struct Timers
{
  // Keeps track of current system time in unit/epoch format.
  int timestamp = 0;

  // Keeps track of current system time in string format.
  String formatedTime = "";

  // Keeps track of the intial time the device powered on in ms.
  unsigned long initialTime = 0;

  // Keeps track of the elapse time since the device powered on in ms.
  unsigned long elapseTime = 0;
};

struct Firebase_t
{
  // Instance of firebase realtime database object.
  FirebaseData fbdo;

  // Instance of firebase authentication.
  FirebaseAuth auth;

  // Instance of firebase config.
  FirebaseConfig config;

  // Global firebase authentication state.
  bool isAuthenticated = false;
};

#endif