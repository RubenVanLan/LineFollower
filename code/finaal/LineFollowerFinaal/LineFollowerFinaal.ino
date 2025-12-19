#include "SerialCommand.h"
#include "BluetoothSerial.h"
#include "EEPROMAnything.h"
#include <EEPROM.h>

// 6396 mm
// kp -18
// kd -0.2
// diff 0.4
// power 220

// forward en backward omdraaien voor positieve kp en kd te gebruiken. 
#define MotorLeftForward 18
#define MotorLeftBackward 19
#define MotorRightForward 16
#define MotorRightBackward 17

// ---------- Bluetooth ----------
BluetoothSerial SerialBT;

// ---------- EEPROM ----------
struct param_t
{
  unsigned long cycleTime = 5000;        // defaults
  unsigned long knipperPeriode = 1000000; // in microseconden bv.
  int black[8];
  int white[8];
  int power;  // 50
  float diff; //0.5
  float kp;
  float ki;
  float kd;
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
const int sensor[] = {14,27,26,25,33,32,35,34}; 
int debugPosition;
bool shutoff = true;
float iTerm;
float lastErr;

unsigned long previous, calculationTime;

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
    activeOut->println("gebruik: set <cycle | knipper | power | diff | kp | ki> <waarde>");
    return;
  }

  //if (strcmp(param, "cycle") == 0)   params.cycleTime = atol(value);
  if (strcmp(param, "knipper") == 0) params.knipperPeriode = atol(value);

  if (strcmp(param, "cycle") == 0)
  { 
    long newCycleTime = atol(value);
    float ratio = ((float) newCycleTime) / ((float) params.cycleTime);

    params.ki *= ratio;
    params.cycleTime = newCycleTime;

  }
  else if (strcmp(param, "power") == 0) params.power = atol(value);
  else if (strcmp(param, "diff") == 0) params.diff = atof(value);
  else if (strcmp(param, "kp") == 0) params.kp = atof(value);
  else if (strcmp(param, "ki") == 0) 
  {
    float cycleTimeInSec = ((float) params.cycleTime) / 1000000;
    params.ki = atof(value) * cycleTimeInSec;
  }
  else if (strcmp(param, "kd") == 0)
  {
    float cycleTimeInSec = ((float) params.cycleTime) / 1000000;
    params.kd = atof(value) / cycleTimeInSec;
  }

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

  activeOut->print("black: ");
  for(int i = 0; i < 8; i++)
  {
    activeOut->print(params.black[i]);
    activeOut->print(" ");
  }
  activeOut->println(" ");

  activeOut->print("white: ");
  for(int i = 0; i < 8; i++)
  {
    activeOut->print(params.white[i]);
    activeOut->print(" ");
  }
  activeOut->println();

  activeOut->print("position: ");
  activeOut->println(debugPosition);

  activeOut->print("kp: ");
  activeOut->println(params.kp);
  
  float cycleTimeInSec = ((float) params.cycleTime) / 1000000;
  float ki = params.ki / cycleTimeInSec;
  activeOut->print("Ki: ");
  activeOut->println(ki);

  float kd = params.kd * cycleTimeInSec;
  activeOut->print("Kd: ");
  activeOut->println(kd);

  activeOut->print("power: ");
  activeOut->println(params.power);

  activeOut->print("diff: ");
  activeOut->println(params.diff);

  activeOut->print("calculation time: ");
  activeOut->println(calculationTime);
  calculationTime = 0;
}
void onCalibrate()
{
  //char* param = sCmd.next();
  char* param = nextArg();

  if (strcmp(param, "black") == 0)
  {
    activeOut->print("start calibrating black... ");
    for (int i = 0; i < 8; i++) params.black[i]=analogRead(sensor[i]);
    activeOut->println("done");
  }
  else if (strcmp(param, "white") == 0)
  {
    activeOut->print("start calibrating white... ");    
    for (int i = 0; i < 8; i++) params.white[i]=analogRead(sensor[i]);  
    activeOut->println("done");      
  }
}

