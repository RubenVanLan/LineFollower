#define INTERNAL_LED 2
const int BUTTON_PIN = 23;

const unsigned long debounceDelay = 200; // ms

volatile unsigned long lastInterruptTime = 0; // voor debounce
volatile bool roboRunning = false;              // LED toestand

void IRAM_ATTR buttonISR() {
  unsigned long now = millis();

  // Debounce: alleen reageren als het langer dan debounceDelay geleden is
  if (now - lastInterruptTime > debounceDelay) {
    lastInterruptTime = now;

    // roboRunning toggelen en schrijven
    roboRunning = !roboRunning;
    digitalWrite(INTERNAL_LED, roboRunning);
  }
}

void setup() {
  pinMode(INTERNAL_LED, OUTPUT);
  digitalWrite(INTERNAL_LED, LOW);

  // knop tussen GPIO23 en GND
  pinMode(BUTTON_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, RISING);



}

void loop() {
  // hier mag je blokken, de interrupt werkt toch
  delay(10000);
}
