/*
Program created for Senior Design DEC1617 
Author Esdras Murillo this is the correct for a thermistor with 2252 resistance at room temp
 Use the motorGo(uint8_t motor, uint8_t direct, uint8_t pwm) 
 function to get motors going in either CW, CCW, BRAKEVCC, or 
 BRAKEGND. Use motorOff(int motor) to turn a specific motor off.
 
 The motor variable in each function should be either a 0 or a 1.
 pwm in the motorGo function should be a value between 0 and 255.
*/

int setDigital=255;
char set;
float setTemp;
// which analog pin to connect
//THERMISTORS TEMPERATURE DEFINITIONS
#define THERMISTORPIN A8         
// resistance at 25 degrees C
#define THERMISTORNOMINAL 2252   
// temp. for nominal resistance (almost always 25 C)
#define TEMPERATURENOMINAL 25   
// how many samples to take and average, more takes longer
// but is more 'smooth'
#define NUMSAMPLES 10
// The beta coefficient of the thermistor (usually 3000-4000)
#define BCOEFFICIENT 3950
// the value of the 'other' resistor
#define SERIESRESISTOR 20000   

//MOTOR SHIELD CONTROL

#define CW   1
#define CCW  2
#define BRAKEGND 3
#define CS_THRESHOLD 100

int inApin[2] = {7, 4};  // INA: Clockwise input
int inBpin[2] = {8, 9}; // INB: Counter-clockwise input
int pwmpin[2] = {5, 6}; // PWM input
int cspin[2] = {2, 3}; // CS: Current sense ANALOG input
int enpin[2] = {0, 1}; // EN: Status of switches output (Analog pin)

int statpin = 13;

float VoutAnalog;
float THERMISTORRESISTANT;
float ThermistorResistance_ohm;
float ThermistorTemp_degC;
 
#include <LiquidCrystal.h> 

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(27, 26, 25,24 ,23, 22);

void setup() {
  // set up the LCD's number of columns and rows:
   lcd.begin(16, 2);
   Serial.begin(9600);
   Serial1.begin(9600);

               //THIS PART IS FOR MOTOR CONTROL
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   
  pinMode(statpin, OUTPUT);
  // Initialize digital pins as outputs
  for (int i=0; i<2; i++)
  {
    pinMode(inApin[i], OUTPUT);
    pinMode(inBpin[i], OUTPUT);
    pinMode(pwmpin[i], OUTPUT);
  }
  // Initialize braked
  for (int i=0; i<2; i++)
  {
    digitalWrite(inApin[i], LOW);
    digitalWrite(inBpin[i], LOW);
    analogWrite(pwmpin[i], 0);
  }
}
//HERE ENDS THE MOTOR CONTROL

void loop() {
                                        //THIS PART IS FOR MOTOR CONTROL
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  //this par works to print the values that are beein printed in the serial print to be exported to excell

  if(Serial.available()){
    set=Serial.read();

    if((setDigital == 't') || (setDigital == 'T')){
    //if((set == 't') || (set == 'T')) {
      setTemp = Serial.parseInt(); 
      setDigital=map(setTemp,3.5,25,255,0);
      motorGo(0, CCW, setDigital); //Motor 1 start at full voltage 12V and it will look every Delay time
      motorGo(1, CCW, setDigital); //Motor 2 start at full voltage 12V and it will look every Delay time
      Serial1.println(setDigital); //send the pwm info to the slave Arduino
    }
    else  //otherwise, store digital value
    { setDigital = set;
    }
    delay(400);
  }
  //its starting at the max voltage value
  else
    delay(500);
 
if ((analogRead(cspin[0]) < CS_THRESHOLD) && (analogRead(cspin[1]) < CS_THRESHOLD))
    digitalWrite(statpin, HIGH);
    
thermistorFunction( );
delay(5000);
}

                                            //METHOD TO CONTROL THE MOTOR
   ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void motorOff(int motor)
{
  // Initialize braked
  for (int i=0; i<2; i++)
  {
    digitalWrite(inApin[i], LOW);
    digitalWrite(inBpin[i], LOW);
  }
  analogWrite(pwmpin[motor], 0);
}

/* motorGo() will set a motor going in a specific direction
 the motor will continue going in that direction, at that speed
 until told to do otherwise.
 
 motor: this should be either 0 or 1, will selet which of the two
 motors to be controlled
 
 direct: Should be between 0 and 3, with the following result
 0: Brake to VCC
 1: Clockwise
 2: CounterClockwise
 3: Brake to GND
 
 pwm: should be a value between ? and 1023, higher the number, the faster
 it'll go
 */
void motorGo(uint8_t motor, uint8_t direct, uint8_t pwm)
{
  if (motor <= 1)
  {
    if (direct <=4)
    {
      // Set inA[motor]
      if (direct <=1)
        digitalWrite(inApin[motor], HIGH);
      else
        digitalWrite(inApin[motor], LOW);

//Set inB[motor] WE ARE ONLY USING MOTOR A
      
      if ((direct==0)||(direct==2))
        digitalWrite(inBpin[motor], HIGH);
      else
        digitalWrite(inBpin[motor], LOW);

      analogWrite(pwmpin[motor], pwm);
    }
  }
}
void thermistorFunction( ) //Thermistor Fucntion  to calculate the temperature
 {
  VoutAnalog=analogRead(THERMISTORPIN);
  THERMISTORRESISTANT=SERIESRESISTOR*(1023.0/(float)VoutAnalog-1.0);
  
//  Serial.print(THERMISTORRESISTANT) ;
  float logRt=log(THERMISTORRESISTANT);
  ThermistorTemp_degC = (1 / ( 0.001468 + 0.0002383*logRt + 0.0000001007 * logRt*logRt*logRt)) - 273.15; //becarefull with too many errosr with parentesis
  float ThermistorTemp_degF= ThermistorTemp_degC*1.8+32;
 

 lcd.setCursor(0,0);
    // Print a message to the LCD.
  lcd.print("Temp Cc: ");
  lcd.print(ThermistorTemp_degC);
  Serial.print(ThermistorTemp_degC);
  Serial.print(",");
    lcd.setCursor(0,1);
    // Print a message to the LCD.
  
  lcd.print("Temp Fc: ");
  lcd.print(ThermistorTemp_degF,DEC);

  Serial.print(ThermistorTemp_degF,DEC);
  Serial.println(",");

  delay(500);
  }



  


 
