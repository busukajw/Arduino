#include <SoftwareSerial.h>
#include <XBee.h>

//
// adafruit 
//   https://learn.adafruit.com/dht/overview  great tutorial
//    https://github.com/adafruit/DHT-sensor-library
//    https://github.com/adafruit/Adafruit_Sensor
// 
// Sheepdoog guide http://sheepdogguides.com/arduino/ar3ne1humDHT11.htm
//
// http://playground.arduino.cc/main/DHT11Lib
//
// DHT11/22 explanation on how the thing works
// http://dalxxdht11.blogspot.nl/2012/12/dht11-library-for-arduino-uno.html

byte bGlobalErr; // for passing error code
byte data[5]; // Array for storing data sent from sensor which will be sent home via ZigBee
float lastHumidity;
float lastTemperatureC;
char charTemp[8];
char charHum[8];

// define SoftSerial RX,TXpins 
uint8_t ssRX = 11;
uint8_t ssTX = 12 ;

int dht_pin = 2;  //data pin to which the sensor it connected to
const byte mask = 0x7f;

// Create the SoftSerial obj and connect it to the RX and TX
SoftwareSerial nss(ssRX,ssTX);
//Create xbee obejct
XBee xbee = XBee();
char payload[30]; // this is the buffer we will fill up with temp & humitdy info
// address of remote XBee
XBeeAddress64 addr64 = XBeeAddress64(0x0013a200, 0x40C822DD);
// create a txStatus we need this to find out the status of the sent payload
ZBTxStatusResponse txStatus = ZBTxStatusResponse();

void setup() {
  pinMode(dht_pin, OUTPUT);
  digitalWrite(dht_pin, HIGH);
  Serial.begin(9600);
  // set baud rate of soft seerial and bind to the xbee
  nss.begin(9600);
  xbee.setSerial(nss);
  delay(1000); // let the system settle before interegating dht22
  Serial.println("Humidity and tempreture Sensor DHT22\n\n");
  delay(1000);

}

void loop() {
  // put your main code here, to run repeatedly:
  ReadDHT();
  ZBTxRequest zbTx = ZBTxRequest(addr64, (uint8_t *)payload, sizeof(payload));
  switch (bGlobalErr) {
    case 0:
     

      Serial.print("Temp: ");
      Serial.print(lastTemperatureC);
      Serial.println("C");
      //convert Temp and Humidty to chars
      dtostrf(lastTemperatureC, 1, 1, charTemp);
      dtostrf(lastHumidity, 1, 1, charHum);
      //build string to send to remote XBee
      strcpy(payload,"{temp:");
      strcat(payload, charTemp);
      strcat(payload,",humidity:");
      strcat(payload, charHum);
      strcat(payload,"}");
      Serial.println(payload);
      // create ZBTx frame
      // send the frame
      xbee.send(zbTx);
      break;
    case 1:
      Serial.println("Error 1: DHT 22 start condition 1 not met.");
      break;
    case 2:
      Serial.println("Error 2: DHT 22start condition 2 not met.");
      break;
    case 3:
      Serial.println("Error: CheckSum error");
      break;
  };
  delay(2000);

}

byte read_dht_data() {
  // collect the 8 bits from the data stream return them as a byte

  byte i = 0;
  byte result = 0;
  for (i = 0; i < 8; i++) {
    // To signal the start of first bits transmission  pin is pulled low by sensor
    while (digitalRead(dht_pin) == LOW); // wait until pin goes HIGH
    // Data pin will now stay HIGH for eith 27us or 70us
    // 27us indicates 0 bit
    // 70us indicates 1 bit
    delayMicroseconds(30);
    if (digitalRead(dht_pin) == HIGH) {
      result |= (1 << 7 - i); // add the 1 to the growing byte
      // ok so here we are shifting the number '1' in to position
    };
    // wait for pin to go low
    while (digitalRead(dht_pin) == HIGH);
  };
  return result;
}

void collectHumidity() {
   /*
   Take out the humidity data and convert to a percentage
      Received bytes are 10 times the value.
      Therefore 256 * first word + second represent 10 times the humidity or
      Temperature.
      */
      lastHumidity = (data[0] * 256 + data[1]);
      lastHumidity = lastHumidity / 10;
      Serial.print("Humidity: ");
      Serial.print(lastHumidity);
      Serial.println("%");
      return;
}

void sepTemp(){
/*
      Take out the temp data from the collected data and convert to celcius
      Received bytes are 10 times the value.
      Therefore 256 * first word + second represent 10 times the humidity or
      Temperature. Most significant Bit of data[2] tells us if the temp is either
      positive or negative.  0 positivie 1 neg temp.
      check the MSB of data[2] for negative temp
*/
      if ((data[2] & mask) != data[2]) {
        data[2] = data[2] & mask;
        lastTemperatureC = -1 * (data[2] * 256 + data[3]);
      }
      else {
        lastTemperatureC = (data[2] * 256 + data[3]);
      }
      lastTemperatureC = lastTemperatureC / 10;
      return;

}
void ReadDHT() {
  /* Uses the global vaiables data[5] and bGlobalErr to pass
   the answer back.  bGlobalError = 0 if all went OK.
   */
  bGlobalErr = 0; // initialize error state
  byte dht_in;
  byte i = 0;

  //send "start reads and report" command to sensor
  // Step 1: Pull-down data pin pin for 18ms
  digitalWrite(dht_pin, LOW);
  delay(18);
  //Step 2: Pull High
  // Needs to be high for between 20-40 micros seconds
  // So choosing middle value
  //
  digitalWrite(dht_pin, HIGH);
  delayMicroseconds(30);
  // Sensor will then take pin LOW this will last 80Microseconds
  //Step 3: Now watch the pin for 80us
  pinMode(dht_pin, INPUT);
  delayMicroseconds(40);

  dht_in = digitalRead(dht_pin); //setting the initial value should be LOW i.e 0
  if (dht_in) { // checking that it is indeed LOW return error if not
    bGlobalErr = 1;
    return;
  };
  delayMicroseconds(80); //wait  after this pin should be high
  dht_in = digitalRead(dht_pin);
  if (!dht_in) {  // check to see if pin is now HIGH if not return error
    bGlobalErr = 2;
    return;
  };
  /* Sensor will now be pull the pin HIGH for 80us.  The low following this hight
  will be the begining of first bit of the 40 that are due
  */
  delayMicroseconds(80);
  // we are now ready to recieve teh data.  use a for loop to pick up the 5 bytes
  for (i = 0; i < 5; i++)
    data[i] = read_dht_data();
  // Next set pinmode to output duties
  pinMode(dht_pin, OUTPUT);
  digitalWrite(dht_pin, HIGH);
  // now check the checksum this is done by adding up the forst 4 bytes and comparing
  // them to the last byte.  They should equal the last byte
  byte check_sum = data[0] + data[1] + data[2] + data[3];
  if (data[4] != check_sum) {
    bGlobalErr = 3; //set global error to 3 and return
    return;
  };
}



