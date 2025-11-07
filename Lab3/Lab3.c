/*
 *
 *      EE 474 Lab 3
 *      Spring 2024
 *
 *      Student Names: Aaron Buck, Nic Bohac
 *      Student IDs: 1903822, 2276637

 */
#include <stdio.h>
#include <stdint.h>
#include <Arduino.h>

#include "Lab3.h"

// extern variables from header file
unsigned long TIMER;


void (*functions[10])();
int function_states[10];
int sleep_count[10];
volatile int sFlag;
volatile int PC;
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

TCB T1;
TCB T2;
TCB SYNC;
TCB T41;
TCB T42;
TCB T51;
TCB T52;
TCB T53;


// TASK 1
void Task1() {
  // turn off LED if in last 750 ms of LED period
  if ((micros() - TIMER) % 1000000 >= 250000) {
    PORTL &= ~BIT_0;
  } else {
    PORTL |= BIT_0;
  }
}

// TASK 2
int song[] = {E, R, E, R, R, E, R, R, C, R, E, R, R, G, R, R, R, R, R, g, R};
static int note = 0;
static int loops = 0;

void Task2RR() {
  static int COUNT = 0;
  if (COUNT % 100 == 0 && COUNT < 2100) {
    OCR4A = song[note % SONGSIZE];
    note++;
    
  }
  if (COUNT == 2100) {
    OCR4A = 0;
    note = 0;
  }
  if (COUNT == 6100) {
    COUNT = 0;

  }
  COUNT++;
}

void Task2SRRI() {
  if (PC % 100 == 0 && PC < 2100) {
    OCR4A = song[note % SONGSIZE];
    note++;
    
  }
  if (PC == 2100) {
    OCR4A = 0;
    note = 0;
  }
  if (PC == 6100) {
    PC = 0;
  }
}

// TASK 3
int pinLed[8]={SEGA, SEGB, SEGC, SEGD, SEGE, SEGF, SEGG, DP};
int pinControl[4]={DIG_4, DIG_3, DIG_2, DIG_1};


// Array taken from: https://wokwi.com/projects/373742871570809857
// 0 = HIGH in this case(might need to be flipped)
bool bitmap[10][8]={ 
                  {1,1,1,1,1,1,0,1}, //0
                  {0,1,1,0,0,0,0,1}, //1
                  {1,1,0,1,1,0,1,1}, //2
                  {1,1,1,1,0,0,1,1}, //3
                  {0,1,1,0,0,1,1,1}, //4
                  {1,0,1,1,0,1,1,1}, //5
                  {1,0,1,1,1,1,1,1}, //6
                  {1,1,1,0,0,0,0,1}, //7
                  {1,1,1,1,1,1,1,1}, //8
                  {1,1,1,1,0,1,1,1}  //9
                  };

void Task3() {
  static int display_val = 0;
  static int digit = 0;
  static int oscillator = 1;

  if (PC % 100 == 0) {
    display_val++;
  }

  if (oscillator == 1) {
    int temp = 1;
    int i = 0;
    while (i < digit) {
      temp *= 10;
      i++;
    }
    digitalWrite(DP, ON);
    assign(pinControl[digit], (display_val/temp) % 10);
    oscillator--;
  } else {
    digitalWrite(pinControl[digit], OFF);
    if (digit == 3) {
      digit = 0;
    } else {
      digit++;
    }

    oscillator++;
    sleep(T3_INDEX, 2);
  }

  if (display_val > 9999) {
    display_val = 0;
    digit = 0;
  }
}

// writes a given seven segment number(int value) to a given SS digit(int digit) 
void assign(int digit, int value) {
  for(int j = 0; j < 4; j++) {
    if (digit != pinControl[j]) {
      digitalWrite(pinControl[j], OFF);

    } else {
      digitalWrite(pinControl[j], ON);
    }

  }
  for (int i = 0; i < 7; i++) {
    digitalWrite(pinLed[i], bitmap[value][i]);
  }
}

// TASK 4
void Task4_1() {
  if (PC % 400 == 0 && PC < 8400) {
    OCR4A = song[note % SONGSIZE];
    note++;
  }
  if (PC == 8400) {
    OCR4A = 0;
    note = 0;
  }
  if (PC == 12400) {
    PC = 0;
  }
}

void Task4_2() {
  static int display_val = 0;
  static int digit = 0;
  static int oscillator = 1;
  
  if (PC >= 0 && PC < 8400) {
    if (song[note % SONGSIZE] == R) {
      display_val = 0;
    } else if (song[note % SONGSIZE] == E) {
      display_val = 659;
    } else if (song[note % SONGSIZE] == C){
      display_val = 523;
    } else if (song[note % SONGSIZE] == G){
      display_val = 784;
    } else if (song[note % SONGSIZE] == g){
      display_val = 392;
    } else {
      display_val = 0;
    }
    digitalWrite(DP, ON);
  }
  if (PC == 8400) {
    display_val = 40;
  }

  if (PC > 8400 && PC < 12400) {
    if (PC % 100 == 0) {
      display_val--;
    }

    if (digit == 1) {
        digitalWrite(DP, OFF);
    } else {
        digitalWrite(DP, ON);
    }
  }

  if (oscillator == 1) {
    int temp = 1;
    int i = 0;
    while (i < digit) {
      temp *= 10;
      i++;
    }
    assign(pinControl[digit], (display_val/temp) % 10);
    oscillator--;
  } else {
    digitalWrite(pinControl[digit], OFF);
    if (digit == 3) {
      digit = 0;
    } else {
      digit++;
    }
    oscillator++;
    sleep(T42_INDEX, 2);
  }

}

