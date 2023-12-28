/*
 * Embedded Systems Mini Project with MSP432P401R
 * Real-time Data Logger with an Alarm Clock
 * Authors: BOUMERZOUG Nadhir, BAYOU Meroua
 * MSc Computer Engineering
 * Dec 2023
 */

#include "logger.h"

int main(void)
{
	WDT_A_holdTimer();
	HW_Init();
	curr_state = run;

	while(1){
		state_table[curr_state]();
	}
}

void HW_Init(){
	/* Zero-filling buffer */
	memset(resultsBuffer, 0x00, 3);

	GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN2); //  LED0   P3.2
	GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN6); //  Buzzer P5.6
	LCD_Init();
	InitKeypad();
	RTC_Init(currentTime);
	ADC14_Init_3channels();
	UART_Init();
	__enable_irq();
}


void idle_f() {

	// Check for '*' keypress to set date/time
	if ((keyC=ReadKeypad())==13){
		tone();
		if ((keyC=ReadKeypad())==13){
			tone();
			while(ReadKeypad() != 0); // wait until key is released
			//set newTime
			updateTime();
		}
	}
	// Check for '#' keypress to set alarm (hours + minutes)
	else if ((keyC=ReadKeypad())==15){
		tone();
		if ((keyC=ReadKeypad())==15){
			tone();
			while(ReadKeypad() != 0);
			setAlarm();
		}
	}
	
	curr_state = run;
}

void run_f() {
	// example: 12/03/2023 12:15

	ADC14_enableConversion();
	ADC14_toggleConversionTrigger();

	char timeStamp[16] = "";
	char values[15] = "";
	time_to_str(timeStamp);
	LCD_Comd(0x80); // force cursor to beginning of 1st line
	UART_LCD_SendStr(timeStamp);

	if(ADC14_getInterruptStatus() & ADC_INT2){
		ADC14_getMultiSequenceResult(resultsBuffer);
		values_to_str(values);
		LCD_Comd(0xC0); // force cursor to beginning of 2nd line
		UART_LCD_SendStr(values);
	}

	curr_state = idle;
}

void tone(){
	// Generate PWM of f=10kHz => T=100µs
	// t=T/2 = 50µs 
	for(i=0; i<100; i++){
		GPIO_toggleOutputOnPin(GPIO_PORT_P5, GPIO_PIN6);
		delay_us(50);
		GPIO_toggleOutputOnPin(GPIO_PORT_P5, GPIO_PIN6);
		delay_us(50);
	}
}

