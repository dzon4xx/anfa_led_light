/*
 * pin.h
 *
 * Created: 2016-03-03 10:58:58
 *  Author: jan.pleszynski
 */ 


#ifndef PIN_H_
#define PIN_H_

#include <stdint.h>

enum pin_mode
{
	OUTPUT,
	INPUT,	
};


class Pin
{
public:

	//zwraca litere portu
	uint8_t get_port_letter() {	return port_letter;}

	//zwraca maske pinu. 
	uint8_t get_pin_mask(){	return pin_mask; }

	//Maska pinu
	uint8_t pin_mask;

	uint8_t port_letter;
	
protected:



};

class Out_pin : virtual public Pin
{
public:

	Out_pin(){};

	Out_pin(uint8_t port_letter, uint8_t pin_num);
	
	//Zwraca port wyjsciowy
	volatile uint8_t* get_out_port(){ return out_port; }

	//Ustawia pin w stan wysoki
	inline void on(){ *(out_port) |= pin_mask; }

	//Ustawia pin w stan niski
	inline void off(){ *(out_port) &= ~pin_mask; }
		
	inline void toggle() {*(out_port) ^= pin_mask;}
		
	inline bool get_state() {return *out_port & pin_mask;}
		 
protected:
	
	//Rejestr wyjsciowy PORTX
	volatile uint8_t *out_port;

};

class In_pin : virtual public Pin
{

public:
	In_pin(){};
	In_pin(uint8_t port_letter, uint8_t pin_num, bool pull_up);

	volatile uint8_t* get_in_port(){return in_port;}

	inline bool read_state() {return *in_port & pin_mask;} 

protected:

	volatile uint8_t *in_port;
	volatile uint8_t *dir_port;

};

class IO_pin : public In_pin, public Out_pin
{
	public:
	IO_pin(uint8_t port_letter, uint8_t pin_num, pin_mode mode = INPUT);
	
	void set_mode(pin_mode mode);
	private:
	
};

#endif