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

const int gateOneCount = 2;
const int gateTwoCount = 3;
const int gateThreeCount = 5;
const int gateFourCount = 6;
const int Anzahl_Tore = 4;
const int ersteTor = 0;
const int zweitesTor = 1;
const int drittesTor = 3;
const int viertesTor = 4;
const unsigned long offsetBetweenGates = 1500;

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
int last_blink;

unsigned long currenTime;
unsigned long Alarm_last_time;
unsigned long blink_last_time[Anzahl_Tore];

const unsigned long flash_duration = 50;
const unsigned long debounce_time = 10;
const unsigned long AlarmDuration = 3000;
const unsigned long BlinkDuration = 3000;

struct Modul_Taster
{
  const int Signal_PIN{};
  const int Led_PIN{};
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

/*
47 > Blau
45 > Grün
51 > Rot
53 > Gelb
49 ? Weis
 */

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
  bool blink{};
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
    /*     else if (blink)
        {
        } */
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

struct Modul_Trigger
{
  const int Signal{};

  unsigned long last_trigger;
  const unsigned long triggerDelay = 2000;

  Modul_Trigger(int Pin) : Signal(Pin) {}

  bool
  lesen()
  {
    bool reading = digitalRead(Signal);

    if (currenTime - last_trigger > triggerDelay)
      return reading;
    else
      return false;
  }
};

Modul_Trigger trigger = 22;

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
  /*  Alle_Lichtschranken[4].aktiv = 1;
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
    Alle_Lichtschranken[16].aktiv = 1; */
}

void setLevelOne()
{
  Alle_Lichtschranken[0].aktiv = 1;
  Alle_Lichtschranken[1].aktiv = 1;
  Alle_Lichtschranken[2].aktiv = 1;
  Alle_Lichtschranken[3].aktiv = 1;
  /*    Alle_Lichtschranken[4].aktiv = 1;
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
     Alle_Lichtschranken[16].aktiv = 1; */
}

void setLevelTwo()
{
  Alle_Lichtschranken[0].aktiv = 1;
  Alle_Lichtschranken[1].aktiv = 1;
  Alle_Lichtschranken[2].aktiv = 1;
  Alle_Lichtschranken[3].aktiv = 1;
  /*    Alle_Lichtschranken[4].aktiv = 1;
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
     Alle_Lichtschranken[16].aktiv = 1; */
}

void levelSwtich(bool force_setLevel = false)
{
  if ((current_level != last_level) || force_setLevel)
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
      current_level = 0;
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
  /*   Alle_Lichtschranken[4].aktiv = 1;
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
    Alle_Lichtschranken[16].aktiv = 1; */
}

void setBlinkOne()
{
  // Tor 1
  if (currenTime - blink_last_time[ersteTor] > BlinkDuration)
  {
    Alle_Lichtschranken[0].aktiv = !Alle_Lichtschranken[0].aktiv;
    Alle_Lichtschranken[1].aktiv = !Alle_Lichtschranken[1].aktiv;
    Alle_Lichtschranken[2].aktiv = !Alle_Lichtschranken[2].aktiv;
    blink_last_time[ersteTor] = currenTime;
  }

  // Tor 2
  if (currenTime - blink_last_time[zweitesTor] > BlinkDuration)
  {
    Alle_Lichtschranken[3].aktiv = !Alle_Lichtschranken[3].aktiv;
    // Alle_Lichtschranken[4].aktiv = !Alle_Lichtschranken[4].aktiv;
    // Alle_Lichtschranken[5].aktiv = !Alle_Lichtschranken[5].aktiv;
    // Alle_Lichtschranken[6].aktiv = !Alle_Lichtschranken[6].aktiv;
    blink_last_time[zweitesTor] = currenTime;
  }

  // Tor 3
  if (currenTime - blink_last_time[drittesTor] > BlinkDuration)
  {
    // Alle_Lichtschranken[7].aktiv = !Alle_Lichtschranken[7].aktiv;
    // Alle_Lichtschranken[8].aktiv = !Alle_Lichtschranken[8].aktiv;
    // Alle_Lichtschranken[9].aktiv = !Alle_Lichtschranken[9].aktiv;
    // Alle_Lichtschranken[10].aktiv = !Alle_Lichtschranken[10].aktiv;
    // Alle_Lichtschranken[11].aktiv = !Alle_Lichtschranken[11].aktiv;
    blink_last_time[drittesTor] = currenTime;
  }

  // Tor 4
  if (currenTime - blink_last_time[viertesTor] > BlinkDuration)
  {
    // Alle_Lichtschranken[12].aktiv = !Alle_Lichtschranken[12].aktiv;
    // Alle_Lichtschranken[13].aktiv = !Alle_Lichtschranken[13].aktiv;
    // Alle_Lichtschranken[14].aktiv = !Alle_Lichtschranken[14].aktiv;
    // Alle_Lichtschranken[15].aktiv = !Alle_Lichtschranken[15].aktiv;
    // Alle_Lichtschranken[16].aktiv = !Alle_Lichtschranken[16].aktiv;
    blink_last_time[viertesTor] = currenTime;
  }
}

