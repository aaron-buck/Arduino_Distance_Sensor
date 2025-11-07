#ifdef __cplusplus
 extern "C" {
#endif
#ifndef LAB2_INCLUDED
#define LAB2_INCLUDED

/*
 *      EE 474 Lab 3
 *      Spring 2024
 *
 *      Student Names: Aaron Buck, Nic Bohac
 *      Student IDs: 1903822, 2276637
 */
#include <stdio.h>
#include <stdint.h>
#include <Arduino.h>

// timing
extern unsigned long TIMER;
#define PERIOD       1000 //us
extern volatile int PC;

// Task1

#define BIT_0        1

extern int T1_INDEX;

// Task2
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


extern int T2_INDEX;


// Task3
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

#define TIMEON       1000 // us
// might need to flip
#define OFF          1
#define ON           0

extern int T3_INDEX;


void assign(int digit, int value);

// Task4
extern int T41_INDEX;
extern int T42_INDEX;


// Task5

extern int T51_INDEX;
extern int T52_INDEX;
extern int T53_INDEX;

void smile();



// SRRI
#define BIT_5       1<<5
#define BIT_4       1<<4
#define BIT_1       1<<1
#define SLEEPING    0
#define READY       1
#define RUNNING     2

#define PENDING     0
#define DONE        1


extern void (*functions[10])();
extern int function_states[10];
extern int sleep_count[10];
extern int NUMFUNCS;

extern int SS_INDEX;
extern volatile int sFlag;

void sleep(int task_index, int t);
void schedule_sync();


// DDS

#define DEAD        0

typedef struct TCB {
  void (*FUNCTION)();
  int STATE;
  int SLEEP;
  int ID; // where it is in the task list
  char* NAME;
  int NUMCALLS;
} TCB;

extern TCB *tasks[10];
extern volatile int states[10];

extern TCB *curr;

extern TCB T1;
extern TCB T2;
extern TCB SYNC;
extern TCB T41;
extern TCB T42;


void task_self_quit();
void task_start(TCB *task);

// Schedulers
void SRRI();
void DDS();

// Tasks
extern void Task1();
extern void Task2RR();
extern void Task2SRRI();
extern void Task3();
extern void Task4_1();
extern void Task4_2();
extern void Task5_1();
extern void Task5_2();
extern void Task5_3();

// Demos
void Demo1();
void Demo2();
void Demo3();
void Demo4();
void Demo5();
void Demo6();




#endif // C_PROG2_H_INCLUDED
#ifdef __cplusplus
}
#endif
