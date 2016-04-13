#include <Wire.h>
// https://github.com/philipforget/coffee/blob/master/serial_trials.arduino/lib/LM75A/LM75A.cpp
#define LM75_ADDR 0x4F
#define LM75_TEMP 0
#define LM75_CONF 1
#define LM75_HYST 3

uint16_t tempdata(byte *a, byte *b)
{
  float temp = 0;
  uint16_t reg_data = 0;
  int bitcheck;
  Serial.print("MSB in hex: ");
  Serial.println(*a,HEX);
  Serial.print("Non-Shifted in binary: ");
  Serial.println(*b, BIN);
  *b = *b >> 5; // shift the last 5 bits's off the byte
  //Serial.print("shifted: ");
  //Serial.println(*b, BIN);
  // if the temperature is negative the fist bit of the MSB will be 1
  bitcheck = *a & 0x80; // so create and apply mask 10000000:
  if (bitcheck) { // 1 pass's bool test so is a negative number
    Serial.println("Need to do 2s Complement");
  }
  else {
        // *a * 8, 'cause first bit isn't 1, but 8, second 16 and so on, a value
        // * 8 has the effect of shifting the number in binary 3 places to the left
    reg_data = *a;
    Serial.print("Just a: ");
    Serial.println(reg_data);
    reg_data = reg_data  << 3;
    Serial.print("RegData: ");
    Serial.println(reg_data, BIN);
    reg_data = reg_data + *b;
    Serial.print("All reg_data: ");
    Serial.println(reg_data);
    temp = reg_data * 0.125;
    Serial.print("Shifting Temp: ");
    Serial.println(temp);
    temp = (*a * 8 +*b) *0.125;
    Serial.print("Temp: " );
    Serial.println(temp);
  }
  
  return temp;
  
}
void getLM75data(uint8_t *msb, uint8_t *lsb)
{
  Wire.begin();
  Wire.beginTransmission(byte(LM75_ADDR));
  Wire.write(LM75_TEMP);
  Wire.requestFrom(LM75_ADDR,2);
  *msb = Wire.read();
  *lsb = Wire.read();
}

void showLM75data()
 {
  uint8_t lsb,msb  = 0;
  uint16_t regdata = 0;
  getLM75data(&msb, &lsb);
  tempdata(&msb, &lsb);
 }
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("LM75 test.");
  delay(500);
  showLM75data();
  Serial.print("LM75 test ended....");
}

void loop() {
  // put your main code here, to run repeatedly:

}
