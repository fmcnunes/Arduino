
void INA219_loop(void)
{
  float shuntvoltage = 0;
  float busvoltage = 0;
  float current_mA = 0;
  float loadvoltage = 0;
  float power_mW = 0;
 
  shuntvoltage = ina219.getShuntVoltage_mV();
  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  power_mW = ina219.getPower_mW();
  loadvoltage = busvoltage + (shuntvoltage / 1000);
 
  sprintf(buffer, PSTR("Bus Voltage: %f V"), busvoltage);
  log(buffer);
  sprintf(buffer, PSTR("Shunt Voltage: %f  mV"), shuntvoltage);
  log(buffer);
  sprintf(buffer, PSTR("Load Voltage: %f  V"), loadvoltage);
  log(buffer);
  sprintf(buffer, PSTR("Current: %f  mA"), current_mA);
  log(buffer);
  sprintf(buffer, PSTR("Power: %f mW"), power_mW);
  log(buffer);

  log(F(""));
}


