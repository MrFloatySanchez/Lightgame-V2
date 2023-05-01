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
/*
  ToDo
  - Exclusiv-Logik Taster
    - Via ID Kontatante?
 */

// define MONITOR auskommentieren um den seriellen Monitor auszulassen
// #define MONITOR

#include <Arduino.h>

/* Konstanten und bennante Varriabeln */
enum Alarm_Status
{
  off = 0,
  on = 1,
  silent = 2,
};
Alarm_Status global_Alarm = Alarm_Status::off;

const int Alarm_Licht_Pin = 12;
const int Alarm_Ton_Pin = 13;

bool Alarm_Ton = 0;
bool Alarm_Licht = 0;
bool Taster_Level_Changed = 0;
bool Taster_Blink_Changed = 0;

unsigned long currenTime;
unsigned long Alarm_last_time;
const unsigned long flash_duration = 50;
const unsigned long debounce_time = 50;
const unsigned long AlarmDuration = 3000;

struct Modul_Taster
{
  const int Signal_PIN{}; // INPUT
  const int Led_PIN{};    // OUTPUT

  bool currentState{};
  bool reading{};
  bool lastButtonState{};
  bool ButtonState{};
  unsigned long lastTimePressed{};

  Modul_Taster(int pinSignal, int pinLed)
    : Signal_PIN(pinSignal)
    , Led_PIN(pinLed)
  {
    currentState = LOW;
  }

  bool checkForTastendruck()
  {
    reading = digitalRead(Signal_PIN);

    if (reading != lastButtonState) {
      lastTimePressed = currenTime;
    }

    if (currenTime - lastTimePressed > debounce_time) {

      if (reading != ButtonState) {
        ButtonState = reading;

        if (ButtonState == LOW) {
          currentState = !currentState;
          return true;
          // Taster_Changed = 1;
        }
      }
    }

    digitalWrite(Led_PIN, currentState);
    lastButtonState = reading;
    return false;
  }

  void changeState(bool newState = LOW)
  {
    digitalWrite(Led_PIN, newState);
    currentState = newState;
  }
};

Modul_Taster Level_Taster[] = {
  Modul_Taster(53, 52),
  Modul_Taster(51, 50),
  Modul_Taster(49, 48),
};
Modul_Taster Blink_Taster[] = {
  Modul_Taster(47, 46),
  Modul_Taster(45, 44),
};

struct Modul_Lichtschranke
{
  const int Signal_PIN{};
  const int Led_PIN{};

  bool unterbrochen{ 0 };
  bool led_state{ 1 };
  bool flash{ 0 };
  unsigned long flash_last_time;

  Modul_Lichtschranke(int pinSignal, int pinLed)
    : Signal_PIN(pinSignal)
    , Led_PIN(pinLed)
  {
    led_state = HIGH;
    digitalWrite(Led_PIN, led_state);
  }

  void light()
  {
    if (flash) {
      if (currenTime - flash_last_time > flash_duration) {
        flash_last_time = currenTime;
        led_state = led_state ? LOW : HIGH;
      }
    } else {
      led_state = HIGH;
    }
  }

  bool checkForUnterbrechung()
  {
    return unterbrochen = !digitalRead(Signal_PIN);
  }
};

Modul_Lichtschranke Alle_Lichtschranken[] = {
  Modul_Lichtschranke(21, A15),
  Modul_Lichtschranke(20, A14),
  Modul_Lichtschranke(19, A13),
  Modul_Lichtschranke(18, A12),
};

void
alarmSwitch()
{
  switch (global_Alarm) {
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
setup()
{
#ifdef MONITOR
  Serial.begin(9600);
#endif
  for (Modul_Taster& Level : Level_Taster) {
    pinMode(Level.Signal_PIN, INPUT_PULLUP);
    pinMode(Level.Led_PIN, OUTPUT);
  }
  for (Modul_Taster& Blink : Blink_Taster) {
    pinMode(Blink.Signal_PIN, INPUT_PULLUP);
    pinMode(Blink.Led_PIN, OUTPUT);
  }
  for (Modul_Lichtschranke& lichtschranke : Alle_Lichtschranken) {
    pinMode(lichtschranke.Signal_PIN, INPUT_PULLUP);
    pinMode(lichtschranke.Led_PIN, OUTPUT);
  }

  pinMode(Alarm_Licht_Pin, OUTPUT);
  pinMode(Alarm_Ton_Pin, OUTPUT);
}

void
loop()
{
  currenTime = millis();

  for (Modul_Taster& Level : Level_Taster) {
    if (Level.checkForTastendruck()) {
      Level_Taster[0].currentState = false;
      Level_Taster[1].currentState = false;
      Level_Taster[2].currentState = false;
      Level.changeState(true);
    }
  }

  for (Modul_Taster& Blink : Blink_Taster) {
    if (Blink.checkForTastendruck()) {
      Blink_Taster[0].currentState = false;
      Blink_Taster[1].currentState = false;
      Blink_Taster[2].currentState = false;
      Blink.changeState(HIGH);
    }
  }

  for (Modul_Lichtschranke& lichtschranke : Alle_Lichtschranken) {
    if (lichtschranke.checkForUnterbrechung() == true) {
      lichtschranke.flash = 1;
      if (global_Alarm == off) {
        Alarm_last_time = currenTime;
        global_Alarm = on;
      }
    }
  }

  if (global_Alarm == on && (currenTime - Alarm_last_time > AlarmDuration)) {
    global_Alarm = silent;
  } else if (global_Alarm == silent) {
    global_Alarm = off;
    for (Modul_Lichtschranke& lichtschranke : Alle_Lichtschranken) {
      if (lichtschranke.unterbrochen) {
        global_Alarm = silent;
        lichtschranke.flash = 1;
      } else {
        lichtschranke.flash = 0;
      }
    }
  }

  alarmSwitch();

  for (Modul_Taster& Level : Level_Taster) {
    digitalWrite(Level.Led_PIN, Level.currentState);
  }

  for (Modul_Taster& Blink : Blink_Taster) {
    digitalWrite(Blink.Led_PIN, Blink.currentState);
  }

  for (Modul_Lichtschranke& lichtschranke : Alle_Lichtschranken) {
    lichtschranke.light();
    digitalWrite(lichtschranke.Led_PIN, lichtschranke.led_state);
  }

  digitalWrite(Alarm_Licht_Pin, Alarm_Licht);
  digitalWrite(Alarm_Ton_Pin, Alarm_Ton);

#ifdef MONITOR
  Serial.print(global_Alarm);
  Serial.print(" > ");
  Serial.print();
  Serial.print(" - ");
  Serial.print();
  Serial.print(" | ");
#endif

#ifdef MONITOR
  Serial.println();
#endif
}
