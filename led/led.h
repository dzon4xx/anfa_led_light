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
	//Inicjalizacja pwm
	static void init_pwms();
//functions
public:
	led(uint8_t channel, Memory *mem);
	
	//Ustaw nastawe mocy
	void set_desired_pwm(uint16_t desired_pwm);

	//Ustaw czas rozjasniania i przyciemniania
	void set_dim_time(uint16_t desired_dim_period);

private:

	//Ustaw wartosc wypelnieniea na mosfecie
	void set_pwm();




//variables
public:

private:
	
	//rejestr pwm
	volatile uint8_t *ocr_port;

	//Czy nastawa mocy jest rozna od aktualnej mocy
	bool is_dim;

	//nastawa mocy
	uint8_t desired_pwm;

	//moc
	uint8_t actual_pwm;

	//kanal
	uint8_t channel;

	//okres rozjasniania, przyciemniania w s
	uint8_t dim_period;

	//czas zmiany poziomu mocy o 1 jednostke
	uint8_t incr_time;

	//Czynnik inkrementacji. 1 dla rozjasniania, -1 dla przyciemniania
	uint8_t	incr_power = 1;


}; //led

extern led *leds[NUM_OF_CHANNELS];

#endif //__LED_H__
