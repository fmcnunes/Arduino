#if defined(ESP32)
#include <WiFiMulti.h>
WiFiMulti wifiMulti;
#define DEVICE "ESP32"
#elif defined(ESP8266)
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti wifiMulti;
#define DEVICE "ESP8266"
#endif

#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

#define INFLUXDB_URL "https://europe-west1-1.gcp.cloud2.influxdata.com"
#define INFLUXDB_TOKEN "bQhhYbvhTmCopKiaYem0mEvQBB2ZwkSdRpBvL2uCL9O6Ub4Ygi_6kMldkB1_0aKEdblNv0N2eTBjSpzsnMk7Yw=="
#define INFLUXDB_ORG "36a23540c9faa123"
#define INFLUXDB_BUCKET "ChickenPark"

// Time zone info
#define TZ_INFO "UTC0"

// Declare InfluxDB client instance with preconfigured InfluxCloud certificate
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

Point sensor("ambiente");


void setupInfluxDB()
{
  /* Setup influxdb   */
  // Check server connection
  if (client.validateConnection())
    {
      log(F("Connected to InfluxDB: %s"), "url");
      Serial.println(client.getServerUrl());
    } 
    else
    {
      log(F("InfluxDB connection failed: %s"), client.getLastErrorMessage());
    }
}
