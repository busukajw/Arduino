#include <Encoder.h>
#include <SPI.h>

/* read rotary encoder with interrupts
 *  Encoder is hooked up to common GROUND
 *  encoder0PinA to Pin 2
 *  encoder0PinB to Pin4
 */
Encoder encoder(2,4);
//set the SPI DAC SS pin
const int dacSlaveSelectPin = 9;

void setup() {
  // put your setup code here, to run once:
 
  attachInterrupt(0, doEncoder, CHANGE); //attach interrupt 0 - pin2 
  Serial.begin(9600);
  Serial.println("Start");

 
  pinMode(dacSlaveSelectPin, OUTPUT);
  //set the pin HIGH initially.  You need to set it LOW when you wish to use the SPI data
  digitalWrite(dacSlaveSelectPin, HIGH);
  //setup the SPI bus
  SPI.begin();
  SPI.beginTransaction(SPISettings(4000000,MSBFIRST,SPI_MODE0));

}

void loop() {
  // put your main code here, to run repeatedly:

}
void doEncoder(){
  /* if PinA and pinB are both high or both low it is spinning forward (CW)
   *  if they are different it is going backward (CCW)
   */
  encoder.update();
  Serial.println( encoder.getPosition() );   
}

