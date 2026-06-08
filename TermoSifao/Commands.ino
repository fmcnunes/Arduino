
void processCommand(char *cmdBuffer)
{
  int relay=-1;
  int schedId=-1;
  int duration=-1;
  char op[200];
  char buffer[200];
  int y;
  int mm;
  int d;
  int h;
  int m;
  int s;
  char strMinute[50];
  char strHour[50];
  char strWeekDay[50];
  char strDay[50];
  char strMonth[50];
  int freq;

  sprintf(buffer,"%s\tProcessing command '%s'", getDateTime(), cmdBuffer);
  Serial.println(buffer);
  mqttSendLog(buffer);
 
  if (! isValidCmd(cmdBuffer))
  {
    sprintf(buffer,"%s\tCommand '%s' is invalid.", getDateTime(), cmdBuffer);
    mqttSendLog(buffer);
    Serial.println(buffer);
    return;
  }

  if (sscanf(cmdBuffer,"sched,%d,change,\"%[^\"]s",&schedId,op) == 2)
  { 
    strcpy(schedule[schedId-1].cronString,op);
    writeSchedulerToEeprom(schedId-1);
    return;
  }
  
  if (sscanf(cmdBuffer,"datetime,%d-%d-%d,%d:%d:%d",&y,&mm,&d,&h,&m,&s) == 6)
  {
    setDateTime(y,mm,d,h,m,s);
    return;
  }
  
  if (sscanf(cmdBuffer,"sched,%d,%[^\n]s",&schedId,op) == 2)
  {
    
    if (schedId < 1 || schedId > maxSchedules) return;
    if (strcmp(op,"enable") == 0)
    {
      schedule[schedId-1].enabled = true;
      writeSchedulerToEeprom(schedId-1);
    }
    if (strcmp(op,"disable") == 0)
    {
      schedule[schedId-1].enabled = false;
      writeSchedulerToEeprom(schedId-1);
    }
    return;
  }
  
  if (sscanf(cmdBuffer,"relay,%d,name,%[^\n]s",&relay,buffer) == 2)
  {
    RelaySetName(relay, buffer);
    return ;
  }
  
 if (sscanf(cmdBuffer,"relay,%d,on,%d",&relay,&duration) == 2)
  {
    RelayCommand(relay, 1, duration);
    return;
  }

 if (sscanf(cmdBuffer,"relay,%d,off",&relay) == 1)
  {
    RelayCommand(relay, 0, 0);
    return;
  }

  
  if (strcmp(cmdBuffer,"reload") == 0)
   {
    Reload();
    return;
   }

  if (strcmp(cmdBuffer,"ntpsync") == 0)
   {
    setClockFromNtp();
    return;
   }

  if (strcmp(cmdBuffer,"help") == 0 || strcmp(cmdBuffer,"h") == 0)
   {
    help();
    return;
   }

  if (strcmp(cmdBuffer,"status") == 0 || strcmp(cmdBuffer,"s") == 0)
  {
    //TestEeprom();
    MqttStatus();
    ShowStatus();
    return;
  }
}

void Reload()
{
  char buffer[200];
  sprintf(buffer,"%s\tReloading.", getDateTime());
  mqttSendLog(buffer);
  Serial.println(buffer);
  esp_restart();
}

/***************************************
 *
 * Help
 *
 **************************************/
 void help()
 {
  String buff;


    Serial.print(String(F("---------------------------------- Help ----------------------------------\n")));
    buff = getDateTime() +  String(F("\tCMD> Status                                   --- Device status\n"));
    Serial.print(buff);
    buff = getDateTime() +  String(F("\tCMD> ntpsync                                   --- Sync time from ntp internet server\n"));
    Serial.print(buff);
    buff = getDateTime() +  String(F("\tCMD> relay,1,on,4                             --- Activate relay 1 for 4 seconds\n"));
    Serial.print(buff);
    buff = getDateTime() +  String(F("\tCMD> relay,1,off                              --- Deactivate relay 1\n"));
    Serial.print(buff);
    buff = getDateTime() +  String(F("\tCMD> reload                                   --- Reload board\n"));
    Serial.print(buff);
    buff = getDateTime() +  String(F("\tCMD> relay,1,name,<name>                      --- Name for relay 1 set to <name>\n"));
    Serial.print(buff);
    buff = getDateTime() +  String(F("\tCMD> datetime,yyyy-mm-dd,hh:mm:ss             --- Change RTC date and time\n"));
    Serial.print(buff);
    buff = getDateTime() +  String(F("\tCMD> sched,1,disable                          --- Disable sched 1\n"));
    Serial.print(buff);
    buff = getDateTime() +  String(F("\tCMD> sched,1,enable                           --- Enable sched 1\n"));
    Serial.print(buff);
    buff = getDateTime() +  String(F("\tCMD> sched,1,change,\"* * * * * 1 relay,1,on,30\" --- Change sched 1 <minute 00..60> <hour 0..23> <weekday 0..6> <day 1..31> <month 1..11> <repeat 1..31> <cmd>\n"));
    Serial.print(buff);
    buff = getDateTime() +  String(F("\tCMD> sched,1,change,\"* * * * * 1 reset\" --- Change sched 1\n"));
    Serial.print(buff);
    Serial.print("--------------------------------------------------------------------------\n");
 }
 
/***************************************
 *
 * Display Relay status
 *
 **************************************/
 void ShowStatus()
 {
  char buffer[512];

  Serial.println(F("\n---------------------------------- Status ----------------------------------"));
  
  Serial.println(F("\n"));
  
  for(int i=0;i< nRelays; i++)
  {
    if (digitalRead(Relays[i])==HIGH)
    {
      sprintf(buffer,"%s\tStatus\t'%s'\tOFF", getDateTime(), relayStatus[i].relayName);
    }
    else
    {
      sprintf(buffer,"%s\tStatus\t'%s'\tON %d sec remaining.", getDateTime(), relayStatus[i].relayName, (int)((relayStatus[i].duration - millis())/1000.0));
    }
    Serial.println(buffer);
  }

  Serial.println(F("\n"));
  
  for(int i=0;i< maxSchedules; i++)
  {
    if (schedule[i].enabled)
     sprintf(buffer,"%s\tScheduler\t%d\tEnabled\t%s", getDateTime(), i+1, schedule[i].cronString);
    else
     sprintf(buffer,"%s\tScheduler\t%d\tDisabled\t%s", getDateTime(), i+1, schedule[i].cronString);
     
    Serial.println(buffer);
  }

  Serial.println(F("\n"));

  if (readInputRelay())
     sprintf(buffer,"%s\tInput relay\tOFF", getDateTime());
    else
     sprintf(buffer,"%s\tInput relay\tON", getDateTime());

  Serial.println(buffer);

    Serial.println(F("\n"));

  if (internetConnectionState)
     sprintf(buffer,"%s\tInternet state\tCONNECTED", getDateTime());
    else
     sprintf(buffer,"%s\tInternet state\tDISCONNECTED", getDateTime());

  Serial.println(buffer);

  if (mqttClient.connected())
     sprintf(buffer,"%s\tMqtt\tCONNECTED", getDateTime());
    else
     sprintf(buffer,"%s\tMqtt\tDISCONNECTED", getDateTime());
  
  Serial.println(buffer);
  
  Serial.print(String(F("\n----------------------------------------------------------------------------\n\n")));
 }
