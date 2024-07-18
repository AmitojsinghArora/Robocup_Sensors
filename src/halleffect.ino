#include <Arduino.h>


int Analog_Eingang = A6; // Analog-Pin
int Digital_Eingang = 7; // Digital-Pin

void setup ()
{
 pinMode (Analog_Eingang, INPUT);
 pinMode (Digital_Eingang, INPUT);

 Serial.begin (9600); // Serial output mit 9600 bps
}

void loop ()
{
 float Analog;
 int Digital;

 //Current values are read, converted to the voltage value...
 Analog = analogRead (Analog_Eingang);
 Digital = digitalRead (Digital_Eingang);
 
//Serial.println(Analog, 4);
if (Analog > 528 && Digital == 0)
  {
    Serial.println("North");
  }
else if (Analog < 521 || Digital == 1)
  {
    Serial.println("South");
  }
delay(500);
}