 #include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into digital pin 2 on the Arduino
#define ONE_WIRE_BUS 14

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass oneWire reference to DallasTemperature library
DallasTemperature sensors(&oneWire);

int deviceCount = 0;
float tempC;


void setupSensors()
{
    // Start up the library
  sensors.begin();
  //timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");
  log(F("Locating temperature sensores..."));
  
  deviceCount = sensors.getDeviceCount();

  log(F("Found %d sensores."),deviceCount);
}

void loopSensors()
{
  sensors.requestTemperatures();            // pede leitura
  float tempC = sensors.getTempCByIndex(0); // primeiro sensor no barramento

  if (tempC == DEVICE_DISCONNECTED_C) {
    log(F("Sensor não encontrado!"));
  } else {

    // Print what are we exactly writing
    log(F("Temperatura do sensor 1: %f"), tempC);
  
   }
}
