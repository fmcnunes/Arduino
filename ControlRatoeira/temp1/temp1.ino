//#include <IRremote.h>
#include <Servo.h>
#define IR_RECEIVE_PIN 7

#define PIN_IR 3
#define PIN_DETECT 2

#define SERVO_PIN 9

const int OUTPUT_LED = 3;

bool state = LOW;

double activateTime = millis();
double testTime = millis();
double protectionTime = millis();

long oldPulseCount  = 0;

int tripWireCount = 0;

volatile unsigned long pulseCount = 0;

//Servo servo;

void setup() {
  Serial.begin(115200);


  pinMode(OUTPUT_LED, OUTPUT);
  TCCR2A = _BV(COM2B0) | _BV(WGM21); // Toggle OC2B on Compare Match, CTC mode
  TCCR2B = _BV(CS20);               // No prescaler
  OCR2A = 209;   

      // Set up pin 7 as input with internal pull-up resistor
  pinMode(IR_RECEIVE_PIN, INPUT);

  // Enable Pin Change Interrupt on Port D (which includes pin 7)
  PCICR |= (1 << PCIE2);    // Enable Pin Change Interrupt for PCINT23..16 (Port D)
  PCMSK2 |= (1 << PCINT23); // Enable interrupt for Pin 7 (PD7/PCINT23)

  Serial.println("System Initialized with PCINT on Pin 7.");


  Serial.println("--------- Relay ------------");

  //servo.attach(2, 500, 2500);
  
  //servo.write(0);
  Serial.println("waiting for servo");
  delay(3000);

  Serial.println("--------- Loop ------------");
}

void loop() {

 if (millis() - activateTime > 10)
  {
     
     for(int i=0; i< 16; i++)
     {
      ledOn();
      delayMicroseconds(570);
      ledOff();
      delayMicroseconds(570);
     }
     activateTime=millis();
  }

 /*
    if (state)
      {
       TCCR2A |= _BV(COM2B0); // Enable 38 kHz signal on OC2B (pin 3)
       state = LOW;
      }
      else
      {
       TCCR2A &= ~_BV(COM2B0); // Disable 38 kHz signal on OC2B (pin 3)
       state = HIGH;
      }
    activateTime = millis();
  } */

  /*if (IrReceiver.decode()) {
    IrReceiver.resume();
    Serial.println(IrReceiver.decodedIRData.command);
  }*/



  if ((millis()-testTime > 1000) && (millis() - protectionTime > 5000))
  {
     if (oldPulseCount != pulseCount)
     {
      Serial.print("Time: ");
      Serial.print(millis());
      Serial.print(" - Delta: ");
      Serial.print(pulseCount - oldPulseCount);
      Serial.print(" - tripWireCount: ");
      Serial.print(tripWireCount);
      Serial.print(" - Count: ");
      Serial.println(pulseCount);
      
/*
      if (pulseCount - oldPulseCount < 100)
       {
        tripWireCount++;
       }*/

       oldPulseCount = pulseCount;
     }
     else
     {
      Serial.print("Time: ");
      Serial.print(millis());
      Serial.print("  ------------------ !! No Pulse ----------------------------------------");
      Serial.print(" - Count: ");
      Serial.println(pulseCount);
      tripWireCount++;
     }
     testTime = millis();

  
     if (tripWireCount > 0)
      {
        tripWire();
      }
  }

}

ISR(PCINT2_vect) {
  if (digitalRead(7) == LOW) { // Ensure it was a button press (falling edge)
    pulseCount++;  // Increment pulse count
  }
}

void setOutputLed()
{
  pinMode(OUTPUT_LED, OUTPUT);
  TCCR2A = _BV(COM2B0) | _BV(WGM21); // Toggle OC2B on Compare Match, CTC mode
  TCCR2B = _BV(CS20);               // No prescaler
  OCR2A = 209;   
}

void ledOn()
{
  TCCR2A |= _BV(COM2B0); // Enable 38 kHz signal on OC2B (pin 3)
}

void ledOff()
{
  TCCR2A &= ~_BV(COM2B0); // Disable 38 kHz signal on OC2B (pin 3)
}

void tripWire()
{
  Serial.println("  ------------------ Tripwire ---------------------");
  tripWireCount = 0;
  //protectionTime = millis();
  /*
  setServoAngle(180); 
  delay(3000);
  setServoAngle(0);
  delay(3000);
  tripWireCount = 0;
  */
}

void setServoAngle(int angle) {
  // Adjust the pulse width range if necessary
  int pulseWidth = map(angle, 0, 180, 540, 2400); // Try narrowing the range

  for (int i = 0; i < 200; i++) { // Send the pulse 50 times
    digitalWrite(SERVO_PIN, HIGH);
    delayMicroseconds(pulseWidth);
    digitalWrite(SERVO_PIN, LOW);
    delayMicroseconds(20000 - pulseWidth); // Complete the 20ms period (50Hz)
  }
}