void onShutoff()
{
  //char* param = sCmd.next();
  char* param = nextArg();

  if (strcmp(param, "aan") == 0) 
  {
    shutoff = true;
    iTerm = 0;
  }
  else if (strcmp(param, "uit") == 0) shutoff = false; 
}


void onRun()
{
  char* param = nextArg();
  if (!param) {
    activeOut->println("gebruik: run aan | run uit");
    return;
  }

  if (strcmp(param, "aan") == 0) {run = true; iTerm = 0;}
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
  activeOut->println("  calibrate white|black");
  activeOut->println("    calibratie van de sensoren, zet de sensor boven het juiste kleur.");
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

void onCalibrateBT()  { setContextBT();  onCalibrate(); }
void onCalibrateUSB() { setContextUSB(); onCalibrate(); }

void onShutoffBT()  { onShutoffBT();  onShutoff(); }
void onShutoffUSB() { onShutoffUSB(); onShutoff(); }

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
  sCmdBT.addCommand("calibrate", onCalibrateBT);
  sCmdBT.addCommand("shutoff", onShutoffBT);
  sCmdBT.setDefaultHandler(onUnknownBT);

  sCmdUSB.addCommand("set",   onSetUSB);
  sCmdUSB.addCommand("debug", onDebugUSB);
  sCmdUSB.addCommand("run",   onRunUSB);
  sCmdUSB.addCommand("help",   onHelpUSB);
  sCmdUSB.addCommand("calibrate", onCalibrateUSB);
  sCmdUSB.addCommand("shutoff", onShutoffUSB);
  sCmdUSB.setDefaultHandler(onUnknownUSB);

  //sensoren
    for(int i = 0; i <8;i++) 
  {
    pinMode(sensor[i], INPUT);
  }

  pinMode(INTERNAL_LED, OUTPUT);
  digitalWrite(INTERNAL_LED, LOW);

  // knop 23
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, FALLING);

  //opstart blinken
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
        long normalised[8];
    for (int i = 0; i < 8; i++)
    {
      long value = analogRead(sensor[i]);
      normalised[i] = map(value, params.black[i], params.white[i], 0, 4096);
    }

    int index = 0;
    for (int i = 1; i < 8; i++) if (normalised[i] < normalised[index]) index = i;

    if(shutoff == true) {if(normalised[index] > 3000) run = false;}

    float position;
    if (index == 0) {position = -30;} //missch nog veranderen
    else if (index == 7) {position = 30;} //missch nog veranderen
    else
    {
      int sNul = normalised[index];
      int sMinEen = normalised[index - 1];
      int sPlusEen = normalised[index + 1];

      float b = sPlusEen - sMinEen;
      b = b/2;

      float a = sPlusEen - b - sNul;

      position = -b /(2*a);

      position += index;
      position -= 3.5;

      position *= 9.525;
    }
    debugPosition = position;

    float error = -position; //error = setpoint - input
    float output = error * params.kp;

    iTerm += params.ki * error;
    iTerm = constrain(iTerm, -510, 510);
    output += iTerm;

    output += params.kd * (error - lastErr);
    lastErr = error;

    output = constrain(output, -510, 510);

    int powerLeft = 0;
    int powerRight = 0;

    if (run) if (output >= 0)
    {
      powerLeft = constrain(params.power + params.diff * output, -255, 255);
      powerRight = constrain(powerLeft - output, -255, 255);
      powerLeft = powerRight + output;
    }
    else
    {
      powerRight = constrain(params.power - params.diff * output, -255, 255);
      powerLeft = constrain(powerRight + output, -255, 255);
      powerRight = powerLeft - output;
    }

    analogWrite(MotorLeftForward, powerLeft > 0 ? powerLeft : 0);
    analogWrite(MotorLeftBackward, powerLeft < 0 ? -powerLeft : 0);
    analogWrite(MotorRightForward, powerRight > 0 ? powerRight : 0);
    analogWrite(MotorRightBackward, powerRight < 0 ? -powerRight : 0);
  }

  unsigned long difference = micros() - current;
  if (difference > calculationTime) calculationTime = difference;
}
