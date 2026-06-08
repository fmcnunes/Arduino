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

#define INFLUXDB_URL "http://192.168.1.50:8086"
#define INFLUXDB_TOKEN "QUpNjMh2zKytPqAD438FZWTnHDfBN58cI71Nn-xZEu_qtdlyzxOJ86A_OW9kHTKONwJb3Fty1qQsWFxnB5GPrQ=="
#define INFLUXDB_ORG "2b78719b79056fd7"
#define INFLUXDB_BUCKET "TermoSifao"

// Time zone info
#define TZ_INFO "UTC0"

// Declare InfluxDB client instance with preconfigured InfluxCloud certificate
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

Point sensor("temperatura");


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

void writeTemperatura(int sensorId, float temperatura)
{
  sensor.clearFields();
  sensor.clearTags();

  sensor.addTag("sensor", String(sensorId).c_str());  // tag dinâmica
  sensor.addField("temperatura", temperatura);        // field float

  if (!client.writePoint(sensor)) {
    log(F("Influx write failed: %s"), client.getLastErrorMessage());
  }
}

