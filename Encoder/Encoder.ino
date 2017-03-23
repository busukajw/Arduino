#include <LiquidCrystal.h>
#include <Encoder.h>
#include <SPI.h>

/* read rotary encoder with interrupts
 *  Encoder is hooked up to common GROUND
 *  encoder0PinA to Pin 2
 *  encoder0PinB to Pin4
 *  LCD Pins
 *  RS ->4
 *  E  ->5
 *  D4 ->6
 *  D5 ->7
 *  D6 ->8
 *  D7 ->9
 *  R/W, VSS -> GRD
 */

LiquidCrystal lcd(4,5,6,7,8,9);

Encoder encoder(2,1);
//set the SPI DAC SS pin
const int dacSlaveSelectPin = 3;
const int adcSlaveSelectPin = 10;
// the resistor values use to reverse the voltage divider

SPISettings mySettings;

void setup() {
  // put your setup code here, to run once:
 
  //attachInterrupt(0, doEncoder, CHANGE); //attach interrupt 0 - pin2 
  lcd.begin(16,2);
  lcd.print("Volts: ");
  lcd.setCursor(1,0);
  lcd.print("Current:");

  SPI.begin(); 
  pinMode(dacSlaveSelectPin, OUTPUT);
  //set the pin HIGH initially.  You need to set it LOW when you wish to use the SPI data
  digitalWrite(dacSlaveSelectPin, HIGH);
  //setup the SPI bus

}

void loop() {
  
  // put your main code here, to run repeatedly:
  //Serial.print("Encoder Position: ");
  //Serial.println(encoder.getPosition());
  setDac(encoder.getPosition());
  float adcValue = readAdc(1);
  float realVolts = voltageOut(adcValue);
  float adcCurrent = readAdc(0);
  lcd.setCursor(7,0);
  lcd.print(realVolts,3);
  lcd.setCursor(9,1);
  lcd.print(adcCurrent);
  delay(400);
}
// function to read from the ADC
float readAdc(int channel) {
  byte primaryMask = B00001111;
  mySettings = SPISettings(1000000, MSBFIRST, SPI_MODE0);
  noInterrupts();
  digitalWrite(adcSlaveSelectPin, LOW);
  SPI.beginTransaction(mySettings);
  byte ersteConfByte = B00000110;
  byte tweedeConfByte = channel<< 6;
  SPI.transfer(ersteConfByte);
  byte firstHalf = SPI.transfer(tweedeConfByte);
  byte secondHalf = SPI.transfer(0x00);
  digitalWrite(adcSlaveSelectPin, HIGH);
  interrupts();
  SPI.endTransaction();
  Serial.print("First Byte : ");
  Serial.println(firstHalf,BIN);
  firstHalf &= primaryMask;
  int adcNumber = (firstHalf << 8) | secondHalf;
  float value = (adcNumber * 2.048) / 4096.000;
  return value;
}

// function to set the dac
void setDac(int rEncoderNumber){
  mySettings = SPISettings(4000000, MSBFIRST, SPI_MODE0);
  SPI.beginTransaction(mySettings);
  byte initConfbyte = B00110000;
  /* shift the 12bit number 8bits to the right. So that the 1st 4 bits are now the last 4 bits
   * then OR the number with the mask the primary byte now has the config data as the first 4 bits
   * and the first 4 bits of the rotary encoder are now the last 4 bits of the primaryByte
   */
  byte primaryByte = (rEncoderNumber >> 8) | initConfbyte;
  byte mask = B00001111;
  byte secondaryByte =  rEncoderNumber & mask ;
  noInterrupts();  //disable interrupts so the rotorary encoder does not interrupt the SPI comms
  digitalWrite(dacSlaveSelectPin, LOW); // pull the DAC low to get ready to send some data
  SPI.transfer(primaryByte); //send the first byte
  SPI.transfer(secondaryByte); //send the second byte
  interrupts(); // turn interrupts back on
  digitalWrite(dacSlaveSelectPin, HIGH);
  SPI.endTransaction();
}
void doEncoder(){
  /* if PinA and pinB are both high or both low it is spinning forward (CW)
   *  if they are different it is going backward (CCW)
   */
  encoder.update();
  // Serial.println( encoder.getPosition() );
}
float voltageOut(float adcNumber){
  /* function which reverses the result of the voltage divider
   * so we can display the the actual voltage.  This should work
   * for showing both the voltage input and the Current being 
   * pulled
    */
   const long r1 = 217000;
   const long r2 = 17950;
   long totalRes = r1 + r2; // 252900
   float number = adcNumber / r2;
   float vOut =   number * totalRes;
   return vOut;
}


