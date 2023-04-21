/*
#ifdef MONITOR
Serial.print ();
Serial.print (" > ");
Serial.print ();
Serial.print (" - ");
Serial.print ();
Serial.print (" | ");

Serial.println ();
#endif

#ifdef MONITOR
int test[5] = { 10, 11, 12, 13, 14 };
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
*/

// define MONITOR auskommentieren um den seriellen Monitor auszulassen
#define MONITOR
#define ANALOG_PIN_A15 A15
#define ANALOG_PIN_A14 A14

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
bool Alarm_Ton;
bool Alarm_Licht;
unsigned long time_alarm_last;
unsigned long time_duration_alarm = 2000;
Alarm_Status Globaler_Alarm = Alarm_Status::off;

bool lichtschranke_unterbrochen;
unsigned long time_current;
unsigned long time_last_flash;
unsigned long time_duration_flash = 60;
unsigned long time_duration_flash2 = time_duration_flash * 2;

const int Anzahl_Lichtschranken = 4;
struct modul_Lichtschranke
{
  const int pin_Signal{};
  const int pin_LED{};
  Alarm_Status alarm{};
  bool led{};

  modul_Lichtschranke (int SignalPin, int ledPin)
      : pin_Signal (SignalPin), pin_LED (ledPin)
  {
    alarm = Alarm_Status::off;
    led = HIGH;
  }

  void
  flash ()
  {
    if (time_current - time_last_flash > time_duration_flash)
      {
        time_last_flash = time_current;
        led = !led;
      }
  }

  Alarm_Status
  SensorenLesen ()
  { // Lichtschranke auswerten und Alarm setzten

    lichtschranke_unterbrochen
        = !digitalRead (pin_Signal); // HIGH > unterbrochen

    if (lichtschranke_unterbrochen && alarm == Alarm_Status::off)
      {
        time_alarm_last = time_current;
        alarm = Alarm_Status::on;
      }

    if (alarm == Alarm_Status::on)
      {
        flash ();

        if (time_current - time_alarm_last < time_duration_alarm)
          {
            return alarm;
          }
        else if (lichtschranke_unterbrochen)
          {
            alarm = Alarm_Status::silent;
          }
        else
          {
            led = HIGH;
            alarm = Alarm_Status::off;
          }
      }
    else
      {
        return alarm;
      }
  }

} Alle_Lichtschranken[] = {
  modul_Lichtschranke (8, 4),
  modul_Lichtschranke (9, 5),
};

void
AlarmAuswerten ()
{ // Alarm-Vairraiable auswerten und? schalten
  switch (Globaler_Alarm)
    {
    case Alarm_Status::off:
      Alarm_Ton = HIGH;
      Alarm_Licht = HIGH;
      break;
    case Alarm_Status::on:
      Alarm_Ton = LOW;
      Alarm_Licht = LOW;
      break;
    case Alarm_Status::silent:
      Alarm_Ton = HIGH;
      Alarm_Licht = LOW;
      break;
    default:
      break;
    }
}

void
setup ()
{
#ifdef MONITOR
  Serial.begin (9600);
#endif

  for (modul_Lichtschranke lichtschranke : Alle_Lichtschranken)
    {
      pinMode (lichtschranke.pin_Signal, INPUT_PULLUP);
      pinMode (lichtschranke.pin_LED, OUTPUT);
    }
  pinMode (pin_Alarm_Ton, OUTPUT);
  pinMode (pin_Alarm_Licht, OUTPUT);
}

void
loop ()
{
  time_current = millis ();
  Globaler_Alarm = Alarm_Status::off;

  // LVL und Blinkmodi lesen
  // timevergleiche schon durchrechnen?

  for (modul_Lichtschranke lichtschranke : Alle_Lichtschranken)
    {
      Alarm_Status Alarm_temp = lichtschranke.SensorenLesen ();
      Globaler_Alarm = max (Globaler_Alarm, Alarm_temp);
#ifdef MONITOR
      Serial.print (lichtschranke.pin_Signal);
      Serial.print (" > ");
      Serial.print (lichtschranke.alarm);
      Serial.print (" - ");
      Serial.print (" | ");
#endif
    }
  // LVL und Blinkmodi schalten

  AlarmAuswerten ();

  for (modul_Lichtschranke lichtschranke : Alle_Lichtschranken)
    {
      digitalWrite (lichtschranke.pin_LED, lichtschranke.led);

#ifdef MONITOR
      Serial.print (lichtschranke.pin_Signal);
      Serial.print (" > ");
      Serial.print (lichtschranke.led);
      Serial.print (" - ");
      Serial.print (" | ");
#endif
    }
  digitalWrite (pin_Alarm_Ton, Alarm_Ton);
  digitalWrite (pin_Alarm_Licht, Alarm_Licht);

#ifdef MONITOR
  Serial.print (Globaler_Alarm);
  Serial.print (" - ");
  Serial.println ();
#endif
}
