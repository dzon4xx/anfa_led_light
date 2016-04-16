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
#include "utilities/clock/clock.h"
#include "utilities/misc/misc.h"
Memory mem;

#define MIN_POWER 0
#define MAX_POWER 100

#define MIN_PWM 0
#define MAX_PWM 100

#define MIN_DIM_PERIOD 0
#define MAX_DIM_PERIOD 65355

#define NUM_OF_DEBUG_VAR 3
void debug()
{
	for (uint8_t i=0; i<NUM_OF_CHANNELS; i++)
	{
		led *crnt_led = leds[i];
		read_reg->regs[i*NUM_OF_DEBUG_VAR] = crnt_led->actual_pwm;
		read_reg->regs[i*NUM_OF_DEBUG_VAR+1] = crnt_led->incr_power;
		read_reg->regs[i*NUM_OF_DEBUG_VAR+2] = crnt_led->incr_time;
		
	}
}

int main(void)
{
	
	leds[0] = new led(1);
	leds[1] = new led(2);
	leds[2] = new led(3);
	
	frame = new Frame;
	write_reg = new Modbus_write_reg;
	read_reg  = new Modbus_read_reg;
	
	led::init_pwms();
	clock = new Clock;
	clock->start();
    /* Replace with your application code */
    while (1) 
    {
		if (write_reg->new_packet_pending)
		{
				uint8_t reg_counter = 0;
				for(uint8_t led_num=0; led_num<NUM_OF_CHANNELS; led_num++)
				{
					uint8_t desired_pwm = constrain(write_reg->regs[reg_counter], MIN_POWER, MAX_POWER)*2.55;
					//uint8_t desired_pwm = map(desired_power, MIN_POWER, MAX_POWER, MIN_PWM, MAX_PWM);
					leds[led_num]->set_desired_pwm(desired_pwm);
					reg_counter++;
					uint16_t dim_time = write_reg->regs[reg_counter];
					leds[led_num]->set_dim_time(dim_time);
					reg_counter++;
				}
			write_reg->new_packet_pending = false;
		}
		debug();
    }
}