void updateTime(){
	//  day/month/year    hours:min
	// d1d2/m1m2/y1y2y3y4 h1h2:min1min2

	LCD_Comd(1); //Clear LCD
	LCD_Comd(0x0F); // enable cursor blinking

	//read and set day
	while (ReadKeypad()==0); // wait until a key is pressed
	if ((keyC=ReadKeypad()) != 0){
		tone();
		delay_ms(2); // debouncing
		keyN = keyC;
		keyC = ReadKeypad();
		if (keyC == keyN){
			while(ReadKeypad() != 0); // wait until key is released
			d1=keyarray[keyC];
			LCD_Data(chars[keyC]);
			LCD_Comd(0x06); //shift cursor to right
		}
	}
	while (ReadKeypad()==0); // wait until a key is pressed
	if ((keyC=ReadKeypad()) != 0){
		tone();
		delay_ms(2); // debouncing
		keyN = keyC;
		keyC = ReadKeypad();
		if (keyC == keyN){
			while(ReadKeypad() != 0); // wait until button is released
			d2=keyarray[keyC];
			LCD_Data(chars[keyC]);
			LCD_Comd(0x06); 
		}
	}
	LCD_Data('/');
	LCD_Comd(0x06); 
	newTime.dayOfmonth=(d1<<4)+d2;


	//read and set month
	while (ReadKeypad()==0);
	if ((keyC=ReadKeypad()) != 0){
		tone();
		delay_ms(2); 
		keyN = keyC;
		keyC = ReadKeypad();
		if (keyC == keyN){
			while(ReadKeypad() != 0);
			m1=keyarray[keyC];
			LCD_Data(chars[keyC]);
			LCD_Comd(0x06);
		}
	}
	while (ReadKeypad()==0);
	if ((keyC=ReadKeypad()) != 0){
		tone();
		delay_ms(2);
		keyN = keyC;
		keyC = ReadKeypad();
		if (keyC == keyN){
			while(ReadKeypad() != 0);
			m2=keyarray[keyC];
			LCD_Data(chars[keyC]);
			LCD_Comd(0x06);
		}
	}
	LCD_Data('/');
	LCD_Comd(0x06); //shift cursor to right
	newTime.month=(m1<<4)+m2;


	//read and set year
	while (ReadKeypad()==0); 
	if ((keyC=ReadKeypad()) != 0){
		tone();
		delay_ms(2);
		keyN = keyC;
		keyC = ReadKeypad();
		if (keyC == keyN){
			while(ReadKeypad() != 0); 
			y1=keyarray[keyC];
			LCD_Data(chars[keyC]);
			LCD_Comd(0x06);
		}
	}
	while (ReadKeypad()==0); 
	if ((keyC=ReadKeypad()) != 0){
		tone();
		delay_ms(2);
		keyN = keyC;
		keyC = ReadKeypad();
		if (keyC == keyN){
			while(ReadKeypad() != 0); 
			y2=keyarray[keyC];
			LCD_Data(chars[keyC]);
			LCD_Comd(0x06);
		}
	}
	while (ReadKeypad()==0); 
	if ((keyC=ReadKeypad()) != 0){
		tone();
		delay_ms(2); 
		keyN = keyC;
		keyC = ReadKeypad();
		if (keyC == keyN){
			while(ReadKeypad() != 0); 
			y3=keyarray[keyC];
			LCD_Data(chars[keyC]);
			LCD_Comd(0x06);
		}
	}
	while (ReadKeypad()==0); 
	if ((keyC=ReadKeypad()) != 0){
		tone();
		delay_ms(2);
		keyN = keyC;
		keyC = ReadKeypad();
		if (keyC == keyN){
			while(ReadKeypad() != 0); 
			y4=keyarray[keyC];
			LCD_Data(chars[keyC]);
			LCD_Comd(0x06);
		}
	}
	LCD_Data(' ');
	LCD_Comd(0x06); 
	newTime.year=(y1<<12)+(y2<<8)+(y3<<4)+y4;


	//read and set hour
	while (ReadKeypad()==0);
	if ((keyC=ReadKeypad()) != 0){
		tone();
		delay_ms(2);
		keyN = keyC;
		keyC = ReadKeypad();
		if (keyC == keyN){
			while(ReadKeypad() != 0);
			h1=keyarray[keyC];
			LCD_Data(chars[keyC]);
			LCD_Comd(0x06); 
		}
	}
	while (ReadKeypad()==0); 
	if ((keyC=ReadKeypad()) != 0){
		tone();
		delay_ms(2);
		keyN = keyC;
		keyC = ReadKeypad();
		if (keyC == keyN){
			while(ReadKeypad() != 0); 
			h2=keyarray[keyC];
			LCD_Data(chars[keyC]);
			LCD_Comd(0x06); 
		}
	}
	LCD_Data(':');
	LCD_Comd(0x06);
	newTime.hours=(h1<<4)+h2;


	//read and set minutes
	while (ReadKeypad()==0);
	if ((keyC=ReadKeypad()) != 0){
		tone();
		delay_ms(2); 
		keyN = keyC;
		keyC = ReadKeypad();
		if (keyC == keyN){
			while(ReadKeypad() != 0);
			min1=keyarray[keyC];
			LCD_Data(chars[keyC]);
			LCD_Comd(0x06); 
		}
	}
	while (ReadKeypad()==0); 
	if ((keyC=ReadKeypad()) != 0){
		tone();
		delay_ms(2);
		keyN = keyC;
		keyC = ReadKeypad();
		if (keyC == keyN){
			while(ReadKeypad() != 0); 
			min2=keyarray[keyC];
			LCD_Data(chars[keyC]);
			LCD_Comd(0x06);
		}
	}
	LCD_Comd(0x06);
	newTime.minutes=(min1<<4)+min2;


	LCD_Comd(0x0C); // disable blinking cursor
	// hold clock
	RTC_C_holdClock(); 
	RTC_C_initCalendar(&newTime, RTC_C_FORMAT_BCD); 
	RTC_C_startClock(); 
}

