/* Arm Servo .c file
	-Set-up Timer2 for arm servo PWM (angle control)
	-Adjust Base/Magnet servo angle at different speeds
	-Use Timer2 for millisecond delays
*/

#include <XC.h>
#include <sys/attribs.h>

#define SYSCLK 40000000L
#define FREQ 100000L // We need the ISR for timer 1 every 10 us


//Prototypes
void __ISR(_TIMER_2_VECTOR, IPL4SOFT) ArmPWM(void);
void SetupTimer2 (void);
void delay_ms (int msecs);
void set_Arm_Base(int req_angle,int delay);
void set_Arm_Magnet(int req_angle,int delay);


//Internal pulse width for base and magnet servos
static volatile int pw_base=60, pw_magnet=60, ISR_cnt=0, ISR_frc;




// The Interrupt Service Routine for timer 2 is used to generate one or more standard
// hobby servo signals.  The servo signal has a fixed period of 20ms and a pulse width
// between 0.6ms and 2.4ms.
void __ISR(_TIMER_2_VECTOR, IPL4SOFT) ArmPWM(void)
{
	IFS0CLR=_IFS0_T2IF_MASK; // Clear timer 2 interrupt flag, bit 10 of IFS0

	ISR_cnt++;
	
	if(ISR_cnt<pw_base) LATAbits.LATA3 = 1;
	else LATAbits.LATA3 = 0;
	
	if(ISR_cnt<pw_magnet) LATAbits.LATA2 = 1;
	else LATAbits.LATA2 = 0;

	if(ISR_cnt>=2000)
	{
		ISR_cnt=0; // 2000 * 10us=20ms
		ISR_frc++;
	}
}

void SetupTimer2 (void)
{
	// Explanation here: https://www.youtube.com/watch?v=bu6TTZHnMPY
	__builtin_disable_interrupts();
	PR2 =(SYSCLK/FREQ)-1; // since SYSCLK/FREQ = PS*(PR1+1)
	TMR2 = 0;
	T2CONbits.TCKPS = 0; // 3=1:256 prescale value, 2=1:64 prescale value, 1=1:8 prescale value, 0=1:1 prescale value
	T2CONbits.TCS = 0; // Clock source
	T2CONbits.ON = 1;  // Turns on clock
	IPC2bits.T2IP = 4; //Sets Timer2 Interrupt priority to 4
	IPC2bits.T2IS = 0; //Sets Timer2 Interrupt sub-priority to 0 (not needed)
	IFS0bits.T2IF = 0; //Resets Timer2 Interrupt flag to 0
	IEC0bits.T2IE = 1; //Sets Timer2 Interrupt Enable
	
	__builtin_enable_interrupts();
}


void delay_ms (int msecs)
{	
	int ticks;
	ISR_frc=0;
	ticks=msecs/20;
	while(ISR_frc<ticks);
}



//set_Arm_Base/set_Arm_Magnet adjusts angle and speed of servo (using PWM width control)
//req_angle = 0 to 180 degrees
//delay = 1 to 10 ms
void set_Arm_Base(int req_angle,int delay)
{
	int x, current_angle;
	current_angle = pw_base;	//Get initial angle
	
	//Tracks current angle and initiates 1 degree/delay
	while(current_angle != req_angle+60){
		if(current_angle < req_angle+60) pw_base++;	//If below, increase pulse width
		else pw_base--;								//If above, decrease pulse width
		
		delay_ms(delay);			//Initiate delay and update current_angle
		current_angle = pw_base;
	}	
}
void set_Arm_Magnet(int req_angle,int delay)
{
	int x, current_angle;
	current_angle = pw_magnet;	//Get initial angle
	
	//Tracks current angle and initiates 1 degree/delay
	while(current_angle != req_angle+60){
		if(current_angle < req_angle+60) pw_magnet++;	//If below, increase pulse width
		else pw_magnet--;							//If above, decrease pulse width
		
		delay_ms(delay);			//Initiate delay and update current_angle
		current_angle = pw_magnet;
	}
}