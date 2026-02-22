#include "DHT.h"

#define DS1307 1


/****************************************************************
 * DHT22 data
 ****************************************************************/
 

#define RELAY 19
#define DHTPIN 18

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11 
//#define DHTTYPE DHT22   // DHT 22  (AM2302)
#define DHTTYPE DHT21   // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);


void setupSensors()
{
  dht.begin();

  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");
}

void loopSensors()
{
    // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  float t = dht.readTemperature(0);

  // check if returns are valid then something went wrong!
  if (isnan(h) || isnan(t))
   {       
    log(F("Failed to read from DHT"));
   } 
   else
   {

    sensor.clearFields();

    // Store measured value into point
    // Report RSSI of currently connected network
    sensor.addField("temperatura", t);
    sensor.addField("humidade", h);
  
    // Print what are we exactly writing
    log(F("Writing to influxDB: ");
    log(sensor.toLineProtocol());
  
    // Check WiFi connection and reconnect if needed
    if (wifiMulti.run() != WL_CONNECTED) {
      log(F("Wifi connection lost"));
    }
  
    // Write point
    if (!client.writePoint(sensor)) {
      log(F("InfluxDB write failed: "));
      log(client.getLastErrorMessage());
    }

  
    Serial.print("Humidity: "); 
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: "); 
    Serial.print(t);
    Serial.println(" *C");
   }
}
