#ifndef LM75A_h
#define LM75A_h

#include "Arduino.h"

#define LM75_ADDR 0x4F
#define LM75_TEMP 0
#define LM75_CONF 1
#define LM75_HYST 3

class LM75A
{
  public:
    LM75A();
    LM75A(int address);
    float getTemp();
   private:
    int _address;
    void _getData(byte*, byte*);
    uint16_t _reg2Data(byte*, byte*);
    float _convData2Temp(uint16_t*, byte*);

};

#endif
