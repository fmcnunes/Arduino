// Time - Version: Latest 
// #include <Time.h>
#include <TimeLib.h>

void setup() {
  
   Serial.begin(115200);
//    This works to set the time ...
    setTime(12,59,11,12,9,2018);

//    This does NOT work ...
//  String newTime = "12,59,11,12,9,2017";
//  setTime(newTime);   // error: no matching function for call to 'setTime(String&)'

//    Niether does this ...
//    char newTime[20] = {12,59,11,12,9,2017};
//    newTime[19] = "\0";
//    setTime(newTime);   //  results = 0:38  1/1/1970


    Serial.println();
//    Serial.println(newTime);
  time_t t = now();
  
  Serial.print(hour(t));
  Serial.print(":");
  Serial.print(minute(t));
    Serial.print(":");
  Serial.print(second(t));
  Serial.print("  ");
  Serial.print(day(t));
  Serial.print("/");
  Serial.print(month(t));
  Serial.print("/");
  Serial.println(year(t));
    Serial.print(hour(t));
  Serial.print(":");
  Serial.print(minute(t));
    Serial.print(":");
  Serial.print(second(t));
  Serial.print("  ");
  Serial.print(day(t));
  Serial.print("/");
  Serial.print(month(t));
  Serial.print("/");
  Serial.println(year(t));
    Serial.print(hour(t));
  Serial.print(":");
  Serial.print(minute(t));
    Serial.print(":");
  Serial.print(second(t));
  Serial.print("  ");
  Serial.print(day(t));
  Serial.print("/");
  Serial.print(month(t));
  Serial.print("/");
  Serial.println(year(t));
}

void loop() {
  time_t t = now();
  
  Serial.print(hour(t));
  Serial.print(":");
  Serial.print(minute(t));
  Serial.print("  ");
  Serial.print(day(t));
  Serial.print("/");
  Serial.print(month(t));
  Serial.print("/");
  Serial.println(year(t));
 delay(1000);
}