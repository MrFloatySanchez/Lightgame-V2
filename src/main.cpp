#include <Arduino.h>
/*
Bei den Pins identische bauteile nebeneinaner anschließen
*/

const int Pin_Lichtschranke = 0;
const int Pin_LED_Lichtschranke = 7;
bool status_Lichtschranke;
const int PIN_RELAYS[4] = { 4, 5, 6, 7 };

void
setup ()
{
  pinMode (Pin_Lichtschranke, INPUT);
  pinMode (Pin_LED_Lichtschranke, OUTPUT);
  pinMode (LED_BUILTIN, OUTPUT);
  Serial.begin (9600);
}

void
loop ()
{
  status_Lichtschranke = digitalRead (Pin_Lichtschranke);
  Serial.println (status_Lichtschranke);
  if (status_Lichtschranke)
    {
      digitalWrite (LED_BUILTIN, HIGH);
      digitalWrite (Pin_LED_Lichtschranke, LOW);
    }
  else
    {
      digitalWrite (LED_BUILTIN, LOW);
      digitalWrite (Pin_LED_Lichtschranke, HIGH);
    }
}