/*
  Program created for Senior Design DEC1617
  Author Esdras Murillo this is the correct for a thermistor with 2252 resistance at room temp
  Use the motorGo(uint8_t motor, uint8_t direct, uint8_t pwm)
  function to get motors going in either CW, CCW, BRAKEVCC, or
  BRAKEGND. Use motorOff(int motor) to turn a specific motor off.

  The motor variable in each function should be either a 0 or a 1.
  pwm in the motorGo function should be a value between 0 and 255.
*/

#include <Servo.h>
#include <SoftwareSerial.h>

#define CW   1
#define CCW  2
#define BRAKEGND 3
#define CS_THRESHOLD 100

int inApin[2] = {7, 4}; // INA: Clockwise input
int inBpin[2] = {8, 9}; // INB: Counter-clockwise input
int pwmpin[2] = {5, 6}; // PWM input
int cspin[2] = {2, 3}; // CS: Current sense ANALOG input
int enpin[2] = {0, 1}; // EN: Status of switches output (Analog pin)

int bluetoothTx = 10; 
int bluetoothRx = 11;

int statpin = 13;
int serv = 52; //servo switch pin number

int pwm = 0; //integer representation of the pulse width
int pos = 0; //servo position
bool servoSwitch = false;

Servo myservo;
SoftwareSerial bluetooth(bluetoothTx, bluetoothRx);

void setup() 
{
  // set up the LCD's number of columns and rows:

  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
  Serial.begin(9600);
  Serial1.begin(9600);
  bluetooth.begin(9600);
  pinMode(serv, INPUT);

  //THIS PART IS FOR MOTOR CONTROL
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  pinMode(statpin, OUTPUT);

  // Initialize digital pins as outputs
  for (int i = 0; i < 2; i++)
  {
    pinMode(inApin[i], OUTPUT);
    pinMode(inBpin[i], OUTPUT);
    pinMode(pwmpin[i], OUTPUT);
  }
  // Initialize braked
  for (int i = 0; i < 2; i++)
  {
    digitalWrite(inApin[i], LOW);
    digitalWrite(inBpin[i], LOW);
    analogWrite(pwmpin[i], 0);
  }
  

}
//HERE ENDS THE MOTOR CONTROL

void loop() { 
  if(Serial1.available())
  { pwm = Serial1.parseInt();
    motorGo(0, CCW, pwm);   //motor 1 at 0 is 0 volts, at 100 is 9.35 volts CW will go clock wise and CWW will turn it Counter clock wise
    motorGo(1, CCW, pwm);
    delay(400);
  }
  else
    delay(500);
    
  servoSwitch = digitalRead(serv);
  if (servoSwitch)
  { myservo.write(58); // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  else
  { if(bluetooth.available()){
      int toSend = bluetooth.read();
      Serial1.println(toSend);
      myservo.write(toSend);
      delay(15);
    }
    else
    { myservo.write(145); // tell servo to go to position in variable 'pos'
      delay(15);                       // waits 15ms for the servo to reach the position
    }
  }

  if ((analogRead(cspin[0]) < CS_THRESHOLD) && (analogRead(cspin[1]) < CS_THRESHOLD))
    digitalWrite(statpin, HIGH);
}

void motorOff(int motor)
{
  // Initialize braked
  for (int i = 0; i < 2; i++)
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
  if ((motor <= 1) && (direct <= 4))
  {
    // Set inA[motor]
    if (direct <= 1)
      digitalWrite(inApin[motor], HIGH);
    else
      digitalWrite(inApin[motor], LOW);

    // Set inB[motor] WE ARE ONLY USING MOTOR A
    if ((direct == 0) || (direct == 2))
      digitalWrite(inBpin[motor], HIGH);
    else
      digitalWrite(inBpin[motor], LOW);

    analogWrite(pwmpin[motor], pwm);
  }
}








