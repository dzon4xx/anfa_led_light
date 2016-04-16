 #ifndef MISC_H_
 #define MISC_H_
 
 #include <stdlib.h>
#include <stdint.h>

#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
inline uint8_t map(uint8_t x, uint8_t in_min, uint8_t in_max, uint8_t out_min, uint8_t out_max) 
{ return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;}

void inline delay_us(unsigned int us);

void * operator new(size_t size);

void operator delete(void * ptr);

#endif