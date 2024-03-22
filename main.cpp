#include "mbed.h"
#include "system/StateMachine.h"
#include "TextLCD.h"
#include "alarm_LED.h"

extern StateTable fTable[15];
DigitalOut myled(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
DigitalOut led4(LED4);

TextLCD lcd(p15, p16, p17, p18, p19, p20);
SPI alarm_led(p5,p6,p7);
DigitalOut cs(p8);
BusOut cols_out(p26, p25, p24); 
BusIn rows_in(p14, p13, p12, p11);
Timer timer_alarm_brinky; // to blink the alarm led in state_exit
Timer timer_alarm_turn_led_on; // to turn on the alarm led in state_alarm
Timer timer_exit; // to countdown in state_exit

int alarm_led_state = 0;

char Keytable[][4] = {
      {'1', '2', '3', 'F'},
      {'4', '5', '6', 'E'},
      {'7', '8', '9', 'D'},
      {'A', '0', 'B', 'C'}
    };

char password[4] = {'1','2','3','4'}; //the right code       

void clear_a_line(int line){
    if(line == 0){
        lcd.locate(0, 0);
        lcd.printf("               ");
        wait(0.1);
        lcd.locate(0,0);
    }
    else if (line == 1) {
        lcd.locate(0, 1);
        lcd.printf("               ");
        wait(0.1);
        lcd.locate(0,1);
    }
    else {
        lcd.cls();
        wait(0.1);
    }
}

char getKey() {
    for (int i = 0; i <= 3; i++) {
        cols_out = i;
        // for each bit in rows
        for (int j = 0; j <= 3; j++) {
            // if j'th bit of "rows_in" is LOW
            if (~rows_in & (1 << j)) {
                // wait until key is released and return the key afterwards
                while (~rows_in & (1 << j)) { }
                return Keytable[j][3-i];
            }
        }
    }
    return '\0';
}

int getinput(fsmType* pFsm) {
    // while loop makes this function wait
    // until any key is pressed
    int CurState = pFsm->curState;
    char input_char = '0';
    char input_password[4] = {'\0','\0','\0','\0'};
    char display_password[4] = {'_','_','_','_'};
    while(1){
        if(timer_exit.read() > 60){
            if(CurState == state_exit){
                timer_exit.reset();
                timer_exit.stop();
                return no_sensor_activate_and_time_out;
            }
        }
        if(timer_alarm_brinky.read() > 1){
            if(CurState == state_exit or CurState == state_entry){
                alarm_blinky();
                timer_alarm_brinky.reset();
            }
            else{
                timer_alarm_brinky.reset();
                timer_alarm_brinky.stop();
            }
        }
        if(timer_alarm_turn_led_on.read() > 120){
            if(CurState == state_alarm){
                alarm_led_off();
                timer_alarm_turn_led_on.reset();
                timer_alarm_turn_led_on.stop();
            }
        }
        for(int i = 0;i < 6; i++){
                cols_out = i;
                if(i < 4){
                    input_char = getKey();//get the input character
                    if(input_char == 'C' && CurState == state_report){
                        return button_C;
                    }
                    while(input_char == 'B'){
                        myled = 1;
                        if(CurState == state_set){
                            myled = 0;
                            return button_B;
                        }
                        for(int j = 0;j < 3;j++){//code can be input by three times
                            for(int k = 0;k < 4;k++){//input code
                                input_char = getKey();
                                while(input_char == '\0' or input_char == ' '){//ignore the empty input
                                    input_char = getKey();
                                    wait(0.1);
                                }
                                if(input_char == 'C'){//delete an input
                                    if(k > 0){
                                        display_password[k] = '_';
                                        input_password[k] = '\0';
                                        k--;
                                        display_password[k] = '_';
                                        input_password[k] = '\0';
                                        k--;//because the for loop will add 1 to the k
                                    }
                                    else{
                                        display_password[k] = '_';
                                        input_password[k] = '\0'; 
                                        k--;
                                    }
                                }
                                else{
                                    //lcd.putc(input_char);
                                    display_password[k] = '*';//display an input
                                    input_password[k] = input_char;
                                }
                                clear_a_line(1);
                                lcd.locate(0, 1);
                                lcd.printf("Code:");
                                wait(0.1);
                                lcd.printf(display_password);//display the input information
                                wait(0.1);
                            }
                            clear_a_line(1);
                            lcd.locate(0, 1);
                            lcd.printf("Press B to set");
                            while(1){
                                input_char = getKey();
                                wait(0.1);
                                if(input_char == 'B')
                                    break;
                                else{//Any other key should cause the code entry procedure to abort without checking the user code.
                                    if(input_char != '\0'){
                                        clear_a_line(1);
                                        lcd.locate(0, 1);
                                        lcd.printf("invalid");
                                        wait(0.1);
                                        myled = 0;
                                        return -1;
                                    }
                                }
                            }
                            //lcd.putc('debug');
                            if(memcmp(input_password,password,sizeof(input_password)) == 0){//compare the string, input the right code
                                led3 = !led3;
                                memset(display_password, '_', sizeof(display_password));
                                memset(input_password,'\0',sizeof(input_password));
                                clear_a_line(1);
                                lcd.locate(0, 1);
                                lcd.printf("right code");
                                wait(1);
                                myled = 0;
                                return right_code;
                            }
                            else{//input wrong
                                memset(display_password, '_', sizeof(display_password));
                                memset(input_password,'\0',sizeof(input_password));//input a wrong code
                                if(j == 2){
                                    myled = !myled;
                                    clear_a_line(1);
                                    lcd.locate(0, 1);
                                    lcd.printf("no more try");
                                    wait(1);
                                    clear_a_line(1);
                                    myled = 0;
                                    return wrong_code_3times;
                                }
                                else{
                                    clear_a_line(1);
                                    lcd.locate(0, 1);
                                    lcd.printf("wrong code");
                                    wait(3);
                                    clear_a_line(1);
                                }
                            }
                            clear_a_line(1);
                            lcd.locate(0, 1);
                            lcd.printf("Code:____");
                            wait(0.1);
                        }
                    }
                }
                if(i == 4 or i == 5){
                    if(rows_in != 15){  //at least a switch is turned on
                        led2 = !led2;
                        return sensor_activate;
                    }
                }
            }
    }
}

void init(){//initial state is unset
    lcd.cls();//clear the screen and locate to (0,0)
    lcd.printf("Unset");
    lcd.locate(0, 1);
    lcd.printf("Code:____");

    myled = 0;
    led2 = 0;
    led3 = 0;
    led4 = 0;
    cs = 0;
    alarm_led.format(16,0);
    alarm_led.frequency(1000000);
    alarm_led_off();
}

int main() {
    fsmType pType;
    fsmRegit(&pType, fTable);
    pType.curState = state_unset;
    pType.size = sizeof(fTable)/sizeof(StateTable);
    int event = 0;

    init();

    while(1){
        event = getinput(&pType);
        wait(0.5);
        fsmEventHandle(&pType,event);
        wait(0.1);
    }
}
