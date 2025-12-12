unsigned long previousMillis;


void setup() {

  
  pinMode(14, INPUT);
  pinMode(27, INPUT);
  pinMode(26, INPUT);
  pinMode(25, INPUT);
  pinMode(33, INPUT);
  pinMode(32, INPUT);
  pinMode(35, INPUT);
  pinMode(34, INPUT);
  Serial.begin(115200);

}

void loop() {

  if(millis() - previousMillis > 700)
  {
    previousMillis = millis();
    Serial.print("sensor 1: ");
    Serial.println(analogRead(14));
    Serial.print("sensor 2: ");
    Serial.println(analogRead(27));
    Serial.print("sensor 3: ");
    Serial.println(analogRead(26));
    Serial.print("sensor 4: ");
    Serial.println(analogRead(25));
    Serial.print("sensor 5: ");
    Serial.println(analogRead(33));
    Serial.print("sensor 6: ");
    Serial.println(analogRead(32));
    Serial.print("sensor 7: ");
    Serial.println(analogRead(35));
    Serial.print("sensor 8: ");
    Serial.println(analogRead(34));
  }

}
