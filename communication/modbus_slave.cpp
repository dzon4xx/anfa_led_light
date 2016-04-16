/*
 * modbusgate.cpp
 *
 *  Created on: 12 lut 2016
 *      Author: jan.pleszynski
 */

#include "modbus_slave.h"

#include "../utilities/clock/clock.h"
#include "../utilities/io/pin/pin.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>


//Read  registers function 3
#define FUN_READ_REGS 3
#define READ_REGS_REQUEST_BYTES_NUM 8

//Read input registers function 4
#define FUN_READ_COILS 4
#define READ_COILS_REQUEST_BYTES_NUM 8

//Write coils function 15
#define FUN_WRITE_COILS 15
#define WRITE_COILS_REQUEST_BYTES_NUM 9

//Write  registers Function 16
#define FUN_WRITE_REGS 16
#define WRITE_REGS_REQUEST_BYTES_NUM 9

//Frame request/response
#define POS_SLAVE_ADDRESS 0
#define POS_FUNCTION 1
//Frame request/response

//Frame request
#define POS_REQUEST_STARTING_ADDR_HI 2
#define POS_REQUEST_STARTING_ADDR_LO 3
#define POS_REQUEST_QUANTITY_OF_REG_HI 4
#define POS_REQUEST_QUANTITY_OF_REG_LO 5
//Frame request function 15 anf 16
#define POS_REQUEST_BYTE_COUNT 6
//Frame request

//Frame response
//Frame request function 3 and 4
#define POS_RESPONSE_BYTE_COUNT 2
//
#define POS_RESPONSE_ERROR_CHECK_LO 6
#define POS_RESPONSE_ERROR_CHECK_HI 7
//Frame response

//Frame exception
#define POS_EXCEPTION 2
//Frame exception

//Exceptions
#define EXCEPTION_ILLEGAL_FUNCTION 1
#define EXCEPTION_ILLEGAL_DATA_ADDR 2
#define EXCEPTION_ILLEGAL_DATA_VAL 3
#define EXCEPTION_SLAVE_DEVICE_BUSY 6
#define EXCEPTION_CRC_ERROR 8
//Exceptions

//Link layers settings settings
#define MS_BETWEEN_FRAMES 5
#define BAUD_PRESCALE (((( F_CPU / 16) + ( DEFAULT_BAUDE_RATE / 2) ) / ( DEFAULT_BAUDE_RATE )) - 1)

 //Link layers settings settings
        
      
 Frame::Frame()
{
	 this->pin = new Out_pin('c', 2);
 	 this->listen_mode();
 	 this->calculate_time_outs(DEFAULT_BAUDE_RATE);
 	 this->set_UART();
 }

uint8_t Frame::read_byte()
{
	 uint8_t in_byte = UDR0;
	 uint32_t byte_arrival_t = clock->get_ms();		// Czas nadejscia bajtu wyrazony w ilosci zliczen timera 1
	 if (byte_arrival_t-this->last_byte_arrival_t>MS_BETWEEN_FRAMES)
	 {
		 this->is_new = true;
	 }
	 this->last_byte_arrival_t = byte_arrival_t;

	 return in_byte;
}

int16_t Frame::check_length(uint8_t in_byte)
{
	int16_t frame_length = -1;	// dlugosc ramki nie zdefiniowana
	 if (this->in_bytes_counter == POS_FUNCTION)	//Jesli przyszedl bajt mowiacy o funkcji
	 {
		 uint8_t modbus_fun = in_byte;
		 if(modbus_fun == FUN_READ_REGS){this->length = READ_REGS_REQUEST_BYTES_NUM;}
	 }
	 if (this->in_bytes_counter == POS_REQUEST_BYTE_COUNT)	//Jesli przyszedl bajt mowiacy o funkcji
	 {
		 uint8_t modbus_fun =  this->data[POS_FUNCTION];
		 if(modbus_fun == FUN_WRITE_REGS)
		 {
			 uint8_t byte_count = in_byte;
			 this->length = WRITE_REGS_REQUEST_BYTES_NUM + byte_count;
		 }	//header + payload +crc
		 else if(this->length == -1)	//Bledna funkcja
		 {
			 this->length = this->in_bytes_counter + 1;	//dzieki temu podstawieniu program wejdzie do modbus update gdzie zwroci kod blednej funkcji
		 }
	 }

	 return frame_length;
}

void Frame::send(uint8_t size)
{
	  if (size >= BUFFER_SIZE){size = BUFFER_SIZE;}
	  this->send_mode();
	  for (uint8_t i = 0; i < size; i++)
	  {

		UDR0 = this->data[i];

		for (unsigned int i=0; i<this->T1_5; i++)
		{
			  	_delay_us(1);
		}	  	
	  }
	  this->listen_mode();
}

