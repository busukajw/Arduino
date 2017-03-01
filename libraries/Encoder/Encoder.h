#ifndef Encoder_h
#define Encoder_h

#include "Arduino.h"

class Encoder
{
	public:
	// Constructor : sets pins as inputs
		Encoder( int8_t PinA, int8_t PinC );
			//call this from the ISR
		void update();
			// returns current position
		long int getPosition();
			// set the position value
		void setPostion( const long int p);
	 private:
		long int position;
	    int8_t pin_a;
	    int8_t pin_c;
};

#endif
