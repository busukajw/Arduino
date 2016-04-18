#include <LM75A.h>
// https://github.com/philipforget/coffee/blob/master/serial_trials.arduino/lib/LM75A/LM75A.cpp


LM75A sensor;

void setup() {
  // put your setup code here, to run once:
  float temp = 0;
  Serial.begin(9600);
  Serial.println("LM75 test.");
  delay(500);
  temp = sensor.getTemp();
  Serial.println(temp);
  Serial.print("LM75 test ended....");
}

void loop() {
  // put your main code here, to run repeatedly:

}
