/* 
* led.cpp
*
* Created: 2016-03-05 19:52:04
* Author: jan.pleszynski
*/


#include "led.h"
#include "../settings.h"
#include <avr/io.h>
#include <avr/pgmspace.h>

#define MAX_PWM 255
#define MAX_DIM_TIME 16711 // MAX_PWM*65355/1000
#define S_TO_MS 1000

const uint8_t gamma_correction[] PROGMEM = {
  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  4,
  4,  4,  4,  5,  5,  5,  6,  6,  6,  6,  7,  7,  7,  8,  8,  8,  9,  9,
 10, 10, 10, 11, 11, 12, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17,
 18, 18, 19, 19, 20, 21, 21, 22, 22, 23, 23, 24, 25, 25, 26, 27, 27, 28,
 29, 29, 30, 31, 31, 32, 33, 33, 34, 35, 36, 36, 37, 38, 39, 40, 40, 41,
 42, 43, 44, 44, 45, 46, 47, 48, 49, 50, 51, 51, 52, 53, 54, 55, 56, 57,
 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 75, 76,
 77, 78, 79, 80, 81, 82, 84, 85, 86, 87, 88, 90, 91, 92, 93, 95, 96, 97,
 98,100,101,102,103,105,106,107,109,110,111,113,114,116,117,118,120,121,
123,124,126,127,128,130,131,133,134,136,137,139,140,142,143,145,147,148,
150,151,153,155,156,158,159,161,163,164,166,168,169,171,173,175,176,178,
180,181,183,185,187,189,190,192,194,196,198,199,201,203,205,207,209,211,
213,214,216,218,220,222,224,226,228,230,232,234,236,238,240,242,244,246,
248,250,252,255
};

 led::led(uint8_t channel)
{
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
		this->ocr_port = &OCR2B;
	}
}

void led::set_desired_pwm(uint8_t desired_pwm)
{

	this->desired_pwm = pgm_read_byte(gamma_correction+desired_pwm) ;

	if (desired_pwm > this->actual_pwm)
	{
		this->incr_power = 1;
	}

	else if (desired_pwm<actual_pwm)
	{
		this->incr_power = -1;
	}
	
	else
	{
		this->incr_power = 0;
	}
}

void led::set_dim_time(uint16_t desired_dim_period)
{	
	this->incr_time = desired_dim_period*S_TO_MS/MAX_PWM;	//Czas co jaki ma siê inkrementowac 1 poziom jasnosci
}

void led::init_pwms()
{
	//timer0 oc0a
	TCCR0B |= (1<<CS01);		//preskaler 64
	TCCR0A |= (1<<WGM00);					//tryb korekcji fazy
	TCCR0A |= ((1<<COM0A0) | (1<<COM0A1));	//kanal 1 pd6  tryb pwm odwrocony. nie odwrocony bez com0a0
	TCCR0A |= ((1<<COM0B0) | (1<<COM0B1));	//kanal 2 pd5  tryb pwm odwrocony. nie odwrocony bez com0a0

	//kanal 3 pd3 timer2 oc2b
	TCCR2B |= (1<<CS21);		//preskaler 64
	TCCR2A |= (1<<WGM20);					//tryb korekcji fazy
	TCCR2A |= ((1<<COM2B0) | (1<<COM2B1)); // kanal 3 tryb pwm odworony. nie odwrocony bez com0a0

	DDRD |= ((1<<PIND3) | (1<<PIND5) | (1<<PIND6));	//ustawienie pinow jako wyjscia

}


led *leds[NUM_OF_CHANNELS];





