// define MONITOR auskommentieren um den seriellen Monitor auszulassen
#define MONITOR

#include <Arduino.h>

const int pin_Lichtschranke = 8;
const int pin_LED_Lichtschranke = 4;
bool lichtschranke_unterbrochen, alarm_on;
unsigned long time_current, time_last_flash, time_alarm_last,
    time_duration_flash = 50, time_duration_alarm = 3000;

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

  // Lichtschranke auswerten
  if (lichtschranke_unterbrochen)
    {
      alarm_on = true;
      time_alarm_last = time_current;
    }

  // Wenn der Alarm schon eine Zeit läuft wieder abschalten
  if (alarm_on && (time_current - time_alarm_last > time_duration_alarm))
    {
      alarm_on = false;
      digitalWrite (pin_LED_Lichtschranke, HIGH);
    }

  // Während der alarm läuft die LED blinkne lassen
  if (alarm_on && (time_current - time_last_flash > time_duration_flash))
    {
      time_last_flash = time_current;
      static bool temp = LOW;
      digitalWrite (pin_LED_Lichtschranke, temp);
      temp = !temp;
    }
/*
  if (alarm_on || !(time_current - time_alarm_last > time_duration_alarm))
    {
      flash ();
      ton();
    }
 */
// else?
/*
  if (time_current - time_alarm_last > time_duration_alarm)
    {
      digitalWrite (pin_LED_Lichtschranke, HIGH);
    }
 */
#ifdef MONITOR
  Serial.print ("> ");
  Serial.print (lichtschranke_unterbrochen);
  Serial.print (" | ");
  Serial.print (alarm_on);
  Serial.print (" | ");
  Serial.println ();
#endif
}

void
flash ()
{
  if (time_current - time_last_flash > time_duration_flash)
    {
      time_last_flash = time_current;
      static bool temp = LOW;
      digitalWrite (pin_LED_Lichtschranke, temp);
      temp = !temp;
    }
}