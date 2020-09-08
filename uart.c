//UART Functions File


#include <string.h>

int UART2Configure(int desired_baud);
int SerialTransmit(const char *buffer);
unsigned int SerialReceive(char *buffer, unsigned int max_size);



#define SYSCLK 40000000L
#define Baud2BRG(desired_baud)      ( (SYSCLK / (16*desired_baud))-1)


/* UART2Configure() sets up the UART2 for the most standard and minimal operation
 *  Enable TX and RX lines, 8 data bits, no parity, 1 stop bit, idle when HIGH
 *
 * Input: Desired Baud Rate
 * Output: Actual Baud Rate from baud control register U2BRG after assignment*/
int UART2Configure( int desired_baud)
{
    // Peripheral Pin Select
    U2RXRbits.U2RXR = 4; //SET RX to RB8
    RPB9Rbits.RPB9R = 2; //SET RB9 to TX
    
    U2MODE = 0;         // disable autobaud, TX and RX enabled only, 8N1, idle=HIGH
    U2STA = 0x1400;     // enable TX and RX
    U2BRG = Baud2BRG(desired_baud); // U2BRG = (FPb / (16*baud)) - 1
    U2MODESET = 0x8000; // enable UART2
    
    // Calculate actual baud rate
    int actual_baud = SYSCLK / (16 * (U2BRG+1));
    return actual_baud;
}



/* SerialTransmit() transmits a string to the UART2 TX pin MSB first
 *
 * Inputs: *buffer = string to transmit */
int SerialTransmit(const char *buffer)
{
    unsigned int size = strlen(buffer);
    while( size)
    {
        while( U2STAbits.UTXBF);    // wait while TX buffer full
        U2TXREG = *buffer;          // send single character to transmit buffer
        buffer++;                   // transmit next character on following loop
        size--;                     // loop until all characters sent (when size = 0)
    }
 
    while( !U2STAbits.TRMT);        // wait for last transmission to finish
 
    return 0;
}


 
/* SerialReceive() is a blocking function that waits for data on
 *  the UART2 RX buffer and then stores all incoming data into *buffer
 *
 * Note that when a carriage return '\r' is received, a nul character
 *  is appended signifying the strings end
 *
 * Inputs:  *buffer = Character array/pointer to store received data into
 *          max_size = number of bytes allocated to this pointer
 * Outputs: Number of characters received */
unsigned int SerialReceive(char *buffer, unsigned int max_size)
{
    unsigned int num_char = 0;
 
    /* Wait for and store incoming data until either a carriage return is received
     *   or the number of received characters (num_chars) exceeds max_size */
    while(num_char < max_size)
    {
        while( !U2STAbits.URXDA);   // wait until data available in RX buffer
        *buffer = U2RXREG;          // empty contents of RX buffer into *buffer pointer

        while( U2STAbits.UTXBF);    // wait while TX buffer full
        U2TXREG = *buffer;          // echo
 
        // insert nul character to indicate end of string
        if( *buffer == '\r')
        {
            *buffer = '\0';     
            break;
        }
 
        buffer++;
        num_char++;
    }
 
    return num_char;
}