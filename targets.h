
/***********************************
 * write to an influx database
 */
void write2influx(void){
        InfluxData measurement(INFLUXDB_SERIES);
        int numvals = 0; //-- just a counter
        //-- iterate through the values read
        for (int obis; obis < numOfValues; ++obis){
          String key = smlkeys[obis].name;
          signed long long  val = smartvalues[obis];
          if ( val != NULL ) {
            measurement.addValue( key, val );
            numvals++;
          }
        }
        measurement.addValue( "bootCount", bootCount );
        numvals++;
        
        // write it into db - if there's data
        if (numvals>0) {
          influx.write(measurement);
        } else {
          Serial.println("no data points to store");
        }
}


/**********************************************
 *    write to volkszaehler API
 */
void write2vz(void){
  HTTPClient http;
  Serial.println("writing to vz");
  
  //-- iterate through vz config array, get obis keys
  for(int i=0; i<numOfValues; i++){
    String obis2do = my_vz_uuids[i].key;

    //-- get corresponding value from sml result
    for (int obis = 0; obis<numOfValues; obis++){
      String key = smlkeys[obis].name;
      
      //-- if found, look for value
      if (obis2do == key ){
        String uuid = my_vz_uuids[i].uuid;
        signed long  val = smartvalues[obis];
        if ( val != NULL ) {
          //-- prepare url to call, like http://localhost/data/(uudid).json?value=1234.56
          String url = vz_base_url;
          url += uuid;
          url += ".json?value=";
          url += val;
          Serial.println(url);

          //-- POST
          http.begin(url);
          http.addHeader("Content-Type", "text/plain");
          int httpResponseCode = http.POST("POSTing from ESP32");

          if (httpResponseCode > 0) {    // Check for the returning code
            String payload = http.getString();
            Serial.println(httpResponseCode);
            Serial.println(payload);
          } else {
            Serial.println("Error on HTTP request");
          }
          http.end();  //Free the resources
        }
      }
    }
  }
}

/*************************************************************
 * write to custom nginx, just to have debug info in its log
 */

void write2nginx(void){
  HTTPClient http;

  //-- build url
  String url = nginx_base_url;
  url += (long) smartvalues[0]; url += "/";
  url += (long) smartvalues[1]; url += "/";
  url += (long) smartvalues[2]; url += "/";
  url += smldata;
  Serial.println(url);

  http.begin(url);  //-- Prepare http call
  int httpResponseCode = http.GET();
  http.end();       //-- Free the resources
}
