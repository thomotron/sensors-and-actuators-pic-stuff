#include <p18f452.h>
#include "../PIC18F452Config.h" // Configuration bits setting for PIC
#include "../SnA.h"
#define byte unsigned char // byte is used for unsigned char type

// Prototypes
void init();
void bar(byte value);
void leds(byte value);

// Initialises ports and other bits and pieces not directly relevant to the main
// run loop
void init()
{
    DDRD = 0b00000000; // Set TRISD to output
    DDRAbits.TRISA3 = 0; // Set PORTA3 as output
    DDRAbits.TRISA5 = 0; // Set PORTA5 as output
    ADCON1 = 0b01000110; // Set the ADC to all-digital for PORTA???
    init_LCD(); // Initialise the LCD

}

void main()
{
    init(); // Initialise our environment

    bar(0b10001100); // Write a pattern to the bar graph
    leds(0b00000010); // Write to the remaining bit of the bar and LEDs

    while (1); // Idle
}

// Display a value on the bar graph
void bar(byte value)
{
    PORTD = value; // Set PORTD to our value
    PORTAbits.RA3 = 1; // Set the clock high
    PORTAbits.RA3 = 0; // Set the clock low
}

// Display a value on the last two bits of the bar graph and four LEDs
void leds(byte value)
{
    PORTD = value & 0b11110011; // Set PORTD to value masked by relevant pins
    PORTAbits.RA5 = 1; // Set the clock high
    PORTAbits.RA5 = 0; // Set the clock low
}