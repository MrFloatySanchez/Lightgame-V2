// define MONITOR auskommentieren um den seriellen Monitor auszulassen
// #define MONITOR

#include <Arduino.h>

/* Konstanten und bennante Varriabeln */
enum Alarm_Status
{
  off = 0,
  on = 1,
  silent = 2,
} alarm;

const int pin_Lichtschranke = 18;
const int pin_LED_Lichtschranke = A12;
const int pin_Alarm_Ton = 13;
const int pin_Alarm_Licht = 12;

/* Status-Varriabeln und Globals */
bool lichtschranke_unterbrochen, alarm_on, alarm_last, alarm_silent;
unsigned long time_current, time_last_flash, time_alarm_last,
    time_duration_flash = 60, time_duration_flash2 = time_duration_flash * 2,
    time_duration_alarm = 1600;

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
toogleAlarm ()
{
  switch (alarm)
    {
    case Alarm_Status::off:
      digitalWrite (pin_LED_Lichtschranke, HIGH);
      digitalWrite (pin_Alarm_Ton, HIGH);
      digitalWrite (pin_Alarm_Licht, HIGH);
      break;
    case Alarm_Status::on:
      digitalWrite (pin_Alarm_Ton, LOW);
      digitalWrite (pin_Alarm_Licht, LOW);
      flash ();
      break;
    case Alarm_Status::silent:
      digitalWrite (pin_Alarm_Ton, HIGH);
      digitalWrite (pin_Alarm_Licht, LOW);
      flash ();
      break;
    default:
      break;
    }
}

void
setup ()
{
  pinMode (pin_Lichtschranke, INPUT_PULLUP);
  pinMode (pin_LED_Lichtschranke, OUTPUT);
  pinMode (pin_Alarm_Ton, OUTPUT);
  pinMode (pin_Alarm_Licht, OUTPUT);

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

  // Lichtschranke auswerten und Alarm setzten
  if (lichtschranke_unterbrochen && alarm == Alarm_Status::off)
    {
      time_alarm_last = time_current;
      alarm = Alarm_Status::on;
    } 

  if ((alarm > Alarm_Status::off) && (time_current - time_alarm_last > time_duration_alarm))
    {
      if (lichtschranke_unterbrochen)
        {
          alarm = Alarm_Status::silent;
        }
      else
        {
          alarm = Alarm_Status::off;
          delay(10);
        }
    }

  // Alarm-Vairraiable auswerten und? schalten
  bool status_Alarm_Ton, status_Alarm_Licht;
  switch (alarm)
    {
    case Alarm_Status::off:
      status_Alarm_Ton = HIGH;
      status_Alarm_Licht = HIGH;
      digitalWrite (pin_LED_Lichtschranke, HIGH);
      break;
    case Alarm_Status::on:
      status_Alarm_Ton = LOW;
      status_Alarm_Licht = LOW;
      flash ();
      break;
    case Alarm_Status::silent:
      status_Alarm_Ton = HIGH;
      status_Alarm_Licht = LOW;
      flash ();
      break;
    default:
      break;
    }

  digitalWrite (pin_Alarm_Ton, status_Alarm_Ton);
  digitalWrite (pin_Alarm_Licht, status_Alarm_Licht);

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
