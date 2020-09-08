/*Wheel Motor .c file
	-Set-up Timer1 for wheel PWM (speed control)
	-Control H-Bridge of Left motor (RB0-RB1) and Right motor (RB2-RB3)
	-One H-Bridge side set for PWM (RB0/RB2) snd direction (RB1/RB3)
*/

#include <XC.h>
#include <sys/attribs.h>	//Needed for ISRs

#define SYSCLK 40000000L
#define DEF_FREQ 16000L

//Prototypes
void __ISR(_TIMER_1_VECTOR, IPL5SOFT) WheelPWM(void);
void SetupTimer1 (void);
void set_Wheel_Left(int pwm, char reverse);
void set_Wheel_Right(int pwm, char reverse);


//Internal dutycycle, counts from 0 to 100, compared to set PWM for left/right
//Internal directions, 0 for forward, 1 for reverse
static volatile int dutycycle = 0, pw_left=0, pw_right=0;
static volatile char rev_left = 0, rev_right = 0;

void __ISR(_TIMER_1_VECTOR, IPL5SOFT) WheelPWM(void)
{

	//Check left wheel (if forward, on means 0/off means 1)
	if(dutycycle >= pw_left) LATBbits.LATB0 = rev_left;		//Turn off when pass dutycycle
	else LATBbits.LATB0 = !rev_left;						//Turn on when not pass dutycycle
	
	//Check right wheel (if forward, on means 0/off means 1)
	if(dutycycle >= pw_left) LATBbits.LATB2 = rev_right;
	else LATBbits.LATB2 = !rev_right;
	
	//Add up dutycycle
	if (dutycycle == 99) dutycycle = 0;
	else dutycycle++;
	
	IFS0CLR=_IFS0_T1IF_MASK;	//At Timer 1 Interrupt flag bit, indicate 1 to clear
}

void SetupTimer1 (void)
{
	// Explanation here:
	// https://www.youtube.com/watch?v=bu6TTZHnMPY
	__builtin_disable_interrupts();
	PR1 =(SYSCLK/(DEF_FREQ*2L))-1; // since SYSCLK/FREQ = PS*(PR1+1)
	TMR1 = 0;					   // Reset Current Time count to 0
	T1CONbits.TCKPS = 0; // Pre-scaler: 1
	T1CONbits.TCS = 0; // Clock source
	T1CONbits.ON = 1;  // Turn on clock
	IPC1bits.T1IP = 5; //Sets Timer1 Interrupt priority to 5
	IPC1bits.T1IS = 0; //Sets Timer1 Interrupt sub-priority to 0 (not needed)
	IFS0bits.T1IF = 0; //Resets Timer1 Interrupt flag to 0
	IEC0bits.T1IE = 1; //Sets Timer1 Interrupt Enable
	
	__builtin_enable_interrupts();
}


//set_Wheel_Left/set_Wheel_Right adjusts PWM of wheels and direction
//speed = 0(stop) to 100(full speed)
//reverse = 0(forward) or 1(reverse)

void set_Wheel_Left(int speed, char reverse){
	pw_left = speed;
	rev_left = reverse;
	LATBbits.LATB1 = rev_left;
}

void set_Wheel_Right(int speed, char reverse){
	pw_right = speed;
	rev_right = reverse;
	LATBbits.LATB3 = rev_right;
}


