
// Definieer de pinnen voor de DRV8833
const int motorA_in1 = 16; // IN1
const int motorA_in2 = 17; // IN2
const int motorB_in1 = 18; // IN3
const int motorB_in2 = 19; // IN4

void setup() {
  pinMode(motorA_in1, OUTPUT);
  pinMode(motorA_in2, OUTPUT);
  pinMode(motorB_in1, OUTPUT);
  pinMode(motorB_in2, OUTPUT);
}

void loop() {
  // Voorbeeld: motor A vooruit en motor B achteruit met PWM
  driveMotorA(255); // Volle snelheid vooruit
  driveMotorB(-128); // Halve snelheid achteruit   

  delay(30000); // Draai 2 seconden

  // Stop alle motoren
  stopMotors();
  delay(1000); // Wacht 1 seconde

  //Voorbeeld: motor A achteruit en motor B vooruit met PWM
  driveMotorA(-128); // Halve snelheid achteruit
  driveMotorB(255); // Volle snelheid vooruit

  delay(2000); // Draai 2 seconden

  // Stop alle motoren
  stopMotors();
  delay(1000); // Wacht 1 seconde
}

// Functie om motor A aan te sturen
void driveMotorA(int speed) {
  if (speed > 0) {
    analogWrite(motorA_in1, speed);
    analogWrite(motorA_in2, 0);
  } else if (speed < 0) {
    analogWrite(motorA_in1, 0);
    analogWrite(motorA_in2, -speed);
  } else {
    stopMotorA();
  }
}

// Functie om motor B aan te sturen
void driveMotorB(int speed) {
  if (speed > 0) {
    analogWrite(motorB_in1, speed);
    analogWrite(motorB_in2, 0);
  } else if (speed < 0) {
    analogWrite(motorB_in1, 0);
    analogWrite(motorB_in2, -speed);
  } else {
    stopMotorB();
  }
}

// Functie om motor A te stoppen
void stopMotorA() {
  analogWrite(motorA_in1, 0);
  analogWrite(motorA_in2, 0);
}

// Functie om motor B te stoppen
void stopMotorB() {
  analogWrite(motorB_in1, 0);
  analogWrite(motorB_in2, 0);
}

// Functie om alle motoren te stoppen
void stopMotors() {
  stopMotorA();
  stopMotorB();
}