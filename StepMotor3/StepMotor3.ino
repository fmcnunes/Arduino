const int A1A=9;
const int A1B=10;
const int B1A=6;
const int B1B=5;

int move(int stepNumber, int Maxpower, int wait);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(A1A, OUTPUT);
  pinMode(A1B, OUTPUT);
  pinMode(B1A, OUTPUT);
  pinMode(B1A, OUTPUT);
  }

void loop() {
  int stepNumber=0;
  // put your main code here, to run repeatedly:
  for(int i=0; i < 3199; i++)
  {
    stepNumber++;
    move(stepNumber, 255, 1000000);
  }
  Serial.println("Delay !");
  delay(3000);
  Serial.println("Delay done !");
  /*
  for(int i=0; i < 1599; i--)
  {
    move(stepNumber, 255, 1000);
  }
  delay(3000);*/

}

int move(int stepNumber, int Maxpower, int wait)
{
  int Pa;
  int Pb;

  Pa = (sin(stepNumber*0.098174)*Maxpower);
  Pb = (cos(stepNumber*0.098174)*Maxpower);

  if (Pa > 0)
    {
      analogWrite(A1A, Pa);
      analogWrite(A1B, 0);
    }
    else
    {
      analogWrite(A1A, 0);
      analogWrite(A1B, abs(Pa));
    }

      if (Pb > 0)
    {
      analogWrite(B1A, Pa);
      analogWrite(B1B, 0);
    }
    else
    {
      analogWrite(B1A, 0);
      analogWrite(B1B, abs(Pa));
    }
    delayMicroseconds(wait);
}
