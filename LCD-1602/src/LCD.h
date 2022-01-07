/****************************************************************************
    Copyright (C) 2022 Ovidiu Sabau
	 
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
 
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
 
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
/****************************************************************************
	File:		LCD.c
	Date:		7th January 2022
	Target:		ATmega328p
	Compiler:	avr-gcc (AVR Studio 6)
	Author:		Ovidiu Sabau
 
	Summary:    LCD 16x2 8-bit data interface, with busy flag implemented.
                Any LCD pin can be connected to any available I/O port.
                Includes a simple write string routine.
 */

/****************************************************************************
 
	The four data lines as well as the three control lines may be
	implemented on any available I/O pin of any port.  These are
	the connections used for this program:
 
                 -----------                   ----------
                | ATmega328p|                 |   LCD    |
                |           |                 |          |
                |        PD7|---------------->|D7        |
                |        PD6|---------------->|D6        |
                |        PD5|---------------->|D5        |
                |        PD4|---------------->|D4        |
                |           |                 |D3        |
                |           |                 |D2        |
                |           |                 |D1        |
                |           |                 |D0        |
                |           |                 |          |
                |        PD3|---------------->|E         |
                |        PD0|---------------->|RW        |
                |        PD1|---------------->|RS        |
                 -----------                   ----------
 
  **************************************************************************/

/* I/O Ports Management
- DDR Register controls if the port is input or output
- PORT Register controls if the port is HIGH or LOW
- PIN Register reads the state of an input port
*/

 #ifndef DEFINESLCD_H_
 #define DEFINESLCD_H_

	#ifndef F_CPU
	#define F_CPU 16000000UL
	#endif
 
	#include <avr/io.h>
	#include <util/delay.h>
	
	#define NONE 255
	
	/* Data ports */
	#define lcd_Data_Port		PORTD
	#define lcd_Data_DDR		DDRD
	
	
	/* Data pins */
	#define lcd_D7_bit			PORTD7
	#define lcd_D7_pin			PIND
	#define lcd_D6_bit			PORTD6
	#define lcd_D5_bit			PORTD5
	#define lcd_D4_bit			PORTD4

	/* Control pins */
	#define lcd_Ctlr_Port		PORTD
	#define lcd_Ctlr_DDR		DDRD
	#define lcd_E_bit			PORTD3
	#define lcd_RS_bit			PORTD1
	#define lcd_RW_bit			PORTD0

	#define lcd_ONOFF_bit		PORTB0
	
	/* LCD module information */
	#define lcd_LineOne     0x00				// Start line 1
	#define lcd_LineTwo     0x40				// Start line 2

	/* LCD instructions */
	#define lcd_Clear           0b00000001		// Replace all characters with ASCII 'space'
	#define lcd_Home            0b00000010		// Return cursor to first position on first line
	#define lcd_EntryMode       0b00000110		// Shift cursor from left to right on read/write
	#define lcd_DisplayOff      0b00001000		// Turn display off
	#define lcd_DisplayOn       0b00001100		// Display on, cursor off, don't blink character
	#define lcd_FunctionReset   0b00110000		// Reset the LCD
	#define lcd_FunctionSet4bit 0b00101000		// 4-bit data, 2-line display, 5 x 7 font
	#define lcd_SetCursor       0b10000000		// Set cursor position

	/* Macros */
	#define set_RW_LOW_write	lcd_Ctlr_Port &= ~(1 << lcd_RW_bit)		// RW low = write
	#define set_RW_HIGH_read	lcd_Ctlr_Port |=  (1 << lcd_RW_bit)		// RW high = read
	
	#define set_RS_LOW_Instr	lcd_Ctlr_Port &= ~(1 << lcd_RS_bit)		// RS low = instruction
	#define set_RS_HIGH_Data	lcd_Ctlr_Port |=  (1 << lcd_RS_bit)		// RS high = data

	#define disable_E_Pin		lcd_Ctlr_Port &= ~(1 << lcd_E_bit)		// Disable E pin
	#define enable_E_Pin		lcd_Ctlr_Port |=  (1 << lcd_E_bit)		// Enable E pin
	
	#define set_D7_input		lcd_Data_DDR &= ~(1 << lcd_D7_bit)
	#define set_D7_output		lcd_Data_DDR |=  (1 << lcd_D7_bit)

	/* Function Prototypes */
	void lcd_write_4(uint8_t);
	void lcd_write_instruction_4f(uint8_t);
	void lcd_write_character_4f(uint8_t);
	void lcd_write_string_4f(uint8_t *);
	void lcd_init_4f(void);
	void lcd_check_BF_4(void);

	/* Custom functions */
	void initLCD(void);
	void configLCDPorts(void);
	
	void updateLCDScreen(uint8_t, char *, uint8_t, char *);
	void displayLCD_main(uint8_t, char *, uint8_t, char *);
	

#endif /* DEFINESLCD_H_ */