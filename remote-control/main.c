// Do some fun switching so we can have both programs in the same file and still
// compile and flash with MPLABX (blasted NetBeans platform IDEs!)
#define TRANSMITTER

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

#ifndef TRANSMITTER
int volume;
int channel;
#endif

void init()
{
    TRISBbits.TRISB0 = 1; // Set B0 to input
    TRISBbits.TRISB1 = 1; // Set B1 to input
    TRISC = 0b10000000; // Set C7 to input and the rest to output
    PORTC = 0b00000000; // Set our outputs low
    IR_TX = IR_IDLE; // ...except for IR_TX which needs to transmit the idle code

#ifndef TRANSMITTER
    // Set the volume and channel to their defaults
    volume = 0;
    channel = 1;
#endif
}

void main()
{
#ifdef TRANSMITTER
    // Silly embedded C things
    byte code;
    byte dipAndButtons;

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
        pause(500);
    }
#endif

#ifndef TRANSMITTER
    // Silly embedded C things
    unsigned char i;
    byte codeBuffer;
    int timingInterval;

    init();

    while (1) // Spin forever
    {
        // Don't do anything if we haven't received a start bit
        if (IR_RX != IR_START) continue;

        // Wait half of the timing interval to synchronise half-way through
        // bit-time
        pause(timingInterval / 2);

        // Double-check if the start bit is still here, continuing if not
        if (IR_RX != IR_START) continue;

        // Wait until the next bit
        pause(timingInterval);

        // Receive the next 8 bits of the code and shift it into the buffer
        for (i = 0; i < 8; i++)
        {
            // Shift left one
            codeBuffer << 1;

            // Append the current bit to the end
            codeBuffer |= IR_RX;

            pause(timingInterval);
        }

        // Wait for the two stop bits
        pause(timingInterval * 2);

        // Determine which code this was and act accordingly
        switch (codeBuffer)
        {
            case CODE_VUP: // Increase volume
                changeVolume(1);
                break;
            case CODE_VDN: // Decrease volume
                changeVolume(-1);
                break;
            case CODE_VMT: // Toggle mute
                // TODO: Add mute/unmute
                break;
            case CODE_CUP: // Increase channel
                changeChannel(1);
                break;
            case CODE_CDN: // Decrease channel
                changeChannel(-1);
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
    }
#endif
}

// Sound a beep via the onboard buzzer with a pre-set tone
void beepOnce()
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

// Modifies the volume, capping at 0 and 10
void changeVolume(int volumeDelta)
{
    // Modify the volume
    volume += volumeDelta;

    // Check if we've crossed a boundary
    if (volume > 10)
    {
        // Cap it at 10
        volume = 10;

        // Beep to indicate a NOP
        beepNOP();
    }
    else if (volume < 0)
    {
        // Cap it at 0
        volume = 0;

        // Beep to indicate a NOP
        beepNOP();
    }
    else
    {
        // Beep to indicate an OP
        beepOP();
    }
}

// Modifies the channel, capping at 1 and 20
void changeChannel(int channelDelta)
{
    // Modify the channel
    volume += channelDelta;

    // Check if we've crossed a boundary
    if (channel > 20)
    {
        // Cap it at 10
        channel = 20;

        // Beep to indicate a NOP
        beepNOP();
    }
    else if (channel < 1)
    {
        // Cap it at 1
        channel = 1;

        // Beep to indicate a NOP
        beepNOP();
    }
    else
    {
        // Beep to indicate an OP
        beepOP();
    }
}
#endif
