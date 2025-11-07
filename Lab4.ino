/*
Aaron Buck
Nic Bohac

EE 474 Lab 4
June 6, 2024

*/

#include <Arduino_FreeRTOS.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>


// #include "Lab4.h"

/* Used pins
Analog stick: A6 = VRx, A7 = VRy
External LED: 40 = OUTPUT
*/

#define E            1516 // 659 Hz
#define C            1912 // 523 Hz
#define G            1274 // 784 Hz
#define g            2550 // 392 Hz
#define R            0 // Rest, play no sound
#define SONGSIZE     21
#define NOTEDURATION 100000 //us
#define REST         4000 //ms
#define BIT_3        1<<3
#define SET_WGM      1<<3 // CTC = 0100 and TCCR4B[3] = WGM42
#define OUTPUT_MODE  1<<6 // COM4A[1:0] = 1, and TCCR4A[7:6] = COM4A
#define PRESCALER    1<<1 // Prescaler is N = 8


// define tasks here
void TaskBlink(void *pvParameters);
void TaskAnalogRead(void *pvParameters);
void TaskBlinkLED(void *pvParameters);
void TaskMarioSong(void *pvParameters);

// the setup function runs once when you press reset or power the board
void setup() {

  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB, on LEONARDO, MICRO, YUN, and other 32u4 based boards.
  } 

  // song stuff
  DDRH |= BIT_3; //pin 6/PH3
  TCCR4A = 0;
  TCCR4B = 0;
  OCR4A = 0; // intialize to inaudible
  TCCR4B |= SET_WGM; // set to CTC mode
  TCCR4A |= OUTPUT_MODE; // set to toggle
  TCCR4B |= PRESCALER; // set prescaler to 8

  // Now set up two tasks to run independently.
  xTaskCreate(
    TaskBlink
    ,  "Blink"   // A name just for humans
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );

  xTaskCreate(
    TaskAnalogRead
    ,  "AnalogRead"
    ,  128  // Stack size
    ,  NULL
    ,  1  // Priority
    ,  NULL );

  xTaskCreate(
    TaskBlinkLED
    , "BlinkLED"
    , 128
    , NULL
    , 2
    , NULL );

  xTaskCreate(
    TaskMarioSong
    , "MarioSong"
    , 128
    , NULL
    , 1 
    , NULL );

  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
  //  (note how the above comment is WRONG!!!)
  vTaskStartScheduler();


}

void loop()
{
  // Empty. Things are done in Tasks.
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void TaskBlink(void *pvParameters)  // This is a task.
{
 // (void) pvParameters;  // allocate stack space for params

/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, LEONARDO, MEGA, and ZERO 
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN takes care 
  of use the correct LED pin whatever is the board used.
  
  The MICRO does not have a LED_BUILTIN available. For the MICRO board please substitute
  the LED_BUILTIN definition with either LED_BUILTIN_RX or LED_BUILTIN_TX.
  e.g. pinMode(LED_BUILTIN_RX, OUTPUT); etc.
  
  If you want to know what pin the on-board LED is connected to on your Arduino model, check
  the Technical Specs of your board  at https://www.arduino.cc/en/Main/Products
  
  This example code is in the public domain.

  modified 8 May 2014
  by Scott Fitzgerald
  
  modified 2 Sep 2016
  by Arturo Guadalupi
*/

  // initialize digital LED_BUILTIN on pin 13 as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  for (;;) // A Task shall never return or exit.
  {
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    vTaskDelay( 1000 / portTICK_PERIOD_MS ); // wait for one second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    vTaskDelay( 1000 / portTICK_PERIOD_MS ); // wait for one second
  }
}

void TaskBlinkLED(void *pvParameters) {
  pinMode(40, OUTPUT);
  for (;;) {
    digitalWrite(40, HIGH);
    vTaskDelay( 100 / portTICK_PERIOD_MS );
    digitalWrite(40, LOW);
    vTaskDelay( 200 / portTICK_PERIOD_MS );
  }
}

void TaskAnalogRead(void *pvParameters) {
  for (;;)
  {
    int sensorValueX = analogRead(A6);
    Serial.print("X: ");
    Serial.print(sensorValueX);
    int sensorValueY = analogRead(A7);  /// modify for your input pin!
    // print out the value you read:
    Serial.print(" Y: ");
    Serial.println(sensorValueY);
    vTaskDelay(500/portTICK_PERIOD_MS);  // 0.5 sec in between reads for stability
  }
}

int song[] = {E, R, E, R, R, E, R, R, C, R, E, R, R, G, R, R, R, R, R, g, R};

void TaskMarioSong(void *pvParameters) {
  for (;;) 
  {
    static int note = 0;
    if (note >= SONGSIZE) {
      OCR4A = 0;
      note = 0;
      vTaskDelay(4000/portTICK_PERIOD_MS);
    } else {
      OCR4A = song[note % SONGSIZE];
      note++;
      vTaskDelay(100/portTICK_PERIOD_MS);
    }
  }

}
