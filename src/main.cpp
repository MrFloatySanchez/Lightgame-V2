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

bool alarm_ton = 0;
bool alarm_licht = 0;
bool level_taster_changed = 0;
bool blink_taster_changed = 0;

int current_level = 0;
int last_level = 0;
int current_blink = 0;
int last_blink;

unsigned long currentTime;
unsigned long alarm_last_time;
unsigned long blink_last_time[Anzahl_Tore];

const unsigned long flash_duration = 50;
const unsigned long debounce_duration = 10;
const unsigned long alarm_duration = 3000;
const unsigned long blink_duration = 3000;

struct Modul_Taster
{
  const int Signal_PIN{};
  const int Led_PIN{};
  const int id{};

  bool led_state{};
  bool button_state{};
  bool new_state{};
  bool last_state{};
  unsigned long lastTimePressed{};

  Modul_Taster(int pinSignal, int pinLed, int nummer)
      : Signal_PIN(pinSignal), Led_PIN(pinLed), id(nummer)
  {
  }

  bool
  checkForTastendruck()
  {
    new_state = digitalRead(Signal_PIN);

    if (new_state != last_state)
    {
      lastTimePressed = currentTime;
    }
    last_state = new_state;

    if (currentTime - lastTimePressed > debounce_duration)
    {
      if (new_state != button_state)
      {
        button_state = new_state; // logik beim lesen!

        if (button_state == LOW)
        {
          led_state = !led_state; // ? konflikt mit letzten operation!
          // und logik beim lesen!
          // ? Taster_Changed = 1;
          return true;
        }
      }
    }
    return false;
  }

  void
  changeState(bool input_state = false)
  { // unsused ATM
    digitalWrite(Led_PIN, input_state);
    led_state = input_state;
  }
  /*
    void
    changeState()
    { // unsused ATM
      digitalWrite(Led_PIN, led_state);
    }
     */
};

/*
47 > Blau
45 > Grün
51 > Rot
53 > Gelb
49 ? Weis
 */
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

  // bool blink{0}; // unbenutzt
  bool new_state{0};
  bool current_state{1};
  bool flash{0};
  bool led_state{1};
  unsigned long flash_last_time;

  Modul_Lichtschranke(int pinSignal, int pinLed)
      : Signal_PIN(pinSignal), Led_PIN(pinLed)
  {
  }

  void
  light()
  {
    if (flash)
    {
      if (currentTime - flash_last_time > flash_duration)
      {
        flash_last_time = currentTime;
        led_state = led_state ? LOW : HIGH;
      }
    }
    else
    {
      led_state = current_state;
    }
    digitalWrite(Led_PIN, current_state);
  }

  bool
  checkForUnterbrechung()
  {
    if (current_state == 1)
      new_state = !digitalRead(Signal_PIN);
    else
      new_state = false;
    return new_state;
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

    if (currentTime - last_trigger > triggerDelay)
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
    alarm_ton = HIGH;
    alarm_licht = HIGH;
    break;
  case Alarm_Status::on:
    alarm_ton = LOW;
    alarm_licht = LOW;
    break;
  case Alarm_Status::silent:
    alarm_ton = HIGH;
    alarm_licht = LOW;
    break;
  default:
    break;
  }
}

void setLevelZero()
{
  Alle_Lichtschranken[0].current_state = 1;
  Alle_Lichtschranken[1].current_state = 1;
  Alle_Lichtschranken[2].current_state = 1;
  Alle_Lichtschranken[3].current_state = 1;
  /*  Alle_Lichtschranken[4].current_state = 1;
    Alle_Lichtschranken[5].current_state = 1;
    Alle_Lichtschranken[6].current_state = 1;
    Alle_Lichtschranken[7].current_state = 1;
    Alle_Lichtschranken[8].current_state = 1;
    Alle_Lichtschranken[9].current_state = 1;
    Alle_Lichtschranken[10].current_state = 1;
    Alle_Lichtschranken[11].current_state = 1;
    Alle_Lichtschranken[12].current_state = 1;
    Alle_Lichtschranken[12].current_state = 1;
    Alle_Lichtschranken[13].current_state = 1;
    Alle_Lichtschranken[15].current_state = 1;
    Alle_Lichtschranken[16].current_state = 1; */
}

