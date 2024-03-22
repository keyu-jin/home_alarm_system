#include "StateMachine.h"
#include "alarm_LED.h"

// Implementation of functions
void f_unset2exit(){
    timer_alarm_brinky.start();
    timer_exit.start();
    lcd.cls();//clear the screen and locate to (0,0)
    lcd.printf("Exit");
};

void f_unset2alarm(){
    alarm_led_on();
    timer_alarm_turn_led_on.start();
    lcd.cls();//clear the screen and locate to (0,0)
    lcd.printf("Alarm");
};

void f_exit2unset(){
    timer_exit.reset();
    timer_exit.stop();
    alarm_led_off();
    timer_alarm_brinky.reset();
    timer_alarm_brinky.stop();
    lcd.cls();//clear the screen and locate to (0,0)
    lcd.printf("Unset");
    lcd.locate(0, 1);
    lcd.printf("Code:____");
};

void f_exit2set(){
    timer_exit.reset();
    timer_exit.stop();
    alarm_led_off();
    timer_alarm_brinky.reset();
    timer_alarm_brinky.stop();
    lcd.cls();//clear the screen and locate to (0,0)
    lcd.printf("Set");
};

void f_exit2alarm(){
    timer_exit.reset();
    timer_exit.stop();
    timer_alarm_brinky.reset();
    timer_alarm_brinky.stop();
    alarm_led_on();
    timer_alarm_turn_led_on.start();
    lcd.cls();//clear the screen and locate to (0,0)
    lcd.printf("alarm");
};

void f_set2entry(){
    timer_alarm_brinky.start();
    lcd.cls();//clear the screen and locate to (0,0)
    lcd.printf("Entry");
};

void f_set2alarm(){
    alarm_led_on();
    timer_alarm_turn_led_on.start();
    lcd.cls();//clear the screen and locate to (0,0)
    lcd.printf("Alarm");
};

void f_entry2unset(){
    timer_alarm_brinky.reset();
    timer_alarm_brinky.stop();
    lcd.cls();//clear the screen and locate to (0,0)
    lcd.printf("Unset");
    lcd.locate(0, 1);
    lcd.printf("Code:____");
};

void f_entry2alarm(){
    timer_alarm_brinky.reset();
    timer_alarm_brinky.stop();
    alarm_led_on();
    timer_alarm_turn_led_on.start();
    lcd.cls();//clear the screen and locate to (0,0)
    lcd.printf("Alarm");
};

void f_alarm2entry(){
    alarm_led_off();
    timer_alarm_turn_led_on.reset();
    timer_alarm_turn_led_on.stop();
    timer_alarm_brinky.start();
    lcd.cls();//clear the screen and locate to (0,0)
    lcd.printf("Entry");
};

void f_alarm2report(){
    alarm_led_off();
    timer_alarm_turn_led_on.reset();
    timer_alarm_turn_led_on.stop();
    lcd.cls();//clear the screen and locate to (0,0)
    lcd.printf("Report: code error %d",state_alarm);
    lcd.locate(0, 1);
    lcd.printf("C key to clear");
};

void f_report2unset(){
    lcd.cls();//clear the screen and locate to (0,0)
    lcd.printf("Unset");
    lcd.locate(0, 1);
    lcd.printf("Code:____");
};

void no_condistion_match(){
    wait(0.1);
};

StateTable fTable[] = {
    //{coming event, current state, function to run, next state}
    {right_code, state_unset, f_unset2exit, state_exit},
    {wrong_code_3times, state_unset, f_unset2alarm, state_alarm},

    {sensor_activate, state_exit, f_exit2alarm, state_alarm},
    {right_code, state_exit, f_exit2unset, state_unset},
    {wrong_code_3times, state_exit, f_exit2alarm, state_alarm},
    {no_sensor_activate_and_time_out,state_exit,f_exit2set,state_set},

    {sensor_activate, state_set, f_set2alarm, state_alarm},
    {button_B, state_set, f_set2entry, state_entry},

    {right_code, state_entry, f_entry2unset, state_unset},
    {wrong_code_3times, state_entry, f_entry2alarm, state_alarm},
    {sensor_activate, state_entry, f_entry2alarm, state_alarm},

    {right_code, state_alarm, f_alarm2report, state_report},

    {button_C, state_report, f_report2unset, state_unset}
};

/*sign up a state machine, give it a state table*/
void fsmRegit(fsmType* pFsm, StateTable* pTable){
    pFsm->stateTable = pTable;
};

/*transmit states*/
void fsmStateTransfer(fsmType* pFsm, int state){
    pFsm->curState = state;
};

/*handle occured event*/
void fsmEventHandle(fsmType* pFsm, int event){
    StateTable* pActTable = pFsm->stateTable;
    void (*eventActFun)() = NULL;//initial the pointer of function as null
    int NextState;
    int CurState = pFsm->curState;
    int maxNum = pFsm->size;
    int flag = 0;//flag shows whether the condistion is satisfied or no_sensor_activate

    /*get current act function*/
    for(int i = 0; i<maxNum; i++){
        //only to execute when the current state and event are all satisfied
        if(event == pActTable[i].event && CurState == pActTable[i].CurState){
            flag = 1;
            eventActFun = pActTable[i].eventActFun;
            NextState = pActTable[i].nextState;
            break;
        }
    }

    if(flag){//if a condition is met
        /*execute act function*/
        if(eventActFun){
            eventActFun();
        }

        //turn to next state
        fsmStateTransfer(pFsm, NextState);
    }
    else {
        no_condistion_match();
    }
};