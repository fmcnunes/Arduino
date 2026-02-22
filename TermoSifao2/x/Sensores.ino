



/****************************************************************
 * DHT22 data
 ****************************************************************/
 



void setupSensors()
{
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
    Serial.print("Temperatura: ");
    Serial.print(tempC);
    Serial.println(" °C");

 
    // Print what are we exactly writing
    log(F("Temperatura do sensor 1: %f"), tempC);
  
   }
}
