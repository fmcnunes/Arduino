/*
 * Temperature Monitoring Sketch
 * This sketch interfaces with multiple DS18B20 temperature sensors on a single bus.
 * It uses the OneWire and DallasTemperature libraries to communicate with the sensors,
 * reads the temperature from each sensor, and outputs the temperature readings to the serial monitor.
 * The temperature is displayed in both Celsius and Fahrenheit.
 */





void setup(void)
{
  // Start serial communication
  Serial.begin(115200);
  
  setupSensors();
}

void loop(void)
{ 

  loopSensors();
  // Wait 1 second before next reading
  delay(1000);
}