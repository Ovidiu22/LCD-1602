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
	//updateLCDScreen(row on LCD, "Text 1", number, "Text 2");		
	updateLCDScreen(1, "Hello there!", NONE, "NONE"); _delay_ms(1000);
	updateLCDScreen(1, "CLEAR", NONE, "NONE");
	
	_delay_ms(1000);
	
	updateLCDScreen(1, "Temperature: ", temperature, "C");
	updateLCDScreen(2, "Humidity: ", rel_humidity, "%");

	return 0;
}