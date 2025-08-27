#include <MQ2.h> 

#define PIN_MQ2  A0
MQ2 mq2(PIN_MQ2);

// NOTE:: после дыхания близко к датчику
int coValueTreshold = 15;

void setup() {
  Serial.begin(9600);
  mq2.begin();
}

void loop() {
  float* values = mq2.read(false);

  // получаем информацию с датчика
  // выводим данные на монитор порта
  // int lpg = mq2.readLPG();
  int co = mq2.readCO();
  // int smoke = mq2.readSmoke();
  float coValue = getPercentOfGas(co);

  Serial.print("CO2: ");
  Serial.print(coValue);
  Serial.println("ppm");

  if (coValue > coValueTreshold) {
    Serial.println("Включаем вентилятор");
  }

  delay(200);
}

float getPercentOfGas(int gasValue) {
  float descCoef = 0.0000001;
  return gasValue * descCoef;
}