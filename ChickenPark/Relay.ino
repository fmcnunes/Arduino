/***************************************
 *
 * return relay status
 *
 **************************************/
 
bool isRelayOn(int relayId)
{
  relayId=relayId-1;
  
  if (digitalRead(Relays[relayId])==HIGH)
    return false;

  return true;
}


/***************************************
 *
 * Check if On onRelay is ended
 *
 **************************************/
 
void VerifyRelaysTimeout()
{
  for(int i=1; i<= nRelays; i++)
    if (relayStatus[i-1].duration < millis() && isRelayOn(i))
    {
       RelayCommand(i, 0, 0);
    }
}

/***************************************
 *
 * Change Name
 *
 **************************************/

void RelaySetName(int relayId, char *newName)
{
  relayId=relayId-1;
  sprintf(relayStatus[relayId].relayName,"%s",newName);
  writeRelayToEeprom(relayId);
}

/***************************************
 *
 * Send Relays commands
 *
 **************************************/

void RelayCommand(int relayId, int action, int duration)
{
  boolean outBit = LOW;
  unsigned long elapsed = duration;
  char buffer[256];


  relayId= relayId-1;
        
  if (action > 0)
   {
    sprintf(buffer,"{ \"status\"=\"ON\", \"duration\"=%d }", duration);
    mqttSendStatus(relayStatus[relayId].relayName, buffer);
    
    sprintf(relayStatus[relayId].lastOn, "%s", getDateTimeShort());

    sprintf(buffer,"%s\tActivating relay %d for %d seconds", getDateTime(), relayId+1, duration);
    mqttSendLog(buffer);


    relayStatus[relayId].lastDuration = elapsed;
    relayStatus[relayId].duration = millis() + elapsed*1000L;
    log(F("Setting relay %d lastDuration to %d"), relayId, relayStatus[relayId].lastDuration);
    outBit = LOW;
   }
  else
   {
    sprintf(buffer,"%s\tDeactivating relay %d", getDateTime(), relayId+1);
    mqttSendLog(buffer);
    Serial.println(buffer);
    
    mqttSendStatus(relayStatus[relayId].relayName, "{ \"status\"=\"OFF\" }");
    
    relayStatus[relayId].duration = 0;
    log(F("Setting relay %d duration to %d"), relayId, relayStatus[relayId].duration);
    outBit = HIGH;
   }

  digitalWrite(Relays[relayId], outBit);

}
