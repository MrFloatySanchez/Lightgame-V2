/*
  ToDo
  -
  Ideen
  - Taster-Funktion setMode
    - level- und blink-Modi ändern im checkTastendruck?
    - pointer im struct der auf die globalen status-varriable (neues_level / akutelles_blinken) zeigt?
 */
// define MONITOR auskommentieren um den seriellen Monitor auszulassen
// #define MONITOR

#include <Arduino.h>

/* Konstanten und bennante Varriabeln */

const int Anzahl_tore = 4;
const int Erstes_tor = 0;
const int Zweites_tor = 1;
const int Drittes_tor = 3;
const int Viertes_tor = 4;
const unsigned long Offset_Between_Gates = 1500;

enum Alarm_Status
{
  off = 0,
  on = 1,
  silent = 2,
};
Alarm_Status global_Alarm = Alarm_Status::off;

const int Alarm_licht_pin = 12;
const int Alarm_ton_pin = 13;

bool alarm_ton = 0;
bool alarm_licht = 0;
bool level_taster_changed = 0;
bool blink_taster_changed = 0;

int neues_level = 0;
int akutelles_level = 0;
int neues_blinken = 0;
int akutelles_blinken = 0;

unsigned long currentTime;
unsigned long alarm_last_time;
unsigned long blink_last_time[Anzahl_tore];

const unsigned long Flash_intervall = 50;
const unsigned long Debounce_dauer = 10;
const unsigned long Alarm_dauer = 3000;
const unsigned long Blink_dauer = 3000;

struct Modul_Taster
{
  const int signal_PIN{};
  const int led_PIN{};
  const int id{};

  bool state{}; // 'current State'
  bool button_state{};
  bool new_state{};
  bool last_state{};
  unsigned long last_time_pressed{};

  Modul_Taster(int pinSignal, int pinLed, int nummer)
      : signal_PIN(pinSignal), led_PIN(pinLed), id(nummer)
  {
  }

  /*
    led_state und button_state das gleiche machen lassen?
    -> led + button = current_state
   */

  bool
  checkForTastendruck()
  {
    new_state = digitalRead(signal_PIN);

    if (new_state != last_state)
    {
      last_time_pressed = currentTime;
    }
    last_state = new_state;

    if (currentTime - last_time_pressed > Debounce_dauer)
    {
      if (new_state != button_state)
      {
        button_state = new_state;

        if (button_state == LOW)
        {
          // led_state = !led_state; // logik beim lesen?
          // ? Taster_Changed = 1;
          return true;
        }
      }
    }
    return false;
  }

  void
  schalteLed()
  {
    digitalWrite(led_PIN, state);
  }
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
  const int signal_PIN{};
  const int led_PIN{};

  // bool blink{0}; // unbenutzt
  bool new_state{0};
  bool current_state{1};
  bool flash{0};
  bool led_state{1};
  unsigned long flash_last_time;

  Modul_Lichtschranke(int pinSignal, int pinLed)
      : signal_PIN(pinSignal), led_PIN(pinLed)
  {
  }

  void
  schalteLed()
  {
    if (flash)
    {
      if (currentTime - flash_last_time > Flash_intervall)
      {
        flash_last_time = currentTime;
        led_state = led_state ? LOW : HIGH;
      }
    }
    else
    {
      led_state = current_state;
    }
    digitalWrite(led_PIN, current_state);
  }

