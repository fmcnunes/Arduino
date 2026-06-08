

void setupMQTT()
{
   lastMqttCheck = millis();
   
   mqttClient.setServer("192.168.0.7", 1883);

   connect2Mqtt();
    
   int retry=0;
   while((!mqttClient.connected()) && retry < 3)
   {
    connect2Mqtt();
    retry++;
    delay(1000);
   }
}

void connect2Mqtt()
{
  char subscribeTopic[100];

  lastMqttCheck = millis();
  
  sprintf(subscribeTopic,"%s/cmd/txt", mqttId);
  
  if (!mqttClient.connected())
    {
      log(F("Connecting to MQTT..."));
 
      if (mqttClient.connect("TermoSifao", "termosifao", "Kusku2026")) 
      {
        mqttClient.publish("termoSifao/status/txt", "TermoSifao connected.");
        mqttClient.setCallback(mqttCallback);
        mqttClient.subscribe(subscribeTopic);
        log(F("MQTT client connected !"));
      }
      else
      {
       log(F("MQTT connect failed with state %d"), mqttClient.state());
      }
   }
   else
   {
     log(F("MQTT is connected, no need to reconnect."));
     mqttClient.loop();
   }
}

void mqttCallback(char* topic, byte* payload, unsigned int length)
{
  char subscribeTopic[100];
  
  sprintf(subscribeTopic,"%s/cmd/txt", mqttId);

  if (length > 0)
   {
     char charPayload[length+1];

     for (int i = 0; i < length; i++)
     {
      charPayload[i]=payload[i];
     }
     charPayload[length] = 0;

    log(F("Received MQTT Message, topic is (%s), payload: %s"), topic, charPayload);

    if (strcmp(topic,subscribeTopic) == 0)
     {
      processCommand(charPayload);
     }
   }
}

void mqttSendLog(char* msg)
{
  char buffer[100];
  
  sprintf(buffer,"%s/status/txt", mqttId);
  mqttClient.publish(buffer, msg);
}

void mqttSendStatus(const char* component, const char* msg)
{
  char buffer[100];

  sprintf(buffer,"%s/%s/status", mqttId, component);
  mqttClient.publish(buffer, msg);
}

void MqttStatus()
{
  char topic[100];
  char payload[100];
  char buffer[128];
  
  for (int relayId = 0; relayId < nRelays; relayId++)
  {
    sprintf(topic, "relay%d", relayId+1);
    
        if (digitalRead(Relays[relayId]) == HIGH)
        {
          sprintf(buffer,"{ \"status\"=\"Off\", \"duration\"=0  }", relayStatus[relayId].duration);
          mqttSendStatus(relayStatus[relayId].relayName, buffer); 
        }         
        else
        {
          sprintf(buffer,"{ \"status\"=\"On\", \"duration\"=%d }", relayStatus[relayId].duration - millis());
          mqttSendStatus(relayStatus[relayId].relayName, buffer);
        }
  }

  for(int schedId=0; schedId< maxSchedules; schedId++)
  {
    sprintf(topic, "sched%d", schedId+1);
    if (schedule[schedId].enabled)
     sprintf(buffer,"{ \"status\": \"enabled\", \"cron\": \"%s\"}", schedule[schedId].cronString);
    else
     sprintf(buffer,"{ \"status\": \"disabled\", \"cron\": \"%s\"}", schedule[schedId].cronString);

    mqttSendStatus(topic, buffer);
  }
  //mqttSendStatus("temperature", "23");
  //mqttSendStatus("humidity", "77");
  sprintf(payload, "%d", freeMemory()); 
  mqttSendStatus("freeMemory", payload);
  if (readInputRelay())
     mqttSendStatus("InputRelay","{ \"status\"=\"Off\" }");
   else
     mqttSendStatus("InputRelay","{ \"status\"=\"On\"} }");
}
