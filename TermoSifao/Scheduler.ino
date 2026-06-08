//schedTimeMatch(i, cmd);


bool schedTimeMatch(int schedId, char* cmd)
{
  char strMinute[50];
  char strHour[50];
  char strWeekDay[50];
  char strDay[50];
  char strMonth[50];
  int repeatFrequency;

  if (! schedule[schedId].enabled ||
         strcmp(schedule[schedId].lastCheck,getDateTimeShort()) == 0)
      return false;

   strcpy(schedule[schedId].lastCheck,getDateTimeShort());
   
   RtcDateTime now = getLocalDateTime();

   bool rc = false;
  
  if (sscanf(schedule[schedId].cronString,"%[0-9,*] %[0-9,*] %[0-9,*] %[0-9,*] %[0-9,*] %d %[^\n]s",strMinute,strHour,strWeekDay,strDay,strMonth,&repeatFrequency,cmd) == 7)
  { 
   //Serial.printf("%s\tCron testing command to execute: %s\n", getDateTime(), cmd);
   rc = cronCheck(strMinute, now.Minute());
   rc = rc && cronCheck(strHour, now.Hour());

   if (repeatFrequency < 2)
   {
    rc = rc && cronCheck(strWeekDay, now.DayOfWeek() + 1);
    rc = rc && cronCheck(strDay, now.Day());
    rc = rc && cronCheck(strMonth, now.Month());
   }
   else
   { 
     rc && repeatCheck(repeatFrequency, now.Day());
   }
  }
  
  return rc;
}

bool cronCheck(char* cronStr, int timeVal)
{
  int i;
  char buffer[256];
  bool rc = false;

  
  strcpy(buffer, cronStr);
  char* token = strtok(buffer, ",");
  while (token)
  {
    if (strcmp(token,"*") == 0) return true;
 
    if (sscanf(token, "%d", &i) == 1)
      {
        if (i == timeVal) return true;
      }

    //Serial.printf("token: %s\n", token);
    token = strtok(NULL, ",");
  }
  return false;
}

bool repeatCheck(int repeatFrequency, int dayOfMonth)
{
   return ( (dayOfMonth % repeatFrequency) == 0);  
}
