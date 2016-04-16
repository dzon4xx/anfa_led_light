/*
 * pin.h
 *
 * Created: 2016-03-03 10:58:58
 *  Author: jan.pleszynski
 */ 


#ifndef PIN_H_
#define PIN_H_

#include <stdint.h>

class Pin
{
	public:
	Pin(uint8_t port_letter, uint8_t pin_num);

	//zwraca litere portu
	uint8_t get_port_letter();

	//zwraca maske pinu. 
	uint8_t get_pin_mask();

	uint8_t get_channel_num();

	volatile uint8_t* get_dir_port();

	volatile uint8_t* get_in_port();

	volatile uint8_t* get_out_port();

	void set_input();

	void set_output();

	//Ustawia pin w stan wysoki
	void high();

	//Ustawia pin w stan wysoki
	void low();

	//Zwraca stan pinu
	bool read();



private:

	//Rejestr wyjsciowy PORTX
	volatile uint8_t *out_port;

	//Rejestr wejœciowy PINX
	volatile uint8_t *in_port;

	//Rejestr kierunku DDRX
	volatile uint8_t *dir_port;

	//Maska pinu
	uint8_t pin_mask;

	uint8_t port_letter;

};


#endif /* PIN_H_ */