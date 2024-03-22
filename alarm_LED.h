#include "mbed.h"

extern SPI alarm_led;
extern DigitalOut cs;
extern Timer timer_alarm_brinky; // define Timer with name "timer_alarm_brinky"
extern Timer timer_alarm_turn_led_on;
extern Timer timer_exit;

extern int alarm_led_state;

void alarm_led_on();
void alarm_led_off();
void alarm_blinky();
