#include "Arduino.h"
#include "LM75A.h"
#include <Wire.h>

LM75A::LM75A()
{
    _address = LM75_ADDR;
}

LM75A::LM75A(int address)
{
  _address = address;
}

float LM75A::getTemp()
{
  byte msb,lsb = 0;
  uint16_t regData = 0;
  _getData(&msb, &lsb);
  regData = _reg2Data(&msb, &lsb);
  return _convData2Temp(&regData, &msb);
}

void LM75A::_getData(byte *msb, byte *lsb)
{
  Wire.begin();
  Wire.beginTransmission(byte(LM75_ADDR));
  Wire.write(LM75_TEMP);
  Wire.requestFrom(LM75_ADDR,2);
  *msb = Wire.read();
  *lsb = Wire.read();
}

uint16_t LM75A::_reg2Data(byte *msb, byte *lsb)
{
  // convert the two bytes and return a float
  // remove the first 5 bits of the lsb as only the first 3 bits are used
  uint16_t reg_data = 0;
  *lsb = *lsb >> 5;
  reg_data = *msb << 3; //need to make space for the lsb
  reg_data = reg_data + *lsb; 
  
  return reg_data;
  
}

float LM75A::_convData2Temp(uint16_t *regData, byte *msb)
{
  uint16_t bitcheck = 0;
  float temp = 0;
  bitcheck = *msb & 0x80; // so create and apply mask 10000000:
  if (bitcheck) { // 1 pass's bool test so is a negative number
    Serial.println("Need to do 2s Complement");
    //call 2's complement
  }
  else {
    temp = *regData * 0.125;
  }
  return temp;
}
