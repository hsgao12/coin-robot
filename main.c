#include <XC.h>
#include "uart.c"					// For UART communications
#include "wheel.c"					// For wheel motor movement
#include "arm_servo.c"				// For arm servo movement
#include "peak_detector.c"			// For detecting perimeter wire

#pragma config FNOSC = FRCPLL       // Internal Fast RC oscillator (8 MHz) w/ PLL
#pragma config FPLLIDIV = DIV_2     // Divide FRC before PLL (now 4 MHz)
#pragma config FPLLMUL = MUL_20     // PLL Multiply (now 80 MHz)
#pragma config FPLLODIV = DIV_2     // Divide After PLL (now 40 MHz) see figure 8.1 in datasheet for more info
#pragma config FWDTEN = OFF         // Watchdog Timer Disabled
#pragma config FPBDIV = DIV_1       // PBCLK = SYCLK

// Define
#define SYSCLK 40000000L

void main (void)
{
	int x;
	volatile unsigned long t=0;
	int perimeter_flag, coin_flag;
	
	DDPCON = 0;
	
	UART2Configure(115200); // Configure UART2 for a baud rate of 115200
	
	//Set-up motor pins
	TRISBbits.TRISB0 = 0;
	TRISBbits.TRISB1 = 0;
	TRISBbits.TRISB2 = 0;
	TRISBbits.TRISB3 = 0;
	
	//Set as analog input for peak detector for perimeter.
    ANSELBbits.ANSB12 = 1;   // set RB3 (AN5, pin 7 of DIP28) as analog pin
    TRISBbits.TRISB12 = 1;   // set RB3 as an input
	
	ADCConf(); // Configure ADC		
	
	//LATBbits.LATB0 = 1;
	//LATBbits.LATB1 = 0;
	//LATBbits.LATB2 = 1;
	//LATBbits.LATB3 = 0;
	
	//Set-up Timer1 (wheel motors) and Timer2 (arm PWM/delay_ms)
	SetupTimer1();
	SetupTimer2();
	
	
	
	INTCONbits.MVEC = 1;

	while (1)
	{
		/*
		set_Wheel_Left(0,0);		//Stop both wheels
		set_Wheel_Right(0,0);
		delay_ms(2000);
		set_Wheel_Left(75,0);		//Go forward
		set_Wheel_Right(75,0);
		delay_ms(2000);
		set_Wheel_Left(50,1);		//Go backward
		set_Wheel_Right(50,1);
		delay_ms(2000);
		*/
		delay_ms(500);
		perimeter_flag = ADCRead(23);		//Check for boundary at pin 23
		printf("\r %f\n", perimeter_flag);
		
/*		
	//If either the boundary or a coin is detected, go backwards and stop.
		if (perimeter_flag == 1 || coin_flag == 1) {
			set_Wheel_Left(0,0);		//Stop both wheels
			set_Wheel_Right(0,0);
			delay_ms(200);
			
			set_Wheel_Left(50,1);		//Go backward
			set_Wheel_Right(50,1);
			delay_ms(1000);				//Note: reverse time for boundary and coin detection should be different.
			
			set_Wheel_Left(0,0);		//Stop both wheels
			set_Wheel_Right(0,0);
			
			while (perimeter_flag == 1) {
				set_Wheel_left(75,0);	//Pivot left(?)
				perimeter_flag = ADCRead(12);
			}
			
		}
	
	
	//If nothing is detected, keep moving forward direction.
		set_Wheel_Left(75,0);		//Go forward
		set_Wheel_Right(75,0);
		
*/		
	}
}