void setLevelOne()
{
  Alle_Lichtschranken[0].current_state = 1;
  Alle_Lichtschranken[1].current_state = 1;
  Alle_Lichtschranken[2].current_state = 1;
  Alle_Lichtschranken[3].current_state = 1;
  /*    Alle_Lichtschranken[4].current_state = 1;
     Alle_Lichtschranken[5].current_state = 1;
     Alle_Lichtschranken[6].current_state = 1;
     Alle_Lichtschranken[7].current_state = 1;
     Alle_Lichtschranken[8].current_state = 1;
     Alle_Lichtschranken[9].current_state = 1;
     Alle_Lichtschranken[10].current_state = 1;
     Alle_Lichtschranken[11].current_state = 1;
     Alle_Lichtschranken[12].current_state = 1;
     Alle_Lichtschranken[12].current_state = 1;
     Alle_Lichtschranken[13].current_state = 1;
     Alle_Lichtschranken[15].current_state = 1;
     Alle_Lichtschranken[16].current_state = 1; */
}

void setLevelTwo()
{
  Alle_Lichtschranken[0].current_state = 1;
  Alle_Lichtschranken[1].current_state = 1;
  Alle_Lichtschranken[2].current_state = 1;
  Alle_Lichtschranken[3].current_state = 1;
  /*    Alle_Lichtschranken[4].current_state = 1;
     Alle_Lichtschranken[5].current_state = 1;
     Alle_Lichtschranken[6].current_state = 1;
     Alle_Lichtschranken[7].current_state = 1;
     Alle_Lichtschranken[8].current_state = 1;
     Alle_Lichtschranken[9].current_state = 1;
     Alle_Lichtschranken[10].current_state = 1;
     Alle_Lichtschranken[11].current_state = 1;
     Alle_Lichtschranken[12].current_state = 1;
     Alle_Lichtschranken[12].current_state = 1;
     Alle_Lichtschranken[13].current_state = 1;
     Alle_Lichtschranken[15].current_state = 1;
     Alle_Lichtschranken[16].current_state = 1; */
}