void setAlarm(){

	LCD_Comd(1); 	// Clear LCD
	LCD_Comd(0x0F); // enable cursor blinking

	//read and set hour ALARM
	while (ReadKeypad()==0); // wait until a key is pressed
	if ((keyC=ReadKeypad()) != 0){
		tone();
		delay_ms(2); // debouncing
		keyN = keyC;
		keyC = ReadKeypad();
		if (keyC == keyN){
			while(ReadKeypad() != 0); // wait until key is released
			h1=keyarray[keyC];
			LCD_Data(chars[keyC]);
			LCD_Comd(0x06); //shift cursor to right
		}
	}
	while (ReadKeypad()==0); // wait until a key is pressed
	if ((keyC=ReadKeypad()) != 0){
		tone();
		delay_ms(2); // debouncing
		keyN = keyC;
		keyC = ReadKeypad();
		if (keyC == keyN){
			while(ReadKeypad() != 0); // wait until key is released
			h2=keyarray[keyC];
			LCD_Data(chars[keyC]);
			LCD_Comd(0x06); //shift cursor to right
		}
	}
	LCD_Data(':');
	LCD_Comd(0x06); //shift cursor to right
	hours=(h1<<4)+h2;


	//read and set minutes ALARM
	while (ReadKeypad()==0); // wait until a key is pressed
	if ((keyC=ReadKeypad()) != 0){
		tone();
		delay_ms(2); // debouncing
		keyN = keyC;
		keyC = ReadKeypad();
		if (keyC == keyN){
			while(ReadKeypad() != 0); // wait until key is released
			min1=keyarray[keyC];
			LCD_Data(chars[keyC]);
			LCD_Comd(0x06); //shift cursor to right
		}
	}
	while (ReadKeypad()==0); // wait until a key is pressed
	if ((keyC=ReadKeypad()) != 0){
		tone();
		delay_ms(2); // debouncing
		keyN = keyC;
		keyC = ReadKeypad();
		if (keyC == keyN){
			while(ReadKeypad() != 0); // wait until key is released
			min2=keyarray[keyC];
			LCD_Data(chars[keyC]);
			LCD_Comd(0x06); //shift cursor to right
		}
	}
	LCD_Comd(0x06); //shift cursor to right
	minutes=(min1<<4)+min2;

	LCD_Comd(0x0C); // disable blinking cursor

	
	RTC_C_clearInterruptFlag(RTC_C_CLOCK_ALARM_INTERRUPT);
	RTC_C_enableInterrupt(RTC_C_CLOCK_READ_READY_INTERRUPT | 
			RTC_C_TIME_EVENT_INTERRUPT | RTC_C_CLOCK_ALARM_INTERRUPT );
	RTC_C_setCalendarAlarm(minutes, hours, RTC_C_ALARMCONDITION_OFF, RTC_C_ALARMCONDITION_OFF);
}

void time_to_str(char *str){
	char tmp0[3]="", tmp1[3]="", tmp2[4]="", tmp3[3]="", tmp4[3]="";
	sprintf(tmp0, "%.2x/", newTime.dayOfmonth);
	strcpy(str, tmp0);
	sprintf(tmp1, "%.2x/", newTime.month);
	strcat(str, tmp1);
	sprintf(tmp2, "%.4x", (uint16_t) newTime.year);
	strcat(str, tmp2);
	sprintf(tmp3, " %.2x", newTime.hours);
	strcat(str, tmp3);
	sprintf(tmp4, ":%.2x", newTime.minutes);
	strcat(str, tmp4);
}

