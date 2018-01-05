#include "math.h"
/*
  TekTips2 - program to highlight Arduino wiring to a Teknic ClearPath integrated servo motor
  Author is Brendan Flosenzier, 1/2/2018    
  Program uses a "MCVC" (Motion Control Velocity Control) ClearPath servo motor 
  Operating Mode is the "Manual Velocity Control" 
  - View operational mode video for further information on this specific Operational Mode:
 https://www.youtube.com/watch?v=tR_LrL7UjdA
      
  TekTips Video: Wiring a ClearPath motor to an Arduino micro controller
  www.youtube.com/blahblahblah
*/

//Wire colors refer to a standard Teknic ClearPath controller cable 
const int Enable = 6; // ClearPath ~enable input; +enable = BLU wire; -enable = ORN wire
const int InputA = 8; // ClearPath Input A; +InputA = WHT wire; -InputA is BRN wire
const int InputB = 9; // ClearPath Input B; +InputB = BLK wire; -InputB = YEL wire
const int HLFB = 4; // ClearPath HLFB Output; +HLFB = GRN wire; -HLFB = RED wire

void RampToVelocity(int finalVelocity, double RampTime=0);
void DoSinusoid(int Amplitude,double Period);

int CurrentVelocity=0;
int VelocityResolution=1;

int DwellTime = 500; // desired time (ms) for ramp and constant velocity

// ============================================================================================
// put your setup code here, to run once:
void setup() 
{
  //Define I/O pins
  pinMode(Enable, OUTPUT);
  pinMode(InputA, OUTPUT);
  pinMode(InputB, OUTPUT);
  //pinMode(HLFB, INPUT_PULLUP);
  pinMode(HLFB, INPUT);

  //STart Serialoutput for dubugging
  Serial.begin(9600);
  
  // start off by ensuring thet the motor is disabled before proceeding
  digitalWrite(Enable, LOW);
  delay(DwellTime);

  // set the two outputs to their initial states
  digitalWrite(InputA, LOW);
  digitalWrite(InputB, LOW);
  
  // Enable motor to start sequence
  digitalWrite(Enable, HIGH);
  delay(DwellTime);
}
// end of setup code

// put your main code here, to loops indefinitely:
void loop()
{
  RampToVelocity(1000,2000);  //Ramp to 1000 RPM in 2 sec
  delay(DwellTime);
   RampToVelocity(0,1000);    //Ramp to 0 RPM in 1 sec
  delay(DwellTime);
   RampToVelocity(-500,100);  //Ramp to -500 RPM in .1 sec
   delay(DwellTime);
  DoSinusoid(100,1000);       //Do a sinusoidal profile with an amplitude of 500 RPM in 1 sec
  delay(DwellTime);
   RampToVelocity(1000);      //Ramp to 1000 RPM as fast as possible
  delay(DwellTime);
   RampToVelocity(0);         //Ramp to 0 RPM as fast as possible
  delay(DwellTime);
 DoSinusoid(100,1000);       //Do a sinusoidal profile with an amplitude of 1000 RPM in 1 sec
   delay(DwellTime);
}
// end of main code, will loop indefinitely:


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//Implementation of RampToVelocity function RamptTime defaults to 0 if not specified
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void RampToVelocity(int finalVelocity, double RampTime)
{
  //Calculate number of quadrature cycles needed to reach new target speed
  double Increments = abs(finalVelocity-CurrentVelocity)/VelocityResolution;
  //Calculate ramp time in microseconds to match the desired ramp time
  double RampDelay = RampTime*1000/Increments;
  for(int i=0; i<Increments;i++)
  {
    //Check which direction the quadrature signal must go in
    if(finalVelocity-CurrentVelocity>0)
  {
    digitalWrite(InputA, HIGH);
    digitalWrite(InputB, HIGH);
    digitalWrite(InputA, LOW);
    digitalWrite(InputB, LOW);
    CurrentVelocity=CurrentVelocity+VelocityResolution;
  }
    else
    {
    digitalWrite(InputB, HIGH);
    digitalWrite(InputA, HIGH);
    digitalWrite(InputB, LOW);
    digitalWrite(InputA, LOW);
    CurrentVelocity=CurrentVelocity-VelocityResolution;
    }

  //If the required delay is very short use delayMicroseconds, other wise use delay (ms)
  if(RampDelay<1000)
      delayMicroseconds(RampDelay);
     else
      delay(RampDelay/1000);
  }     
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//Implementation of DoSinusoid function 
/////////////////////////////////////////////////////////////////////////////////////////////////////////           
void DoSinusoid(int Amplitude,double Period)
{
   //Calculate number of quadrature cycles needed to reach the full amplitude (1/4 sin wave)
  int Increments=Amplitude/VelocityResolution;
  double delaytime;  //(ms)
  //Execute the first 1/4 of the sin ramping up to full amplitude
  for(double i=1;i<=Increments;i++)
  {
      digitalWrite(InputB, HIGH);
      digitalWrite(InputA, HIGH);
      digitalWrite(InputB, LOW);
      digitalWrite(InputA, LOW);
     delaytime=Period*(asin(i/Increments)-asin((i-1)/Increments))/(2*PI);
     if(delaytime<10)
      delayMicroseconds(delaytime*1000);
     else
      delay(delaytime);
  }
  
  //Execute the 2nd 1/4 of the sin ramping back down to the original speed
  for(double i=Increments;i>0;i--)
  {
      digitalWrite(InputA, HIGH);
      digitalWrite(InputB, HIGH);
      digitalWrite(InputA, LOW);
      digitalWrite(InputB, LOW);
     delaytime=Period*(asin(i/Increments)-asin((i-1)/Increments))/(2*PI);
     if(delaytime<10)
      delayMicroseconds(delaytime*1000);
     else
      delay(delaytime);
  }
  //Execute the third 1/4 of the sin ramping up to full amplitude in opposite direction
  for(double i=1;i<=Increments;i++)
  {
      digitalWrite(InputA, HIGH);
      digitalWrite(InputB, HIGH);
      digitalWrite(InputA, LOW);
      digitalWrite(InputB, LOW);
     delaytime=Period*(asin(i/Increments)-asin((i-1)/Increments))/(2*PI);
     if(delaytime<10)
      delayMicroseconds(delaytime*1000);
     else
      delay(delaytime);
  }
  //Execute the last 1/4 of the sin ramping back down to the original speed
  for(double i=Increments;i>0;i--)
  {
      digitalWrite(InputB, HIGH);
      digitalWrite(InputA, HIGH);
      digitalWrite(InputB, LOW);
      digitalWrite(InputA, LOW);
     delaytime=Period*(asin(i/Increments)-asin((i-1)/Increments))/(2*PI);
     if(delaytime<10)
      delayMicroseconds(delaytime*1000);
     else
      delay(delaytime);
  }
}



