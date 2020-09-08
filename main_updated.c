#include "ai_robot.c"
#include <XC.h>
#include <sys/attribs.h>
#include "uart.c"					// For UART communications
#define SYSCLK 40000000L
#define DEF_FREQ 16000L

extern float init_freq;


void main(void){

	
	DDPCON = 0;
	
	UART2Configure(115200); // Configure UART2 for a baud rate of 115200
	
	//Set-up motor pins
	TRISBbits.TRISB0 = 0;
	TRISBbits.TRISB1 = 0;
	TRISBbits.TRISB2 = 0;
	TRISBbits.TRISB3 = 0;
	
	//RA2 and RA3 for magnet and base respectively.
	TRISAbits.TRISA3 = 0; 
	TRISAbits.TRISA2 = 0; 
	
	
	//Set as analog input for peak detector for perimeter.
    ANSELBbits.ANSB12 = 1;   // set RB3 (AN5, pin 7 of DIP28) as analog pin
    TRISBbits.TRISB12 = 1;   // set RB3 as an input
	
	//Set as analog input for coin detector.
	ANSELB &= ~(1<<5); // Set RB5 as a digital I/O
    TRISB |= (1<<5);   // configure pin RB5 as input
    CNPUB |= (1<<5);   // Enable pull-up resistor for RB5
	
	ADCConf();
	
	SetupTimer1();
	SetupTimer2();
	
	
	
	INTCONbits.MVEC = 1;
	
	init_frequency();
	init_volts(); 
	
	set_Arm_Magnet(30,25); //Initial Pos. 
	set_Arm_Base(30,25);
	
	while(1){
		ai_robot();
		//set_Wheel_Left(50,0);
		//set_Wheel_Right(50,0);
	
	}




}