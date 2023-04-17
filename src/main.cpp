// define MONITOR auskommentieren um den seriellen Monitor auszulassen
// #define MONITOR

#include <Arduino.h>

enum Alarm_Status
{
  off = 0,
  on = 1,
  silent = 2,
} alarm;

const int pin_Lichtschranke = 8;
const int pin_LED_Lichtschranke = 4;
bool lichtschranke_unterbrochen, alarm_on, alarm_last, alarm_silent;
unsigned long time_current, time_last_flash, time_alarm_last,
    time_duration_flash = 80, time_duration_flash2 = time_duration_flash * 2,
    time_duration_alarm = 4000;

void
alarmTon ()
{
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
  // timevergleiche schon durchrechnen?
  time_current = millis ();
  lichtschranke_unterbrochen
      = digitalRead (pin_Lichtschranke); // HIGH > unterbrochen

  // Lichtschranke + Alarmstatus auswerten
  if (lichtschranke_unterbrochen)
    {
      if (alarm == Alarm_Status::off)
        {
          time_alarm_last = time_current;
        }
      alarm = Alarm_Status::on;
    }

  // wenn der Alarm schon eine Zeit läuft Ton abschalten, oder ganz ausschalten 
  if ((time_current - time_alarm_last > time_duration_alarm))
    {
      if (alarm == Alarm_Status::on)
        {
          alarm = Alarm_Status::silent;
        }
      else
        {
          alarm = Alarm_Status::off;
        }
    }

  switch (alarm)
    {
    case Alarm_Status::off:
      digitalWrite (pin_LED_Lichtschranke, HIGH);
      // Pin Alarm abschalten
      break;
    case Alarm_Status::on:
      alarmTon ();
      if (time_current - time_last_flash > time_duration_flash)
        {
          time_last_flash = time_current;
          static bool temp = LOW;
          digitalWrite (pin_LED_Lichtschranke, temp);
          temp = !temp;
        }
      break;
    case Alarm_Status::silent:

      if (time_current - time_last_flash > time_duration_flash2)
        {
          time_last_flash = time_current;
          static bool temp = LOW;
          digitalWrite (pin_LED_Lichtschranke, temp);
          temp = !temp;
        }

      break;
    default:
      break;
    }

#ifdef MONITOR
  Serial.print ("> ");
  Serial.print (lichtschranke_unterbrochen);
  Serial.print (" | ");
  Serial.print (alarm);
  Serial.print (" | ");
  Serial.print (time_alarm_last);
  Serial.print (" | ");
  Serial.println ();
#endif
}
