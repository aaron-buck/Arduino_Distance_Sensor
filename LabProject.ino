/*
Aaron Buck
Nic Bohac

EE 474 Lab 4 Project
June 6, 2024

*/


#include <SevSeg.h>
#include <SoftwareSerial.h>
#include <Arduino_FreeRTOS.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>


SevSeg sevseg;

#define DIG_4        33
#define DIG_3        32
#define DIG_2        31
#define DIG_1        30
#define DP           29
#define SEGA         22
#define SEGB         23
#define SEGC         24
#define SEGD         25
#define SEGE         26
#define SEGF         27
#define SEGG         28

#define OFF          1
#define ON           0

// #define BLUE         13
#define TRIGGER      8
#define ECHO         9
#define LED          12

SoftwareSerial BT(10, 11); 

char a;
volatile float DISTANCE = 0;
volatile int PAUSED = 0;

void TaskBlinkLED(void *pvParameters);
TaskHandle_t BlinkLED = NULL;
void TaskHEXDriver(void *pvParameters);
TaskHandle_t HEXDriver = NULL;
void TaskGetDist(void *pvParameters);
TaskHandle_t GetDist = NULL;
void TaskBluetooth(void *pvParameters);
TaskHandle_t Bluetooth = NULL;


// the setup function runs once when you press reset or power the board
void setup() {

  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB, on LEONARDO, MICRO, YUN, and other 32u4 based boards.
  }
  // pinMode(BLUE, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);

  BT.begin(9600);


  byte numDigits = 4;
  byte digitPins[] = {DIG_1, DIG_2, DIG_3, DIG_4};
  byte segmentPins[] = {SEGA, SEGB, SEGC, SEGD, SEGE, SEGF, SEGG, DP};
  bool resistorsOnSegments = false; // 'false' means resistors are on digit pins
  byte hardwareConfig = COMMON_CATHODE; // See README.md for options
  bool updateWithDelays = false; // Default 'false' is Recommended
  bool leadingZeros = false; // Use 'true' if you'd like to keep the leading zeros
  bool disableDecPoint = true; // Use 'true' if your decimal point doesn't exist or isn't connected
  

  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments,
  updateWithDelays, leadingZeros, disableDecPoint);
  sevseg.setBrightness(90);

  xTaskCreate(
    TaskBlinkLED
    , "BlinkLED"
    , 128
    , NULL
    , 2
    , &BlinkLED );

  xTaskCreate(
    TaskHEXDriver
    , "Drive HEX"
    , 128
    , NULL
    , 2
    , &HEXDriver );

  xTaskCreate(
    TaskGetDist
    , "Gets Distance"
    , 128
    , NULL
    , 2
    , &GetDist );
  
  xTaskCreate(
    TaskBluetooth
    , "Runs Bluetooth"
    , 128
    , NULL
    , 2
    , &Bluetooth );

  


  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
  // (note how the above comment is WRONG!!!)
  vTaskStartScheduler();


}

void loop()
{
  // Empty. Things are done in Tasks.
}


void TaskBlinkLED(void *pvParameters) {
  for (;;) {
    digitalWrite(12, HIGH);
    vTaskDelay( 100 / portTICK_PERIOD_MS );
    digitalWrite(12, LOW);
    vTaskDelay( 200 / portTICK_PERIOD_MS );
  }
}

void TaskHEXDriver(void *pvParameters){
  for (;;) 
  {
    sevseg.refreshDisplay();
    sevseg.setNumberF(DISTANCE/10, 1);
  }
}

void TaskGetDist(void *pvParameters) {
  for (;;) 
  {
    digitalWrite(TRIGGER, LOW);
    delayMicroseconds(5);
    digitalWrite(TRIGGER, HIGH);
    delayMicroseconds(5);
    digitalWrite(TRIGGER, LOW);

    float duration = pulseIn(ECHO, HIGH);
    DISTANCE = (duration*0.0343)/2;

    vTaskDelay(400/portTICK_PERIOD_MS);
  }
}

void TaskBluetooth(void *pvParameters) {
  for (;;) 
  {
    if (BT.available())
  // if text arrived in from BT serial...
    {
      a=(BT.read());
      if (a == '1')
      {
        BT.print(DISTANCE);
        BT.println(" cm");
        BT.print((DISTANCE) * 0.394);
        BT.println(" in");
      } else if(a == '2') {
        if(PAUSED == 1) {
          vTaskResume(GetDist);
          BT.println("Measurement Unlocked");
          PAUSED = 0;

        } else {
          vTaskSuspend(GetDist);
          BT.println("Measurement Locked");
          PAUSED = 1;
        }
      }
      // you can add more "if" statements with other characters to add more commands
    }
    vTaskDelay(100/portTICK_PERIOD_MS);

  }

}

