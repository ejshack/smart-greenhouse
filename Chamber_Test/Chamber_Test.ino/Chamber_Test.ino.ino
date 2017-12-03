//TODO: Add indications for preheating times

#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include "Adafruit_SHT31.h"
#include <Adafruit_PWMServoDriver.h>

#define I2C_MUX_1 0x70
#define I2C_MUX_2 0x71

Adafruit_ADS1015 ads;
Adafruit_SHT31 sht31 = Adafruit_SHT31();
Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver(0x41); //TECs
Adafruit_PWMServoDriver pwm2 = Adafruit_PWMServoDriver(0x42); //Small fan
const float VRefer = 4.096 * 2;

void setup(void)
{
  Serial.begin(9600);
  Serial.println("Engaging on turbo encabulator...");
  delay(200);
  Serial.println("Automatically synchronizing cardinal grammeters...");
  delay(200);
  Serial.println("Winding panendermic semi-boloid slots...");
  delay(200);




  pwm1.begin();
  pwm1.setPWMFreq(1000);
  pwm2.begin();
  pwm2.setPWMFreq(1000);
  //Check for SHT31 and ADS1015 on all chambers
  //for (int i = 0; i < 16; i++) {
    //chamberSelect(i);
    ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
    ads.begin();
    if (! sht31.begin(0x44)) {
      Serial.print("Couldn't find SHT31 for chamber #");
      //Serial.println(i);
    }
  //}
/*  
  for (uint8_t pwmnum=0; pwmnum < 16; pwmnum++) {
        pwm1.setPWM(pwmnum, 0, 1024);
        pwm2.setPWM(pwmnum, 0, 4095);
    }
*/
    pwm1.setPWM(0, 0, 1024);
    Serial.println("Control Box initialized.");
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

void loop(void)
{
  int16_t adc0, adc1, adc2, adc3;
  float t = sht31.readTemperature();
  float h = sht31.readHumidity();

  Serial.println("===================================");
  /////////////////ADC/////////////////////
  adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);
  adc2 = ads.readADC_SingleEnded(2);
  adc3 = ads.readADC_SingleEnded(3);
  Serial.print("O2 Concentration: "); Serial.println(readO2Concentration());
  Serial.print("CO2 Concentration: "); Serial.println(readCO2Concentration());
  Serial.print("AIN2: "); Serial.println(adc2);
  Serial.print("AIN3: "); Serial.println(adc3);
  Serial.println(" ");

  ///////////SHT31-D///////////////
  if (! isnan(t)) {  // check if 'is not a number'
    Serial.print("Temp *C = "); Serial.println(t);
  } else {
    Serial.println("Failed to read temperature");
  }

  if (! isnan(h)) {  // check if 'is not a number'
    Serial.print("Hum. % = "); Serial.println(h);
  } else {
    Serial.println("Failed to read humidity");
  }
  Serial.println();

  Serial.println("Turning on the small fan....");
  pwm2.setPWM(0, 0, 4095);
  delay(5000);
  Serial.println("Turning off the small fan....");
  pwm2.setPWM(0, 0, 0);
  delay(5000);
}