void Frame::exception_response(uint8_t function, uint8_t exception)
{
	 this->data[POS_SLAVE_ADDRESS] = this->address;
	 this->data[POS_FUNCTION] = (function | 0x80); // set MSB bit high, informs the master of an exception
	 this->data[POS_EXCEPTION] = exception;
	 uint16_t crc16 = this->calculate_CRC(3); // ID, function|0x80, exception code
	 this->data[3] = crc16 >> 8;
	 this->data[4] = crc16 & 0xFF;
	 // exception response is always 5 bytes
	 // ID, function + 0x80, exception code, 2 bytes crc
	 this->send(5);
}

void Frame::set_UART()
{
	UCSR0B = (1 << RXEN0) | (1 << TXEN0); // Turn on the transmission and reception circuitry
	UCSR0C = (1 << UMSEL01) | (1 << UCSZ00) | (1 << UCSZ01); // Use 8- bit character sizes
	UBRR0H = (BAUD_PRESCALE>>8);
	UBRR0L = (BAUD_PRESCALE);
	UCSR0B |= (1 << RXCIE0);	//Zezwolenie na przerwania od UART
}

void Frame::listen_mode()
{
	pin->low();
}

void Frame::send_mode()
{
	pin->high();
}

void Frame::calculate_time_outs(uint32_t baude_rate)
{
	 if (baude_rate > 19200)
	 {
		 this->T1_5 = 750;
		 this->T3_5 = 1750;
	 }
	 else
	 {
		 this->T1_5 = 15000000/baude_rate; // 1T * 1.5 = T1.5
		 this->T3_5 = 35000000/baude_rate; // 1T * 3.5 = T3.5
	 }
}

uint16_t Frame::calculate_CRC(uint8_t size)
{	 
	unsigned int temp, temp2, flag;
	temp = 0xFFFF;
	for (unsigned char i = 0; i < size; i++)
	{
		temp = temp ^ this->data[i];
		for (unsigned char j = 1; j <= 8; j++)
		{
			flag = temp & 0x0001;
			temp >>= 1;
			if (flag)
			temp ^= 0xA001;
		}
	}
	// Reverse byte order.
	temp2 = temp >> 8;
	temp = (temp << 8) | temp2;
	temp &= 0xFFFF;
	// the returned value is already swapped
	// crcLo byte is first & crcHi byte is last
	return temp;	 
}




void Modbus_write::populate_regs_with_frame(uint8_t data[], uint8_t no_of_registers)
 {
	 uint8_t frame_counter = 0;
	 for(uint8_t reg_num=0; reg_num<no_of_registers; reg_num++)
	 {
		 uint16_t high_byte = data[frame_counter]<<8;
		 uint16_t low_byte  = data[frame_counter+1];
		 this->regs[reg_num] = high_byte+low_byte;
		 frame_counter+=2;
	 }
 }



Modbus_write_reg::Modbus_write_reg(){}

void Modbus_write_reg::write_holding_regs()
{
	uint8_t *data = frame->data;
	uint8_t num_of_bytes = data[POS_REQUEST_BYTE_COUNT];
	unsigned int error_check_lo_index = (POS_REQUEST_BYTE_COUNT+1)+num_of_bytes;
	unsigned int error_check_hi_index = error_check_lo_index+1;
	unsigned int crc = ((data[error_check_lo_index] << 8) | data[error_check_hi_index]); // combine the crc Low & High bytes
	if ( frame->calculate_CRC(error_check_lo_index) == crc) // if the calculated crc matches the recieved crc continue
	{
		unsigned int starting_address = ((data[POS_REQUEST_STARTING_ADDR_HI] << 8) | data[POS_REQUEST_STARTING_ADDR_LO]); // combine the starting address bytes
		unsigned int no_of_registers = ((data[POS_REQUEST_QUANTITY_OF_REG_HI] << 8) | data[POS_REQUEST_QUANTITY_OF_REG_LO]); // combine the number of register bytes
		unsigned int max_data = starting_address + no_of_registers;

		if (starting_address < no_of_registers) // check exception 2 ILLEGAL DATA ADDRESS
		{
			if (max_data <= MAX_REGISTERS) // check exception 3 ILLEGAL DATA VALUE
			{

				this->populate_regs_with_frame(data+7, no_of_registers);	//uzywaj ramki od 7 pozycji

				unsigned int crc16 = frame->calculate_CRC(6); 	// only the first 6 bytes are used for CRC calculation
				data[6] = crc16 >> 8; // split crc into 2 bytes
				data[7] = crc16 & 0xFF;

				frame->send(8); // a function 16 response is an echo of the first 6 bytes from the request + 2 crc bytes

				this->new_packet_pending = true;
			}
			else{frame->exception_response(FUN_WRITE_REGS, EXCEPTION_ILLEGAL_DATA_VAL);} // exception 3 ILLEGAL DATA VALUE
		}
		else {frame->exception_response(FUN_WRITE_REGS, EXCEPTION_ILLEGAL_DATA_ADDR);} // exception 2 ILLEGAL DATA ADDRESS
	}
	else{frame->exception_response(FUN_WRITE_REGS, EXCEPTION_CRC_ERROR);} // exception 1 ILLEGAL FUNCTION
}



