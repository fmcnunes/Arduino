int freeMemory()
{
 return ESP.getFreeHeap();
}

char* getDateTime()
{  
   RtcDateTime now = getLocalDateTime();

   sprintf(dateTime,"%04d-%02d-%02d %02d:%02d:%02d", now.Year(), now.Month(), now.Day(), now.Hour(), now.Minute(), now.Second());
   
   return dateTime;
}

bool ClockOk()
{
  RtcDateTime now = getLocalDateTime();

  return (now.Year() >= 2018);
}

char* getDateTimeShort()
{
    
   RtcDateTime now = getLocalDateTime();

   if (now.Year() >= 2018)
   {
    lastClockCheckOk = millis();
   }
   else
   {
    if (lastClockSetup < (millis() - 10000))
    {
     lastClockSetup = millis();
     Serial.print("Clock failure...Performing RTC Setup\n");
     mqttSendLog("Clock failure...Performing RTC Setup");
     SetupRTC();
     now = getLocalDateTime();
    }
   }

   if (now.Year() < 2018 && lastClockCheckOk < (millis() - 120000))
   {
    Serial.print("Clock failure. Year=");
    Serial.print(now.Year());
    Serial.print("Performing system reset\n");
    mqttSendLog("Clock failure...Performing system reset");
    Reload();
   }
   sprintf(dateTime,"%04d-%02d-%02d %02d:%02d", now.Year(), now.Month(), now.Day(), now.Hour(), now.Minute());

   return dateTime;
}

/*
char* getDateTime()
{
  sprintf(dateTime,"%4d-%2d-%2d %2d:%2d:%2d", 2018, 8, 15, 16, 43, 45);
  return dateTime;
}

char* getDateTimeShort()
{  
   sprintf(dateTime,"%4d-%2d-%2d %2d:%2d", 2018, 8, 15, 16, 43);
    return dateTime;
}
*/

void GetFreeMemory()
{
 log(F("Free memory: %d"), (int)(freeMemory()));
}

void   showMemoryUsage()
{
  char buffer[100];
  sprintf(buffer,"%s\tFree heap memory %d bytes\n", getDateTime(), ESP.getFreeHeap()); 
  Serial.print(buffer);
  sprintf(buffer,"%d",ESP.getFreeHeap());
  mqttSendStatus("freeMemory", buffer);
}


String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

boolean isValidNumber(String str)
{
   for(byte i=0;i<str.length();i++)
   {
      if(! isDigit(str.charAt(i))) return false;
   }
   return true;
}

void Fcopy(char* buf, int n, const __FlashStringHelper *ifsh)
{
  const char PROGMEM *p = (const char PROGMEM *)ifsh;
  int i = 0;
  uint8_t c  = 0;
  do
  {
    c = pgm_read_byte(p++);
    buf[i++] = c;
  } while ( c != 0 && i < n);
}


void decodeFromBase64(char *decoded, int len, char* encoded)
{
  decoded[0]=0;
  size_t outputLength;
  unsigned char * tmpdecoded = base64_decode((const unsigned char *)encoded, strlen(encoded), &outputLength);
  
  Serial.print("Length of decoded message: ");
  Serial.println(outputLength);
  Serial.printf("%.*s", outputLength, tmpdecoded);
    
  log(F("Decoded len is %d"), outputLength);
  if (outputLength > len)
    {
      free(tmpdecoded);
      return;
    }
  
  strcpy(decoded,(const char*) tmpdecoded);
  free(tmpdecoded);
  log(F("Decoded <%s>"), decoded);
    
  return;
}
