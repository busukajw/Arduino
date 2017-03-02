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
  setDac(encoder.getPosition());

}
// function to set the dac
void setDac(int rEncoderNumber){
  byte initConfbyte = 0b00110000;
  /* shift the 12bit number 8bits to the right. So that the 1st 4 bits are now the last 4 bits
   * then OR the number with the mask the primary byte now has the config data as the first 4 bits
   * and the first 4 bits of the rotary encoder are now the last 4 bits of the primaryByte
   */
  byte primaryByte = (rEncoderNumber >> 8) | initConfbyte;
  byte mask = 0b00001111;
  byte secondaryByte =  rEncoderNumber & mask ;
  noInterrupts();  //disable interrupts so the rotorary encoder does not interrupt the SPI comms
  digitalWrite(dacSlaveSelectPin, LOW); // pull the DAC low to get ready to send some data
  SPI.transfer(primaryByte); //send the first byte
  SPI.transfer(secondaryByte); //send the second byte
  interrupts(); // turn interrupts back on
  SPI.end();
  digitalWrite(SlaveSelectionPin, HIGH);
}
void doEncoder(){
  /* if PinA and pinB are both high or both low it is spinning forward (CW)
   *  if they are different it is going backward (CCW)
   */
  encoder.update();
  // Serial.println( encoder.getPosition() );   
}

