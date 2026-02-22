#include <TimeLib.h>
#include <SoftwareSerial.h>
#include <ThreeWire.h>  
#include <arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_INA219.h>
 
#include <TinyGPS.h>

/* This sample code demonstrates the normal use of a TinyGPS object.
   It requires the use of SoftwareSerial, and assumes that you have a
   4800-baud serial GPS device hooked up on pins 4(rx) and 3(tx).
*/

TinyGPS gps;
SoftwareSerial ss(16, 17);
Adafruit_INA219 ina219;

char dateTime[20];

char buffer[1000]; /* Generic buffer */

static void smartdelay(unsigned long ms);
static void print_float(float val, float invalid, int len, int prec);
static void print_int(unsigned long val, unsigned long invalid, int len);
static void print_date(TinyGPS &gps);
static void print_str(const char *str, int len);


bool timeSync = false;

void setup()
{
  time_t time = 0;

  Serial.begin(115200);



  //setTime(time);
  setTime(0, 0, 0, 1, 1, 2000);

  log(F("Default date time set...."));

      Serial.print("CPU Freq: ");
    Serial.println(getCpuFrequencyMhz());
 
    //setCpuFrequencyMhz(80);
 
    Serial.print("CPU Freq: ");
    Serial.println(getCpuFrequencyMhz());

  
  ss.begin(9600);


  log(F("Setup started"));

  SetupDisplay();
  DisplayStatus();
  
  SdCardSetup();
  
  if (! ina219.begin())
   {
    log(F("Failed to find INA219 chip"));
   }


  log(F("Testing TinyGPS library"));
  log(TinyGPS::library_version());

}

void loop()
{
  float flat, flon;
  unsigned long age, date, time, chars = 0;
  unsigned short sentences = 0, failed = 0;
  static const double LONDON_LAT = 51.508131, LONDON_LON = -0.128002;
  int year;
  byte month, day, hour, minute, second, hundredths;

  DisplayStatus();

  if (timeSync)
   {
    INA219_loop();
    WriteData();
   }
   
   gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
   if (gps.satellites() != TinyGPS::GPS_INVALID_SATELLITES && age != TinyGPS::GPS_INVALID_AGE && age < 1500)
    {
     Serial.print("Age: ");
     Serial.println(age);
     log(F("----------------------------------------- Set time ----------------------------------------"));
     setTime(hour, minute, second, day, month, year);
     timeSync = true;
     log(F("----------------------------------------- Set time done ----------------------------------------"));
    }
  else
  {
    log(F("######## time is not valid !"));
    if (gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES)
      { 
         log(F("GPS_INVALID_SATELLITES !"));
      }
    if (age == TinyGPS::GPS_INVALID_AGE)
     {
       log(F("Invalid age GPS_INVALID_AGE !"));
    }
    log(F("Age is %l"), age);
    log(F("#############################"));
  }

  log(F(""));
  Serial.println("Sats HDOP Latitude  Longitude  Fix  Date       Time     Date Alt    Course Speed Card  Chars Sentences Checksum");
  Serial.println("          (deg)     (deg)      Age                      Age  (m)    --- from GPS ----   RX    RX        Fail");
  Serial.println("-------------------------------------------------------------------------------------------------------------------------------------");

  print_int(gps.satellites(), TinyGPS::GPS_INVALID_SATELLITES, 5);
  print_int(gps.hdop(), TinyGPS::GPS_INVALID_HDOP, 5);
  gps.f_get_position(&flat, &flon, &age);
  print_float(flat, TinyGPS::GPS_INVALID_F_ANGLE, 10, 6);
  print_float(flon, TinyGPS::GPS_INVALID_F_ANGLE, 11, 6);
  print_int(age, TinyGPS::GPS_INVALID_AGE, 5);
  print_date(gps);
  print_float(gps.f_altitude(), TinyGPS::GPS_INVALID_F_ALTITUDE, 7, 2);
  print_float(gps.f_course(), TinyGPS::GPS_INVALID_F_ANGLE, 7, 2);
  print_float(gps.f_speed_kmph(), TinyGPS::GPS_INVALID_F_SPEED, 6, 2);
  print_str(gps.f_course() == TinyGPS::GPS_INVALID_F_ANGLE ? "*** " : TinyGPS::cardinal(gps.f_course()), 6);
 

  gps.stats(&chars, &sentences, &failed);
  print_int(chars, 0xFFFFFFFF, 6);
  print_int(sentences, 0xFFFFFFFF, 10);
  print_int(failed, 0xFFFFFFFF, 9);
  Serial.println();
  
  smartdelay(1000);
}

static void smartdelay(unsigned long ms)
{
  unsigned long start = millis();
  int cnt=0;
  char c;
  do 
  {

    while (ss.available())
      {
        c = ss.read();
        //buffer[cnt++]=c;
        gps.encode(c);
      }
  } while (millis() - start < ms);
  /*Serial.print(F("Read count from Gps: "));
  Serial.println(cnt);
  if (cnt > 0)
   {
    buffer[cnt]=0;
    Serial.println(buffer);
   }*/
}

static void print_float(float val, float invalid, int len, int prec)
{
  if (val == invalid)
  {
    while (len-- > 1)
      Serial.print('*');
    Serial.print(' ');
  }
  else
  {
    Serial.print(val, prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i=flen; i<len; ++i)
      Serial.print(' ');
  }
  smartdelay(0);
}

static void print_int(unsigned long val, unsigned long invalid, int len)
{
  char sz[32];
  if (val == invalid)
    strcpy(sz, "*******");
  else
    sprintf(sz, "%ld", val);
  sz[len] = 0;
  for (int i=strlen(sz); i<len; ++i)
    sz[i] = ' ';
  if (len > 0) 
    sz[len-1] = ' ';
  Serial.print(sz);
  smartdelay(0);
}

static void print_date(TinyGPS &gps)
{
  int year;
  byte month, day, hour, minute, second, hundredths;
  unsigned long age;
  gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
  if (age == TinyGPS::GPS_INVALID_AGE)
    Serial.print("********** ******** ");
  else
  {
    char sz[32];
    sprintf(sz, "%02d/%02d/%02d %02d:%02d:%02d ",
        month, day, year, hour, minute, second);
    Serial.print(sz);

    time_t t = 0;
  }
  print_int(age, TinyGPS::GPS_INVALID_AGE, 5);
  smartdelay(0);
}

static void print_str(const char *str, int len)
{
  int slen = strlen(str);
  for (int i=0; i<len; ++i)
    Serial.print(i<slen ? str[i] : ' ');
  smartdelay(0);
}