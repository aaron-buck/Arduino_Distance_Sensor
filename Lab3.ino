#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "Lab3.h"

/*
 *      EE 474 Lab 3
 *      Spring 2024
 *
 *      Student Names: Aaron Buck, Nic Bohac
 *      Student IDs: 1903822, 2276637
*/

// extern variables from header file
unsigned long TIMER;
volatile int PC = 0;

void (*functions[10])();
int function_states[10];
int sleep_count[10];
volatile int sFlag = PENDING;

int NUMFUNCS;
int T1_INDEX;
int T2_INDEX;
int T3_INDEX;
int T41_INDEX;
int T42_INDEX;
int T51_INDEX;
int T52_INDEX;
int T53_INDEX;
int SS_INDEX;

TCB *tasks[10];
volatile int states[10];

TCB *curr;

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);

  // Task1
  DDRL |= BIT_0; // pin 49/PL0

  // Task2
  DDRH |= BIT_3; //pin 6/PH3
  TCCR4A = 0;
  TCCR4B = 0;
  OCR4A = 0; // intialize to inaudible
  TCCR4B |= SET_WGM; // set to CTC mode
  TCCR4A |= OUTPUT_MODE; // set to toggle
  TCCR4B |= PRESCALER; // set prescaler to 8

  // Task3

  // Arduino(display) pin: 22(11), 23(7), 24(4), 25(2), 26(1), 27(10), 28(5) 29(3)
  // Arduino(display) pin: 30(12), 31(9), 32(8), 33(6)
  int pinLed[8]={SEGA, SEGB, SEGC, SEGD, SEGE, SEGF, SEGG, DP};
  int pinControl[4]={DIG_1, DIG_2, DIG_3, DIG_4};

  for (int i=0; i<=7; i++) {
  pinMode(pinLed[i], OUTPUT);
  }
  for (int j=0; j<=3; j++) {
    pinMode(pinControl[j], OUTPUT);
  }
  digitalWrite(29, OFF);  // start with the DP off
  // start with SS displays off
  digitalWrite(30, OFF);
  digitalWrite(31, OFF);
  digitalWrite(32, OFF);
  digitalWrite(33, OFF);

  // SRRI/DDS
  DDRB |= BIT_5; // pin 11/PB5
  TCCR1A = 0;
  TCCR1B = 0;
  TIFR1 = 0;
  OCR1A = 1999; // 500 Hz (every 2 ms)
  TCCR1B |= SET_WGM; // set to CTC mode
  TCCR1A |= OUTPUT_MODE; // set to toggle
  TCCR1B |= PRESCALER; // set prescaler to 8
  TIMSK1 |= BIT_1; // Set OCF1A
  


  // // Demo2(SRRI setup)
  // NUMFUNCS = 3;
  // T1_INDEX = 0;
  // T2_INDEX = 1;
  // SS_INDEX = 2;
  // functions[T1_INDEX] = Task1;
  // functions[T2_INDEX] = Task2SRRI;
  // functions[SS_INDEX] = schedule_sync;
  // functions[NUMFUNCS] = NULL;
  // function_states[T1_INDEX] = READY;
  // function_states[T2_INDEX] = READY;
  // function_states[SS_INDEX] = READY;
  // sleep_count[T1_INDEX] = 0;
  // sleep_count[T2_INDEX] = 0;
  // sleep_count[SS_INDEX] = 0;

  
  // // Demo3(DDS setup)

  // NUMFUNCS = 3;
  // T1_INDEX = 0;
  // T2_INDEX = 1;
  // SS_INDEX = 2;

  // TCB T1;
  // T1.FUNCTION = Task1;
  // T1.STATE = READY;
  // T1.SLEEP = 0;
  // T1.ID = T1_INDEX;
  // T1.NAME = "Flash LEDs";
  // T1.NUMCALLS = 0;

  // TCB T2;
  // T2.FUNCTION = Task2SRRI;
  // T2.STATE = READY;
  // T2.SLEEP = 0;
  // T2.ID = T2_INDEX;
  // T2.NAME = "Play Mario Theme";
  // T2.NUMCALLS = 0;

  // TCB SYNC;
  // SYNC.FUNCTION = schedule_sync;
  // SYNC.STATE = READY;
  // SYNC.SLEEP = 0;
  // SYNC.ID = SS_INDEX;
  // SYNC.NAME = "Synchronizer";
  // SYNC.NUMCALLS = 0;

  // tasks[T1.ID] = &T1;
  // tasks[T2.ID] = &T2;
  // tasks[SYNC.ID] = &SYNC;
  // tasks[NUMFUNCS] = NULL;
  // states[T1.ID] = T1.STATE;
  // states[T2.ID] = T2.STATE;
  // states[SYNC.ID] = SYNC.STATE;

  // // Demo4(SRRI setup)
  // NUMFUNCS = 4;
  // T1_INDEX = 0;
  // T2_INDEX = 1;
  // T3_INDEX = 2;
  // SS_INDEX = 3;
  // functions[T1_INDEX] = Task1;
  // functions[T2_INDEX] = Task2SRRI;
  // functions[T3_INDEX] = Task3;
  // functions[SS_INDEX] = schedule_sync;
  // functions[NUMFUNCS] = NULL;
  // function_states[T1_INDEX] = READY;
  // function_states[T2_INDEX] = READY;
  // function_states[T3_INDEX] = READY;
  // function_states[SS_INDEX] = READY;
  // sleep_count[T1_INDEX] = 0;
  // sleep_count[T2_INDEX] = 0;
  // sleep_count[T3_INDEX] = 0;
  // sleep_count[SS_INDEX] = 0;

  // // Demo5(DDS setup)
  // NUMFUNCS = 3;
  // T41_INDEX = 0;
  // T42_INDEX = 1;
  // SS_INDEX = 2;

  // TCB T41;
  // T41.FUNCTION = Task4_1;
  // T41.STATE = READY;
  // T41.SLEEP = 0;
  // T41.ID = T41_INDEX;
  // T41.NAME = "Play Mario";
  // T41.NUMCALLS = 0;

  // TCB T42;
  // T42.FUNCTION = Task4_2;
  // T42.STATE = READY;
  // T42.SLEEP = 0;
  // T42.ID = T42_INDEX;
  // T42.NAME = "HEX Driver";
  // T42.NUMCALLS = 0;

  // TCB SYNC;
  // SYNC.FUNCTION = schedule_sync;
  // SYNC.STATE = READY;
  // SYNC.SLEEP = 0;
  // SYNC.ID = SS_INDEX;
  // SYNC.NAME = "Synchronizer";
  // SYNC.NUMCALLS = 0;

  // tasks[T41.ID] = &T41;
  // tasks[T42.ID] = &T42;
  // tasks[SYNC.ID] = &SYNC;
  // tasks[NUMFUNCS] = NULL;
  // states[T41.ID] = T41.STATE;
  // states[T42.ID] = T42.STATE;
  // states[SYNC.ID] = SYNC.STATE;

  // Demo 6
  NUMFUNCS = 5;
  T1_INDEX = 0;
  T51_INDEX = 1;
  T52_INDEX = 2;
  T53_INDEX = 3;
  SS_INDEX = 4;

  TCB T1;
  T1.FUNCTION = Task1;
  T1.STATE = READY;
  T1.SLEEP = 0;
  T1.ID = T1_INDEX;
  T1.NAME = "Flash LEDs";
  T1.NUMCALLS = 0;

  TCB T51;
  T51.FUNCTION = Task5_1;
  T51.STATE = READY;
  T51.SLEEP = 0;
  T51.ID = T51_INDEX;
  T51.NAME = "Play Mario";
  T51.NUMCALLS = 0;

  TCB T52;
  T52.FUNCTION = Task5_2;
  T52.STATE = DEAD; // starts dead
  T52.SLEEP = 0;
  T52.ID = T52_INDEX;
  T52.NAME = "Countdown";
  T52.NUMCALLS = 0;

  TCB T53;
  T53.FUNCTION = Task5_3;
  T53.STATE = DEAD; // starts dead
  T53.SLEEP = 0;
  T53.ID = T53_INDEX;
  T53.NAME = "Smile";
  T53.NUMCALLS = 0;

  TCB SYNC;
  SYNC.FUNCTION = schedule_sync;
  SYNC.STATE = READY;
  SYNC.SLEEP = 0;
  SYNC.ID = SS_INDEX;
  SYNC.NAME = "Synchronizer";
  SYNC.NUMCALLS = 0;

  tasks[T1.ID] = &T1;
  tasks[T51.ID] = &T51;
  tasks[T52.ID] = &T52;
  tasks[T53.ID] = &T53;
  tasks[SYNC.ID] = &SYNC;
  tasks[NUMFUNCS] = NULL;

  states[T1.ID] = T1.STATE;
  states[T51.ID] = T51.STATE;
  states[T52.ID] = T52.STATE;
  states[T53.ID] = T53.STATE;
  states[SYNC.ID] = SYNC.STATE;

  
  unsigned long TIMER = micros();
}

// SRRI timer
ISR(TIMER1_COMPA_vect) {
  sFlag = DONE;
  PC++;
}

// the loop function runs over and over again forever
void loop() {

  // Demo1();

  // Demo2();

  // Demo3();

  // Demo4();

  // Demo5();

  Demo6();
  

}
