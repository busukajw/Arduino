#include "Arduino.h"
#include "LM75A.h"
#include <Wire.h>



LM75A::LM75A(int address = LM75_ADDR)
{
  _address = address;
}

float LM75A::getTemp()
{
  uint8_t msb,lsb = 0;
  _getData(&msb, &lsb);
  return _convData2Temp(_reg2Data(&msb, &lsb),&msb);
}

void LM75A::_getData(uint8_t *msb, uint8_t *lsb)
{
  Wire.begin();
  Wire.beginTransmission(byte(LM75_ADDR));
  Wire.write(LM75_TEMP);
  Wire.requestFrom(LM75_ADDR,2);
  *msb = Wire.read();
  *lsb = Wire.read();
}

uint16_t LM75A::_reg2Data(uint8_t *msb, uint8_t *lsb)
{
  // convert the two bytes and return a float
  // remove the first 5 bits of the lsb as only the first 3 bits are used
  uint16_t reg_data = 0;
  *lsb = *lsb >> 5;
  reg_data = *msb << 3; //need to make space for the lsb
  reg_data = reg_data + *lsb; 
  
  return reg_data;
  
}

float LM75A::_convData2Temp(uint16_t regData, uint8_t *msb)
{
  uint16_t bitcheck = 0;
  float temp = 0;
  bitcheck = *msb & 0x80; // so create and apply mask 10000000:
  if (bitcheck) { // 1 pass's bool test so is a negative number
    Serial.println("Need to do 2s Complement");
    //call 2's complement
  }
  else {
    temp = regData * 0.125;
  }
  return temp;
}
