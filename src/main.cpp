
/*
  ToDo
  - 
  Ideen
  - Taster-Funktion setMode
    - level- und blink-Modi ändern im checkTastendruck?
    - pointer im struct der auf die globalen status-varriable (aktuelles_level / letztes_blinken) zeigt? 
 */
// define MONITOR auskommentieren um den seriellen Monitor auszulassen
// #define MONITOR

#include <Arduino.h>

/* Konstanten und bennante Varriabeln */

const int anzahl_tore = 4;
const int erstes_tor = 0;
const int zweites_tor = 1;
const int drittes_tor = 3;
const int viertes_tor = 4;
const unsigned long offset_Between_Gates = 1500;

enum Alarm_Status
{
  off = 0,
  on = 1,
  silent = 2,
};
Alarm_Status global_Alarm = Alarm_Status::off;

enum Game_States
{
  waiting,
  playing,
  alarm,
} game_state;

const int alarm_licht_pin = 12;
const int alarm_ton_pin = 13;

bool alarm_ton = 0;
bool alarm_licht = 0;
bool level_taster_changed = 0;
bool blink_taster_changed = 0;

int aktuelles_level = 0;
int letztes_level = 0;
int aktuelles_blinken = 0;
int letztes_blinken = 0;

unsigned long currentTime;
unsigned long alarm_last_time;
unsigned long blink_last_time[anzahl_tore];

const unsigned long flash_intervall = 50;
const unsigned long debounce_dauer = 10;
const unsigned long alarm_dauer = 3000;
const unsigned long blink_dauer = 3000;

struct Modul_Taster
{
  const int signal_PIN{};
  const int led_PIN{};
  const int id{};

  bool led_state{}; // 'current State'
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

