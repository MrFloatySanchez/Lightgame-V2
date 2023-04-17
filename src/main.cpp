// define MONITOR auskommentieren um den seriellen Monitor auszulassen
#define MONITOR

#include <Arduino.h>

const int pin_Lichtschranke = 8;
const int pin_LED_Lichtschranke = 4;
bool lichtschranke_unterbrochen;

unsigned long time_current, time_last_flash, time_duration_flash = 50;

void
setup ()
{
  pinMode (pin_Lichtschranke, INPUT_PULLUP);
  pinMode (pin_LED_Lichtschranke, OUTPUT);

#ifdef MONITOR
  Serial.begin (9600);
#endif
}

void
loop ()
{
  // LVL und Blinkmodi lesen
  // LVL und Blinkmodi schalten
  time_current = millis ();
  lichtschranke_unterbrochen
      = digitalRead (pin_Lichtschranke); // HIGH > unterbrochen

#ifdef MONITOR
  Serial.print ("> ");
  Serial.print (lichtschranke_unterbrochen);
  Serial.print (" | ");
#endif

  if (lichtschranke_unterbrochen)
    { // Alarm
      if (time_current - time_last_flash > time_duration_flash)
        {
          time_last_flash = time_current;
          static bool temp = LOW;
          digitalWrite (pin_LED_Lichtschranke, temp);
          temp = !temp;
        }
    }
  else
    { // keine Unterbrechung
      digitalWrite (pin_LED_Lichtschranke, HIGH);
    }

#ifdef MONITOR
  Serial.println ();
#endif
}