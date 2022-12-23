#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "LCD.h"

/* *****************************************************************
Name:		initLCD
Inputs:		none
Outputs:	none
Description:Configures the Data / command 
			ports and initializes the LCD
******************************************************************** */
void initLCD(void)
{
	/* Configure the data ports and pins */
	configLCDPorts();

	/* Initialize the LCD controller as determined by instructions */
	lcd_init_4f();
	
	_delay_ms(100);
}

/* *****************************************************************
Name:		configLCDPorts
Inputs:		none
Outputs:	none
Description:maps the data and control pins to the uC i/o ports
******************************************************************** */
void configLCDPorts(void)
{
	/* configure the microprocessor pins for the data lines */
	lcd_Data_DDR |= (1<<lcd_D7_bit) | (1<<lcd_D6_bit) | (1<<lcd_D5_bit) | (1<<lcd_D4_bit);
	
	/* configure the microprocessor pins for the control lines */
	lcd_Ctlr_DDR |= (1<<lcd_RW_bit) | (1<<lcd_RS_bit) | (1<<lcd_E_bit);
	
	/* Turn on LCD */
	DDRB |= (1<<lcd_ONOFF_bit);
}


/* *****************************************************************
Name:		lcd_init_4f
Inputs:		none
Outputs:	none
Description:initializes the LCD for 4-bit data interface; uses the 
			busy flag instead of time delay when possible
******************************************************************** */
void lcd_init_4f(void)
{
	/* Power-up delay */
	_delay_ms(50);                                 // initial 40 mSec delay

	/* IMPORTANT - At this point the LCD module is in the 8-bit mode and it is expecting to receive
	   8 bits of data, one bit on each of its 8 data lines, each time the 'E' line is pulsed.
	
	 Since the LCD module is wired for the 4-bit mode, only the upper four data lines are connected to
	 the microprocessor and the lower four data lines are typically left open.  Therefore, when
	 the 'E' line is pulsed, the LCD controller will read whatever data has been set up on the upper
	 four data lines and the lower four data lines will be high (due to internal pull-up circuitry).
	
	 Fortunately the 'FunctionReset' instruction does not care about what is on the lower four bits so
	 this instruction can be sent on just the four available data lines and it will be interpreted
	 properly by the LCD controller.  The 'lcd_write_4' subroutine will accomplish this if the
	 control lines have previously been configured properly. */

	/* Set up the RS, E, and RW lines for the 'lcd_write_4' function. */
	set_RS_LOW_Instr;	// Select the Instruction Register (RS low)
	disable_E_Pin;		// Make sure E is initially low
	set_RW_LOW_write;	// write to LCD module (RW low)

	/* Reset the LCD controller */
	lcd_write_4(lcd_FunctionReset);		// First part of reset sequence
	_delay_us(10);                      // Delay of 10 us
	
	lcd_write_4(lcd_FunctionReset);		// Second part of reset sequence
	_delay_us(200);						// Delay of 200 us

	lcd_write_4(lcd_FunctionReset);		// Third part of reset sequence
	_delay_us(80);						// This delay is omitted in the data sheet

	/* Preliminary Function Set instruction - used only to set the 4-bit mode.
	 The number of lines or the font cannot be set at this time since the controller is still in the
	 8-bit mode, but the data transfer mode can be changed since this parameter is determined by one
	 of the upper four bits of the instruction. */
	
	lcd_write_4(lcd_FunctionSet4bit);	// Set 4-bit mode
	// From this point on the busy flag is available

	// Function Set instruction
	lcd_check_BF_4();    // Makes sure LCD controller is ready
                         
	lcd_write_instruction_4f(lcd_FunctionSet4bit);  // Set mode, lines, and font

	/*The next three instructions are specified in the data sheet as part of the initialization routine,
	  so it is a good idea (but probably not necessary) to do them just as specified and then redo them
	  later if the application requires a different configuration.*/

	/* Display On/Off Control instruction */
	lcd_check_BF_4();
	lcd_write_instruction_4f(lcd_DisplayOff);       // Turn display OFF

	/* Clear Display instruction */
	lcd_check_BF_4();
	lcd_write_instruction_4f(lcd_Clear);            // Clear display RAM
	
	/* Entry Mode Set instruction */
	lcd_check_BF_4();
	lcd_write_instruction_4f(lcd_EntryMode);        // Set desired shift characteristics
	/*This is the end of the LCD controller initialization as specified in the data sheet, but the display
	  has been left in the OFF condition.  This is a good time to turn the display back ON. */

	/* Display On/Off Control instruction */
	lcd_check_BF_4();
	lcd_write_instruction_4f(lcd_DisplayOn);        // turn the display ON
}

/* *****************************************************************
Name:		updateLCDScreen
Inputs:		two pointers to a uint8 type data and 2 values, one for each row
Outputs:	none
Description:commands writing on LCD line by line 
******************************************************************** */
void updateLCDScreen(uint8_t row, char * s1, uint8_t data, char * s2)
{
	/* convert data to string [buf] */
	char data2char[4] = "   ";
	char charRow[17];
	char emptyString[] = "                 ";	//17 empty spaces
	
	if (data != NONE)	{ itoa(data, data2char, 10);}
	if (strcmp(s1, "NONE") == 0 ) { s1 = ""; }
	if (strcmp(s2, "NONE") == 0 ) { s2 = ""; }
	
	/* Create the strings to be displayed */
	snprintf( charRow, sizeof charRow, "%s%s%s%s", s1, data2char, s2, emptyString );
	if (strcmp(s1, "CLEAR") == 0 ) { snprintf( charRow, sizeof charRow, "%s", emptyString ); };
		
	switch(row) {
		case 1: /* ======= Line 1 ======= */
			lcd_write_instruction_4f(lcd_SetCursor | lcd_LineOne);
			break;
		case 2: /* ======= Line 2 ======= */
			lcd_write_instruction_4f(lcd_SetCursor | lcd_LineTwo);
			break;
	}
	lcd_write_string_4f((unsigned char*) charRow);
}

