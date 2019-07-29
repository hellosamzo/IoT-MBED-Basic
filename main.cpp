#include "mbed.h"
#include "C12832.h"

/*
 * Author: Sam M (hellosamzo)
 *
 * Bonus Feature: When pressing the Joystick down button,
 * the next time SW2 is pressed, a descending tone is played.
 * When pressing the Joystick up button, the configuration is
 * then set back to playing an ascending tone whenever SW2 is
 * pressed.
 *
 * The SW2 button plays whatever configuration is currently
 * set from the joystick up and down buttons.
 */

// buttons
InterruptIn joyUp(A2);
InterruptIn joyDown(A3);
InterruptIn sw(SW2);

// speaker and screen
PwmOut speaker(D6);
C12832 screen(D11, D13, D12, D7, D10);

// global int variables
static int counter = 0;
static int toneFlag = 0;
static int countFlag = 0;
static int loopCount = 0;
static int loopCountDesc = 8000;
static int joyFlag = 0;
static int loopFlag = 0;
static int loopFlagDesc = 0;

// Ticker to increment seconds
Ticker tick;
// Timeout used to make the speaker wait
Timeout timeout;

// if sw2 is pressed, put the flag to 1
void trigger_sound(void)
{
    toneFlag = 1;
}

// increment a counter every second
void countAdd(void)
{
    screen.locate(0,1);
    screen.printf("%d",counter++);
}

// if joystick up is pressed put the flag to 0
void trigger_joy_up(void)
{
    joyFlag = 0;
}

// if joystick down is pressed put the flag to 1
void trigger_joy_down(void)
{
    joyFlag = 1;
}

// when ticker activates, put the flag to 1
void trigger_count(void)
{
    countFlag = 1;
}

// put loop flag to 1
void trigger_loop(void)
{
    loopFlag = 1;
}

// put descending loop flag to 1
void trigger_loopDesc(void)
{
    loopFlagDesc = 1;
}

// play a continuous ascending tone
void makeTone(void)
{
    loopCount = 200 + loopCount;
    if(loopCount < 8000) {
        speaker.period(1.0/(loopCount));
        speaker=0.25;
        timeout.attach(trigger_loop, 0.1);
    } else {
        speaker = 0.0;
        loopCount = 0;
    }
}

// play a continuous descending tone
void makeToneDesc(void)
{
    loopCountDesc = loopCountDesc - 200;
    if(loopCountDesc > 0) {
        speaker.period(1.0/(loopCountDesc));
        speaker=0.25;
        timeout.attach(trigger_loopDesc, 0.1);
    } else {
        speaker = 0.0;
        loopCountDesc = 8000;
    }
}

// main method which goes round in a loop watching for flags to be activated
int main()
{
    sw.fall(trigger_sound);
    joyUp.fall(trigger_joy_up);
    joyDown.fall(trigger_joy_down);
    tick.attach(trigger_count, 1.0);
    while(1) {
        if(toneFlag) {
            toneFlag = 0;

            if(joyFlag == 0) {
                makeTone();
            } else {
                makeToneDesc();
            }
        }
        if(countFlag) {
            countFlag = 0;
            countAdd();
        }
        if(loopFlag) {
            loopFlag = 0;
            makeTone();
        }
        if(loopFlagDesc) {
            loopFlagDesc = 0;
            makeToneDesc();
        }
        sleep();
    }
}

