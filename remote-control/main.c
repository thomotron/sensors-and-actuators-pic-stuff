// Do some fun switching so we can have both programs in the same file and still
// compile and flash with MPLABX (blasted NetBeans platform IDEs!)
//#define TRANSMITTER

#include <p18f452.h>
#include "../PIC18F452Config.h" // Configuration bits setting for PIC
#include "../SnA.h"
#include "codes.h"
#define byte unsigned char // byte is used for unsigned char type

// Prototypes
void init();
void beepOnce();
void beep(byte tone, int cycles);
void send_code(byte code);
#ifndef TRANSMITTER
void beepOP();
void beepNOP();
#endif
extern void init_LCD();
extern void LCD_BCD2(byte num);

// Sanity definitions
#define IR_RX PORTCbits.RC7
#define IR_TX PORTCbits.RC6
#define IR_HIGH 0
#define IR_LOW 1
#define IR_STOP IR_LOW
#define IR_START IR_HIGH
#define IR_IDLE IR_LOW;
#define BUTTON_RED PORTBbits.RB0
#define BUTTON_GRN PORTBbits.RB1
#define BUTTON_HIGH 0
#define BUTTON_LOW 1
#define BUZZER PORTCbits.RC1
#define IR_TIMING 30

#ifndef TRANSMITTER
int volume;
int channel;
unsigned char muted;
#endif

void init()
{
    // Buttons
    TRISBbits.TRISB0 = 1; // Button 0
    TRISBbits.TRISB1 = 1; // Button 1

    // Bar graph and LEDs
    TRISD = 0b00000000; // Set TRISD to output
    TRISAbits.TRISA3 = 0; // Bar latch
    TRISAbits.TRISA5 = 0; // LED latch

    // IR
    TRISC = 0b10000000; // Set C7 to input and the rest to output
    PORTC = 0b10000000; // Set our outputs low
    IR_TX = IR_IDLE; // ...except for IR_TX which needs to transmit the idle code

#ifndef TRANSMITTER
    // Set the volume and channel to their defaults
    volume = 0;
    channel = 1;
#endif

    init_LCD();
}

void main()
{
#ifdef TRANSMITTER
    // Silly embedded C things
    byte code = 0;
    byte dipAndButtons = 0;

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
            beepOnce();
        }

        // Wait 500ms to allow the operator time to change the buttons
        pause(100);
    }
#endif

#ifndef TRANSMITTER
    // Silly embedded C things
    unsigned char i;
    byte codeBuffer;

    init();

    while (1) // Spin forever
    {
        // Don't do anything if we haven't received a start bit
        if (IR_RX != IR_START) continue;

        // Wait half of the timing interval to synchronise half-way through
        // bit-time
        pause(IR_TIMING / 2);

        // Double-check if the start bit is still here, continuing if not
        if (IR_RX != IR_START) continue;

        // Wait until the next bit
        pause(IR_TIMING);

        // Clear the code buffer
        codeBuffer = 0;

        // Receive the next 8 bits of the code and shift it into the buffer
        for (i = 0; i < 8; i++)
        {
            // Shift left one
            codeBuffer <<= 1;

            // Append the current bit to the end
            codeBuffer |= !IR_RX;

            pause(IR_TIMING);
        }

        // Wait for the two stop bits
        // We run this twice due to issues with multiplying the IR_TIMING macro
        pause(IR_TIMING);
        pause(IR_TIMING);

        // Determine which code this was and act accordingly
        switch (codeBuffer)
        {
            case CODE_VUP: // Increase volume
                if (volume == 10)
                {
                    beepNOP();
                }
                else
                {
                    volume++;
                    beepOP();
                }
                break;
            case CODE_VDN: // Decrease volume
                if (volume == 0)
                {
                    beepNOP();
                }
                else
                {
                    volume--;
                    beepOP();
                }
                break;
            case CODE_VMT: // Toggle mute
                // TODO: Add mute/unmute
                break;
            case CODE_CUP: // Increase channel
                if (channel == 20) {
                    beepNOP();
                }
                else
                {
                    channel++;
                    beepOP();
                }
                break;
            case CODE_CDN: // Decrease channel
                if (channel == 1) {
                    beepNOP();
                }
                else
                {
                    channel--;
                    beepOP();
                }
                break;
            case CODE_CRS: // Reset channel to 1
                channel = 1;
                beepOP();
                break;
            case CODE_NOP: // Do nothing
            default:
                // Beep to indicate a NOP
                beepNOP();
                continue;
        }

        // Update the display to reflect the changes
        updateDisplay();
    }
#endif
}

// Sound a beep via the onboard buzzer with a pre-set tone
void beepOnce()
{
    beep(200, 25);
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
    byte mask;

    // Send the start bit
    IR_TX = IR_START;
    pause(IR_TIMING);

    // Send the code
    for (mask = 0b10000000; mask > 0b00000000; mask >>= 1)
    {
        IR_TX = !(code & mask); // Set IR to masked bit
                                // Inverted for circuit reasons
        pause(IR_TIMING);
    }

    // Send both stop bits
    // We run this twice due to issues with multiplying the IR_TIMING macro
    IR_TX = IR_STOP;
    pause(IR_TIMING);
    pause(IR_TIMING);

    // Go back to transmitting the idle code
    IR_TX = IR_IDLE;
}

#ifndef TRANSMITTER
// Beeps to indicate an OP
void beepOP()
{
    beepOnce();
}

// Beeps to indicate a NOP
void beepNOP()
{
    beepOnce();
    pause(10);
    beepOnce();
}

// Writes the current channel and mute status to the LCD
void updateDisplay()
{
    LCD_line1;
    LCD_message("Channel ");
    LCD_BCD2(channel);

    LCD_line2;
    LCD_message(muted ? "Mute" : "    ");
}

// Display a value on the first eight segments of the bar graph
void bar(byte value)
{
    PORTD = value; // Set PORTD to our value

    // Pulse the bar latch
    PORTAbits.RA3 = 1;
    PORTAbits.RA3 = 0;
}

// Display a value on the last two bits of the bar graph and four LEDs
void leds(byte value)
{
    PORTD = value & 0b11110011; // Set PORTD to value masked by relevant pins

    // Pulse the LED latch
    PORTAbits.RA5 = 1;
    PORTAbits.RA5 = 0;
}

// Display a value on the whole bar graph
void extendedBar(unsigned short value)
{
    // Write the first eight segments
    bar(value & 0b11111111);

    // Write the remaining two segments
    leds(value >> 8 & 0b11);
}
#endif
