#define PIN_ANALOG  A0

void setup() {
  pinMode(PIN_ANALOG, INPUT);
  Serial.begin(9600);
}

void loop() {
  float value = analogRead(PIN_ANALOG);
  float percent = value / 1024 * 100;

  Serial.print(percent);
  Serial.println("%");

  delay(500);
}
