#include <Arduino.h>

const int pin_Lichtschranke = 8;
const int pin_LED_Lichtschranke = 4;
bool read_Lichtschranke;

void
setup ()
{
  pinMode (pin_Lichtschranke, INPUT_PULLUP);
  pinMode (pin_LED_Lichtschranke, OUTPUT);

  pinMode (LED_BUILTIN, OUTPUT);
  Serial.begin (9600);
}

void
loop ()
{

      read_Lichtschranke = digitalRead (pin_Lichtschranke);

      Serial.print (">");
      Serial.print (read_Lichtschranke);
      Serial.print (" | ");

      if (read_Lichtschranke)
        {
          digitalWrite (pin_LED_Lichtschranke, LOW);
          delay (3000);
        }
      else
        {
          digitalWrite (pin_LED_Lichtschranke, HIGH);
        }

  Serial.println ();
}