#include "alarm_LED.h"

void alarm_led_on(){
    alarm_led.write(0xFFFF);
    cs = 1;
    cs = 0;
}

void alarm_led_off(){
    alarm_led.write(0x0000);
    cs = 1;
    cs = 0;
}

void alarm_blinky(){
    alarm_led_state = !alarm_led_state;
    if(alarm_led_state == 1){
        alarm_led_on();
    }
    else{
        alarm_led_off();
    }
}
