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
    DDRAbits.TRISA0 = 1; // Set PORTA0 as input
    DDRAbits.TRISA3 = 0; // Set PORTA3 as output
    DDRAbits.TRISA5 = 0; // Set PORTA5 as output
    DDRCbits.TRISC6 = 0; // Set PORTC6 as output
    ADCON1 = 0b01000110; // Set the ADC to all-digital for PORTA???
    init_LCD(); // Initialise the LCD
}

void main()
{
    byte value; // Darn PIC C

    init(); // Initialise our environment

    PORTCbits.RC6 = 0; // Bring the IR LED high (it's inverted)

    while (1)
    {
        if (PORTAbits.RA0)
        {
            leds(0b10000000); // Show high
        }
        else
        {
            leds(0b00010000); // Show low
        }

        value = a2d(0); // Get the sensor value
        LCD_line1; // Reset the LCD cursor position
        LCD_BCD(value); // Write to the LCD
        pause(300); // Wait for 300ms
    }
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