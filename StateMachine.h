#include "mbed.h"
#include "TextLCD.h"

//state ID
typedef enum {
    state_unset = 1,
    state_exit,
    state_set,
    state_entry,
    state_alarm,
    state_report
}State;


//Event ID
typedef enum{
    sensor_activate = 1,
    no_sensor_activate,
    right_code,
    wrong_code_3times,
    button_C,
    button_B,
    no_sensor_activate_and_time_out
}EventID;

// State table
typedef struct {
    EventID event;          // Event
    State CurState;         // Current state
    void (*eventActFun)();  // Function pointer
    State nextState;        // Next state
} StateTable;

// Declaration of functions
void f_unset2exit();
void f_unset2alarm();

void f_exit2unset();
void f_exit2set();
void f_exit2alarm();

void f_set2entry();
void f_set2alarm();

void f_entry2unset();
void f_entry2alarm();

void f_alarm2entry();
void f_alarm2report();

void f_report2unset();


//type of state machine
typedef struct {
    int curState;               //current state
    StateTable* stateTable;     //state table
    int size;                   //number of items in the table 
}fsmType;


void fsmRegit(fsmType* pFsm, StateTable* pTable);
void fsmStateTransfer(fsmType* pFsm, int state);
void fsmEventHandle(fsmType* pFsm, int event);

void no_condistion_match();

extern BusOut cols_out; 
extern BusIn rows_in;
extern Timer timer_alarm_brinky;
extern Timer timer_alarm_turn_led_on;
extern Timer timer_exit;
extern TextLCD lcd;
extern DigitalOut myled;
extern DigitalOut led2;
extern DigitalOut led3;
extern DigitalOut led4;