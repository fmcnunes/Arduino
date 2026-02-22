const int A1A=9;
const int A1B=10;
const int B1A=6;
const int B1B=5;

#define A1 6  // Motor A pins
#define A2 5
#define B1 9 // Motor B pins
#define B2 10

int incomingByte = 0; // for incoming serial data

int stepnumber = 0;
int Pa;
int Pb;

int steps=16;

int  input = 0;
int x = 0;

unsigned long interval;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  pinMode(A1A, OUTPUT);
  pinMode(A1B, OUTPUT);
  pinMode(B1A, OUTPUT);
  pinMode(B1B, OUTPUT);

 

 interval = millis();
 
  Serial.println("select direction of movement");
  Serial.println("1.forward");
  Serial.println("2.backward");
  Serial.println("3.stop");
}
 
void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(F("Volta 360"));

    for(int i=0; i<(50*steps -1); i++)
  {
    stepnumber+=1;
    if (stepnumber > 64) { stepnumber = 0; }
    move(stepnumber, 255, 10000);
  }

/*
  for(int i=0; i<3199; i++)
  {
    stepnumber++;
    if (stepnumber > 64) { stepnumber = 0; }
    move(stepnumber, 255, 50);
  }*/
  move(0, 255, 1000);
  Serial.println(stepnumber);
  Serial.println("done");
  delay(3000);
  /*
  Serial.println("Volta 90");
 for(int i=0; i<1599; i++)
  {
    stepnumber++;
    move(stepnumber, 255, 1000);
  }
  move(0, 255, 1000);
  Serial.println(stepnumber);
  Serial.println("done");
  delay(3000);
*/
 if (millis() > interval + 100)
   {
     interval = millis();
   }
 

}

void move(int stepnumber, int MAXpower, int wait)
{
  //0.098174 = 2* pi / 64
  int n=1;
  if (stepnumber < 8)                         { Pb=MAXpower;  Pa=0; x++; Serial.println(x); }
  n++;
  if (stepnumber > 8 and stepnumber < n*8 )   { Pb=MAXpower/2;  Pa=MAXpower/2; }
  n++;
  if (stepnumber >= (n-1)*8 and stepnumber < n*8 ) { Pb=0;         Pa=MAXpower; }
  n++;
  if (stepnumber >= (n-1)*8 and stepnumber < n*8 )  { Pb=-MAXpower/2; Pa= MAXpower/2; }
  n++;
  if (stepnumber >= (n-1)*8 and stepnumber < n*8 )  { Pb=-MAXpower; Pa= 0; }
  n++;
  if (stepnumber >= (n-1)*8 and stepnumber < n*8 ) { Pb=-MAXpower/2; Pa= - MAXpower/2; }
  n++;
  if (stepnumber >= (n-1)*8 and stepnumber < n*8 )  { Pb=0 ; Pa= - MAXpower; }
  n++;
  if (stepnumber >= (n-1)*8 and stepnumber < n*8 )  { Pb= MAXpower / 2; Pa= - MAXpower /2; }

//Serial.println(Pa);
//Serial.println(Pb);



  Pb = (sin(stepnumber * 2 * PI / steps) * MAXpower);
  Pa = (cos(stepnumber * 2 * PI / steps) * MAXpower);

  if (Pa > 0)
  {
    analogWrite(A1A,Pa);
    analogWrite(A1B, 0);  
  }
  else
    {
    analogWrite(A1A, 0);
    analogWrite(A1B, abs(Pa));  
  }

  if (Pb > 0)
  {
    analogWrite(B1A,Pb);
    analogWrite(B1B, 0);  
  }
  else
    {
    analogWrite(B1A, 0);
    analogWrite(B1B, abs(Pb));  
  }
 
  delayMicroseconds(wait);
}





   /*
   L9110S-Stepper-DC-motor-Driver-Module
  made on 28 oct 2020
  by Amir Mohammad Shojaee @ Electropeak
  Home

*/


void loop2() {

  // send data only when you receive data:
  if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.read();
    input = incomingByte - 48; //convert ASCII code of numbers to 1,2,3

  switch (input) { 
    case 1:         // if input=1 ....... motors turn forward
      forward();
      break;
    case 2:         // if input=2 ....... motors turn backward
      backward();
      break;
    case 3:         // if input=1 ....... motors turn stop
      Stop();
      break;
  }
  delay(200);
  input=0;
}
}
void forward() {          //function of forward 
  Serial.println("Forward");
  analogWrite(A1, 255);
  analogWrite(A2, 0);
  analogWrite(B1, 255);
  analogWrite(B2, 0);
}

void backward() {         //function of backward
  Serial.println("Backward");
  analogWrite(A1, 0);
  analogWrite(A2, 210);
  analogWrite(B1, 0);
  analogWrite(B2, 210);
}

void Stop() {              //function of stop
  Serial.println("Stop");
  digitalWrite(A1, LOW);
  digitalWrite(A2, LOW);
  digitalWrite(B1, LOW);
  digitalWrite(B2, LOW);
}
