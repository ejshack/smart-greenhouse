#define STUMPY_SET_NUM_PLANTS 'N'
#define STUMPY_SET_WATER_CONTENT 'W'
#define STUMPY_SET_WATERING_TIME_SECONDS 'T'
#define STUMPY_SET_DELAY_TIME_SECONDS 'D'
#define MINIMUM_TIME_TO_CHECK_MESSAGES_SECONDS 60
#define ENABLE_PUMPS 8
#define ENABLE_THICKNESS_SENSORS 7
#define ENABLE_MOISTURE_SENSORS 6
#define THICKNESS_SENSOR_PIN A0
#define MOISTURE_SENSOR_PIN A1

// the water level which turns on the pump to water the plant
double PLANT_WATER_THRESHOLDS[] = {
                                    0.0, 0.0, 0.0, 0.0,
                                    0.0, 0.0, 0.0, 0.0,
                                    0.0, 0.0, 0.0, 0.0,
                                    0.0, 0.0, 0.0, 0.0
                                  };
int NUM_PLANTS = 16;

int PLANT_SELECT_PINS[] = {2, 3, 4, 5};


int currentPlant = 0;
int time_between_readings_seconds = 0;// Set by STUMPY //10*60;// 10 mins* 60 seconds
float watering_time_seconds = 0; // Set by STUMPY //30 seconds * 1000 ms ~ 27.5 mL

void setup() {
  Serial.begin(9600);
  pinMode(PLANT_SELECT_PINS[0], OUTPUT);
  pinMode(PLANT_SELECT_PINS[1], OUTPUT);
  pinMode(PLANT_SELECT_PINS[2], OUTPUT);
  pinMode(PLANT_SELECT_PINS[3], OUTPUT);
  digitalWrite(PLANT_SELECT_PINS[0], HIGH);
  digitalWrite(PLANT_SELECT_PINS[1], HIGH);
  digitalWrite(PLANT_SELECT_PINS[2], HIGH);
  digitalWrite(PLANT_SELECT_PINS[3], HIGH);
  
  pinMode(ENABLE_THICKNESS_SENSORS, OUTPUT);
  pinMode(ENABLE_MOISTURE_SENSORS, OUTPUT);
  pinMode(ENABLE_PUMPS, OUTPUT);
  digitalWrite(ENABLE_THICKNESS_SENSORS, LOW);
  digitalWrite(ENABLE_MOISTURE_SENSORS, LOW);
  digitalWrite(ENABLE_PUMPS, HIGH);
}

void loop() {
    // select appropriate plant
    selectPlant(currentPlant);

    // read sensors of plant
    int soil_sensor = analogRead(THICKNESS_SENSOR_PIN);
    int leaf_sensor = analogRead(MOISTURE_SENSOR_PIN);
    float moisturePercentReading = getMoisturePercentage(soil_sensor);
    boolean turnOnPump = PLANT_WATER_THRESHOLDS[currentPlant] > moisturePercentReading;

    // write output for plant
    Serial.print("plant_id:");
    Serial.print(currentPlant);
    Serial.print(";soil:");
    Serial.print(moisturePercentReading);
    Serial.print(";leaf:");
    Serial.print(getThickness(leaf_sensor));
    Serial.print(";watered:");
    Serial.print(turnOnPump);
    Serial.println(";");

    // turn on the pump if needed
    if(turnOnPump)
    {
        // turn on pumps
        // enable is switched, low=on, high=off
        digitalWrite(ENABLE_PUMPS, LOW);
        delay(watering_time_seconds * 1000);
        digitalWrite(ENABLE_PUMPS, HIGH);
    }

    // increment current plant
    if (currentPlant == NUM_PLANTS-1)
    {
        handleSerialMsg();
        currentPlant = 0;
        int i=0;
        for(;i<time_between_readings_seconds;i++)
        {
            //look for new messages at least every minute
            if (i % MINIMUM_TIME_TO_CHECK_MESSAGES_SECONDS == 0 && handleSerialMsg())
            {
                break;
            }
            delay(1000);
        }
    }
    else
    {
        currentPlant++;
    }
}

