
// CONNECTIONS:
// DS1302 CLK/SCLK --> 17
// DS1302 DAT/IO --> 25
// DS1302 RST/CE --> 26
// DS1302 VCC --> 3.3v - 5v
// DS1302 GND --> GND
//Para usar o DS1302

#ifdef DS1302
 ThreeWire myWire(19, 23, 18); // DAT=19, CLK=23, RST=18
 RtcDS1302<ThreeWire> rtc(myWire);
#endif


//Para usar o Ds1307
#ifdef DS1307
RTC_DS1307 rtc;
#endif


/********************************
 * Setup RTC
 ********************************/

void SetupRTC()
{
  //Wire.begin();

    Serial.println("Starting RTC DS1302");
    rtc.Begin();

  if (!rtc.GetIsRunning()) {
    Serial.println("RTC was not running, starting it now");
    rtc.SetIsRunning(true);
  }

  RtcDateTime now = rtc.GetDateTime();

  if (!now.IsValid()) {
    Serial.println("RTC time not valid, setting compile time");
    rtc.SetDateTime(RtcDateTime(__DATE__, __TIME__));
      RtcDateTime now = rtc.GetDateTime();
  }

    if (rtc.GetIsWriteProtected())
    {
        Serial.println("RTC was write protected, enabling writing now");
        rtc.SetIsWriteProtected(false);
    }


  now = rtc.GetDateTime();
  if (!now.IsValid()) {
    Serial.println("!!!!!!!!!! RTC read ERROR !!!!!!!!!!!!!!!!");
  }
}


/********************************
 * Get Local DateTime
 ********************************/
 
RtcDateTime getLocalDateTime()
{
#ifdef DS1302
    RtcDateTime dt = rtc.GetDateTime();
#endif
#ifdef DS1307
    DateTime _now = rtc.now();
    RtcDateTime dt = RtcDateTime(_now.year(),_now.month(),_now.day(),_now.hour(),_now.minute(), _now.second());
#endif    
  
  if (! isDst(dt))
  {
    dt.InitWithEpoch32Time(dt.Epoch64Time() - 3600u);
  }

  return dt;
}


/***************************************
 * Verify if DayLighSaving Time active
 ***************************************/

bool isDst(RtcDateTime dt)
{
  if (dt.Month() < 3 || dt.Month() > 10)  return false; 
  if (dt.Month() > 3 && dt.Month() < 10)  return true; 

  int previousSunday = dt.Day() - dt.DayOfWeek();

  if (dt.Month() == 3) return previousSunday >= 25;
  if (dt.Month() == 10) return previousSunday < 25;

  return false; // this line never gonna happend
}

/********************************
 * Adjust RTC from unixtime - epoch
 ********************************/
 
void adjustRTC(unsigned long epoch)
{
  RtcDateTime dt;
  dt.InitWithEpoch64Time(epoch);
  //sprintf(buffer, "Setting RTC to %04d-%02d-%02d %02d:02%d:%02d\n", dt.Year(),dt.Month(),dt.Day,dt.Hour(),dt.Minute(), dt.Second());  
  
#ifdef DS1302  
  rtc.SetDateTime(dt);
#endif
#ifdef DS1307
  rtc.adjust(DateTime(dt.Year(),dt.Month(),dt.Day(),dt.Hour(),dt.Minute(), dt.Second()));
#endif    
}

/********************************
 * Change RTC datetime
 ********************************/
void setDateTime(String cmdStr)
{
  cmdStr.replace("-",",");
  cmdStr.replace(":",",");

  String y  = getValue(cmdStr, ',', 1);
  String mm = getValue(cmdStr, ',', 2);
  String d  = getValue(cmdStr, ',', 3);
  String h  = getValue(cmdStr, ',', 4);
  String m  = getValue(cmdStr, ',', 5);
  String s  = getValue(cmdStr, ',', 6);

  char buffer[200];
  sprintf(buffer,"%s\tSetting DateTime to %d-%d-%d %d:%d:%d",getDateTime(), y, mm, d, h ,m ,s);
  Serial.println(buffer);
  
#ifdef DS1302  
  rtc.SetDateTime(RtcDateTime(y.toInt(),mm.toInt(),d.toInt(),h.toInt(),m.toInt(),s.toInt()));
#endif
#ifdef DS1307
  rtc.adjust(DateTime(y.toInt(),mm.toInt(),d.toInt(),h.toInt(),m.toInt(),s.toInt()));
#endif   
    
}

void setDateTime(int year, int month, int day, int hour, int minute, int second)
{
  char buffer[200];
  
  Serial.print(getDateTime());
  sprintf(buffer,"\tSetting dateTime to %04d-%02d-%02d %02d:%02d:%02d",year,month,day,hour,minute,second);

#ifdef DS1302  
  rtc.SetDateTime(RtcDateTime(year,month,day,hour,minute,second));
#endif
#ifdef DS1307
  rtc.adjust(DateTime(year,month,day,hour,minute,second));
#endif  
    
}


void serialPrintDateTime(const RtcDateTime& dt)
{
    char datestring[20];

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
            dt.Month(),
            dt.Day(),
            dt.Year(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
    Serial.print(datestring);
}
