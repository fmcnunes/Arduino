
// CONNECTIONS:
// DS1302 CLK/SCLK --> 17
// DS1302 DAT/IO --> 25
// DS1302 RST/CE --> 26
// DS1302 VCC --> 3.3v - 5v
// DS1302 GND --> GND
//Para usar o DS1302

#ifdef DS1302
 ThreeWire myWire(19, 18, 23); // IO, SCLK, CE
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
#ifdef DS1302
    rtc.Begin();
#endif


#ifdef DS1307
    rtc.begin();
    if (! rtc.begin())
     {
      Serial.println("Couldn't find RTC !!!!!!");
     }
     else
     {
      Serial.println("RTC is running..............................................");
     }
#endif

    DateTime compiled = DateTime(__DATE__, __TIME__);
    Serial.print("Code compiled at ");
    serialPrintDateTime(compiled);
    Serial.println();

#ifdef DS1302
    if (!rtc.IsDateTimeValid())
#endif       

#ifdef DS1307
     if (! rtc.isrunning())
#endif  
      {    
        // Common Causes:
        //    1) first time you ran and the device wasn't running yet
        //    2) the battery on the device is low or even missing

        Serial.println("RTC lost confidence in the DateTime!");
        Serial.print("RTC setting date time to ");
        serialPrintDateTime(compiled);
        Serial.println();
       
#ifdef DS1302  
        rtc.SetDateTime(compiled);
#endif
#ifdef DS1307
       rtc.adjust(DateTime(compiled.Year(),compiled.Month(),compiled.Day(),compiled.Hour(),compiled.Minute(), compiled.Second()));
#endif       
    }

#ifdef DS1302  
    if (rtc.GetIsWriteProtected())
    {
        Serial.println("RTC was write protected, enabling writing now");
        rtc.SetIsWriteProtected(false);
    }
   

    if (!rtc.GetIsRunning())
    {
        Serial.println("RTC was not actively running, starting now");
        rtc.SetIsRunning(true);
    }
#endif     

#ifdef DS1302
    RtcDateTime now = rtc.GetDateTime();
#endif
#ifdef DS1307
    DateTime _now = rtc.now();
    RtcDateTime now = RtcDateTime(_now.year(),_now.month(),_now.day(),_now.hour(),_now.minute(), _now.second());
#endif    
    if (now < compiled) 
    {
        Serial.println("RTC is older than compile time!");
        Serial.println("Updating DateTime !");

#ifdef DS1302  
        rtc.SetDateTime(compiled);
#endif
#ifdef DS1307
       rtc.adjust(DateTime(compiled.Year(),compiled.Month(),compiled.Day(),compiled.Hour(),compiled.Minute(), compiled.Second()));
#endif        
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


void serialPrintDateTime(const DateTime& dt)
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
