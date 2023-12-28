/*
 * Embedded Systems Mini Project with MSP432
 * Real-time Data Logger with an Alarm Clock
 * Authors: BOUMERZOUG Nadhir, BAYOU Meroua
 * MSc Computer Engineering
 * Dec 2023
 */

#include <stdio.h>
#include <string.h>
#include "driverlib.h"
#include "LCD.h"
#include "keypad.h"


#ifndef LOGGER_H_
#define LOGGER_H_


void updateTime();
void setAlarm();
void tone();
void run_f();
void idle_f();
void HW_Init();
void UART_Init();
void RTC_Init(RTC_C_Calendar currentTime);
void time_to_str(char *str);
void values_to_str(char *str);
void UART_LCD_SendStr(char *str);
void ADC14_Init_3channels();
RTC_C_Calendar my_RTC_C_getCalendarTime(void);

// FSM states
typedef enum {
	run = 0,
	idle
}State_Type;
State_Type curr_state;

// FSM state table transition
void (*state_table[])() = {run_f, idle_f};

// used to print on LCD
char chars[] = {'0', '1', '2', '3', '0', '4', '5', '6', '0', '7', '8', '9', '0','0','0','0','0'};
// used to operate
char keyarray[]={0, 1, 2, 3, 0, 4, 5, 6, 0, 7, 8, 9, 0, 0, 0, 0, 0};

char d1, d2, m1, m2, y1, y2, y3, y4, h1, h2, min1, min2, s1, s2;
char keyC, keyN;
int i,k, count;
uint8_t minutes, hours;
static uint16_t resultsBuffer[3];
float results[3];
const RTC_C_Calendar currentTime = {
		0x00, 0x03, 0x15, 0x03, 0x13, 0x12, 0x2023
};

static volatile RTC_C_Calendar newTime = {
		0x00, 0x03, 0x15, 0x03, 0x13, 0x12, 0x2023
};

#endif /* LOGGER_H_ */
