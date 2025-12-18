#include "SerialCommand.h"
#include "BluetoothSerial.h"
#include <EEPROM.h>

// ---------- Bluetooth ----------
BluetoothSerial SerialBT;

// ---------- EEPROM ----------
struct param_t
{
  unsigned long cycleTime = 5000;        // defaults
  unsigned long knipperPeriode = 1000000; // in microseconden bv.
} params;

#define EEPROM_SIZE sizeof(param_t)

// ---------- SerialCommand (2 interfaces) ----------
SerialCommand sCmdBT(SerialBT);
SerialCommand sCmdUSB(Serial);

// Actieve context (welke interface riep het commando op?)
static SerialCommand* activeCmd = nullptr;  // welke parser (USB of BT)
static Stream* activeOut = nullptr;         // waar we antwoorden printen

static inline char* nextArg() {
  return activeCmd ? activeCmd->next() : nullptr;
}

// ---------- IO / button ----------
#define INTERNAL_LED 2
const int BUTTON_PIN = 23;

const unsigned long debounceDelay = 200; // ms
volatile unsigned long lastInterruptTime = 0;
volatile bool run = false;

bool LedState = false;
unsigned long lastBlinkTime = 0;

unsigned long previous = 0, calculationTime = 0;

// ---------- ISR ----------
void IRAM_ATTR buttonISR()
{
  unsigned long now = millis();
  if (now - lastInterruptTime > debounceDelay) {
    lastInterruptTime = now;
    run = !run;
  }
}

// ---------- Command handlers (interface-onafhankelijk) ----------
void onUnknownCommand(char *command)
{
  if (!activeOut) return;
  activeOut->print("unknown command: \"");
  activeOut->print(command);
  activeOut->println("\"");
  activeOut->println("Typ 'help' voor alle commando's.");
}

void onSet()
{
  char* param = nextArg();
  char* value = nextArg();

  if (!param || !value) {
    activeOut->println("gebruik: set cycle <waarde>  |  set knipper <waarde>");
    return;
  }

  if (strcmp(param, "cycle") == 0)   params.cycleTime = atol(value);
  if (strcmp(param, "knipper") == 0) params.knipperPeriode = atol(value);

  EEPROM.put(0, params);
  EEPROM.commit();

  activeOut->println("OK");
}

void onDebug()
{
  activeOut->print("run: ");
  activeOut->println(run ? "aan" : "uit");

  activeOut->print("cycle time: ");
  activeOut->println(params.cycleTime);

  activeOut->print("knipper Periode: ");
  activeOut->println(params.knipperPeriode);

  activeOut->print("calculation time: ");
  activeOut->println(calculationTime);
  calculationTime = 0;
}

void onRun()
{
  char* param = nextArg();
  if (!param) {
    activeOut->println("gebruik: run aan | run uit");
    return;
  }

  if (strcmp(param, "aan") == 0) run = true;
  else if (strcmp(param, "uit") == 0) run = false;
  else activeOut->println("gebruik: run aan | run uit");
}

void onHelp()
{
  activeOut->println("Beschikbare commando's:");
  activeOut->println("  help");
  activeOut->println("    Toon deze hulp.");
  activeOut->println();
  activeOut->println("  debug");
  activeOut->println("    Toon run status + parameters + calculation time.");
  activeOut->println();
  activeOut->println("  run aan|uit");
  activeOut->println("    Start/stop de robot (LED knippert enkel als run=aan).");
  activeOut->println();
  activeOut->println("  set cycle <us>");
  activeOut->println("    Zet cyclustijd in microseconden. Voorbeeld: set cycle 5000");
  activeOut->println();
  activeOut->println("  set knipper <us>");
  activeOut->println("    Zet knipperperiode in microseconden. Voorbeeld: set knipper 1000000");
  activeOut->println();
  activeOut->println("Tips:");
  activeOut->println("  - Gebruik spaties tussen woorden.");
  activeOut->println("  - Serial Monitor: kies 'Both NL & CR'.");
}

// ---------- Wrappers: zetten activeCmd/activeOut en roepen echte handler ----------
void setContextBT()  { activeCmd = &sCmdBT;  activeOut = &SerialBT; }
void setContextUSB() { activeCmd = &sCmdUSB; activeOut = &Serial;   }

void onSetBT()    { setContextBT();  onSet(); }
void onSetUSB()   { setContextUSB(); onSet(); }

void onDebugBT()  { setContextBT();  onDebug(); }
void onDebugUSB() { setContextUSB(); onDebug(); }

void onRunBT()    { setContextBT();  onRun(); }
void onRunUSB()   { setContextUSB(); onRun(); }

void onUnknownBT(char *cmd)  { setContextBT();  onUnknownCommand(cmd); }
void onUnknownUSB(char *cmd) { setContextUSB(); onUnknownCommand(cmd); }

void onHelpBT()  { setContextBT();  onHelp(); }
void onHelpUSB() { setContextUSB(); onHelp(); }

// ---------- Setup ----------
void setup()
{
  Serial.begin(115200);
  SerialBT.begin("ESP32_RVL");

  // EEPROM
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.get(0, params);       // laad wat er is (defaults blijven als EEPROM leeg/rommel is)
  EEPROM.commit();             // harmless; zorgt dat begin/partition ok is

  // Commands registreren op beide interfaces
  sCmdBT.addCommand("set",   onSetBT);
  sCmdBT.addCommand("debug", onDebugBT);
  sCmdBT.addCommand("run",   onRunBT);
  sCmdBT.addCommand("help",   onHelpBT);
  sCmdBT.setDefaultHandler(onUnknownBT);

  sCmdUSB.addCommand("set",   onSetUSB);
  sCmdUSB.addCommand("debug", onDebugUSB);
  sCmdUSB.addCommand("run",   onRunUSB);
  sCmdUSB.addCommand("help",   onHelpUSB);
  sCmdUSB.setDefaultHandler(onUnknownUSB);

  pinMode(INTERNAL_LED, OUTPUT);
  digitalWrite(INTERNAL_LED, LOW);

  // knopIO23 en GND 
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, FALLING);

  for (int i = 0; i < 5; i++) {
    digitalWrite(INTERNAL_LED, HIGH); delay(150);
    digitalWrite(INTERNAL_LED, LOW);  delay(300);
  }

  Serial.println("USB ready.");
  SerialBT.println("BT ready.");
}

// ---------- Loop ----------
void loop()
{
  // lees beide command streams
  sCmdUSB.readSerial();
  sCmdBT.readSerial();

  unsigned long current = micros();
  if (current - previous >= params.cycleTime)
  {
    previous = current;

    if (run)
    {
      lastBlinkTime += params.cycleTime;
      if (lastBlinkTime > (params.knipperPeriode / 2))
      {
        LedState = !LedState;
        digitalWrite(INTERNAL_LED, LedState);
        lastBlinkTime = 0;
      }
    }
    else
    {
      LedState = false;
      digitalWrite(INTERNAL_LED, LedState);
      lastBlinkTime = 0;
    }
  }

  unsigned long difference = micros() - current;
  if (difference > calculationTime) calculationTime = difference;
}