/* *****************************************************************
Name:		lcd_write_string_4f
Inputs:		string to be displayed (stringData)
Outputs:	none
Description:writes a string of characters on LCD; uses the busy flag
******************************************************************** */
void lcd_write_string_4f(uint8_t stringData[])
{
    volatile int i = 0;                             // Character counter
    while (stringData[i] != 0)
    {
        lcd_check_BF_4();                           // Make sure LCD controller is ready    
        lcd_write_character_4f(stringData[i]);
        i++;
    }
}


/* *****************************************************************
Name:		lcd_write_character_4f
Inputs:		character to be displayed (theChar)
Outputs:	none
Description:sends a byte to the LCD data register, configures RW 
			(busy flag is implemented)
******************************************************************** */
void lcd_write_character_4f(uint8_t theChar)
{
    set_RW_LOW_write;					// Write to LCD module (RW low)
    set_RS_HIGH_Data;					// Select the Data Register (RS high)
    disable_E_Pin;						// Make sure E is initially low
    lcd_write_4(theChar);				// Write the upper 4-bits of the data
    lcd_write_4(theChar << 4);			// Write the lower 4-bits of the data
}

/* *****************************************************************
Name:		lcd_write_instruction_4f
Inputs:		character to be displayed (theChar)
Outputs:	none
Description:sends a byte to the LCD instruction register, configures RW 
			(busy flag is implemented)
******************************************************************** */
void lcd_write_instruction_4f(uint8_t theInstruction)
{
    set_RW_LOW_write;					// Write to LCD module (RW low)
    set_RS_LOW_Instr;					// Select the Data Register (RS high)
    disable_E_Pin;						// Make sure E is initially low
    lcd_write_4(theInstruction);		// Write the upper 4-bits of the data
    lcd_write_4(theInstruction << 4);	// Write the lower 4-bits of the data
}


/* *****************************************************************
Name:		lcd_write_4
Inputs:		(theByte) is a byte with the desired 4 bits in the upper nibble
Outputs:	none
Description:sends 4 bits to the LCD module; 
			RS is configured for the expected register
			E is low, RW is low
			uses either time delays or the busy flag
******************************************************************** */
void lcd_write_4(uint8_t theNibble)
{
	uint8_t mask[4] = {0};
	for (uint8_t i = 0; i < 4; i++)
	{
		if (theNibble & 1 << (7 - i))
		{
			mask[i] = 1;
		}
	}
	
	/* Clear bits in first step */
	lcd_Data_Port &= ~((1 << lcd_D7_bit) | (1 << lcd_D6_bit) | (1 << lcd_D5_bit) | (1 << lcd_D4_bit));

	/* Set bits where necessary */
	lcd_Data_Port |= ((mask[0]) << lcd_D7_bit | (mask[1]) << lcd_D6_bit | (mask[2]) << lcd_D5_bit | (mask[3]) << lcd_D4_bit);

    /* Write the data */
    enable_E_Pin;                   // Enable pin high
    _delay_us(1);                   // implement 'Data set-up time' (80 nS) and 'Enable pulse width' (230 nS)
    disable_E_Pin;                  // Enable pin low
    _delay_us(1);                   // implement 'Data hold time' (10 nS) and 'Enable cycle time' (500 nS)
}


/* *****************************************************************
Name:		lcd_check_BF_4
Inputs:		none
Outputs:	none
Description:checks busy flag and wait until LCD is ready 
			Desired information is the higher nibble, however both are read
			data is read while 'E' is high
******************************************************************** */
void lcd_check_BF_4(void)
{
	
    uint8_t busy_flag_copy;             // busy flag 'mirror'

    set_D7_input;						// set D7 data direction to input
    set_RS_LOW_Instr;					// select the Instruction Register (RS low)
    set_RW_HIGH_read;					// read from LCD module (RW high)

    do
    {
        busy_flag_copy = 0;                         // initialize busy flag 'mirror'
        lcd_Ctlr_Port |= (1<<lcd_E_bit);            // Enable pin high
        _delay_us(1);                               // implement 'Delay data time' (160 nS) and 'Enable pulse width' (230 nS)

        busy_flag_copy |= (lcd_D7_pin & (1<<lcd_D7_bit));  // get actual busy flag status

        lcd_Ctlr_Port &= ~(1<<lcd_E_bit);           // Enable pin low
        _delay_us(1);                               // implement 'Address hold time' (10 nS), 'Data hold time' (10 nS), and 'Enable cycle time' (500 nS )
        
// read and discard alternate nibbles (D3 information)
        lcd_Ctlr_Port |= (1<<lcd_E_bit);            // Enable pin high
        _delay_us(1);                               // implement 'Delay data time' (160 nS) and 'Enable pulse width' (230 nS)
        lcd_Ctlr_Port &= ~(1<<lcd_E_bit);           // Enable pin low
        _delay_us(1);                               // implement 'Address hold time (10 nS), 'Data hold time' (10 nS), and 'Enable cycle time' (500 nS )

    } while (busy_flag_copy);                       // check again if busy flag was high

// arrive here if busy flag is clear -  clean up and return 
    set_RW_LOW_write;               // write to LCD module (RW low)
    set_D7_output;                  // reset D7 data direction to output	
}


