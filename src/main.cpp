// define MONITOR auskommentieren um den seriellen Monitor auszulassen
#define MONITOR

#include <Arduino.h>

/* Konstanten und bennante Varriabeln */
enum Alarm_Status
{
  off = 0,
  on = 1,
  silent = 2,
};

const int pin_Alarm_Ton = 13;
const int pin_Alarm_Licht = 12;

int test[5] = { 10, 11, 12, 13, 14 };

const int pin_Lichtschranke_Signal = 21;
const int pin_Lichtschranke_LED = A15;
Alarm_Status alarm = off;
bool led = 1;

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
      led = !led;
    }
}

void
setup ()
{
  pinMode (pin_Lichtschranke_Signal, INPUT_PULLUP);
  pinMode (pin_Lichtschranke_LED, OUTPUT);
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
#ifdef MONITOR
  int i = 0;
  for (auto testNumber : test)
    {
      Serial.print (i);
      Serial.print (" > ");
      Serial.print (testNumber);
      Serial.print (" | ");
      ++i;
    }
  Serial.println ();
#endif

  time_current = millis ();
  lichtschranke_unterbrochen
      = digitalRead (pin_Lichtschranke_Signal); // HIGH > unterbrochen

  // Lichtschranke auswerten und Alarm setzten
  if (lichtschranke_unterbrochen && alarm == Alarm_Status::off)
    {
      time_alarm_last = time_current;
      alarm = Alarm_Status::on;
    }

  if ((time_current - time_alarm_last
       < time_duration_alarm)) // alarm > Alarm_Status::off
    {
      flash ();
    }
  else if (lichtschranke_unterbrochen)
    {
      alarm = Alarm_Status::silent;
      flash ();
    }
  else
    {
      alarm = Alarm_Status::off;
      led = HIGH;
      delay (10);
    }

  // Alarm-Vairraiable auswerten und? schalten
  bool status_Alarm_Ton, status_Alarm_Licht;
  switch (alarm)
    {
    case Alarm_Status::off:
      status_Alarm_Ton = HIGH;
      status_Alarm_Licht = HIGH;
      led = HIGH;
      break;
    case Alarm_Status::on:
      status_Alarm_Ton = LOW;
      status_Alarm_Licht = LOW;
      break;
    case Alarm_Status::silent:
      status_Alarm_Ton = HIGH;
      status_Alarm_Licht = LOW;
      break;
    default:
      break;
    }

  digitalWrite (pin_Lichtschranke_LED, led);
  digitalWrite (pin_Alarm_Ton, status_Alarm_Ton);
  digitalWrite (pin_Alarm_Licht, status_Alarm_Licht);

  /* #ifdef MONITOR
    Serial.print ("> ");
    Serial.print (lichtschranke_unterbrochen);
    Serial.print (" | ");
    Serial.print (alarm);
    Serial.print (" | ");
    Serial.print (time_alarm_last);
    Serial.print (" | ");
    Serial.println ();
  #endif */
}
