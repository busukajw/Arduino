
#include "Arduino.h"
#include "Encoder.h"

Encoder::Encoder( int8_t PinA, int8_t PinC ): pin_a(PinA), pin_c(PinC) {
// set pin a and pin c as inputs
	pinMode(pin_a, INPUT);
    pinMode(pin_c, INPUT);
}
void Encoder::update() {
	if (digitalRead(pin_a)) digitalRead(pin_c) ? position++ : position--;
    else digitalRead(pin_c) ? position-- : position++;
}
// returns current position
long int Encoder::getPosition( ) { return position; };

// set the position value
void Encoder::setPostion( const long int p) { position = p; };