void selectPlant(int currentPlant)
{
    digitalWrite(PLANT_SELECT_PINS[3], currentPlant & 8 ? HIGH : LOW);
    digitalWrite(PLANT_SELECT_PINS[2], currentPlant & 4 ? HIGH : LOW);
    digitalWrite(PLANT_SELECT_PINS[1], currentPlant & 2 ? HIGH : LOW);
    digitalWrite(PLANT_SELECT_PINS[0], currentPlant & 1 ? HIGH : LOW);
}

/**
 * Converts the raw sensor value (0-1023) to
 * a moisture percentage (0.0 - 54.031)
 *
 * Calibrated as (water mass (g)/(water mass (g) + soil mass (g)))
 */
float getMoisturePercentage(long raw)
{
    return -0.039 * raw + 54.031;
}

float getThickness(long raw)
{
    //return -(2 *(250 * raw - 167919)) / 54887.0;
    return -109.774 * raw + 671.676;
}

boolean handleSerialMsg()
{
  boolean hasMessage = false;
  while(true)
  {
    Serial.println("Entering handleSerialMsg() while loop");
    String input = "";
    delay(500);
    if (Serial.available() > 0) {
      input = Serial.readString();
      Serial.print("Message: ");
      Serial.println(input);

      // decide how to handle message based on second byte
      String ack = "";
      if (input.length() > 1)
      {
          hasMessage = true;
          switch (input[1])
          {
            case STUMPY_SET_WATER_CONTENT:
              ack = setWaterContent(input);
              break;
            case STUMPY_SET_NUM_PLANTS:
              ack = setNumberPlants(input);
              break;
            case STUMPY_SET_DELAY_TIME_SECONDS:
              ack = setDelayTime(input);
              break;
            case STUMPY_SET_WATERING_TIME_SECONDS:
              ack = setWateringTime(input);
              break;
            default:
              break;
          }
      }

      // acknowledge message to controller
      sendAck(ack);

      // break if there is no more to read
      if(input[0] != 'M')
      {
          Serial.println("No more to read.");
          break;
      }

      // at this point there is more to read so we wait
      // wait a little bit for the controller to recieve the ack and send more data
      Serial.println("More Data");
      delay(2000);
    }
    // nothing to read, continue on
    else
    {
        Serial.println("Leaving handleSerialMsg()");
        break;
    }
  }
  return hasMessage;
}

void sendAck(String ack)
{
    Serial.println(ack);
}

String setWaterContent(String packet)
{
    Serial.println("Entering setWaterContent()");

    // break data into two strings, plant index and water content for that plant
    int indexOfComma = packet.indexOf(',');
    String plantIndexStr = packet.substring(2, indexOfComma);
    String waterContentStr = packet.substring(indexOfComma+1);

    Serial.print("plantIndexStr ");
    Serial.print(plantIndexStr);
    Serial.print(", waterContentStr ");
    Serial.println(waterContentStr);

    int plantIndex = plantIndexStr.toInt();
    float waterContent = waterContentStr.toFloat();

    Serial.print("plantIndex ");
    Serial.print(plantIndex);
    Serial.print(", waterContent ");
    Serial.println(waterContent);

    if (plantIndex < NUM_PLANTS)
    {
        PLANT_WATER_THRESHOLDS[plantIndex] = waterContent;
    }

    int i;
    for(i=0; i<NUM_PLANTS;i++)
    {
      Serial.print("plant:");
      Serial.print(i);
      Serial.print(" thresh:");
      Serial.println(PLANT_WATER_THRESHOLDS[i]);
    }

    return "ACK";
}

String setNumberPlants(String packet)
{
    NUM_PLANTS = packet.substring(2).toInt();
    return "ACK";
}

String setDelayTime(String packet)
{
    time_between_readings_seconds = packet.substring(2).toInt();
    return "ACK";
}

String setWateringTime(String packet)
{
    watering_time_seconds = packet.substring(2).toFloat();
    return "ACK";
}