    if (currentTime - last_time_pressed > debounce_dauer)
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
  changeState(bool input_state = false)
  { // unsused ATM
    digitalWrite(led_PIN, input_state);
    led_state = input_state;
  }
  /*
  Change State nur status-varriable setzten?
  -> write()
    void
    changeState()
    { // unsused ATM
      digitalWrite(led_PIN, led_state);
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
  light()
  { // add if (blink)
    if (flash)
    {
      if (currentTime - flash_last_time > flash_intervall)
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

void levelSwtich(bool force_setLevel = false)
{
  if ((aktuelles_level != letztes_level) || force_setLevel)
  {
    switch (aktuelles_level)
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
      aktuelles_level = 0;
      break;
    }
    letztes_level = aktuelles_level;
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
  if (currentTime - blink_last_time[erstes_tor] > blink_dauer)
  {
    Alle_Lichtschranken[0].current_state = !Alle_Lichtschranken[0].current_state;
    Alle_Lichtschranken[1].current_state = !Alle_Lichtschranken[1].current_state;
    Alle_Lichtschranken[2].current_state = !Alle_Lichtschranken[2].current_state;
    blink_last_time[erstes_tor] = currentTime;
  }

  // Tor 2
  if (currentTime - blink_last_time[zweites_tor] > blink_dauer)
  {
    Alle_Lichtschranken[3].current_state = !Alle_Lichtschranken[3].current_state;
    Alle_Lichtschranken[4].current_state = !Alle_Lichtschranken[4].current_state;
    Alle_Lichtschranken[5].current_state = !Alle_Lichtschranken[5].current_state;
    Alle_Lichtschranken[6].current_state = !Alle_Lichtschranken[6].current_state;
    blink_last_time[zweites_tor] = currentTime;
  }

  // Tor 3
  if (currentTime - blink_last_time[drittes_tor] > blink_dauer)
  {
    Alle_Lichtschranken[7].current_state = !Alle_Lichtschranken[7].current_state;
    Alle_Lichtschranken[8].current_state = !Alle_Lichtschranken[8].current_state;
    Alle_Lichtschranken[9].current_state = !Alle_Lichtschranken[9].current_state;
    Alle_Lichtschranken[10].current_state = !Alle_Lichtschranken[10].current_state;
    Alle_Lichtschranken[11].current_state = !Alle_Lichtschranken[11].current_state;
    blink_last_time[drittes_tor] = currentTime;
  }

  // Tor 4
  if (currentTime - blink_last_time[viertes_tor] > blink_dauer)
  {
    Alle_Lichtschranken[12].current_state = !Alle_Lichtschranken[12].current_state;
    Alle_Lichtschranken[13].current_state = !Alle_Lichtschranken[13].current_state;
    Alle_Lichtschranken[14].current_state = !Alle_Lichtschranken[14].current_state;
    Alle_Lichtschranken[15].current_state = !Alle_Lichtschranken[15].current_state;
    Alle_Lichtschranken[16].current_state = !Alle_Lichtschranken[16].current_state;
    blink_last_time[viertes_tor] = currentTime;
  }
}

void setBlinkTwo()
{
  // Tor 1
  if (currentTime - blink_last_time[erstes_tor] > blink_dauer)
  {
    static bool newState = false;
    Alle_Lichtschranken[0].current_state = newState;
    Alle_Lichtschranken[1].current_state = !newState;
    Alle_Lichtschranken[2].current_state = newState;
    newState = !newState;
    blink_last_time[erstes_tor] = currentTime;
  }
  // Tor 2/*
  if (currentTime - blink_last_time[zweites_tor] > blink_dauer)
  {
    static bool newState = false;
    Alle_Lichtschranken[3].current_state = !newState;
    Alle_Lichtschranken[4].current_state = newState;
    Alle_Lichtschranken[5].current_state = !newState;
    Alle_Lichtschranken[6].current_state = newState;
    newState = !newState;
    blink_last_time[zweites_tor] = currentTime;
  }
  // Tor 3
  if (currentTime - blink_last_time[drittes_tor] > blink_dauer)
  {
    static bool newState = false;
    Alle_Lichtschranken[7].current_state = !newState;
    Alle_Lichtschranken[8].current_state = newState;
    Alle_Lichtschranken[9].current_state = !newState;
    Alle_Lichtschranken[10].current_state = newState;
    Alle_Lichtschranken[11].current_state = !newState;
    newState = !newState;
    blink_last_time[drittes_tor] = currentTime;
  }
  // Tor 4
  if (currentTime - blink_last_time[viertes_tor] > blink_dauer)
  {
    static bool newState = false;
    Alle_Lichtschranken[12].current_state = newState;
    Alle_Lichtschranken[13].current_state = !newState;
    Alle_Lichtschranken[14].current_state = newState;
    Alle_Lichtschranken[15].current_state = !newState;
    Alle_Lichtschranken[16].current_state = newState;
    newState = !newState;
    blink_last_time[viertes_tor] = currentTime;
  }
}

void blinkSwitch()
{
  if (aktuelles_blinken != letztes_blinken)
  {
    switch (aktuelles_blinken)
    {
    case 0:
      Blink_Taster[0].led_state = HIGH;
      Blink_Taster[1].led_state = LOW; //
      Blink_Taster[2].led_state = LOW; //
      // levelSwtich(true);
      letztes_level = 3; // garantiert einen setLevel
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
      aktuelles_blinken = 0;
      break;
    }
    blink_last_time[erstes_tor] = currentTime;
    blink_last_time[zweites_tor] = currentTime + (1 * offset_Between_Gates);
    blink_last_time[drittes_tor] = currentTime + (2 * offset_Between_Gates);
    blink_last_time[viertes_tor] = currentTime + (3 * offset_Between_Gates);
  } // if (aktuelles_blinken != letztes_blinken)

  switch (aktuelles_blinken)
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
    aktuelles_blinken = 0;
    break;
  }
  letztes_blinken = aktuelles_blinken;
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
  pinMode(alarm_licht_pin, OUTPUT);
  pinMode(alarm_ton_pin, OUTPUT);
  game_state = Game_States::waiting;
}

void loop()
{
  /* Lesen */
  currentTime = millis();

  for (Modul_Taster &lvl : Level_Taster)
  {
    if (lvl.checkForTastendruck())
    {
      aktuelles_level = lvl.id;
    }
  }
  for (Modul_Taster &blink : Blink_Taster)
  {
    if (blink.checkForTastendruck())
    {
      aktuelles_blinken = blink.id;
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

  if (global_Alarm == on && (currentTime - alarm_last_time > alarm_dauer))
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

  /* Pins Schalten */

  for (Modul_Taster &lvl : Level_Taster)
  {
    digitalWrite(lvl.led_PIN, lvl.led_state);
    // lvl.changeState()
  }
  for (Modul_Taster &blink : Blink_Taster)
  {
    digitalWrite(blink.led_PIN, blink.led_state);
  }
  for (Modul_Lichtschranke &ls : Alle_Lichtschranken)
  {
    ls.light();
  }
  digitalWrite(alarm_licht_pin, alarm_licht);
  digitalWrite(alarm_ton_pin, alarm_ton);
}
