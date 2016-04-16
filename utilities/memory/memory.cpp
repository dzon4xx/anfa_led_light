/*
 * eeprom.cpp
 *
 * Created: 2015-11-14 15:38:01
 *  Author: jan.pleszynski
 */ 
#include "memory.h"
#include "../../settings.h"
#include <avr/eeprom.h>


//////////////////EEPROM/////////////////////////////
#define MODULE_NOT_PROGRAMMED 255
//////////////////EEPROM/////////////////////////////

#define SIZE_MEMORY 1024
#define SIZE_LONG	4
#define SIZE_INT	2
#define SIZE_uint8_t	1
// ILOSC PAMIECI POTRZEBNA DO ZAKODOWANIA POSZCZEGOLNYCH USTAWIEN
#define SIZE_MODULE_NAME 1
#define SIZE_MODULE_PRODUCTION_TIME 4
#define SIZE_MODBUS_ADDRESS 1
#define SIZE_BAUDE_RATE 4
#define SIZE_DOUBLE_PRECISSION 1
#define SIZE_SETTINGS SIZE_MODULE_NAME+SIZE_MODULE_PRODUCTION_TIME+SIZE_MODBUS_ADDRESS+SIZE_BAUDE_RATE+SIZE_DOUBLE_PRECISSION

#define SIZE_NAME 10

//POZYCJA USTAWIEN W PAMIECI
#define POS_MODULE_NAME				0
#define POS_MODULE_PRODUCTION_TIME	SIZE_MODULE_NAME
#define POS_MODBUS_ADDRESS			POS_MODULE_PRODUCTION_TIME+SIZE_MODULE_PRODUCTION_TIME
#define POS_BAUDE_RATE				POS_MODBUS_ADDRESS+SIZE_MODBUS_ADDRESS
#define POS_DOUBLE_PRECISSION		POS_BAUDE_RATE+SIZE_BAUDE_RATE


//////////////////PUBLIC FUNCTIONS//////////////////////
Memory::Memory()
{
	//TODO: Obsluga nowej stacji. Ewentualnie jest to czêœc do zerowania jej
	//TODO: PRzywracanie pamieci do wartosci poczatkowej powoduje restartowanie kodu w kolko
	//this->restore_memory_to_default();
}

void Memory::restore_memory_to_default()
{
	for (uint8_t i=0; i<NUM_OF_CHANNELS; i++)
	{

	}

}

void Memory::set_module_name(char module_name)
{
	if(	this->read(POS_MODULE_NAME) == 255)
	{
	update(POS_MODULE_NAME, module_name);
	}
}

void Memory::set_production_time(unsigned long production_time)
{
	bool is_prod_t_set = true;
	//Sprawdz czy czas produkcji ustawiony
	uint8_t not_set_counter=0;
	for (uint8_t i=0; i<SIZE_LONG; i++)
	{
		
		//Jesli pole na ktorym zapisywany jest czas produkcji wynosi 255
		if(this->read(POS_MODULE_PRODUCTION_TIME+i)==255)
		{
			not_set_counter++;
			//Jesli wszystkie 4 pola rowne 255 to znaczy ze czas produkcji nie ustawiony
			if (not_set_counter==SIZE_LONG)
			{
				is_prod_t_set = false;
			}
		}
	}
	//jesli czas produkcji nie ustawiony to ustaw.
	if(!is_prod_t_set)
	{
		uint8_t production_time_array[SIZE_LONG];
		long_to_uint8_t_array(production_time, production_time_array);
		for (uint8_t i=0; i<SIZE_LONG; i++)
		{
			update(POS_MODULE_PRODUCTION_TIME+i, production_time_array[i]);
		}
	}
}

void Memory::set_modbus_address(uint8_t modbus_address)
{
	update(POS_MODBUS_ADDRESS, modbus_address);
}

void Memory::set_baud_rate(unsigned long baude_rate)
{
	uint8_t baude_rate_array[SIZE_LONG];
	uint8_t_array_to_long(baude_rate_array);
	for (uint8_t i=0; i<SIZE_LONG; i++)
	{
		this->update(POS_BAUDE_RATE+i, baude_rate_array[i]);
	}
}


void Memory::set_name(uint8_t channel, char name[])
{
	
}

char Memory::get_module_name()
{
	return this->read(POS_MODULE_NAME);
}

long Memory::get_production_time()
{
	uint8_t production_time_array[SIZE_LONG];
	for (uint8_t i=0; i<SIZE_LONG; i++)
	{
		production_time_array[i] = this->read(POS_MODULE_PRODUCTION_TIME+i);
	}
	return uint8_t_array_to_long(production_time_array);
}

long Memory::get_baud_rate()
{
	uint8_t baud_rate[SIZE_LONG];
	for (uint8_t i=0; i<SIZE_LONG; i++)
	{
		baud_rate[i] = this->read(POS_BAUDE_RATE+i);
	}
	return uint8_t_array_to_long(baud_rate);
}

uint8_t Memory::get_modbus_address()
{
	return this->read(POS_MODBUS_ADDRESS);
}

void Memory::get_name(uint8_t channel, char name[])
{

}


void Memory::clear_name(uint8_t channel)
{
	for (uint8_t i=0; i<SIZE_NAME; i++)
	{
		
	}
}

bool Memory::is_empty()
{
	if (this->read(POS_MODULE_NAME)==MODULE_NOT_PROGRAMMED)
	{
		return true;
	}
	return false;
}



void Memory::write(unsigned int addr, uint8_t val)
{
	eeprom_write_byte((unsigned char *) addr, val);
}

uint8_t Memory::read(unsigned int addr)
{
	return eeprom_read_byte((unsigned char *) addr);
}

////////////////PRIVATE FUNCTIONS///////////////////////////
void Memory::write_name(unsigned int position, char name[])
{
	uint8_t name_pointer = 0;
	while(name[name_pointer]!='\0')
	{
		update(position, name[name_pointer]);
		name_pointer++;
		if(name_pointer==SIZE_NAME){break;} // Osiagnieta maksymalna dlugosc napisu
	}
}

void Memory::read_name(unsigned int position, char name[])
{

}

unsigned int Memory::get_position(uint8_t channel, unsigned int (*pos_fun)(uint8_t))
{			 
		return (*pos_fun)(channel);
}

void Memory::update(unsigned int pos, uint8_t data)
{
	uint8_t old_data = this->read(pos);
	if (old_data!=data)
	{
		this->write(pos, data);
	}
}

void Memory::int_to_uint8_t_array(int num, uint8_t* array)
{
	array[0] = num >> 8;
	array[1] = num;
}

void Memory::long_to_uint8_t_array(long num, uint8_t* array)
{
	
	array[0] = num >> 24;
	array[1] = num >> 16;
	array[2] = num >> 8;
	array[3] = num;
}

unsigned int Memory::uint8_t_array_to_int(uint8_t* array)
{
	return  ((unsigned int)array[0] << 8) +
	((unsigned int)array[1]);
}

unsigned long Memory::uint8_t_array_to_long(uint8_t* array)
{
	return  (((unsigned long)array[0]) << 24) +
	(((unsigned long)array[1]) << 16) +
	(((unsigned long)array[2]) << 8) +
	((unsigned long)array[3]);
}



