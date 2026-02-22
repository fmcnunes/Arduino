File loggerFile;

void SdCardSetup()
{
  //log(F("Setting relay %d lastDuration to %d"), relayId, relayStatus[relayId].lastDuration);
  
  log(F("SdCard initialization..."));

  //Pin ligado ao CS do ScCard
  if (!SD.begin(5)) {
    log(F("SdCard initialization failed!"));
  }
  else  {
    WriteHeader();
    log(F("SdCard initialization done."));
      uint8_t cardType = SD.cardType();

      if(cardType == CARD_NONE){
         log(F("No SD card attached"));
        }
      else
       {
         if(cardType == CARD_MMC){
           log(F("SD Card Type: MMC"));
         } else if(cardType == CARD_SD){
           log(F("SD Card Type: SDSC"));
          } else if(cardType == CARD_SDHC){
           log(F("SD Card Type: SDHC"));
          } else {
           log(F("SD Card Type: UNKNOWN"));
          }
         
         uint64_t cardSize = SD.cardSize() / (1024 * 1024);
         log(F("SD Card Size: %l MB\n"), cardSize);
        }
  }
}

void WriteHeader()
{
  loggerFile = SD.open("/logger.txt", FILE_APPEND);
  loggerFile.println("DateTime\tVoltage\tCurrent_ma\tPower_mW\tAge\tSatellites\tHdop\tLat\tLon\tAlt\tCource\tSpeed");
  loggerFile.close();

}

void WriteData()
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
 
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  loggerFile = SD.open("/logger.txt", FILE_APPEND);

  float flat, flon;
  unsigned long age, date, time, chars = 0;

  gps.f_get_position(&flat, &flon, &age);

  // if the file opened okay, write to it:
  if (loggerFile) {
    log(F("Writing to /logger.txt..."));
    loggerFile.print(getDateTime());
    loggerFile.print("\t");
    loggerFile.print(loadvoltage);
    loggerFile.print("\t");
    loggerFile.print(current_mA);
    loggerFile.print("\t");
    loggerFile.print(power_mW);
    loggerFile.print("\t");
    loggerFile.print(age);
    loggerFile.print("\t");
    loggerFile.print(gps.satellites());
    loggerFile.print("\t");
    loggerFile.print(gps.hdop());
    loggerFile.print("\t");
    sprintf(buffer, "%3.6f", flat);
    loggerFile.print(buffer);
    loggerFile.print("\t");
    sprintf(buffer, "%3.6f", flon);
    loggerFile.print(buffer);
    loggerFile.print("\t");
    loggerFile.print(gps.f_altitude());
    loggerFile.print("\t");
    loggerFile.print(gps.f_course());
    loggerFile.print("\t");
    loggerFile.println(gps.f_speed_kmph());
    // close the file:
    loggerFile.close();
    log(F("done."));
  } else {
    // if the file didn't open, print an error:
    log(F("error opening logger.txt"));
  }
}