void values_to_str(char *str){
	char tmp0[5]="", tmp1[5]="", tmp2[5]="";
	results[0] = resultsBuffer[0]*(3.3/1024);
	sprintf(tmp0, " %.2f", results[0]);
	strcat(str, tmp0);
	results[1] = resultsBuffer[1]*(3.3/1024);
	sprintf(tmp1, " %.2f", results[1]);
	strcat(str, tmp1);
	results[2] = resultsBuffer[2]*(3.3/1024);
	sprintf(tmp2, " %.2f", results[2]);
	strcat(str, tmp2);
}


void UART_Init(){
	//UART data structure
	const eUSCI_UART_Config uartConfig =
	{
			EUSCI_A_UART_CLOCKSOURCE_SMCLK, //SMCLK Clock Source
			19, // 78, //BRDIV = 789.5. CODE EXAMPLES 413
			8, // 2, //UCxBRF = 2
			85, // 0, //UCxBRS = 0
			EUSCI_A_UART_NO_PARITY, //No Parity
			EUSCI_A_UART_LSB_FIRST, //LSB First
			EUSCI_A_UART_ONE_STOP_BIT, //One stop bit
			EUSCI_A_UART_MODE, //UART mode
			EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION //Oversampling
	};

	//This code snippet is the actual configuration of the UART
	//module using the DriverLib APIs:
	/* Selecting P1.2 and P1.3 in UART mode */
	GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

	/* Setting DCO to 3MHz */
	CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_3);

	//Configuring UART Module
	UART_initModule(EUSCI_A0_BASE, &uartConfig);
	//Enable UART module
	UART_enableModule(EUSCI_A0_BASE);
	//Enabling interrupts
	//MAP_UART_enableInterrupt(EUSCI_A0_BASE,	EUSCI_A_UART_RECEIVE_INTERRUPT, EUSCI_A_UART_TRANSMIT_INTERRUPT);
	//MAP_Interrupt_enableInterrupt(INT_EUSCIA0);
	//MAP_Interrupt_enableSleepOnIsrExit();
	//MAP_Interrupt_enableMaster();
}

void UART_LCD_SendStr(char *str){
	char c;
	while (c = *str++) {
		LCD_Data(c);
		UART_transmitData(EUSCI_A0_BASE,c);
	}
	UART_transmitData(EUSCI_A0_BASE,'\r');
	UART_transmitData(EUSCI_A0_BASE,'\n');
}


void RTC_Init(RTC_C_Calendar currentTime){

	/* Configuring pins for peripheral/crystal usage and LED for output */
	MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_PJ,
			GPIO_PIN0 | GPIO_PIN1, GPIO_PRIMARY_MODULE_FUNCTION);
	MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);

	/* Setting the external clock frequency. This API is optional, but will
	 * come in handy if the user ever wants to use the getMCLK/getACLK/etc
	 * functions
	 */
	CS_setExternalClockSourceFrequency(32000,3000000); //3MHz Clk Frequency

	/* Starting LFXT in non-bypass mode without a timeout. */
	CS_startLFXT(false);

	//Initializing RTC with current time as described in time in
	//definitions section
	RTC_C_initCalendar(&currentTime, RTC_C_FORMAT_BCD);

	//Specify an interrupt to assert every minute
	RTC_C_setCalendarEvent(RTC_C_CALENDAREVENT_MINUTECHANGE);

	//Enable interrupt for RTC Ready Status, which asserts when the RTC
	//Calendar registers are ready to read.
	//Also, enable interrupts for the Calendar alarm and Calendar event.
	RTC_C_clearInterruptFlag(RTC_C_CLOCK_READ_READY_INTERRUPT |
			RTC_C_TIME_EVENT_INTERRUPT | RTC_C_CLOCK_ALARM_INTERRUPT);
	RTC_C_enableInterrupt(RTC_C_CLOCK_READ_READY_INTERRUPT |
			RTC_C_TIME_EVENT_INTERRUPT | RTC_C_CLOCK_ALARM_INTERRUPT);

	//Start RTC Clock
	RTC_C_startClock();
	Interrupt_enableInterrupt(INT_RTC_C);
}

