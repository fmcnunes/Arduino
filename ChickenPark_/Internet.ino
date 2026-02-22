void checkInternet()
{
  char buffer[512];
   bool newInternetConnectionState = checkInternetConnection(); 
   if (newInternetConnectionState != internetConnectionState) 
     {

       if (newInternetConnectionState)
         sprintf(buffer, "%s\tWiFI connection changed to CONNECTED.\n", getDateTime());  
        else
         sprintf(buffer, "%s\tWiFI connection changed to DISCONNECTED.\n", getDateTime());  
      Serial.print(buffer); 
      lastConnectionChange = millis();
      internetConnectionState = newInternetConnectionState;
     }
  
   if ( (! internetConnectionState) && (millis() - lastConnectionChange > internetResetAfter))
     {
      sprintf(buffer, "%s\tRecovering router WIFI.\n", getDateTime()); 
      Serial.print(buffer);
      RelayCommand(8, 1, 10);
      lastConnectionChange = millis();
     }
}

boolean checkInternetConnection()
{
  if (WiFi.status() != WL_CONNECTED)
    return false;
  
  char buffer[256];
  IPAddress googleDns(8, 8, 8, 8);
  char googleServerName[] = "www.google.com";
  IPAddress ip(0,0,0,0);

  WiFi.hostByName(googleServerName, ip);

  //sprintf(buffer, "IP is %u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);
  
  if ((ip[0] == 0) && (ip[1] == 0) && (ip[2] == 0) && (ip[3] == 0))
    return false;
  else
    return true;
}
