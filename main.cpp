/*
 * led_light.cpp
 *
 * Created: 2016-03-05 17:40:24
 * Author : jan.pleszynski
 */ 

#include <avr/io.h>
#include "led/led.h"
#include "communication/modbus_slave.h"
#include "settings.h"
Memory mem;

int main(void)
{
	
	frame = new Frame;
	write_reg = new Modbus_write_reg;
	clock.start();
    /* Replace with your application code */
    while (1) 
    {
		if (write_reg->new_packet_pending)
		{
				uint8_t reg_counter = 0;
				for(uint8_t led_num=0; led_num<NUM_OF_CHANNELS; led_num++)
				{
					leds[led_num]->set_desired_pwm(write_reg->regs[reg_counter]);
					reg_counter++;
					leds[led_num]->set_dim_time(write_reg->regs[reg_counter]);
					reg_counter++;
				}
			write_reg->new_packet_pending = false;
		}
    }
}

