

/*
   SerialEvent occurs whenever a new data comes in the hardware serial RX. This
   routine is run between each time loop() runs, so using delay inside loop can
   delay response. Multiple bytes of data may be available.
 */
void serialEvent()
{
   char inChar = 0;
   while (Serial.available())
   {
     // get the new byte:
     inChar = (char)Serial.read();
     // add it to the input Buffer
     if (inChar == '\r')
     {
       inputBufferComplete = true;
     }
     else
     {
      if (inChar != '\n' && inputBufferPos < (INPUTBUFFERSIZE-1))
      {
        inputBuffer[inputBufferPos++] = inChar;
        inputBuffer[inputBufferPos] = 0;
      }
     }
   }
}

void clearInputBuffer()
{
  for(int i=0; i< INPUTBUFFERSIZE; i++) 
    inputBuffer[i] = 0;
  inputBufferPos = 0;
  inputBufferComplete = false;
}

