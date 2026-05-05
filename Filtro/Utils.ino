
char dateTime[25];

char* getDateTime()
{  

   sprintf(dateTime,"%04d-%02d-%02d %02d:%02d:%02d", 2026, 01, 23, 22, 15, 45);
   
   return dateTime;
}

char* seconds2Time(unsigned long seconds)
{
    unsigned int m = seconds / 60;
    unsigned int s = seconds % 60;

    sprintf(dateTime, "%02u:%02u", m, s);

    return dateTime;
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
