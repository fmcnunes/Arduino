char buffer[400];

#define READSTRING_SIZE 250
#define MAX_CMDS 4
char readString[READSTRING_SIZE]; //string for fetching data from address
int  readStringLen = 0;
char* cmds[MAX_CMDS];

WiFiClient tcpClient;

WiFiServer server(80);

void setupWebServer()
{
  readString[0]=0;

  server.begin();
  //log(F("Server is at %s"), WiFi.localIP());
}

void WebServer() {
  // listen for incoming tcpClients
  tcpClient = server.available();

  if (tcpClient)
  {
    readString[0] = 0;
    readStringLen = 0;
    readString[READSTRING_SIZE-1] = 0;
    
    log(F("New HTTP tcpClient connected."));
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;

    unsigned long connectTime = millis();

    while (tcpClient.connected() && (millis() < (connectTime + 4000L)))
    {
      if (tcpClient.available())
      {
        char c = tcpClient.read();

        //log("WebServer (%d)(%d) c=%d %c", strlen(readString), readStringLen, (int) c, c);

        //read char by char HTTP request
        if (readStringLen < READSTRING_SIZE -1)
        {
          //store characters to string
          readString[readStringLen]=c;
          readString[readStringLen+1]=0;
          readStringLen++;
        }
        else
        {
          log("WebServer, readString buffer is full.");
        }

        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank)
        {
          log("WebServer, 'c' is NL and currentLineIsBlank=true");
          //Serial.println(readString);
          // send a standard http response header

          int j = 0;
          int nCmds = 0;
          cmds[0] = readString;

          log(F("CMD is %s"), readString);

          for (j = 6; (j < READSTRING_SIZE && readString[j] != 0 && readString[j] != '\n' && nCmds < MAX_CMDS); j++)
           {
             if ((readString[j - 1] == ',' && readString[j] != ',') ||
                 (readString[j]     != ',' &&
                  readString[j - 1] == '=' &&
                  readString[j - 2] == 'd' &&
                  readString[j - 3] == 'm' &&
                  readString[j - 4] == 'c' &&
                  readString[j - 5] == '?'))
              {
               readString[j - 1] = 0;
               cmds[nCmds++] = readString + j;
              }
              if (readString[j - 1] == ',') readString[j - 1] = 0;
              if (readString[j] == ',' && readString[j+1] == 0) readString[j] = 0;
            }

            for (j = 0; j < nCmds; j++)
            {
              for(int i=0; i < strlen(cmds[j]); i++)
              {
                if (cmds[j][i] == ' ')
                  {
                    cmds[j][i]=0;
                  }
              }

              decodeFromBase64(buffer, 100, cmds[j]);
              log(F("Received command: >%s< Decoded: >%s<"), cmds[j], buffer);
              processCommand(buffer); 
            }


          tcpClient.println("HTTP/1.1 200 OK");

          tcpClient.println("Content-Type: application/json; charset=UTF-8");
          tcpClient.println("Cache-Control: no-cache");
          tcpClient.println("Connection: close");  // the connection will be closed after completion of the response
          tcpClient.println("Transfer-Encoding: chunked");
          tcpClient.println();

          GetSystemStatus();
          /*
          GetSystemStatus_header(buffer);
          sprintf(teststring, "%02X", strlen(buffer));
          tcpClient.println(teststring);
          tcpClient.println(buffer);
          
          GetSystemStatus_relay(buffer);
          sprintf(teststring, "%02X", strlen(buffer));
          tcpClient.println(teststring);
          tcpClient.println(buffer);
                  
          GetSystemStatus_sch(buffer);
          sprintf(teststring, "%02X", strlen(buffer));
          tcpClient.println(teststring);
          tcpClient.println(buffer);
          
          GetSystemStatus_footer(buffer);
          sprintf(teststring, "%02X", strlen(buffer));
          tcpClient.println(teststring);
          tcpClient.println(buffer);
          */
                 
          tcpClient.println(0);
          tcpClient.println("");
          // give the web browser time to receive the data
          delay(10);
          // close the connection:
          tcpClient.stop();
          log(F("Tcp client disconnected"));
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }


  }
}

/***********************************************/
void SendAndFlushBuffer()
{
  
  char buffLen[4];
  int len = strlen(buffer);
  if (len > 0)
  {
   sprintf(buffLen, "%02X", strlen(buffer));
   tcpClient.println(buffLen);
   tcpClient.println(buffer);
   strcpy(buffer, "");
  }
}
/********************* Status ******************/
void GetSystemStatus()
{
  char str[200];

  strcpy(buffer, "{\r\n");

  sprintf(str, "\"Command Status\": \"%s\",\r\n", "ok");
  strcat(buffer, str);

  sprintf(str, "\"DateTime\": \"%s\",\r\n", getDateTime());
  strcat(buffer, str);

  SendAndFlushBuffer();

  buffer[0] = 0;
  
  // Relay Status

  GetFreeMemory();

  strcat(buffer, "\"Relays\": [ \r\n");


  for (int i = 0; i < nRelays; i++)
  {
    if (i > 0) strcat(buffer, ", \r\n");

    strcat(buffer, " { ");

    sprintf(str, "\"Id\": %d,", i+1);
    strcat(buffer, str);

    sprintf(str, "\"Name\": \"%s\",", relayStatus[i].relayName);
    strcat(buffer, str);
    
    sprintf(str, "\"LastOn\": \"%s\",", relayStatus[i].lastOn);
    strcat(buffer, str);

    sprintf(str, "\"LastDuration\": %d,", relayStatus[i].lastDuration);
    strcat(buffer, str);
    
    //Serial.println(getDateTime() + i + String(F("\tduration: ")) + relayStatus[i].duration + "\t" + millis());
    
    if (digitalRead(Relays[i]) == HIGH)
    {
     sprintf(str, "\"Duration\": %d,", 0);
     strcat(buffer, str);
     sprintf(str, "\"Status\": \"%s\"","off");
     strcat(buffer, str);
    }
    else
    {
      if (relayStatus[i].duration > millis())
      {
        sprintf(str, "\"Duration\": %d,", (int)((relayStatus[i].duration - millis()) / 1000.0));
        strcat(buffer, str);
      }
      else
      {
        sprintf(str, "\"Duration\": %d,", 0);
        strcat(buffer, str);
      }
      sprintf(str, "\"Status\": \"%s\"", "On");
      strcat(buffer, str);      
    }


    strcat(buffer, " } ");
    if (i%2 == 0) SendAndFlushBuffer();
  }
  
 
  strcat(buffer,"\r\n], \r\n");

  SendAndFlushBuffer();

  // Scheduler Status
  GetFreeMemory();

  buffer[0] = 0;

  strcat(buffer, "\"Schedulers\": [ \r\n");

//maxSchedules
  for (int i = 0; i < maxSchedules; i++)
  {
    if (i > 0) strcat(buffer, ", \r\n");
    
    strcat(buffer, " { ");
    
    sprintf(str, "\"Id\": %d,", i+1);
    strcat(buffer, str);  

    if (schedule[i].enabled)
      strcat(buffer,"\"Status\": \"Enabled\", ");
    else
      strcat(buffer,"\"Status\": \"Disabled\", ");

    sprintf(str, "\"LastRun\": \"%s\",", schedule[i].lastRun);
    strcat(buffer, str);
    
    sprintf(str, "\"Schedule\": \"%s\"", schedule[i].cronString);
    strcat(buffer, str);

    strcat(buffer, "}");
    GetFreeMemory();
    if (i%2 == 0) SendAndFlushBuffer();
  }

  SendAndFlushBuffer();
  log(F("Total json response size is %d bytes."), strlen(buffer));
  
/*****************/

  buffer[0] = 0;
    
  strcpy(buffer, "\r\n],\r\n");

  strcat(buffer, "\"InputRelay\": [ {\r\n");

  strcat(buffer,"\"Name\": \"Bomba do furo\", ");
  
  if (readInputRelay())
      strcat(buffer,"\"Status\": \"OFF\", ");
    else
      strcat(buffer,"\"Status\": \"ON\", ");

  sprintf(str, "\"Duration\": %d", (int)(millis() - lastInputRelayDebounceTime));
  strcat(buffer, str);
  
  strcat(buffer, "}\r\n]\r\n }");


  log(F("Total json response size is %d bytes."), strlen(buffer));
  
  SendAndFlushBuffer();
  return;
}