void ADC14_Init_3channels(){
	/* Setting reference voltage to 2.5  and enabling reference */
	//MAP_REF_A_setReferenceVoltage(REF_A_VREF2_5V);
	//MAP_REF_A_enableReferenceVoltage();

	/* Initializing ADC (MCLK/1/1) */
	ADC14_enableModule();
	ADC14_initModule(ADC_CLOCKSOURCE_MCLK, ADC_PREDIVIDER_1, ADC_DIVIDER_1, 0);
	ADC14_setResolution(ADC_10BIT);

	/* Configuring GPIOs for Analog In */
	GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4,
			GPIO_PIN3 | GPIO_PIN5 | GPIO_PIN7, GPIO_TERTIARY_MODULE_FUNCTION);

	/* Configuring ADC Memory (ADC_MEM0 - ADC_MEM7 (A0 - A2)  with no repeat)
	 * with internal AVCC 3.3v - VSS 0 reference */
	ADC14_configureMultiSequenceMode(ADC_MEM0, ADC_MEM2, false);

	ADC14_configureConversionMemory(ADC_MEM0,
			ADC_VREFPOS_AVCC_VREFNEG_VSS,
			ADC_INPUT_A6, false); // potentiometer
	ADC14_configureConversionMemory(ADC_MEM1,
			ADC_VREFPOS_AVCC_VREFNEG_VSS,
			ADC_INPUT_A8, false); // light sensor
	ADC14_configureConversionMemory(ADC_MEM2,
			ADC_VREFPOS_AVCC_VREFNEG_VSS,
			ADC_INPUT_A10, false); // temperature sensor

	/* Enabling the interrupt when a conversion on channel 7 (end of sequence)
	 *  is complete and enabling conversions */
	//ADC14_enableInterrupt(ADC_INT7);
	//Interrupt_enableInterrupt(INT_ADC14);
	ADC14_enableSampleTimer(ADC_AUTOMATIC_ITERATION);

}

void RTC_C_IRQHandler(void)
{
	uint32_t status;
	status = RTC_C_getEnabledInterruptStatus();
	RTC_C_clearInterruptFlag(status);
	
	if(status & RTC_C_CLOCK_READ_READY_INTERRUPT)
	{
		//interrupt each 1s : RTC_C_CLOCK_READ_READY_INTERRUPT
		GPIO_toggleOutputOnPin(GPIO_PORT_P3, GPIO_PIN2); // LED0 Blinking
	}
	
	if(status & RTC_C_TIME_EVENT_INTERRUPT)
	{
		//Interrupts every minute
		newTime = my_RTC_C_getCalendarTime(); //updateTime each 1min
		if (curr_state == idle) curr_state = run;
	}

	if(status & RTC_C_CLOCK_ALARM_INTERRUPT){
		// alarm tone
		for(k=0; k<20; k++ )
			tone();
	}
	
}

RTC_C_Calendar my_RTC_C_getCalendarTime(void){
	RTC_C_Calendar tempCal;

	while (!(BITBAND_PERI(RTC_C->CTL13, RTC_C_CTL13_RDY_OFS)));

	tempCal.seconds = RTC_C->TIM0 & (RTC_C_TIM0_SEC_LD_MASK | RTC_C_TIM0_SEC_HD_MASK);
	tempCal.minutes = (RTC_C->TIM0 & (RTC_C_TIM0_MIN_LD_MASK | RTC_C_TIM0_MIN_HD_MASK))>>RTC_C_TIM0_MIN_OFS;
	tempCal.hours   = RTC_C->TIM1 & (RTC_C_TIM1_HOUR_LD_MASK | RTC_C_TIM1_HOUR_HD_MASK);
	tempCal.dayOfWeek   = (RTC_C->TIM1 & RTC_C_TIM1_DOW_MASK)>>RTC_C_TIM1_DOW_OFS;
	tempCal.dayOfmonth = RTC_C->DATE & (RTC_C_DATE_DAY_LD_MASK | RTC_C_DATE_DAY_HD_MASK);
	tempCal.month = (RTC_C->DATE & (RTC_C_DATE_MON_LD_MASK|RTC_C_DATE_MON_HD))>>RTC_C_DATE_MON_OFS;
	tempCal.year = RTC_C->YEAR & 0x0ffff;

	return (tempCal);
}
