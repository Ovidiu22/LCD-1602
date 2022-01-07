/*******************************/
/*** Created by Ovidiu Sabau ***/
/***	7th January 2022	 ***/
/*******************************/

/*----------------- Include -------------------*/
#include "LCD.h"

/*-------------- Main function ----------------*/
int main(void)
{
	uint8_t temperature = 16; //C
	uint8_t rel_humidity = 42; //%
	
	initLCD();
//displayLCD_main(row on LCD, "Text 1", number, "Text 2");		
	displayLCD_main(1, "Hello there!", NONE, "NONE"); _delay_ms(1000);
	displayLCD_main(1, "CLEAR", NONE, "NONE");
	
	_delay_ms(1000);
	
	displayLCD_main(1, "Temperature: ", temperature, "C");
	displayLCD_main(2, "Humidity: ", rel_humidity, "%");

	return 0;
}