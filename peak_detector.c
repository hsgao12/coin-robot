//Function for peak detector to check for the perimeter wire.

#include <XC.h>
#include <sys/attribs.h>

#define SYSCLK 40000000L
#define FREQ 100000L // We need the ISR for timer 1 every 10 us


//Prototypes
int detect_perimeter(int AN0);
void ADCConf(void);
int ADCRead(char analogPIN);


/*Function will read from ADC pin AN0 to see if there
  the robot is at the boundary or not. It will then return
  that to the main function.
  
  Parameters: ADC pin AN0
  Return: perimeter_flag = 1 if at boundary, 0 otherwise.	
*/

void ADCConf(void)
{
    AD1CON1CLR = 0x8000;    // disable ADC before configuration
    AD1CON1 = 0x00E0;       // internal counter ends sampling and starts conversion (auto-convert), manual sample
    AD1CON2 = 0;            // AD1CON2<15:13> set voltage reference to pins AVSS/AVDD
    AD1CON3 = 0x0f01;       // TAD = 4*TPB, acquisition time = 15*TAD 
    AD1CON1SET=0x8000;      // Enable ADC
}

int ADCRead(char analogPIN)
{
    AD1CHS = analogPIN << 16;    // AD1CHS<16:19> controls which analog pin goes to the ADC
 
    AD1CON1bits.SAMP = 1;        // Begin sampling
    while(AD1CON1bits.SAMP);     // wait until acquisition is done
    while(!AD1CON1bits.DONE);    // wait until conversion done
 
    return ADC1BUF0;             // result stored in ADC1BUF0
}