void levelSwtich(bool force_setLevel = false)
{
  if ((current_level != last_level) || force_setLevel)
  { /*
        for (auto &lvl : Level_Taster)
        {
          lvl.led_state = LOW;
        } */
    switch (current_level)
    {
    case 0:
      Level_Taster[0].led_state = HIGH;
      Level_Taster[1].led_state = LOW; //
      Level_Taster[2].led_state = LOW; //
      setLevelZero();
      break;
    case 1:
      Level_Taster[0].led_state = LOW; //
      Level_Taster[1].led_state = HIGH;
      Level_Taster[2].led_state = LOW; //
      setLevelTwo();
      break;
    case 2:
      Level_Taster[0].led_state = LOW; //
      Level_Taster[1].led_state = LOW; //
      Level_Taster[2].led_state = HIGH;
      setLevelTwo();
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
  for (auto &ls : Alle_Lichtschranken)
    ls.current_state = 1;
}

void setBlinkOne()
{
  // Tor 1
  if (currentTime - blink_last_time[ersteTor] > blink_duration)
  {
    Alle_Lichtschranken[0].current_state = !Alle_Lichtschranken[0].current_state;
    Alle_Lichtschranken[1].current_state = !Alle_Lichtschranken[1].current_state;
    Alle_Lichtschranken[2].current_state = !Alle_Lichtschranken[2].current_state;
    blink_last_time[ersteTor] = currentTime;
  }

  // Tor 2
  if (currentTime - blink_last_time[zweitesTor] > blink_duration)
  {
    Alle_Lichtschranken[3].current_state = !Alle_Lichtschranken[3].current_state;
    // Alle_Lichtschranken[4].current_state = !Alle_Lichtschranken[4].current_state;
    // Alle_Lichtschranken[5].current_state = !Alle_Lichtschranken[5].current_state;
    // Alle_Lichtschranken[6].current_state = !Alle_Lichtschranken[6].current_state;
    blink_last_time[zweitesTor] = currentTime;
  }

  // Tor 3
  if (currentTime - blink_last_time[drittesTor] > blink_duration)
  {
    // Alle_Lichtschranken[7].current_state = !Alle_Lichtschranken[7].current_state;
    // Alle_Lichtschranken[8].current_state = !Alle_Lichtschranken[8].current_state;
    // Alle_Lichtschranken[9].current_state = !Alle_Lichtschranken[9].current_state;
    // Alle_Lichtschranken[10].current_state = !Alle_Lichtschranken[10].current_state;
    // Alle_Lichtschranken[11].current_state = !Alle_Lichtschranken[11].current_state;
    blink_last_time[drittesTor] = currentTime;
  }

  // Tor 4
  if (currentTime - blink_last_time[viertesTor] > blink_duration)
  {
    // Alle_Lichtschranken[12].current_state = !Alle_Lichtschranken[12].current_state;
    // Alle_Lichtschranken[13].current_state = !Alle_Lichtschranken[13].current_state;
    // Alle_Lichtschranken[14].current_state = !Alle_Lichtschranken[14].current_state;
    // Alle_Lichtschranken[15].current_state = !Alle_Lichtschranken[15].current_state;
    // Alle_Lichtschranken[16].current_state = !Alle_Lichtschranken[16].current_state;
    blink_last_time[viertesTor] = currentTime;
  }
}

void setBlinkTwo()
{
  // Tor 1
  if (currentTime - blink_last_time[ersteTor] > blink_duration)
  {
    static bool newState = false;
    Alle_Lichtschranken[0].current_state = newState;
    Alle_Lichtschranken[1].current_state = !newState;
    Alle_Lichtschranken[2].current_state = newState;
    newState = !newState;
    blink_last_time[ersteTor] = currentTime;
  }
  // Tor 2/*
  if (currentTime - blink_last_time[zweitesTor] > blink_duration)
  {
    static bool newState = false;
    Alle_Lichtschranken[3].current_state = !newState;
    // Alle_Lichtschranken[4].current_state = newState;
    // Alle_Lichtschranken[5].current_state = !newState;
    // Alle_Lichtschranken[6].current_state = newState;
    newState = !newState;
    blink_last_time[zweitesTor] = currentTime;
  }
  // Tor 3
  if (currentTime - blink_last_time[drittesTor] > blink_duration)
  {
    static bool newState = false;
    // Alle_Lichtschranken[7].current_state = !newState;
    // Alle_Lichtschranken[8].current_state = newState;
    // Alle_Lichtschranken[9].current_state = !newState;
    // Alle_Lichtschranken[10].current_state = newState;
    // Alle_Lichtschranken[11].current_state = !newState;
    newState = !newState;
    blink_last_time[drittesTor] = currentTime;
  }
  // Tor 4
  if (currentTime - blink_last_time[viertesTor] > blink_duration)
  {
    static bool newState = false;
    // Alle_Lichtschranken[12].current_state = newState;
    // Alle_Lichtschranken[13].current_state = !newState;
    // Alle_Lichtschranken[14].current_state = newState;
    // Alle_Lichtschranken[15].current_state = !newState;
    // Alle_Lichtschranken[16].current_state = newState;
    newState = !newState;
    blink_last_time[viertesTor] = currentTime;
  }
}

void blinkSwitch()
{
  if (current_blink != last_blink)
  { /*
      for (auto &blink : Blink_Taster)
        blink.led_state = LOW; */
    switch (current_blink)
    {
    case 0:
      Blink_Taster[0].led_state = HIGH;
      Blink_Taster[1].led_state = LOW; //
      Blink_Taster[2].led_state = LOW; //
      // levelSwtich(true);
      last_level = 3; // garantiert einen setLevel
      break;
    case 1:
      Blink_Taster[0].led_state = LOW; //
      Blink_Taster[1].led_state = HIGH;
      Blink_Taster[2].led_state = LOW; //
      break;
    case 2:
      Blink_Taster[0].led_state = LOW; //
      Blink_Taster[1].led_state = LOW; //
      Blink_Taster[2].led_state = HIGH;
      break;
    default:
      current_blink = 0;
      break;
    }
    blink_last_time[ersteTor] = currentTime;
    blink_last_time[zweitesTor] = currentTime + (1 * offsetBetweenGates);
    blink_last_time[drittesTor] = currentTime + (2 * offsetBetweenGates);
    blink_last_time[viertesTor] = currentTime + (3 * offsetBetweenGates);
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
  for (Modul_Taster &lvl : Level_Taster)
  {
    pinMode(lvl.Signal_PIN, INPUT_PULLUP);
    pinMode(lvl.Led_PIN, OUTPUT);
  }

  for (Modul_Taster &blink : Blink_Taster)
  {
    pinMode(blink.Signal_PIN, INPUT_PULLUP);
    pinMode(blink.Led_PIN, OUTPUT);
  }

  for (Modul_Lichtschranke &ls : Alle_Lichtschranken)
  {
    pinMode(ls.Signal_PIN, INPUT_PULLUP);
    pinMode(ls.Led_PIN, OUTPUT);
  }
  pinMode(trigger.Signal, INPUT_PULLUP);
  pinMode(Alarm_Licht_Pin, OUTPUT);
  pinMode(Alarm_Ton_Pin, OUTPUT);
}

void loop()
{
  /* Lesen */
  currentTime = millis();

  for (Modul_Taster &lvl : Level_Taster)
  {
    if (lvl.checkForTastendruck())
    {
      current_level = lvl.id;
    }
  }

  for (Modul_Taster &blink : Blink_Taster)
  {
    if (blink.checkForTastendruck())
    {
      current_blink = blink.id;
    }
  }
  for (Modul_Lichtschranke &ls : Alle_Lichtschranken)
  {
    if (ls.checkForUnterbrechung())
    {
      ls.flash = 1;
      if (global_Alarm == off)
      {
        alarm_last_time = currentTime;
        global_Alarm = on;
      }
    }
  }

  /* Auswerten */

  if (global_Alarm == on && (currentTime - alarm_last_time > alarm_duration))
  {
    global_Alarm = silent;
  }
  else if (global_Alarm == silent)
  {
    global_Alarm = off;
    for (Modul_Lichtschranke &ls : Alle_Lichtschranken)
    {
      if (ls.new_state)
      {
        global_Alarm = silent;
        ls.flash = 1;
      }
      else
      {
        ls.flash = 0;
      }
    }
  }

  blinkSwitch();
  levelSwtich(true);
  alarmSwitch();

  /* Schalten */

  for (Modul_Taster &lvl : Level_Taster)
  {
    digitalWrite(lvl.Led_PIN, lvl.led_state);
    lvl.changeState()
  }
  for (Modul_Taster &blink : Blink_Taster)
  {
    digitalWrite(blink.Led_PIN, blink.led_state);
  }
  for (Modul_Lichtschranke &ls : Alle_Lichtschranken)
  {
    ls.light();
    digitalWrite(ls.Led_PIN, ls.led_state);
  }
  digitalWrite(Alarm_Licht_Pin, alarm_licht);
  digitalWrite(Alarm_Ton_Pin, alarm_ton);
}
