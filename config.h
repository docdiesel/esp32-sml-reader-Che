
//-- general section -----------------------------
#define numOfValues      3       // I'm reading 1.8.0, 2.8.0 and 16.7.0 
#define SENSORPOWERPIN   4       // number of GPIO port for powering sensor

#define TIME_TO_SLEEP       57   // Time ESP32 will go to sleep (in seconds)
#define uS_TO_S_FACTOR 1000000   // Conversion factor for micro seconds to seconds 

//-- wifi section --------------------------------
const char* wifi_ssid = "NameOfYourWifi";
const char* wifi_pass = "**************";

//-- serial section ------------------------------
#define BAUD1 115200  // for USB debugging
#define BAUD2 9600    // for the smartmeter line

//-- influxdb section -------------------------
#define WRITE2INFLUX true
#define INFLUXDB_HOST      "my.influx.com"   // hostname or ip address
#define INFLUXDB_PORT      8086              // port number
#define INFLUXDB_DATABASE  "somedb"          // database
#define INFLUXDB_SERIES    "smartmeterdata"  // series ('table')
#define INFLUXDB_USER      "dbuser"          // user
#define INFLUXDB_PASS      "dbP4$$"          // password

//-- volkszaehler.org section -------------------------
// step 1/2: define vz base url
// step 2/2: configure uuids from vz 

#define WRITE2VZ     true
#define vz_base_url "http://vz.server.tld/middleware.php/data/";

// don't touch struct; write uuids to array below
typedef struct { char key[10]; char uuid[40]; } VZ_UUID;

// don't forget to terminate each string with zero byte!
const VZ_UUID my_vz_uuids[numOfValues] = {
  {"1.8.0\0",  "d000xxxx-6eae-11e9-aa97-xxxxxxxx32a6\0"},
  {"2.8.0\0",  "e75cxxxx-6eae-11e9-94d0-xxxxxxxx3683\0"},
  {"16.7.0\0", "acdcxxxx-6eae-11e9-af6e-xxxxxxxx45ab\0"}
};

//-- nginx debug section -----------------------------
#define WRITE2NGINX    true
#define nginx_base_url "http://www.myserver.tld/smlvals/"

