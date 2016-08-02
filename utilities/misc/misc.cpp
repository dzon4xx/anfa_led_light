 
#include "misc.h"
#include <util/delay.h>

 void * operator new(size_t size)
 {
	 return malloc(size);
 }

 void operator delete(void * ptr)
 {
	 free(ptr);
 }

void inline delay_us(unsigned int us)
{
	for (unsigned int i=0; i<us; i++)
	{
		_delay_us(1);
	}
}
