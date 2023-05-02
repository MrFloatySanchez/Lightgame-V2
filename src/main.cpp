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
#define MONITOR

#include <Arduino.h>

/* Konstanten und bennante Varriabeln */

const int gateOneCount = 2;
const int gateTwoCount = 3;
const int gateThreeCount = 5;
const int gateFourCount = 6;

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

int current_level = 0;
int last_level = 0;
int current_blink = 0;

unsigned long currenTime;
unsigned long Alarm_last_time;
unsigned long blink_last_time;

const unsigned long flash_duration = 50;
const unsigned long debounce_time = 50;
const unsigned long AlarmDuration = 3000;
const unsigned long BlinkDuration = 3000;

struct Modul_Taster
{
  const int Signal_PIN{}; // INPUT
  const int Led_PIN{};    // OUTPUT
  const int id{};

  bool currentState{};

  bool reading{};
  bool lastButtonState{};
  bool ButtonState{};
  unsigned long lastTimePressed{};

  Modul_Taster(int pinSignal, int pinLed, int nummer)
      : Signal_PIN(pinSignal), Led_PIN(pinLed), id(nummer)
  {
    currentState = LOW;
  }

  bool
  checkForTastendruck()
  {
    reading = digitalRead(Signal_PIN);

    if (reading != lastButtonState)
    {
      lastTimePressed = currenTime;
    }

    if (currenTime - lastTimePressed > debounce_time)
    {

      if (reading != ButtonState)
      {
        ButtonState = reading;

        if (ButtonState == LOW)
        {
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

  void
  changeState(bool newState = LOW)
  {
    digitalWrite(Led_PIN, newState);
    currentState = newState;
  }
};

Modul_Taster Level_Taster[] = {
    Modul_Taster(47, 46, 0),
    Modul_Taster(45, 44, 1),
    Modul_Taster(49, 48, 2),
};

Modul_Taster Blink_Taster[] = {
    Modul_Taster(43, 42, 0), // Taster gibt es nicht!
    Modul_Taster(53, 52, 1),
    Modul_Taster(51, 50, 2),
};

struct Modul_Lichtschranke
{
  const int Signal_PIN{};
  const int Led_PIN{};

  bool unterbrochen{0};
  bool flash{0};
  bool aktiv{1};

  bool led_state{1};
  unsigned long flash_last_time;

  Modul_Lichtschranke(int pinSignal, int pinLed)
      : Signal_PIN(pinSignal), Led_PIN(pinLed)
  {
    led_state = HIGH;
    digitalWrite(Led_PIN, led_state);
  }

  void
  light()
  {
    if (flash)
    {
      if (currenTime - flash_last_time > flash_duration)
      {
        flash_last_time = currenTime;
        led_state = led_state ? LOW : HIGH;
      }
    }
    else
    {
      led_state = HIGH;
    }
  }

  bool
  checkForUnterbrechung()
  {
    if (aktiv == 1)
      unterbrochen = !digitalRead(Signal_PIN);
    else
      unterbrochen = false;
    return unterbrochen;
  }
};

Modul_Lichtschranke Alle_Lichtschranken[] = {
    Modul_Lichtschranke(21, A15),
    Modul_Lichtschranke(20, A14),
    Modul_Lichtschranke(19, A13),
    Modul_Lichtschranke(18, A12),
};

void alarmSwitch()
{
  switch (global_Alarm)
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

void setLevelZero()
{
  Alle_Lichtschranken[0].aktiv = 1;
  Alle_Lichtschranken[1].aktiv = 1;
  Alle_Lichtschranken[2].aktiv = 1;
  Alle_Lichtschranken[3].aktiv = 1;
  Alle_Lichtschranken[4].aktiv = 1;
  Alle_Lichtschranken[5].aktiv = 1;
  Alle_Lichtschranken[6].aktiv = 1;
  Alle_Lichtschranken[7].aktiv = 1;
  Alle_Lichtschranken[8].aktiv = 1;
  Alle_Lichtschranken[9].aktiv = 1;
  Alle_Lichtschranken[10].aktiv = 1;
  Alle_Lichtschranken[11].aktiv = 1;
  Alle_Lichtschranken[12].aktiv = 1;
  Alle_Lichtschranken[12].aktiv = 1;
  Alle_Lichtschranken[13].aktiv = 1;
  Alle_Lichtschranken[15].aktiv = 1;
  Alle_Lichtschranken[16].aktiv = 1;
}

void setLevelOne()
{
  Alle_Lichtschranken[0].aktiv = 1;
  Alle_Lichtschranken[1].aktiv = 1;
  Alle_Lichtschranken[2].aktiv = 1;
  Alle_Lichtschranken[3].aktiv = 1;
  Alle_Lichtschranken[4].aktiv = 1;
  Alle_Lichtschranken[5].aktiv = 1;
  Alle_Lichtschranken[6].aktiv = 1;
  Alle_Lichtschranken[7].aktiv = 1;
  Alle_Lichtschranken[8].aktiv = 1;
  Alle_Lichtschranken[9].aktiv = 1;
  Alle_Lichtschranken[10].aktiv = 1;
  Alle_Lichtschranken[11].aktiv = 1;
  Alle_Lichtschranken[12].aktiv = 1;
  Alle_Lichtschranken[12].aktiv = 1;
  Alle_Lichtschranken[13].aktiv = 1;
  Alle_Lichtschranken[15].aktiv = 1;
  Alle_Lichtschranken[16].aktiv = 1;
}

void setLevelTwo()
{
  Alle_Lichtschranken[0].aktiv = 1;
  Alle_Lichtschranken[1].aktiv = 1;
  Alle_Lichtschranken[2].aktiv = 1;
  Alle_Lichtschranken[3].aktiv = 1;
  Alle_Lichtschranken[4].aktiv = 1;
  Alle_Lichtschranken[5].aktiv = 1;
  Alle_Lichtschranken[6].aktiv = 1;
  Alle_Lichtschranken[7].aktiv = 1;
  Alle_Lichtschranken[8].aktiv = 1;
  Alle_Lichtschranken[9].aktiv = 1;
  Alle_Lichtschranken[10].aktiv = 1;
  Alle_Lichtschranken[11].aktiv = 1;
  Alle_Lichtschranken[12].aktiv = 1;
  Alle_Lichtschranken[12].aktiv = 1;
  Alle_Lichtschranken[13].aktiv = 1;
  Alle_Lichtschranken[15].aktiv = 1;
  Alle_Lichtschranken[16].aktiv = 1;
}

void levelSwtich()
{
  if (current_level != last_level)
  {
    switch (current_level)
    {
    case 0:
      Level_Taster[0].currentState = HIGH;
      Level_Taster[1].currentState = LOW;
      Level_Taster[2].currentState = LOW;
      setLevelZero();
      break;
    case 1:
      Level_Taster[0].currentState = LOW;
      Level_Taster[1].currentState = HIGH;
      Level_Taster[2].currentState = LOW;
      setLevelTwo();
      break;
    case 2:
      Level_Taster[0].currentState = LOW;
      Level_Taster[1].currentState = LOW;
      Level_Taster[2].currentState = HIGH;
      setLevelZero();
      break;

    default:
      break;
    }
    last_level = current_level;
  }
}

void setBlinkZero()
{
  Alle_Lichtschranken[0].aktiv = 1;
  Alle_Lichtschranken[1].aktiv = 1;
  Alle_Lichtschranken[2].aktiv = 1;
  Alle_Lichtschranken[3].aktiv = 1;
  Alle_Lichtschranken[4].aktiv = 1;
  Alle_Lichtschranken[5].aktiv = 1;
  Alle_Lichtschranken[6].aktiv = 1;
  Alle_Lichtschranken[7].aktiv = 1;
  Alle_Lichtschranken[8].aktiv = 1;
  Alle_Lichtschranken[9].aktiv = 1;
  Alle_Lichtschranken[10].aktiv = 1;
  Alle_Lichtschranken[11].aktiv = 1;
  Alle_Lichtschranken[12].aktiv = 1;
  Alle_Lichtschranken[12].aktiv = 1;
  Alle_Lichtschranken[13].aktiv = 1;
  Alle_Lichtschranken[15].aktiv = 1;
  Alle_Lichtschranken[16].aktiv = 1;
}

void setBlinkOne()
{
  const unsigned long offsetBetweenGates = 1500;
  const int gateTwoIdx = gateOneCount - 1;
  const int gateThreeIdx = gateOneCount + gateTwoCount - 1;
  const int gateFourIdx = gateOneCount + gateTwoCount + gateThreeCount - 1;

  unsigned long deltaTime = currenTime - blink_last_time;

  if (deltaTime > BlinkDuration)
    for (int i = 0; i <= gateTwoIdx; i++)
    {
      Alle_Lichtschranken[i].aktiv = !Alle_Lichtschranken[i].aktiv;
    }

  deltaTime = deltaTime - offsetBetweenGates;
  if (deltaTime > offsetBetweenGates)
    for (int i = gateTwoIdx; i <= gateThreeIdx; i++)
    {
      Alle_Lichtschranken[i].aktiv = !Alle_Lichtschranken[i].aktiv;
    }

  deltaTime = deltaTime - offsetBetweenGates;
  if (deltaTime > offsetBetweenGates)
    for (int i = gateThreeIdx; i <= gateFourIdx; i++)
    {
      Alle_Lichtschranken[i].aktiv = !Alle_Lichtschranken[i].aktiv;
    }

  deltaTime = deltaTime - offsetBetweenGates;
  if (deltaTime > offsetBetweenGates)
    for (int i = gateThreeIdx; i <= gateFourIdx; i++)
    {
      Alle_Lichtschranken[i].aktiv = !Alle_Lichtschranken[i].aktiv;
    }

  blink_last_time = currenTime;
}

void setBlinkTwo()
{
  // jede Zweite lichtschranke 'an'
}

void setBlinkAll()
{
  if (currenTime - blink_last_time > BlinkDuration)
    for (auto &lichtschranke : Alle_Lichtschranken)
    {
      lichtschranke.aktiv = !lichtschranke.aktiv
    }
  blink_last_time = currenTime;
}

void setup()
{
#ifdef MONITOR
  Serial.begin(9600);
#endif
  for (Modul_Taster &Level : Level_Taster)
  {
    pinMode(Level.Signal_PIN, INPUT_PULLUP);
    pinMode(Level.Led_PIN, OUTPUT);
  }

  for (Modul_Taster &Blink : Blink_Taster)
  {
    pinMode(Blink.Signal_PIN, INPUT_PULLUP);
    pinMode(Blink.Led_PIN, OUTPUT);
  }

  for (Modul_Lichtschranke &lichtschranke : Alle_Lichtschranken)
  {
    pinMode(lichtschranke.Signal_PIN, INPUT_PULLUP);
    pinMode(lichtschranke.Led_PIN, OUTPUT);
  }

  pinMode(Alarm_Licht_Pin, OUTPUT);
  pinMode(Alarm_Ton_Pin, OUTPUT);
}

void loop()
{
  currenTime = millis();

  for (Modul_Taster &Level : Level_Taster)
  {
    if (Level.checkForTastendruck())
    {
      current_level = Level.id;
    }
#ifdef MONITOR
    Serial.print(Level.Signal_PIN);
    Serial.print(" > ");
    Serial.print(Level.currentState);
    Serial.print(" | ");
#endif
  }

  for (Modul_Taster &Blink : Blink_Taster)
  {
    if (Blink.checkForTastendruck())
    {
      current_blink = Blink.id;
#ifdef MONITOR
      Serial.print(Blink.Signal_PIN);
      Serial.print(" > ");
      Serial.print(Blink.currentState);
      Serial.print(" | ");
#endif
    }
  }
#ifdef MONITOR
  Serial.println();
#endif

  for (Modul_Lichtschranke &lichtschranke : Alle_Lichtschranken)
  {
    if (lichtschranke.checkForUnterbrechung() == true)
    {
      lichtschranke.flash = 1;
      if (global_Alarm == off)
      {
        Alarm_last_time = currenTime;
        global_Alarm = on;
      }
    }
  }

  if (global_Alarm == on && (currenTime - Alarm_last_time > AlarmDuration))
  {
    global_Alarm = silent;
  }
  else if (global_Alarm == silent)
  {
    global_Alarm = off;
    for (Modul_Lichtschranke &lichtschranke : Alle_Lichtschranken)
    {
      if (lichtschranke.unterbrochen)
      {
        global_Alarm = silent;
        lichtschranke.flash = 1;
      }
      else
      {
        lichtschranke.flash = 0;
      }
    }
  }

  switch (current_blink)
  {
  case 0:
    Blink_Taster[0].currentState = HIGH;
    Blink_Taster[1].currentState = LOW;
    Blink_Taster[2].currentState = LOW;
    // Modus nur setzten und später noch ne switch?
    setBlinkZero();
    break;
  case 1:
    Blink_Taster[0].currentState = LOW;
    Blink_Taster[1].currentState = HIGH;
    Blink_Taster[2].currentState = LOW;
    setBlinkOne();
    break;
  case 2:
    Blink_Taster[0].currentState = LOW;
    Blink_Taster[1].currentState = LOW;
    Blink_Taster[2].currentState = HIGH;
    setBlinkTwo();
    break;
  default:
    current_blink = 0;
    break;
  }

  levelSwtich();
  alarmSwitch();

  for (Modul_Taster &Level : Level_Taster)
  {
    digitalWrite(Level.Led_PIN, Level.currentState);
  }
  /*
    for (Modul_Taster& Blink : Blink_Taster) {
      digitalWrite(Blink.Led_PIN, Blink.currentState);
    }
   */
  for (Modul_Lichtschranke &lichtschranke : Alle_Lichtschranken)
  {
    lichtschranke.light();
    digitalWrite(lichtschranke.Led_PIN, lichtschranke.led_state);
  }

  digitalWrite(Alarm_Licht_Pin, Alarm_Licht);
  digitalWrite(Alarm_Ton_Pin, Alarm_Ton);
}
