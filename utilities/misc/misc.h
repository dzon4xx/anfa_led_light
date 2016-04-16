 #ifndef MISC_H_
 #define MISC_H_
 
 #include <stdlib.h>

//#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

void inline delay_us(unsigned int us);

void * operator new(size_t size);

void operator delete(void * ptr);

#endif