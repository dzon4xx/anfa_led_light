#ifndef EEPROM_H_
#define EEPROM_H_
/*
 * eeprom.h
 *
 * Created: 2015-11-14 19:19:23
 *  Author: jan.pleszynski
 */ 

/*SETTINGS
1					4					3				1						1
MODULE_NAME	MODULE_PRODUCTION_TIME	BAUDE_RATE	TRANSSMISION_MODE	 	DOUBLE_PRECISSION


*/
#include <stdint.h>


class Memory
{
	public:
	
	Memory();
	//Setters
	
	void restore_memory_to_default();

	//Ustawia nazwe modulu. Nazwa to jedna litera odpowiadajaca danemu modulowi.
	void set_module_name(char module_name);
	
	//Ustawia czas produkcji w sekudnach. Jest to liczba 4 bajtowa. Czas produkcji jednoznacznie okresla dane urzadzenie.
	void set_production_time(unsigned long production_time);

	//Ustawia adres modbus urzadzenia
	void set_modbus_address(uint8_t modbus_address);

	//Ustawia predkosc transmisji modbus.
	void set_baud_rate(unsigned long baude_rate);
		
	void set_name(uint8_t channel, char name[]);
	
	//Zapisz nastawe mocy
	void set_power(uint8_t power_set_point);


		
	//Getters
	char get_module_name();

	long get_production_time();

	uint8_t get_modbus_address();

	long get_baud_rate();

	void get_name(uint8_t channel, char name[]);
			
	//Wczytaj nastawê mocy
	uint8_t get_power();
				
	//Przywraca nazwe czujnika do ustawien fabrycznych
	void clear_name(uint8_t channel);
	
	//Sprawdza czy pamiec jest pusta. Pamiec kazdego nowo zaprogramowanego urzadzenia jest pusta.
	bool is_empty();
		
	
	private:
	
	void write(unsigned int addr, uint8_t val);

	uint8_t read(unsigned int addr);

	//Ustawia tablice z nazwa czujnika na wskazana pozycje	
	void write_name(unsigned int position, char name[]);
	
	//Odczytuje nazwê czujnika ze wskazanej pozycji
	void read_name(unsigned int position, char name[]);

	//Zwraca pozycje atrybutu w pamiêci dla czujnika zadeklarowanego na danym kanale
	unsigned int get_position(uint8_t channel, unsigned int (*pos_fun)(uint8_t));
		
	//Uaktualnia wartosc w pamieci. Jesli wartosc nie ulegla zmianie nie przeprowadza zapisu
	void update(unsigned int pos, uint8_t data);
	
	// Konwertuje zmienna typu int do 2 elementowej tablicy typu uint8_t
	void int_to_uint8_t_array(int num, uint8_t* array);
	
	// Konwertuje zmienna typu long do 4 elementowej tablicy typu uint8_t
	void long_to_uint8_t_array(long num, uint8_t* array);
	
	// Konwertuje 2 elementowa tablice typu uint8_t do zmiennej typu int 
	unsigned int uint8_t_array_to_int(uint8_t* array);
	
	// Konwertuje 4 elementowa tablice typu uint8_t do zmiennej typu long
	unsigned long uint8_t_array_to_long(uint8_t* array);
};



#endif /* EEPROM_H_ */