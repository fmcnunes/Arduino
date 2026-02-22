void setup()
 {
  pinMode(3, OUTPUT);               // Set pin 3 as an output
  TCCR2A = _BV(COM2B0) | _BV(WGM21); // Toggle OC2B on Compare Match, CTC mode
  TCCR2B = _BV(CS20);               // No prescaler
  OCR2A = 209;   
}
void loop() 
{

}