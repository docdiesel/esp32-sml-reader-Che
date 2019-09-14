/* 
 * ==  esp32-sml-reader-Che v0.8 ==
 * - (c)2019 Bernd Künnen; code under MIT license ==
 * - use an ESP32 to read smartmeter data from IR interface
 * - reads data via hardware serial
 * - uses IR photo transistor based sensor as input 
 * - may write data to different backends (currently: influxdb, volkszaehler, plain http server log)
 * - uses deep sleep to save power
 * + define passwords etc. in config.h
 * + early version; use at own risk
 * + improvement welcome
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include "HardwareSerial.h"
#include <InfluxDb.h>

//-- load configuration for wifi, serial etc.
#include "config.h"

//== global objects section ==
HardwareSerial   SerialX(2);
Influxdb         influx(INFLUXDB_HOST, INFLUXDB_PORT);

RTC_DATA_ATTR int bootCount = 0;

char*     smldata;          // pointer to the sml data read
long long smartvalues[3];   // buffer for the extracted values

// raw read buffer definition
#define bufsz 2000
char    myBuffer[bufsz];
int     bufix = 0;
String  hx = "";

//== load helper tools and functions ==
#include "smltools.h"
#include "targets.h"


void setup() {
  
  /**** serial part ****/
  Serial.begin(BAUD1);
  SerialX.begin(BAUD2); // default is 8N1, SERIAL_8N1, RX=GPIO13, TX=GPIO15
  Serial.println("Serials initialized.");  

  /**** wifi part ****/
  Serial.println("Connecting to wifi ... ");
  WiFi.mode(WIFI_STA);
  // WiFi.hostname(ESPHOSTNM);
  WiFi.begin(wifi_ssid, wifi_pass);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(10000);
    ESP.restart();
  }
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  /**** influxdb part ****/
  if (WRITE2INFLUX) {
    influx.setDbAuth(INFLUXDB_DATABASE, INFLUXDB_USER, INFLUXDB_PASS); // with authentication
  }

  /**** deep sleep part; see: https://randomnerdtutorials.com/esp32-timer-wake-up-deep-sleep/ ****/
  //-- Increment boot number and print it every reboot
  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for " + String(TIME_TO_SLEEP) + " Seconds");

  // set GPIOx as (power) output, means switch on sensor
  pinMode(SENSORPOWERPIN,  OUTPUT);
  digitalWrite(SENSORPOWERPIN, HIGH);

  // clear buffer
  resetReadBuffer();
}
 
void loop() {
   
  if (SerialX.available()) {

    readIntoBuffer();
    smldata = contains_valid_sml(myBuffer);
    if (smldata){
      Serial.println("found sml data");
      Serial.println( smldata );
      for (int k=0; k<numOfValues; k++) {
        smartvalues[k] = extractDataFrom( smldata, k) ;
        Serial.println( (long) smartvalues[k] );
      }
      if (WRITE2INFLUX) { write2influx(); }
      if (WRITE2VZ)     { write2vz();     }
      if (WRITE2NGINX)  { write2nginx();  }
      Serial.println("going to deep sleep ...");
      Serial.flush();
      esp_deep_sleep_start();
    }
    else
    {
      // Serial.println("no sml found in " + String(myBuffer) );
    }
  }
}
