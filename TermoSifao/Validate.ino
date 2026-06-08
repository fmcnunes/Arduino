


bool isValidCmd(char *cmdBuffer)
{
 int  n;
 int relay=-1;
 int sched=-1;
 int duration=-1;
 char op[200];
  int y;
  int mm;
  int dd;
  int h;
  int m;
  int s;
  char strMinute[50];
  char strHour[50];
  char strWeekDay[50];
  char strDay[50];
  char strMonth[50];
  int freq;
 
  
  if (sscanf(cmdBuffer,"sched,%d,change,\"%[0-9,*] %[0-9,*] %[0-9,*] %[0-9,*] %[0-9,*] %d %[^\"]s",&sched,strMinute,strHour,strWeekDay,strDay,strMonth,&freq,op) == 8)
  { 
    if (!isValidateNumberArray(strMinute,0,59)) return false;
    if (!isValidateNumberArray(strHour,0,23)) return false;
    if (!isValidateNumberArray(strWeekDay,1,7)) return false;
    if (!isValidateNumberArray(strDay,1,31)) return false;
    if (!isValidateNumberArray(strMonth,1,12)) return false;
    if (sched < 1 || sched > maxSchedules) return false;
    if (freq < 1 || freq > 30) return false;
    if (! isValidCmd(op)) return false;
    return true;
  }
  
  if (sscanf(cmdBuffer,"datetime,%d-%d-%d,%d:%d:%d",&y,&mm,&dd,&h,&m,&s) == 6)
  {
    if (y < 2017 || y > 3000) return false;
    if (mm < 1 || mm > 12) return false;
    if (dd < 1 || dd > 31) return false;
    if (h < 0 || h > 23) return false;
    if (m < 0 || m > 59) return false;
    if (s < 0 || s > 59) return false;
    return true;
  }
  
  if (sscanf(cmdBuffer,"sched,%d,%[^\n]s",&sched,op) == 2)
  {
    if (sched < 1 || sched > maxSchedules) return false;
    if (strcmp(op,"enable") != 0 && strcmp(op,"disable") != 0)
      return false;
    return true;
  }

  if (sscanf(cmdBuffer,"relay,%d,name,%[^\n]s",&relay,op) == 2)
  {
    if (relay < 1 || relay > nRelays) return false;
    return true;
  }
 
 if (sscanf(cmdBuffer,"relay,%d,on,%d",&relay,&duration) == 2)
  {
    if (duration < 1) return false;
    if (relay < 1 || relay > nRelays) return false;
    return true;
  }

 if (sscanf(cmdBuffer,"relay,%d,%[of]",&relay, op) == 2)
  {
    if (strcmp(op, "off") != 0) return false;
    if (relay < 1 || relay > nRelays) return false;

    return true;
  }

  if (strcmp(cmdBuffer,"status") == 0 || strcmp(cmdBuffer,"s") == 0)
  {
    return true;
  }

  if (strcmp(cmdBuffer,"h") == 0 || strcmp(cmdBuffer,"help") == 0)
  {
    return true;
  }

  if (strcmp(cmdBuffer,"reload") == 0)
  {
    return true;
  }

  if (strcmp(cmdBuffer,"ntpsync") == 0)
  {
    return true;
  }
  
 return false;
}

bool isValidateNumberArray(char* str, int vMin, int vMax)
{
  int i;
  char buffer[256];
  
  strcpy(buffer, str);
  char* token = strtok(buffer, ",");
  while (token)
  {
    if (strcmp(token,"*") != 0)
    {
      if (sscanf(token, "%d", &i) == 1)
      {
        if (i<vMin || i > vMax) return false;
      }
      else
      {
        return false;
      }
    }
    //Serial.printf("token: %s\n", token);
    token = strtok(NULL, ",");
  }
  return true;
}
