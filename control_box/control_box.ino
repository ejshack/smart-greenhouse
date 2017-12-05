#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include "Adafruit_SHT31.h"
#include <Adafruit_PWMServoDriver.h>

#define I2C_MUX_1 0x70
#define I2C_MUX_2 0x71

// state of execution 
typedef enum State {
  // start send sequence to Pi
  START_SEND,
  // end send sequence to Pi
  END_SEND,
  // start receive sequence from Pi
  START_RECEIVE,
  // end receive sequence from Pi
  END_RECEIVE,
  // receive UI request from Pi (get sensor reading, change temp)
  RECEIVE_REQUEST,
  // get sensor readings from chamber
  GET_SENSOR_READINGS,
  // set chamber temperature
  SET_CHAMBER_TEMP,
  // send sensor readings to Pi
  SEND_SENSOR_READINGS
};

// current state of execution
State state;
// buffer for receiving and sending requests
String recBuffer = "";
String sendBuffer = "";
// parts of requests to parse 
int reqChamber = 0;
int reqType = 0;
int chamberNumber = 0;
float O2 = 0;
float CO2 = 0;
float degC = 0;
float humidity = 0;
int DELIMITER = 32;
int MESSAGE_START = 60;
int MESSAGE_END = 62;
int received = 0;

Adafruit_ADS1015 ads;
Adafruit_SHT31 sht31 = Adafruit_SHT31();
Adafruit_PWMServoDriver smallFanPWM = Adafruit_PWMServoDriver(0x42);        //pin 3
Adafruit_PWMServoDriver TEC_Direction_PWM = Adafruit_PWMServoDriver(0x41);  //pin 11 
Adafruit_PWMServoDriver TEC_Magnitude_PWM = Adafruit_PWMServoDriver(0x40);  //pin 7
const float VRefer = 4.096 * 2;
bool isFanOn = false;
bool isHeating = true;

void setup() {
  Serial.begin(9600);
  // set initial state of execution
  state = START_RECEIVE;

  Wire.begin();

  ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  ads.begin();
  smallFanPWM.begin();
  smallFanPWM.setPWMFreq(1600);
  TEC_Direction_PWM.begin();
  TEC_Direction_PWM.setPWMFreq(1600);
  TEC_Magnitude_PWM.begin();
  TEC_Magnitude_PWM.setPWMFreq(1600);
}


/*Function to select which chamber's I2C to communicate with*/
void chamberSelect(uint8_t i) {
  //select I2C_MUX_1 for chambers 0-7
  if (i >= 0 && i <= 7) {
    Wire.beginTransmission(I2C_MUX_1);
    Wire.write(1 << i);
    Wire.endTransmission();
  }
  //select I2C_MUX_2 for chambers 8-15
  if (i >= 8 && i <= 15) {
    Wire.beginTransmission(I2C_MUX_2);
    Wire.write(1 << (i - 8));
    Wire.endTransmission();
  }
  if (i > 15) return;
}

float readO2Vout() {
  long sum = 0;
  for (int i = 0; i < 32; i++)
  {
    //TODO: select [chmaberNum] channel in i2c mux
    sum += ads.readADC_SingleEnded(0);
  }
  sum >>= 5;
  float MeasuredVout = sum * (VRefer / 4095.0);
  return MeasuredVout;
}

float readO2Concentration() {
  // Vout samples are with reference to 4.096V
  float MeasuredVout = readO2Vout();

  //float Concentration = FmultiMap(MeasuredVout, VoutArray,O2ConArray, 6);
  //when its output voltage is 2.0V,
  float Concentration = MeasuredVout * 0.21 / 2.0;
  float Concentration_Percentage = Concentration * 100;
  return Concentration_Percentage;
}

float readCO2Vout() {
  long sum = 0;
  for (int i = 0; i < 32; i++)
  {
    //TODO: select [chmaberNum] channel in i2c mux
    sum += ads.readADC_SingleEnded(1);
  }
  sum >>= 5;
  float MeasuredVout = 1000 * sum * (VRefer / 4095.0);
  return MeasuredVout;
}

float readCO2Concentration() {
  // Vout samples are with reference to 4.096V
  float MeasuredVout = readCO2Vout();

  float voltage_diference = MeasuredVout - 400;
  float Concentration = voltage_diference * 50.0 / 16.0;;
  return Concentration;
}

