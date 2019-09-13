// The header file for library riutunes used in Sensors and Actuators

//#include <p18f452.h>

#define byte unsigned char

#define green_LED PORTBbits.RB7			// address of debug LEDs
#define amber_LED PORTBbits.RB6
#define red_LED   PORTBbits.RB3

#define BP _asm NOP _endasm;			// nop to stop at breakpoint
#define bp			BP
#define break_point BP
#define BREAK_POINT BP
#define nop			bp					// nop
//#define NOP			nop

#define LCD_line1 LCD_cmd(0x80);
#define LCD_line2 LCD_cmd(0xc0);


extern void init (void);

extern void pause (unsigned int time);

extern void delay_10us(unsigned int time);

extern void pulsout (byte pin, byte duration);

extern byte read_EEPROM (byte address);

extern void write_EEPROM (byte address, byte dat);

extern void debug_LEDS (byte data);

extern void init_LCD (void);

extern void LCD (byte data);

extern void LCD_cmd (byte code);

extern void LCD_HEX (byte num);

extern void LCD_BCD (byte num);

extern void LCD_BCD2 (byte num);

extern byte ASCII (byte num);

extern void LCD_message (const rom char* msg);

extern void LCD_display (char* msg);

extern byte a2d0 (void);

extern byte a2d (byte channel);

extern void LEDS (byte num);

extern void BAR (byte num);

extern void init_motor(void);

extern void motor_control(void);



