#include "Clock.h"
#include "../../communication/modbus_slave.h"
#include "../../led/led.h"
#include "../../settings.h"
#include <avr/io.h>
#include <avr/interrupt.h>
//#include "../../settings.h"

#define DEFAULT_RESPOND_RATE 100000		// Co jaki czas stacja uzyskuje
#define  OVF_VAL 249	//wartosc rejestru OCR1 aby timer przepelnial sie co 1ms

//Zmienna globalna - szybszy dostep
volatile uint32_t miliseconds_passed = 0;

 Clock::Clock()
{
	cli();
	OCR1A = OVF_VAL;
	TIMSK1 = (1<<OCIE1A);
	TCCR1B |= (1<WGM12);			
	sei();
}

void Clock::start()
{
	TCCR1B |= ((1<<CS10)|(1<<CS11));
}

void Clock::stop()
{
	TCCR1B &= ~((1<<CS10)|(1<<CS11));
}

void Clock::restart()
{
	miliseconds_passed = 0;
	this->stop();
	OCR1A = OVF_VAL;
	this->start();
}

unsigned long Clock::get_ms()
{
	return miliseconds_passed;
}


Clock *clock;

ISR (TIMER1_COMPA_vect) 
{
	miliseconds_passed++;

	for (uint8_t i=0; i<NUM_OF_CHANNELS; i++)
	{
		led *crnt_led = leds[i];

		if(crnt_led->actual_pwm != crnt_led->desired_pwm)
		{
			
			if (crnt_led->incr_time == 0)
			{
				crnt_led->actual_pwm = crnt_led->desired_pwm;
			}			
			else if (miliseconds_passed%crnt_led->incr_time==0) //linearyzacja mocy ze wzgledu na incr_time
			{
					crnt_led->actual_pwm += crnt_led->incr_power;
			}
			*(crnt_led->ocr_port) = crnt_led->actual_pwm;
		}
		
	}
	TCNT1 = 0;
}