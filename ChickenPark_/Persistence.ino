
/*********************************************************************
 * 
 * 
 * EEProm read write data
 * 
 * 
 *********************************************************************/
  void TestEeprom()
 {
   char buffer[200];
   
   //EEPROM.begin(FLASH_MEM_ALOC_SIZE);
   delay(2000); // Some delay
   
   Serial.print(getDateTime());
   Serial.println("Testing eeprom.......................");
   sprintf(buffer,"Write CheckByte = %d", EEPRomVersison);
   Serial.println(buffer);
   EEPROM.write(0, EEPRomVersison);
   EEPROM.commit();
   //EEPROM.end();
 }
 
 void InitEeprom()
 {
   char buffer[200];
   
   //EEPROM.begin(FLASH_MEM_ALOC_SIZE);
   
   Serial.print(getDateTime());
   Serial.print(F("Init eeprom, size = "));
   Serial.println(FLASH_MEM_ALOC_SIZE);
   EEPROM.write(0, EEPRomVersison);

   for(int schedId=0; schedId < maxSchedules; schedId++)
   {
    schedule[schedId].enabled = false;
    strcpy(schedule[schedId].cronString, "");
    writeSchedulerToEeprom(schedId);
   }

   for(int relayId=0; relayId < nRelays; relayId++)
   {
    relayStatus[relayId].enabled = true;
    relayStatus[relayId].maxDuration = 82800;
    sprintf(relayStatus[relayId].relayName, "Relay%d", relayId+1);
    writeRelayToEeprom(relayId);
   }

   EEPROM.commit();
   Serial.print(F("eeprom data commited."));
   //EEPROM.end();
 }

 void readAllFromEeprom()
 {
   EEPROM.begin(FLASH_MEM_ALOC_SIZE);
   
   for(int schedId=0; schedId < maxSchedules; schedId++)
   {
      readSchedulerFromEeprom(schedId);
   }
   
   for(int relayId=0; relayId < nRelays; relayId++)
   {
      readRelayFromEeprom(relayId);
   }

   //EEPROM.commit();
   //EEPROM.end();
 }
 
 void writeAllToEeprom()
 {
   //EEPROM.begin(FLASH_MEM_ALOC_SIZE);
   
   for(int schedId=0; schedId<maxSchedules;schedId++)
   {
    writeSchedulerToEeprom(schedId);
   }

   for(int relayId=0; relayId < nRelays;relayId++)
   {
    writeRelayToEeprom(relayId);
   }

   EEPROM.commit();
   //EEPROM.end();
 }

 /*********************************************************************
 * 
 * 
 * EEProm Relay functions
 * 
 * 
 *********************************************************************/

 void readRelayFromEeprom(int relayId)
 {
   int baseAddress = 1 + maxSchedules * sizeof(EEPromSchedule);
   
   byte checkByte = EEPROM.read(0);
   
   Serial.print("EEPROM checkbyte is: ");
   Serial.println(checkByte);
   Serial.print("EEPROM version is: ");
   Serial.println(EEPRomVersison);

   if (checkByte != EEPRomVersison)
   {
    Serial.print(getDateTime());
    Serial.print("\tError: the eeprom is not initialized.\n");
    Serial.print(getDateTime());
    Serial.print("\tInitializing EEProm...\n");
    InitEeprom();
   }

   EEPROM_readAnything(baseAddress + relayId * sizeof(EEPromRelay), eepromRelay);
   
   relayStatus[relayId].enabled = eepromRelay.enabled;
   relayStatus[relayId].maxDuration = eepromRelay.maxDuration;
   strcpy(relayStatus[relayId].relayName, eepromRelay.relayName);
  
   relayStatus[relayId].duration = 0;
   relayStatus[relayId].lastDuration = 0;
   strcpy(relayStatus[relayId].lastOn, "");
  }

 void writeRelayToEeprom(int relayId)
  {
    
   //EEPROM.begin(FLASH_MEM_ALOC_SIZE);

   int baseAddress = 1 + maxSchedules * sizeof(EEPromSchedule);

   eepromRelay.enabled = relayStatus[relayId].enabled;
   eepromRelay.maxDuration = relayStatus[relayId].maxDuration;  
   strcpy(eepromRelay.relayName, relayStatus[relayId].relayName);

   //relayStatus[relayId].relayName.toCharArray(eepromRelay.relayName, 32);
   
   EEPROM_writeAnything(baseAddress + relayId * sizeof(EEPromRelay), eepromRelay);

   Serial.print(getDateTime());
   Serial.print("\tWrite: Relay " );
   Serial.print(relayId);
   Serial.print(" base addr is " );
   Serial.print(baseAddress);
   Serial.println(relayId * sizeof(EEPromRelay));
   
   Serial.print(getDateTime());
   Serial.print("\tWrite: Relay " );
   Serial.print(relayId);
   Serial.print(" name is '" );
   Serial.print(eepromRelay.relayName );
   Serial.println("'");
   
   Serial.print(getDateTime());
   Serial.print("\tStore relay ");
   Serial.println(relayId);

   EEPROM.commit();
   //EEPROM.end();
  }

 /*********************************************************************
 * 
 * 
 * EEProm Scheduler functions
 * 
 * 
 *********************************************************************/

 void readSchedulerFromEeprom(int schedId)
 {
   int baseAddress = 1;
   byte checkByte = EEPROM.read(0);

/*
Serial.print("EEPROM checkbyte is: ");
Serial.println(checkByte);
Serial.print("EEPROM version is: ");
Serial.println(EEPRomVersison);
*/
   if (checkByte != EEPRomVersison)
   {
    Serial.print(getDateTime());
    Serial.println("\tError: eeprom not initialized.");
    Serial.print(getDateTime());
    Serial.println("\tInitializing EEProm...");
    InitEeprom();
   }

   EEPROM_readAnything(baseAddress + schedId * sizeof(EEPromSchedule), eepromSchedule);
   
   schedule[schedId].enabled = eepromSchedule.enabled;
   strcpy(schedule[schedId].cronString, eepromSchedule.cronString);
   schedule[schedId].lastCheck[0] = 0;
   schedule[schedId].lastRun[0] = 0;
  }

 void writeSchedulerToEeprom(int schedId)
  {
    int baseAddress = 1;

    //EEPROM.begin(FLASH_MEM_ALOC_SIZE);

    eepromSchedule.enabled = schedule[schedId].enabled;
    strcpy(eepromSchedule.cronString, schedule[schedId].cronString);
    //schedule[schedId].cronString.toCharArray(eepromSchedule.cronString,128);
    
    EEPROM_writeAnything(baseAddress + schedId * sizeof(EEPromSchedule), eepromSchedule);  

    EEPROM.commit();
    //EEPROM.end();
  }
 
/*********************************************************************
 * 
 * 
 * EEProm function
 * 
 * 
 *********************************************************************/
 
template <class T> int EEPROM_writeAnything(int ee, const T& value)
{
    const byte* p = (const byte*)(const void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
          EEPROM.write(ee++, *p++);
    return i;
}

template <class T> int EEPROM_readAnything(int ee, T& value)
{
    byte* p = (byte*)(void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
          *p++ = EEPROM.read(ee++);
    return i;
}
