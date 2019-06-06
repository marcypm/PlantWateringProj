/* Automated Plant Watering System

   This Program will take in readings from a moisture sensor and water the plant accordingly
   An LCD display is also used to display moisture information, threshold to water, and last watering
   Data will be sent serially to be logged.
   Serial commands will also be implemented to overwrite program parameters and force actions

   created May 29th, 2019
   by Marcel Morin

   Last modified May 30th, 2019
*/

#include "lcdDisplay.h"

//Defining starting values, these will be able to be overwritten when the program starts
#define startThresh 103         //Default threshold on startup = 10%
#define startWateringTime 13000 //Default watering time on startup
#define startSamplingRate 8000  //Default sampling rate on startup

const int plantSensor1 = A0;// sensor pins
const int pump = 3;         // PWM pin for pump
const int controlPin1 = 4;  // connected to pin 7 on the H-bridge
const int controlPin2 = 2;  // connected to pin 2 on the H-bridge


enum State_enum {POLLING, WATER, WATERING, CONFIRM_WATER, TANK_EMPTY, JUST_POLL, END};
uint8_t state = POLLING;


unsigned long wateredTimeStamp = 0; //Holds the millis time when last watered
unsigned long wateringStart = 0;    //Used as starting timestamp of when watering starts
unsigned long lastPolling = 0;      //Used as timestamp of last sample
unsigned long lastloop = 0;         //Used as timestamp of last loop
unsigned long currentMillis = 0;    //Hold current time
unsigned long  lastWatered = 0;     //currentMillis-wateredTimeStamp
int sensorReading = 0;              //Holds sensor reading
int index = 0;
int last3[3] = {1000, 1000, 1000};
int avg = 1000;
float moisture = 100;

//These variables can be overwritten via Serial commands
int samplingRate = startSamplingRate; //rate at which to acquire
float threshold = startThresh;          //threshold to water at
unsigned long wateringTime = startWateringTime; //

const int loopRate = 500; //everyhalf second check for commands at least
unsigned long lastLoop = 0;


unsigned long timeBetween(unsigned long pastTime, unsigned long currentTime);
int average(int *samples);
void getSerialCharArray(char *charHolder);
void parseCommand(char *command);

void setup() {
  lcd.SETUP();
  pinMode(controlPin1, OUTPUT);
  pinMode(controlPin2, OUTPUT);
  pinMode(pump, OUTPUT);
  digitalWrite(pump, LOW);
  digitalWrite(controlPin1, LOW);
  digitalWrite(controlPin2, HIGH);
  analogWrite(pump, 0);
  delay(500);
  Serial.begin(9600);
}

void loop() {
  currentMillis = millis();

  if (timeBetween(lastLoop, currentMillis) >= loopRate) {
    lastLoop = currentMillis;

    if (timeBetween(lastPolling, currentMillis) >= samplingRate) { //acquire, send data serially
      sensorReading = analogRead(plantSensor1); //from 0-1023
      last3[index++] = sensorReading;
      index = index % 3; //inedx loops back around 0-2
      avg = average(last3);

      moisture = sensorReading * (100.0 / 1023.0);

      //TODO: overflow of millis?
      lastWatered = timeBetween(wateredTimeStamp, currentMillis) / 1000; //in seconds
      lastPolling = currentMillis;
      Serial.print(" data: ");
      Serial.print(sensorReading);
      Serial.print(" ");
      Serial.print(avg);
      Serial.print(" ");
      Serial.println(state);
    }

    char cmd[30] = "";
    getSerialCharArray(cmd);

    parseCommand(cmd);//will change system variables or change FSM state

    //the above code will always execute every interval
    //the behavior to follow depends on the system state

    switch (state) {
      case POLLING:
        {
          //Serial.println("in POLLING");
          analogWrite(pump, 0);
          lcd.updateLCD(1, moisture, (threshold / 1024) * 100, lastWatered);
          if (avg < threshold) state = WATER;
        }
        break;

      case WATER:
        {
          //Serial.println("in WATER");
          analogWrite(pump, 100);
          lcd.waterLCD(2);
          wateringStart = currentMillis;
          state = WATERING;
        }
        break;

      case WATERING:
        {
          //Serial.println("WATERING..");
          if (timeBetween(wateringStart, currentMillis) >= wateringTime) {
            state = CONFIRM_WATER;
          }
        }
        break;

      case CONFIRM_WATER:
        {
          //Serial.println("in CONFIRM_WATER");
          analogWrite(pump, 0);

          if (avg > threshold) {
            state = POLLING;
            wateredTimeStamp = millis();
          } else {
            state = TANK_EMPTY;
          }
        }
        break;

      case TANK_EMPTY:
        {
          //Serial.println("in TANK_EMPTY");
          lcd.emptyTankLCD(3, moisture);
          if (avg > threshold) {
            wateredTimeStamp = millis();
            state = POLLING;
          }
        }
        break;

      case JUST_POLL:
        {
          //Serial.println("in JUST_POLL");
          lcd.justPollLCD(4, moisture);
          state = JUST_POLL;
        }
        break;

      case END: //enters infinite loop waiting for command to break out of it.
        {
          //Serial.println("in STOP");
          analogWrite(pump, 0);

          while (1) {
            lcd.stopLCD(5);
            char cmd2[30] = "";
            getSerialCharArray(cmd2);
            parseCommand(cmd2);//will change system variables or change FSM state
            if (state != END) break;
          }
        }
        break;
    }
  }
}


