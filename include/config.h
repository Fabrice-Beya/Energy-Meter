#ifndef CONFIG
#define CONFIG

/**
 * Set this to false to disable Serial logging
 */
#define DEBUG true

/**
 * Set this to true to monitor Task Execution duration
 */
#define TASK_MONITORING false

/**
 * The GPIO ADC pin were the CT sensor is connected.
 */
#define ADC_INPUT 34

/**
 * The voltage in our apartment. Usually this is 230V in South Africa.
 */
#define HOME_VOLTAGE 230.0

/**
 * Timeout for the WiFi connection. When this is reached,
 * the ESP goes into deep sleep for 30seconds to try and
 * recover.
 */
#define WIFI_TIMEOUT 20000 // 20 seconds

/**
 * How long should we wait after a failed WiFi connection
 * before trying to set one up again.
 */
#define WIFI_RECOVER_TIME_MS 20000 // 20 seconds

/**
 * Force Emonlib to assume a 3.3V supply to the CT sensor
 */
#define emonTxV3 1

/**
 * Local measurements
 */
#define LOCAL_MEASUREMENTS 30

/**
 * Syncing time with an NTP server
 */
#define NTP_TIME_SYNC_ENABLED true
#define NTP_SERVER "pool.ntp.org"
#define NTP_OFFSET_SECONDS 7200
#define NTP_UPDATE_INTERVAL_MS 5000

/**
 * GPIO pin for SD card chip select line.
 */
#define SD_CS 5


/**
 * File paths for Energy and System logs.
 */
#define ENERGY_FILE "/energy.txt"
#define SYSTETM_FILE "/system.txt"


/**
 * ADC configuration
 */
#define ADC_BITS 10
#define ADC_COUNTS (1 << ADC_BITS)

/**
 * Measure and server synchronisation intervals
 * Defaults: Energy measurement are taken every 5 seconds.
 *          Energy logs are upload to the cloud every 20 seconds.
 *          System logs are upload to the cloud every 20 seconds.
 */
#define ENERGY_MEASUREMENT_INTERVAL 5000
#define ENERGY_LOGS_SYNC_INTERVAL 60000
#define SYSTEM_LOGS_SYNC_INTERVAL 3600000

/**
 * System initialisation time. This is the time taken for all sub-systems to initialise correct.
 *
 */
#define SYSTEM_INIT_PERIOD 15000

/**
 * Firebase token generation time. This is the time needed for firebase token to be ready.
 *
 */
#define FIREBASE_TOKEN_READY_PERIOD 5000

/**
 * Task wait period. This is the time delay used when a task is prevented from executing due to prerequisit condition.
 *
 */
#define TASK_LOCK_PERIOD 1000

#endif