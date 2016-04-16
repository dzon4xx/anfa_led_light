/*
 * pin.cpp
 *
 * Created: 2016-03-03 10:58:24
 *  Author: jan.pleszynski
 */ 

 #include <avr/io.h>
#include "pin.h"
 Pin::Pin(uint8_t port_letter, uint8_t pin_num)
 {
	pin_mask = (1<<pin_num);
	this->port_letter = port_letter; 

	#ifdef PORTA
	if (port_letter == 'A')
	{
		out_port = &PORTA;
		in_port  = &PINA;
		dir_port = &DDRA;
	}
	#endif


	#ifdef PORTB
	if (port_letter == 'B')
	{
		out_port = &PORTB;
		in_port  = &PINB;
		dir_port = &DDRB;
	}
	#endif

	#ifdef PORTC
	if (port_letter == 'C')
	{
		out_port = &PORTC;
		in_port  = &PINC;
		dir_port = &DDRC;
	}
	#endif

	#ifdef PORTD
	if (port_letter == 'D')
	{
		out_port = &PORTD;
		in_port  = &PIND;
		dir_port = &DDRD;
	}
	#endif

	#ifdef PORTE
	if (port_letter == 'E')
	{
		out_port = &PORTE;
		in_port  = &PINE;
		dir_port = &DDRE;
	}
	#endif

	#ifdef PORTF
	if (port_letter == 'F')
	{
		out_port = &PORTF;
		in_port  = &PINF;
		dir_port = &DDRF;
	}
	#endif

 }

uint8_t Pin::get_port_letter()
{
	return port_letter;
}

uint8_t Pin::get_pin_mask()
{
	return pin_mask;
}

volatile uint8_t* Pin::get_dir_port()
{
	return this->dir_port;
}

volatile uint8_t* Pin::get_in_port()
{
	return this->in_port;
}

volatile uint8_t* Pin::get_out_port()
{
	return this->out_port;
}

void Pin::set_input()
{
	*(dir_port) &= ~pin_mask;
}

void Pin::set_output()
{
	*(dir_port) |= pin_mask;
}

void Pin::high()
{
	*(out_port) |= pin_mask;
}

void Pin::low()
{
	*(out_port) &= ~pin_mask;
}

bool Pin::read()
{
	return	*(in_port) & pin_mask;
}