  bool
  checkForUnterbrechung()
  {
    if (current_state == 1) // logik beim lesen!
      new_state = !digitalRead(signal_PIN);
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
  Alle_Lichtschranken[4].current_state = 1;
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
  Alle_Lichtschranken[16].current_state = 1;
}

void setLevelOne()
{
  Alle_Lichtschranken[0].current_state = 1;
  Alle_Lichtschranken[1].current_state = 1;
  Alle_Lichtschranken[2].current_state = 1;
  Alle_Lichtschranken[3].current_state = 1;
  Alle_Lichtschranken[4].current_state = 1;
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
  Alle_Lichtschranken[16].current_state = 1;
}

void setLevelTwo()
{
  Alle_Lichtschranken[0].current_state = 1;
  Alle_Lichtschranken[1].current_state = 1;
  Alle_Lichtschranken[2].current_state = 1;
  Alle_Lichtschranken[3].current_state = 1;
  Alle_Lichtschranken[4].current_state = 1;
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
  Alle_Lichtschranken[16].current_state = 1;
}

void levelSwtich()
{
  akutelles_level = neues_level;
  switch (akutelles_level)
  {
  case 0:
    Level_Taster[0].state = HIGH;
    Level_Taster[1].state = LOW;
    Level_Taster[2].state = LOW;
    setLevelZero();
    break;
  case 1:
    Level_Taster[0].state = LOW;
    Level_Taster[1].state = HIGH;
    Level_Taster[2].state = LOW;
    setLevelTwo();
    break;
  case 2:
    Level_Taster[0].state = LOW;
    Level_Taster[1].state = LOW;
    Level_Taster[2].state = HIGH;
    setLevelTwo();
    break;
  default:
    neues_level = 0;
    break;
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
  if (currentTime - blink_last_time[Erstes_tor] > Blink_dauer)
  {
    Alle_Lichtschranken[0].current_state = !Alle_Lichtschranken[0].current_state;
    Alle_Lichtschranken[1].current_state = !Alle_Lichtschranken[1].current_state;
    Alle_Lichtschranken[2].current_state = !Alle_Lichtschranken[2].current_state;
    blink_last_time[Erstes_tor] = currentTime;
  }

  // Tor 2
  if (currentTime - blink_last_time[Zweites_tor] > Blink_dauer)
  {
    Alle_Lichtschranken[3].current_state = !Alle_Lichtschranken[3].current_state;
    Alle_Lichtschranken[4].current_state = !Alle_Lichtschranken[4].current_state;
    Alle_Lichtschranken[5].current_state = !Alle_Lichtschranken[5].current_state;
    Alle_Lichtschranken[6].current_state = !Alle_Lichtschranken[6].current_state;
    blink_last_time[Zweites_tor] = currentTime;
  }

  // Tor 3
  if (currentTime - blink_last_time[Drittes_tor] > Blink_dauer)
  {
    Alle_Lichtschranken[7].current_state = !Alle_Lichtschranken[7].current_state;
    Alle_Lichtschranken[8].current_state = !Alle_Lichtschranken[8].current_state;
    Alle_Lichtschranken[9].current_state = !Alle_Lichtschranken[9].current_state;
    Alle_Lichtschranken[10].current_state = !Alle_Lichtschranken[10].current_state;
    Alle_Lichtschranken[11].current_state = !Alle_Lichtschranken[11].current_state;
    blink_last_time[Drittes_tor] = currentTime;
  }

  // Tor 4
  if (currentTime - blink_last_time[Viertes_tor] > Blink_dauer)
  {
    Alle_Lichtschranken[12].current_state = !Alle_Lichtschranken[12].current_state;
    Alle_Lichtschranken[13].current_state = !Alle_Lichtschranken[13].current_state;
    Alle_Lichtschranken[14].current_state = !Alle_Lichtschranken[14].current_state;
    Alle_Lichtschranken[15].current_state = !Alle_Lichtschranken[15].current_state;
    Alle_Lichtschranken[16].current_state = !Alle_Lichtschranken[16].current_state;
    blink_last_time[Viertes_tor] = currentTime;
  }
}

void setBlinkTwo()
{
  // Tor 1
  if (currentTime - blink_last_time[Erstes_tor] > Blink_dauer)
  {
    static bool newState = false;
    Alle_Lichtschranken[0].current_state = newState;
    Alle_Lichtschranken[1].current_state = !newState;
    Alle_Lichtschranken[2].current_state = newState;
    newState = !newState;
    blink_last_time[Erstes_tor] = currentTime;
  }
  // Tor 2/*
  if (currentTime - blink_last_time[Zweites_tor] > Blink_dauer)
  {
    static bool newState = false;
    Alle_Lichtschranken[3].current_state = !newState;
    Alle_Lichtschranken[4].current_state = newState;
    Alle_Lichtschranken[5].current_state = !newState;
    Alle_Lichtschranken[6].current_state = newState;
    newState = !newState;
    blink_last_time[Zweites_tor] = currentTime;
  }
  // Tor 3
  if (currentTime - blink_last_time[Drittes_tor] > Blink_dauer)
  {
    static bool newState = false;
    Alle_Lichtschranken[7].current_state = !newState;
    Alle_Lichtschranken[8].current_state = newState;
    Alle_Lichtschranken[9].current_state = !newState;
    Alle_Lichtschranken[10].current_state = newState;
    Alle_Lichtschranken[11].current_state = !newState;
    newState = !newState;
    blink_last_time[Drittes_tor] = currentTime;
  }
  // Tor 4
  if (currentTime - blink_last_time[Viertes_tor] > Blink_dauer)
  {
    static bool newState = false;
    Alle_Lichtschranken[12].current_state = newState;
    Alle_Lichtschranken[13].current_state = !newState;
    Alle_Lichtschranken[14].current_state = newState;
    Alle_Lichtschranken[15].current_state = !newState;
    Alle_Lichtschranken[16].current_state = newState;
    newState = !newState;
    blink_last_time[Viertes_tor] = currentTime;
  }
}

void blinkSwitch()
{
  if (neues_blinken != akutelles_blinken)
  {
    blink_last_time[Erstes_tor] = currentTime;
    blink_last_time[Zweites_tor] = currentTime + (1 * Offset_Between_Gates);
    blink_last_time[Drittes_tor] = currentTime + (2 * Offset_Between_Gates);
    blink_last_time[Viertes_tor] = currentTime + (3 * Offset_Between_Gates);
    neues_level = neues_blinken;
  }

  switch (akutelles_blinken)
  {
  case 0:
    Blink_Taster[0].state = HIGH;
    Blink_Taster[1].state = LOW;
    Blink_Taster[2].state = LOW;
    break;
  case 1:
    Blink_Taster[0].state = LOW;
    Blink_Taster[1].state = HIGH;
    Blink_Taster[2].state = LOW;
    setBlinkOne();
    break;
  case 2:
    Blink_Taster[0].state = LOW;
    Blink_Taster[1].state = LOW;
    Blink_Taster[2].state = HIGH;
    setBlinkTwo();
    break;
  default:
    neues_blinken = 0;
    break;
  }
}

void setup()
{
#ifdef MONITOR
  Serial.begin(9600);
#endif
  for (Modul_Taster &lvl : Level_Taster)
  {
    pinMode(lvl.signal_PIN, INPUT_PULLUP);
    pinMode(lvl.led_PIN, OUTPUT);
  }
  for (Modul_Taster &blink : Blink_Taster)
  {
    pinMode(blink.signal_PIN, INPUT_PULLUP);
    pinMode(blink.led_PIN, OUTPUT);
  }
  for (Modul_Lichtschranke &ls : Alle_Lichtschranken)
  {
    pinMode(ls.signal_PIN, INPUT_PULLUP);
    pinMode(ls.led_PIN, OUTPUT);
  }
  pinMode(Alarm_licht_pin, OUTPUT);
  pinMode(Alarm_ton_pin, OUTPUT);
}

void loop()
{
  /* Lesen */
  currentTime = millis();
  for (Modul_Taster &lvl : Level_Taster)
  {
    if (lvl.checkForTastendruck())
    {
      neues_level = lvl.id;
    }
  }
  for (Modul_Taster &blink : Blink_Taster)
  {
    if (blink.checkForTastendruck())
    {
      neues_blinken = blink.id;
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

  /* Auswerten + Status Setzten */
  if (global_Alarm == on && (currentTime - alarm_last_time > Alarm_dauer))
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
  levelSwtich();
  alarmSwitch();

  /* Pins Schalten */
  for (Modul_Taster &lvl : Level_Taster)
  {
    lvl.schalteLed()
  }
  for (Modul_Taster &blink : Blink_Taster)
  {
    blink.schalteLed();
  }
  for (Modul_Lichtschranke &ls : Alle_Lichtschranken)
  {
    ls.schalteLed();
  }
  digitalWrite(Alarm_licht_pin, alarm_licht);
  digitalWrite(Alarm_ton_pin, alarm_ton);
}