void setBlinkTwo()
{
  // Tor 1
  if (currenTime - blink_last_time[ersteTor] > BlinkDuration)
  {
    static bool newState = false;
    Alle_Lichtschranken[0].aktiv = newState;
    Alle_Lichtschranken[1].aktiv = !newState;
    Alle_Lichtschranken[2].aktiv = newState;
    newState = !newState;
    blink_last_time[ersteTor] = currenTime;
  }
  // Tor 2/*
  if (currenTime - blink_last_time[zweitesTor] > BlinkDuration)
  {
    static bool newState = false;
    Alle_Lichtschranken[3].aktiv = !newState;
    // Alle_Lichtschranken[4].aktiv = newState;
    // Alle_Lichtschranken[5].aktiv = !newState;
    // Alle_Lichtschranken[6].aktiv = newState;
    newState = !newState;
    blink_last_time[zweitesTor] = currenTime;
  }
  // Tor 3
  if (currenTime - blink_last_time[drittesTor] > BlinkDuration)
  {
    static bool newState = false;
    // Alle_Lichtschranken[7].aktiv = !newState;
    // Alle_Lichtschranken[8].aktiv = newState;
    // Alle_Lichtschranken[9].aktiv = !newState;
    // Alle_Lichtschranken[10].aktiv = newState;
    // Alle_Lichtschranken[11].aktiv = !newState;
    newState = !newState;
    blink_last_time[drittesTor] = currenTime;
  }
  // Tor 4
  if (currenTime - blink_last_time[viertesTor] > BlinkDuration)
  {
    static bool newState = false;
    // Alle_Lichtschranken[12].aktiv = newState;
    // Alle_Lichtschranken[13].aktiv = !newState;
    // Alle_Lichtschranken[14].aktiv = newState;
    // Alle_Lichtschranken[15].aktiv = !newState;
    // Alle_Lichtschranken[16].aktiv = newState;
    newState = !newState;
    blink_last_time[viertesTor] = currenTime;
  }
}

void blinkSwitch()
{
  if (current_blink != last_blink)
  {
    switch (current_blink)
    {
    case 0:
      Blink_Taster[0].currentState = HIGH;
      Blink_Taster[1].currentState = LOW;
      Blink_Taster[2].currentState = LOW;
      // levelSwtich(true);
      last_level = 3; // garantiert einen setLevel
      break;
    case 1:
      Blink_Taster[0].currentState = LOW;
      Blink_Taster[1].currentState = HIGH;
      Blink_Taster[2].currentState = LOW;
      break;
    case 2:
      Blink_Taster[0].currentState = LOW;
      Blink_Taster[1].currentState = LOW;
      Blink_Taster[2].currentState = HIGH;
      break;
    default:
      current_blink = 0;
      break;
    }
    blink_last_time[ersteTor] = currenTime;
    blink_last_time[zweitesTor] = currenTime + (1 * offsetBetweenGates);
    blink_last_time[drittesTor] = currenTime + (2 * offsetBetweenGates);
    blink_last_time[viertesTor] = currenTime + (3 * offsetBetweenGates);
  } // if

  switch (current_blink)
  {
  case 0:
    break;
  case 1:
    setBlinkOne();
    break;
  case 2:
    setBlinkTwo();
    break;
  default:
    current_blink = 0;
    break;
  }
  last_blink = current_blink;
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
  /* Lesen */
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
    Serial.print(" - ");
    Serial.print(Level.reading);
    Serial.print(" | ");
#endif
  }

  for (Modul_Taster &Blink : Blink_Taster)
  {
    if (Blink.checkForTastendruck())
    {
      current_blink = Blink.id;
    }
#ifdef MONITOR
    Serial.print(Blink.Signal_PIN);
    Serial.print(" > ");
    Serial.print(Blink.currentState);
    Serial.print(" - ");
    Serial.print(Blink.reading);
    Serial.print(" | ");
#endif
  }
#ifdef MONITOR
  Serial.print(current_level);
  Serial.print(" | ");
  Serial.print(current_blink);
  Serial.print(" | ");
  Serial.println();
#endif
  for (Modul_Lichtschranke &lichtschranke : Alle_Lichtschranken)
  {
    if (lichtschranke.checkForUnterbrechung())
    {
      lichtschranke.flash = 1;
      if (global_Alarm == off)
      {
        Alarm_last_time = currenTime;
        global_Alarm = on;
      }
    }
  }

  /* Auswerten */

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

  blinkSwitch();
  levelSwtich(true);
  alarmSwitch();

  /* Schalten */

  for (Modul_Taster &Level : Level_Taster)
  {
    digitalWrite(Level.Led_PIN, Level.currentState);
  }
  for (Modul_Taster &Blink : Blink_Taster)
  {
    digitalWrite(Blink.Led_PIN, Blink.currentState);
  }
  for (Modul_Lichtschranke &lichtschranke : Alle_Lichtschranken)
  {
    lichtschranke.light();
    digitalWrite(lichtschranke.Led_PIN, lichtschranke.led_state);
  }
  digitalWrite(Alarm_Licht_Pin, Alarm_Licht);
  digitalWrite(Alarm_Ton_Pin, Alarm_Ton);
}