// Task 5
bool smilemap[3][8]={
                  {0,1,0,0,1,0,1,0}, /* \ */ 
                  {1,0,0,1,0,0,0,0}, /* = */
                  {0,0,1,0,0,1,1,0}, /* / */
                  };

static int plays = 0;


void Task5_1() {
  if (PC % 100 == 0 && PC < 2100) {
    OCR4A = song[note % SONGSIZE];
    note++;
  }
  if (PC == 2100) {
    OCR4A = 0;
    note = 0;
    plays++;
  }

  if (PC == 5100) {
    PC = 0;
  }

  if (plays > 3) {
    // task_start(tasks[T53_INDEX]);
    note = 0;
    OCR4A = 0;
    task_self_quit();
  } 
}

void Task5_2() {
  static int display_val = 30;
  static int digit = 0;
  static int oscillator = 1;

  if (PC == 5100) {
    PC = 0;
  }

  if (plays == 2 && PC <= 2100 && PC >= 0) {
    digitalWrite(pinControl[0], OFF);
    digitalWrite(pinControl[1], OFF);
    digitalWrite(pinControl[2], OFF);
    digitalWrite(pinControl[3], OFF);

    task_self_quit();
  } else if (plays == 2) {
    if (PC % 100 == 0) {
      display_val--;
    }


    if (digit == 1) {
      digitalWrite(DP, OFF);
    } else {
      digitalWrite(DP, ON);
    }

    if (oscillator == 1) {
      int temp = 1;
      int i = 0;
      while (i < digit) {
        temp *= 10;
        i++;
      }
      assign(pinControl[digit], (display_val/temp) % 10);
      oscillator--;
    } else {
      digitalWrite(pinControl[digit], OFF);
      if (digit == 3) {
        digit = 0;
      } else {
        digit++;
      }
      oscillator++;
      sleep(T52.ID, 2);
    }
  }
}

void Task5_3() {
  static int digit = 0;
  static int oscillator = 1;

  if (PC == 5100) {
      PC = 0;
  }

  if (plays == 4 && ((PC < 2100 && PC >= 0) || PC > 4100)) {
    digitalWrite(pinControl[0], OFF);
    digitalWrite(pinControl[1], OFF);
    digitalWrite(pinControl[2], OFF);
    digitalWrite(pinControl[3], OFF);

    task_self_quit();
  } else if (plays == 4) {
    if (oscillator == 1) {
      int temp = 1;
      int i = 0;
      while (i < digit) {
        temp *= 10;
        i++;
      }
      digitalWrite(DP, ON);
      smile(pinControl[digit]);
      oscillator--;
    } else {
      digitalWrite(pinControl[digit], OFF);
      if (digit == 3) {
        digit = 0;
      } else {
        digit++;
      }
      oscillator++;
      sleep(T53.ID, 2);
    }
  
  }
}


void smile(int digit) {
  // make sure we get the right part of the smile for the HEX digit
  int value;
  if (digit == pinControl[0]){
    value = 0;
  } else if (digit == pinControl[3]) {
    value = 2;
  } else {
    value = 1;
  }

  for(int j = 0; j < 4; j++) {
    if (digit != pinControl[j]) {
      digitalWrite(pinControl[j], OFF);

    } else {
      digitalWrite(pinControl[j], ON);
    }

  }
  for (int i = 0; i < 7; i++) {
    digitalWrite(pinLed[i], smilemap[value][i]);
  }
}




// SRRI
void SRRI() {
  int i = 0;
  while (functions[i] != NULL) {
    if (function_states[i] != SLEEPING) {
      function_states[i] = RUNNING;
      (*functions[i])();
      function_states[i] = READY;
    }
    i++;
  }
}

// sleep
void sleep(int task_index, int t) {
  // SRRI
  sleep_count[task_index] = t;
  function_states[task_index] = SLEEPING;

  // DDS
  tasks[task_index]->SLEEP = t;
  tasks[task_index]->STATE = SLEEPING;
  states[task_index] = SLEEPING;
  return;
}


void schedule_sync() {
  while (sFlag == PENDING) {
  }
  for(int j = 0; j < NUMFUNCS; j++) {
    // DDS
    tasks[j]->SLEEP  -= 2;
    if (tasks[j]->SLEEP < 2) {
      tasks[j]->STATE = READY;
      
    }

    // SRRI
    sleep_count[j] -= 2;
    if (sleep_count[j] < 2)
      function_states[j] = READY;
    }
  sFlag = PENDING;
}

// DDS
TCB *curr;

void DDS() {
  int i = 0;
  while (tasks[i] != NULL) {
    curr = tasks[i];
    if (curr->STATE == READY /*|| states[i] == READY*/) {
      curr->STATE = RUNNING;

      (*(curr->FUNCTION))();
      curr->NUMCALLS++;

      curr->STATE = READY;
    }
    i++;
  }
}

void task_self_quit() {
  curr->STATE = DEAD;
  states[curr->ID] = DEAD;
}

void task_start(TCB *task) {
  task->STATE = READY;
  states[task->ID] = READY;
}


// DEMO 1
void Demo1() {
  // cycle through and run all READY functions
  Task1();
  Task2RR();
  // wait until end of period (1 ms)
  delayMicroseconds(1000);
  
}

// DEMO 2
void Demo2() {
  SRRI();

}
//DEMO 3
void Demo3() {
  DDS();
}
//DEMO 4
void Demo4() {
  SRRI();
}
//DEMO 5
void Demo5() {
  DDS();
}
//DEMO 6
void Demo6() {
  DDS();
}