void parseCommand(char *command) { //dont use String class use char arrays
  char *tok;
  tok = strtok(command, " ");//tokenizing the command will get rid of \r\n at the end
  Serial.println(tok);

  if (strcmp(command, "default") == 0) {
    samplingRate = startSamplingRate;
    threshold = startThresh;
    wateringTime = startWateringTime;
    //Serial.println(command);
  } else if (strcmp(command, "water") == 0) {
    state = WATER;
    //Serial.println(command);
  } else if (strcmp(command, "poll") == 0) {
    state = POLLING;
    //Serial.println(command);
  } else if (strcmp(command, "stop") == 0) {
    state = END;
    //Serial.println(command);
    //Serial.println("STOPPING");
  } else if (strcmp(command, "empty") == 0) {
    state = TANK_EMPTY;
    //Serial.println(command);
  } else if (strcmp(command, "config") == 0) {
    state = CONFIRM_WATER;
    //Serial.println(command);
  } else {

    //char *firstArg;
    //firstArg = strtok(command, " ");

    if (strcmp(tok, "sample") == 0) {
      Serial.print("cmd:  ");
      Serial.println(tok);
      tok = strtok(NULL, " ");
      Serial.print("arg:  ");
      Serial.println(tok);
      int newSampling = atoi(tok);
      if (newSampling >= 1 && newSampling <= 3600) {
        samplingRate = newSampling * 1000;
      } else {
        Serial.println("~Value needs to be between 1 & 3600 seconds");
      }

    } else if (strcmp(tok, "thresh") == 0) {
      Serial.print("cmd:  ");
      Serial.println(tok);
      tok = strtok(NULL, " ");
      Serial.print("arg:  ");
      Serial.println(tok);
      int newThresh = atoi(tok);
      if (newThresh >= 1 && newThresh <= 90) {
        threshold = ((float)newThresh / 100) * 1024;
      } else {
        Serial.println("~Value needs to be between 1 & 90 %");
      }

    } else if (strcmp(tok, "watertime") == 0) {
      Serial.print("cmd:  ");
      Serial.println(tok);
      tok = strtok(NULL, " ");
      Serial.print("arg:  ");
      Serial.println(tok);
      int newWatering = atoi(tok);
      if (newWatering >= 3 && newWatering <= 20) {
        wateringTime = newWatering * 1000;
      } else {
        Serial.println("~Value needs to be between 3 & 20 seconds");
      }
    }

  }

}

unsigned long timeBetween(unsigned long pastTime, unsigned long currentTime) {
  return currentTime - pastTime; //accounts for rollover?
}

int average(int *samples) {
  return (samples[0] + samples[1] + samples[2]) / 3;
}

void getSerialCharArray(char *charHolder) {
  String serialCmd = Serial.readString();
  serialCmd.toCharArray(charHolder, 30);
}