void Modbus_read::populate_frame_with_regs(uint8_t no_of_registers)
{
	uint8_t *data = frame->data;
	uint8_t index = 3; // PDU starts at the 4th byte
	unsigned int temp;
	for (uint8_t reg_num = starting_reg; reg_num < max_reg; reg_num++)
	{
		temp = this->regs[reg_num];
		data[index] = temp >> 8; // split the register into 2 bytes
		index++;
		data[index] = temp & 0xFF;
		index++;
	}
}



Modbus_read_reg::Modbus_read_reg(){}

void Modbus_read_reg::read_holding_regs()
{
	uint8_t *data = frame->data;
	unsigned int crc = ((data[POS_RESPONSE_ERROR_CHECK_LO] << 8) | data[POS_RESPONSE_ERROR_CHECK_HI]); // combine the crc Low & High bytes
	if (frame->calculate_CRC(POS_RESPONSE_ERROR_CHECK_LO) == crc) // if the calculated crc matches the recieved crc continue
	{
		starting_reg = ((data[POS_REQUEST_STARTING_ADDR_HI] << 8) | data[POS_REQUEST_STARTING_ADDR_LO]); // combine the starting address bytes
		unsigned int no_of_registers = ((data[POS_REQUEST_QUANTITY_OF_REG_HI] << 8) | data[POS_REQUEST_QUANTITY_OF_REG_LO]); // combine the number of register bytes
		max_reg = starting_reg + no_of_registers;

		if (starting_reg < no_of_registers) // check exception 2 ILLEGAL DATA ADDRESS
		{
			if (max_reg <= MAX_REGISTERS) // check exception 3 ILLEGAL DATA VALUE
			{
				data[POS_RESPONSE_BYTE_COUNT] = no_of_registers*2;
				this->populate_frame_with_regs(no_of_registers);
				  
				uint8_t response_frame_size = 5 + no_of_registers*2;	// 5 - slave addr, fun, byte_count, crc_lo, crc_hi
				uint16_t crc16 = frame->calculate_CRC(response_frame_size - 2);	//Wylicz sume kontrolna
				  
				data[response_frame_size - 2] = crc16 >> 8; // split crc into 2 bytes
				data[response_frame_size - 1] = crc16 & 0xFF;

				frame->send(response_frame_size);
			}
			else{frame->exception_response(FUN_READ_REGS, EXCEPTION_ILLEGAL_DATA_VAL);} // exception 3 ILLEGAL DATA VALUE
		}
		else {frame->exception_response(FUN_READ_REGS, EXCEPTION_ILLEGAL_DATA_ADDR);} // exception 2 ILLEGAL DATA ADDRESS
	}
	else{frame->exception_response(FUN_READ_REGS, EXCEPTION_CRC_ERROR);} // exception 1 ILLEGAL FUNCTION
}

Frame *frame;
Modbus_write_reg *write_reg;
Modbus_read_reg *read_reg;

 ISR(USART_RX_vect)
{
	 uint8_t in_byte = frame->read_byte();

	 if(frame->is_new)
	 {
		 frame->check_length(in_byte);

		 frame->data[frame->in_bytes_counter] = in_byte; // zapisuj bajty zapytania
		 frame->in_bytes_counter++;

		 if (frame->in_bytes_counter==frame->length)
		 {
			 frame->in_bytes_counter=0;
			 frame->is_new = false;
			 frame->length = -1;
			 if (frame->data[POS_SLAVE_ADDRESS] == frame->address)	// Jesli zapytanie pod dobry adres
			 {
				 uint8_t function_num = frame->data[POS_FUNCTION];

				 if(function_num == FUN_WRITE_REGS)
				 {
				 	 write_reg->write_holding_regs();
				 }
				 else if(function_num == FUN_READ_REGS)
				 {
				 	 read_reg->read_holding_regs();
				 }
				 else
				 {
					 frame->exception_response(function_num, EXCEPTION_ILLEGAL_FUNCTION);
				 }
			 }
		 }
	 }	 
 }
 



