#include <Arduino.h>
/*
Bei den Pins identische bauteile nebeneinaner anschließen
*/

const int Lichtschranken_Anzahl = 4;
const int Lichtschranke_Pin[Lichtschranken_Anzahl] = { 4, 5, 6, 7 };
const int Lichtschranke_LED_Pin[Lichtschranken_Anzahl] = { 8, 9, 10, 11 };
bool Lichtschranke_Status[Lichtschranken_Anzahl];

void
setup ()
{
  for (int idx = 0; idx < Lichtschranken_Anzahl; idx++)
    {
      pinMode (Lichtschranke_Pin[idx], INPUT_PULLUP);
      pinMode (Lichtschranke_LED_Pin[idx], OUTPUT);
    }
  // pinMode (0, INPUT_PULLUP);
  // pinMode (4, OUTPUT);
  pinMode (LED_BUILTIN, OUTPUT);
  Serial.begin (9600);
}

void
loop ()
{
  for (int idx = 0; idx < Lichtschranken_Anzahl; idx++)
    {
      Lichtschranke_Status[idx] = digitalRead (Lichtschranke_Pin[idx]);

      Serial.print (idx);
      Serial.print (">");
      Serial.print (Lichtschranke_Status[idx]);
      Serial.print (" | ");

      if (Lichtschranke_Status[idx])
        {
          digitalWrite (Lichtschranke_LED_Pin[idx], HIGH);
        }
      else
        {
          digitalWrite (Lichtschranke_LED_Pin[idx], LOW);
        }
    }
  Serial.println ();
}