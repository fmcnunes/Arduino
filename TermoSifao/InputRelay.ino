/***************************************
 *
 * Configure input relay
 *
 **************************************/
 
void configureInputRelay()
{
   pinMode(INPUTRELAY, INPUT);  // Set as input
   pinMode(INPUTRELAY, INPUT_PULLUP); // Input with pullup resitor
}
/***************************************
 *
 * return inuo relay value
 *
 **************************************/
 
bool readInputRelay()
{
  return digitalRead(INPUTRELAY);
}
