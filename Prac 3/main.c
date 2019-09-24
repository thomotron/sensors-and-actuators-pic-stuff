#include <p18f452.h>
#include "../PIC18F452Config.h" // Configuration bits setting for PIC
#include "../SnA.h"
#define byte unsigned char // byte is used for unsigned char type

// Prototypes
void init();
void beep(byte tone, int cycles);

// Initialises our ports
void init()
{
    DDRC = DDRC & 0b10000000; // Set C7 to input and the rest to output
    PORTC = PORTC & 0b10000000; // Set our outputs low
}

void main()
{
    init();

    beep(2000, 500); // idk what kind of noise this will make, I'm no math man
}

// Sound a beep via the onboard buzzer
void beep(byte tone, int cycles)
{
    unsigned int i;
    for (i = 0; i < cycles; i++)
    {
        PORTCbits.RC1 = 1;
        delay_10us(tone);
        PORTCbits.RC1 = 0;
        delay_10us(tone);
    }
}
