#ifndef MODBUS_SLAVE_H_
#define MODBUS_SLAVE_H_

/*
 * modbusgate.h
 *
 *  Created on: 12 lut 2016
 *      Author: jan.pleszynski
 */


 #include "../utilities/io/pin/pin.h"
#include <stdint.h>

#define MAX_FIXED_BYES_IN_REQUEST 9
#define MAX_REGISTERS 64

 #define DEFAULT_MODBUS_ADDRESS 98
 #define DEFAULT_BAUDE_RATE 115200


//Transmision buffer size
#define BUFFER_SIZE 2*MAX_REGISTERS+MAX_FIXED_BYES_IN_REQUEST//maks 256 bajtow do komunikacji

class Frame
{
public:

	Frame();

	//Czyta przychodzace bajty. sprawdza czy przyszla nowa ramka
	uint8_t read_byte();

	//Sprawdza dlugosc przychodzacej ramki
	int16_t check_length(uint8_t in_byte);

	//Wyslij ramke poprzez modbus
	void send(unsigned char bufferSize);

	// Odpowiedz bledu.
	void exception_response(uint8_t function, uint8_t exception);

	//Wyliczanie sumy kontrolnej
	uint16_t calculate_CRC(uint8_t size);

public:

	//Adres sluzacy do komunikacji modbus
	uint8_t address = DEFAULT_MODBUS_ADDRESS;

	//Predkosc transmisji
	unsigned long baude_rate = DEFAULT_BAUDE_RATE;

	//Licznik przychodzacych bajtow
	uint8_t in_bytes_counter = 0;

	//Flaga mowiaca czy przyszla nowa ramka danych.
	bool is_new = false;

	//Dlugosc przychodzacej ramki
	uint16_t length = -1;

	//Bufor sluzacy do odczytywania i wysylania danych przez UART
	uint8_t data[BUFFER_SIZE] = {0};

private:

	//Konfiguracja komunikacji szeregowej
	void set_UART();

	void listen_mode();

	void send_mode();

	//Wylicz czasy opoznien do T1_5 oraz T3_5
	void calculate_time_outs(uint32_t baude_rate);

	unsigned int T1_5 = 750; // inter character time out

	unsigned int T3_5 = 1750; // frame delay

private:

	Pin *pin;
	
	long last_byte_arrival_t = 0;
};

class Modbus_base
{
	public:

	//Rejestry modbusowe
	uint16_t regs[MAX_REGISTERS] = {0};
};

class Modbus_write : public Modbus_base
{
public:

	void populate_regs_with_frame(uint8_t frame[], uint8_t no_of_registers);

	//Nowy pakiet czekajacy na osbluzenie
	volatile bool new_packet_pending = false;
};

class Modbus_write_reg : public Modbus_write
{
public:
	Modbus_write_reg();

	//Zapisuje rejestry modbus
	void write_holding_regs();

private:
};

extern Frame *frame;
extern  Modbus_write_reg *write_reg;


#endif //MODBUS_SLAVE_H_