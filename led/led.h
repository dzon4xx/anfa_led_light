/* 
* led.h
*
* Created: 2016-03-05 19:52:04
* Author: jan.pleszynski
*/


#ifndef __LED_H__
#define __LED_H__

#include "../utilities/memory/memory.h"
#include "../settings.h"

class led
{

//functions
public:
	led(uint8_t channel);
	
	//Ustaw nastawe mocy
	void set_desired_pwm(uint8_t desired_pwm);

	//Ustaw czas rozjasniania i przyciemniania
	void set_dim_time(uint16_t desired_dim_period);
	
	//Inicjalizacja pwm
	static void init_pwms();


//variables
public:
	
	//rejestr pwm
	volatile uint8_t *ocr_port;

	//nastawa mocy
	uint8_t desired_pwm = 0;

	//moc
	uint8_t actual_pwm = 0;

	//kanal
	uint8_t channel;


	//czas zmiany poziomu mocy o 1 jednostke
	uint32_t incr_time = 1;

	//Czynnik inkrementacji. 1 dla rozjasniania, -1 dla przyciemniania
	int8_t	incr_power = 1;


}; //led

extern led *leds[NUM_OF_CHANNELS];

#endif //__LED_H__