void setTemperature(float targetTemperature, bool isHeating){
  if(isHeating){
     //direction pin is high (heating)
    TEC_Direction_PWM.setPWM(0, 0, 4095);
    isHeating = true;
  }
  else{
    //direction pin is low (cooling)
    TEC_Direction_PWM.setPWM(0, 0, 0);
    isHeating = false;
  }
   /*
    * set magnitude as a function of the difference between the target
   * themperature and the chamber themperature
   */
   //magnitude pwm is 12.5% dutycycle
  TEC_Magnitude_PWM.setPWM(0, 0, 2047);
}

bool toggleVentilationFan(){

  if(isFanOn){
    smallFanPWM.setPWM(0, 0, 0);
    isFanOn = false;
  }
  else{
    smallFanPWM.setPWM(0, 0, 4095);
    isFanOn = true;
  }
  return isFanOn;
}


void loop() {
  // select state of execution 
  switch(state) {
    // start send sequence to Pi
    case START_SEND:
      // reset received buffer
      received = 0;
      // initiate send sequence 
      while(received != MESSAGE_START) {
        // send start identifier
        Serial.write(MESSAGE_START);
        // check for confirmation 
        if(Serial.available() > 0) {
          received = Serial.read();
        }
        delay(2000);
      }
      // set next state 
      state = SEND_SENSOR_READINGS;
      break;
      
    // end send sequence to Pi
    case END_SEND:
      // reset received buffer
      received = 0;
      // initiate end send sequence 
      while(received != MESSAGE_END) {
        // send end identifier 
        Serial.write(MESSAGE_END);
        // check for confirmation 
        if(Serial.available() > 0) {
          received = Serial.read();
        }
        delay(2000);
      }
      // set next state 
      state = START_RECEIVE;
      break;
      
    // start receive sequence from Pi
    case START_RECEIVE:
      // reset received buffer
      received = 0;
      // initiate start receive sequence 
      while(received != MESSAGE_START) {
        // wait for start identifier 
        if(Serial.available() > 0) {
          received = Serial.read();
        }
        delay(2000);
      }
      // send start receive confirmation
      Serial.write(MESSAGE_START);
      
      // set next state 
      state = RECEIVE_REQUEST;
      break;
      
    // end receive sequence from Pi
    case END_RECEIVE:
      // reset received buffer
      received = 0;
      // initiate end receive sequence 
      while(received != MESSAGE_END) {
        // wait for end identifier 
        if(Serial.available() > 0) {
          received = Serial.read();
        }
        delay(2000);
      }
      // send end receive confirmation
      Serial.write(MESSAGE_END);
      
      // set next state 
      state = GET_SENSOR_READINGS;
      break;
      
    // receive UI request from Pi (get sensor reading, change temp)
    case RECEIVE_REQUEST:
      // reset buffers
      reqChamber = 0;
      reqType = 0;

      received = MESSAGE_START;
      while((received == MESSAGE_START) || (received == MESSAGE_END)) {
        if(Serial.available() > 0) {
          received = Serial.read();
        }
      }
      // parse chamber number requested 
      reqChamber = received - 48;
      
      // set next state 
      state = END_RECEIVE;
      break;
      
    // get sensor readings from chamber
    case GET_SENSOR_READINGS:
      // reset send buffer
      sendBuffer = "";
  
      // get sensor values
      chamberNumber = reqChamber;
      O2 = readO2Concentration();
      CO2 = readCO2Concentration();
      degC = sht31.readTemperature();
      humidity = sht31.readHumidity();

      // format sensor request string to send to Pi
      sendBuffer.concat(String(chamberNumber));
      sendBuffer.concat(' ');
      if(!isnan(O2)) {
        sendBuffer.concat(String(O2));
      } else {
        sendBuffer.concat('0');
      }
      sendBuffer.concat(' ');
      if(!isnan(CO2)) {
        sendBuffer.concat(String(CO2));
      } else {
        sendBuffer.concat('0');
      }
      sendBuffer.concat(' ');
      if(!isnan(degC)) {
        sendBuffer.concat(String(degC));
      } else {
        sendBuffer.concat('0');
      }
      sendBuffer.concat(' ');
      if(!isnan(humidity)) {
        sendBuffer.concat(String(humidity));
      } else {
        sendBuffer.concat('0');
      }

      setTemperature(82.3, true);
      toggleVentilationFan();
      
      // set next state 
      state = START_SEND;
      break;
      
      
    // send sensor readings to Pi
    case SEND_SENSOR_READINGS:
      // send sensor readings 
      Serial.println(sendBuffer);
      
      // set next state
      state = END_SEND;
      break;
    
  }
  
}
