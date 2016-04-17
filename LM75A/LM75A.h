#ifndef LM75A_h
#define LM75A_h

include "Arduino.h"

#define LM75_ADDR 0x4F
#define LM75_TEMP 0
#define LM75_CONF 1
#define LM75_HYST 3

class LM75A
{
  public:
    LM75A(int);
    float getTemp();
   private:
    void _getData(uint8_t, uint8_t);
    void _reg2Data(uint8_t, uint8_t);
    float _convData2Temp(uint16_t, uint8_t)

}

#endif
