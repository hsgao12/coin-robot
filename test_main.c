#include <XC.h>
#include <stdio.h>
#include "uart.c"					// For UART communications
#include "wheel.c"					// For wheel motor movement
#include "arm_servo.c"				// For arm servo movement
#include "peak_detector.c"			// For detecting perimeter wire
#include "coin_detector.c"

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
	int perimeter_flag = 0; 
	int coin_flag = 0; 
	long int count;
	float T, f, f_last;


	DDPCON = 0;
	
	UART2Configure(115200); // Configure UART2 for a baud rate of 115200
	
	//Set-up motor pins
	TRISBbits.TRISB0 = 0;
	TRISBbits.TRISB1 = 0;
	TRISBbits.TRISB2 = 0;
	TRISBbits.TRISB3 = 0;
	
	//RA2 and RA3 for magnet and base respectively.
	TRISAbits.TRISA2 = 0;
	TRISAbits.TRISA3 = 0;
	
	
	//Set as analog input for peak detector for perimeter.
    ANSELBbits.ANSB12 = 1;   // set RB3 (AN5, pin 7 of DIP28) as analog pin
    TRISBbits.TRISB12 = 1;   // set RB3 as an input
	
	//Set as analog input for coin detector.
	ANSELB &= ~(1<<5); // Set RB5 as a digital I/O
    TRISB |= (1<<5);   // configure pin RB5 as input
    CNPUB |= (1<<5);   // Enable pull-up resistor for RB5

	ADCConf(); // Configure ADC		
	float voltage = ADCRead(12)*3.3/1023.0; 
	float last_voltage = 0;

	
	//Set-up Timer1 (wheel motors) and Timer2 (arm PWM/delay_ms)
	SetupTimer1();
	SetupTimer2();
	
	
	
	INTCONbits.MVEC = 1;
	

	while (1)
	{
		set_Wheel_Left(0,0);		//Stop both wheels
		set_Wheel_Right(0,0);
		delay_ms(1000);
		set_Wheel_Left(75,0);		//Go forward
		set_Wheel_Right(75,0);
		
		set_Wheel_Left(0,0);		//Stop both wheels
		set_Wheel_Right(0,0);
		
		delay_ms(2000);
		
		set_Arm_Base(90, 1000);
		delay_ms(1000);
		set_Arm_Magnet(45, 1000);
		delay_ms(1000);
		
		set_Wheel_Left(50,1);		//Go backwards
		set_Wheel_Right(50,1);
		
		set_Wheel_Left(0,0);		//Stop both wheels
		set_Wheel_Right(0,0);
		delay_ms(2000);
		
		set_Arm_Base(0, 1000);
		delay_ms(1000);
		set_Arm_Magnet(0, 1000);
		delay_ms(1000);
	}
}

