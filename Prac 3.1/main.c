#include <p18f452.h>
#include "../PIC18F452Config.h" // Configuration bits setting for PIC
#include "../SnA.h"
#define byte unsigned char // byte is used for unsigned char type

// Sanity definitions
#define IR_RX PORTCbits.RC7
#define IR_TX PORTCbits.RC6
#define IR_HIGH 0
#define IR_LOW 1
#define BUTTON_RED PORTBbits.RB0
#define BUTTON_GRN PORTBbits.RB1
#define BUTTON_HIGH 0
#define BUTTON_LOW 1
#define BUZZER PORTCbits.RC1


// Prototypes
void init();
void beep(byte tone, int cycles);
void send_code(byte code);

// Initialises our ports
void init()
{
    TRISBbits.TRISB0 = 1; // Set B0 to input
    TRISBbits.TRISB1 = 1; // Set B1 to input
    TRISC = 0b10000000; // Set C7 to input and the rest to output
    PORTC = 0b11000000; // Set our outputs low
}

void main()
{
    // Silly embedded C things
    byte code;

    init();

    while (1)
    {
        if (IR_RX == IR_HIGH) // IR receiver active
        {
            beep(100, 50); // Beep if we can see an IR signal
        }

        switch (PORTB & 0b00000011)
        {
            case 0b00000000:
                code = 0b11110000;
                break;
            case 0b00000001:
                code = 0b11001100;
                break;
            case 0b00000010:
                code = 0b10101010;
                break;
            default:
                code = 0b00000000;
                break;
        }

        if (code)
        {
            send_code(code);
            pause(500);
        }
    } // Spin forever
}

// Sound a beep via the onboard buzzer
void beep(byte tone, int cycles)
{
    unsigned int i;
    for (i = 0; i < cycles; i++)
    {
        BUZZER = 1;
        delay_10us(tone);
        BUZZER = 0;
        delay_10us(tone);
    }
}

// Send a single byte over IR
void send_code(byte code)
{
    byte mask;
    for (mask = 0b00000001; mask > 0b00000000; mask <<= 1)
    {
        IR_TX = !(code & mask); // Set IR to masked bit
                                // Inverted for circuit reasons
        pause(100);
    }

    IR_TX = IR_LOW; // Turn the IR LED off
}
