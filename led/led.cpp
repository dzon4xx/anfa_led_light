/* 
* led.cpp
*
* Created: 2016-03-05 19:52:04
* Author: jan.pleszynski
*/


#include "led.h"
#include "../settings.h"

#define MAX_PWM 255
#define MAX_DIM_TIME 16711 // MAX_PWM*65355/1000

 led::led(uint8_t channel, Memory *mem)
{
#ifdef ATMEGA328
	if (channel==1)
	{
		this->ocr_port = &OCR0A;
	}
	else if (channel==2)
	{
		this->ocr_port = &OCR0B;
	}
	else if (channel==3)
	{
		this->ocr_port = &OCR2A;
	}
#endif
}

void led::set_desired_pwm(uint16_t desired_pwm)
{
	if(desired_pwm > MAX_PWM)
	{
		desired_pwm = MAX_PWM;
	}

	if (desired_pwm > this->actual_pwm)
	{
		this->is_dim = true;
		this->incr_power = 1;
		this->desired_pwm = desired_pwm;
	}

	else if (desired_pwm<actual_pwm)
	{
		this->is_dim = true;
		this->incr_power = -1;
		this->desired_pwm = desired_pwm;
	}
}

void led::set_dim_time(uint16_t desired_dim_period)
{
	if (desired_dim_period > MAX_DIM_TIME)
	{
		desired_dim_period = MAX_DIM_TIME;
	}

	if (this->dim_period != desired_dim_period)
	{
		this->dim_period = desired_dim_period;
		this->incr_time = (this->desired_pwm*1000/MAX_PWM);
	}
}

void led::set_pwm()
{
	*(this->ocr_port) = actual_pwm;
}

void led::init_pwms()
{
#ifdef ATMEGA328
	//timer0 oc0a
	TCCR0B |= (1<<CS00) | (1<<CS01);		//preskaler 64
	TCCR0A |= (1<<WGM00);					//tryb korekcji fazy
	TCCR0A |= (1<<COM0A0) | (1<<COM0A1);	//kanal 1 pd6  tryb pwm odwrocony. nie odwrocony bez com0a0
	TCCR0A |= (1<<COM0B0) | (1<<COM0B1);	//kanal 2 pd5  tryb pwm odwrocony. nie odwrocony bez com0a0

	//kanal 3 pd3 timer2 oc2b

	TCCR2B |= (1<<CS22);
	TCCR2A |= |= (1<<COM2B0) | (1<<COM2B1); // kanal 3 tryb pwm odworony. nie odwrocony bez com0a0
	TCCR2A |= (1<<WGM20);					//tryb korekcji fazy

	DDRD |= (1<<PIND3) | (1<<PIND5) | (1<<PIND6);	//ustawienie pinow jako wyjscia
#endif
}


led *led1, *led2, *led3;

led *leds[NUM_OF_CHANNELS] = {led1, led2, led3};





