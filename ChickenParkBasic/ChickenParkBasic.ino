// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, modified for integer use, public domain

#include "DHT.h"


#include <WiFiMulti.h>
WiFiMulti wifiMulti;
  
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>
  
// WiFi AP SSID
#define WIFI_SSID "Quintinha_Dos_Lirios_Sala"
// WiFi password
#define WIFI_PASSWORD "!fsnunes2020!"
  
#define INFLUXDB_URL "https://europe-west1-1.gcp.cloud2.influxdata.com"
#define INFLUXDB_TOKEN "bQhhYbvhTmCopKiaYem0mEvQBB2ZwkSdRpBvL2uCL9O6Ub4Ygi_6kMldkB1_0aKEdblNv0N2eTBjSpzsnMk7Yw=="
#define INFLUXDB_ORG "36a23540c9faa123"
#define INFLUXDB_BUCKET "ChickenPark"
  
// Time zone info
#define TZ_INFO "UTC0"

// Declare InfluxDB client instance with preconfigured InfluxCloud certificate
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

Point sensor("ambiente");

#define RELAY 19
#define DHTPIN 18

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11 
//#define DHTTYPE DHT22   // DHT 22  (AM2302)
#define DHTTYPE DHT21   // DHT 21 (AM2301)


// Connect pin 1 (on the left) of the sensor to +5V
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) 
//    of the sensor

DHT dht(DHTPIN, DHTTYPE);

int flag = true;

void setup() {
  Serial.begin(115200); // Output status on Uno serial monitor

  pinMode(RELAY, OUTPUT);

  Serial.println("DHTxx test!");
 
  dht.begin();

      // Setup wifi
    WiFi.mode(WIFI_STA);
    wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);
  
    Serial.print("Connecting to wifi");
    while (wifiMulti.run() != WL_CONNECTED)
    {
      Serial.print(".");
      delay(100);
    }
    Serial.println("");
    Serial.println("Connected...");

    timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");

        // Check server connection
    if (client.validateConnection())
    {
      Serial.print("Connected to InfluxDB: ");
      Serial.println(client.getServerUrl());
    } 
    else
    {
      Serial.print("InfluxDB connection failed: ");
      Serial.println(client.getLastErrorMessage());
    }

}

void loop() {





  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  int8_t h = dht.readHumidity();
  int16_t t = dht.readTemperature(0);

  // check if returns are valid then something went wrong!
  if (isnan(h) || isnan(t))
   {       
    Serial.println("Failed to read from DHT");
   } 
   else
   {

     if (hour() > 7 || hour() < 22)
     {
       digitalWrite(RELAY, LOW); // Liga a Luz
     }
     else
     {
      digitalWrite(RELAY, HIGH); // Desliga a Luz
     }

    sensor.clearFields();

    // Store measured value into point
    // Report RSSI of currently connected network
    sensor.addField("temperatura", t);
    sensor.addField("humidade", h);
  
    // Print what are we exactly writing
    Serial.print("Writing: ");
    Serial.println(sensor.toLineProtocol());
  
    // Check WiFi connection and reconnect if needed
    if (wifiMulti.run() != WL_CONNECTED) {
      Serial.println("Wifi connection lost");
    }
  
    // Write point
    if (!client.writePoint(sensor)) {
      Serial.print("InfluxDB write failed: ");
      Serial.println(client.getLastErrorMessage());
    }

  
    Serial.print("Humidity: "); 
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: "); 
    Serial.print(t);
    Serial.println(" *C");
   }
  delay(15000);
}