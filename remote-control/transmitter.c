#include <p18f452.h>
#include "../PIC18F452Config.h" // Configuration bits setting for PIC
#include "../SnA.h"
#define byte unsigned char // byte is used for unsigned char type

// Prototypes
void init();
void beep();
void beep(byte tone, int cycles);
void send_code(byte code);

// Sanity definitions
#define IR_RX PORTCbits.RC7
#define IR_TX PORTCbits.RC6
#define IR_HIGH 0
#define IR_LOW 1
#define IR_STOP IR_HIGH
#define IR_START IR_LOW
#define BUTTON_RED PORTBbits.RB0
#define BUTTON_GRN PORTBbits.RB1
#define BUTTON_HIGH 0
#define BUTTON_LOW 1
#define BUZZER PORTCbits.RC1

// Code definitions
// Only the last 3 bits are used
//                 XXXXX---
#define CODE_NOP 0b00000000 // No-op
#define CODE_VUP 0b00000001 // Volume up
#define CODE_VDN 0b00000010 // Volume down
#define CODE_VMT 0b00000011 // Mute/unmute
#define CODE_CUP 0b00000100 // Channel up
#define CODE_CDN 0b00000101 // Channel down
#define CODE_CRS 0b00000110 // Channel reset

void init()
{
    TRISBbits.TRISB0 = 1; // Set B0 to input
    TRISBbits.TRISB1 = 1; // Set B1 to input
    TRISC = 0b10000000; // Set C7 to input and the rest to output
    PORTC = 0b00000000; // Set our outputs low
}

void main()
{
    // Silly embedded C things
    byte code;
    byte dipAndButtons

    init();

    while (1) // Spin forever
    {
        // Get the B5 DIP setting and B0-1 settings in a single three-bit block
        // PORTB is inverted (~) and shifted to move B5 to 2 and B0-1 to 0-1
        // So the output is interpreted as: 0b101
        //                            DIP on  ^||
        //                        Button 0 off ^|
        //                          Button 1 on ^
        //                           B5                    B0-1        Mask
        dipAndButtons = (~PORTB & 0b00100000 >> 3) | (~PORTB & 0b11) & 0b111;

        // Choose which code we should send based on the DIP and buttons
        switch (dipAndButtons)
        {
            case 0b001:
                code = CODE_VUP;
                break;
            case 0b010:
                code = CODE_VDN;
                break;
            case 0b011:
                code = CODE_VMT;
                break;
            case 0b101:
                code = CODE_CUP;
                break;
            case 0b110:
                code = CODE_CDN;
                break;
            case 0b111:
                code = CODE_CRS;
                break;
            case 0b000:
            case 0b100:
            default:
                code = CODE_NOP;
                break;
        }

        // Send the code if it isn't NOP
        if (code != CODE_NOP)
        {
            // Send the code
            send_code(code);

            // Beep to let the operator know we've sent something
            beep();
        }

        // Wait 500ms to allow the operator time to change the buttons
        pause(500);
    }
}

// Sound a beep via the onboard buzzer with a pre-set tone
void beep()
{
    beep(2000, 500);
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

// Send a single byte over IR
void send_code(byte code)
{
    int timingInterval = 100; // 100ms timing

    // Send the start bit
    IR_TX = IR_START;
    pause(timingInterval);

    // Send the code
    byte mask;
    for (mask = 0b00000001; mask > 0b00000000; mask <<= 1)
    {
        IR_TX = !(code & mask); // Set IR to masked bit
                                // Inverted for circuit reasons
        pause(timingInterval);
    }

    // Send both stop bits
    IR_TX = IR_STOP;
    pause(timingInterval * 2);
}
