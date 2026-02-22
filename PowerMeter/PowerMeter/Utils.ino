
int freeMemory()
{
 return ESP.getFreeHeap();
}


void GetFreeMemory()
{
 log(F("Free memory: %d"), (int)(freeMemory()));
}

char* getDateTime()
{
  if (timeSync)
   sprintf(dateTime,"%04d-%02d-%02d %02d:%02d:%02d", year(), month(), day(), hour(), minute(), second());
  else
   sprintf(dateTime,"%d", millis());
   
   return dateTime;
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

