/* ------------------- counter.c ---------------------
 This program is the C version of the program to generate
 a running up-counter on the LEDs of PORTD.
 ---------------------------------------------------*/
#include <p18f452.h>
#include "../PIC18F452Config.h" // Configuration bits setting for PIC
#include "../SnA.h"
#define byte unsigned char // byte is used for unsigned char type

// Prototypes
void init();

// Initialises ports and other bits and pieces not directly relevant to the main
// run loop
void init()
{
    DDRB = DDRB & 0b00000001; // Set the 0th bit of TRISB to input
    DDRD = 0; // PORTD as output
    PORTBbits.RB0 = 0; // Clear the 0th bit
    PORTD = 0; // clear LEDs of PortD
    init_LCD(); // Initialise the LCD
}

void main()
{
    // Variable declarations
    byte counter; // Counter byte

    init(); // Initialise our environment

    counter = 0; // Set the counter to nothing
    while (1) { // Repeat forever
        LCD_line1; // Move to the first line
        LCD_message("W00t!"); // Write some arbitrary rubbish to the first line

        LCD_line2; // Move to the second line
        LCD_BCD(counter); // Write the counter as hex to the LCD
        LCD(0xDF); // Display the degree symbol
        LCD('C'); // Display the Celsuis symbol
        LCD(' '); // Space
        if (counter <= 100)
        {
            LCD_message("Low   ");
        }
        else if (counter <= 200)
        {
            LCD_message("Medium");
        }
        else
        {
            LCD_message("High  ");
        }

        pause(200); // Pause for 200ms
        counter = a2d(0); // Read from ADC
    }
}