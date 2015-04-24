
// Basics
#ifndef BASICSINCLUDED
#define BASICSINCLUDED

// Includes

#include <sys/types.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

// Memory management

#define malloc(n) Malloc(n)
#define free(p) Free(p)
#define realloc(p,n) Realloc(p,n)

void *Malloc(int n);
void Free(void *p);
void *Realloc(void *p, int n);

// Data types

#ifndef byte
#define byte unsigned char
#endif 

//typedef unsigned char byte;
// typedef unsigned int uint;

//typedef int bool;
//#define true 1
//#define false 0

//#define max(x,y) ((x)>(y)?(x):(y))
//#define min(x,y) ((x)<(y)?(x):(y))

// Bitstream management

//#define myW (8*sizeof(uint))
#define myW (32)

// bits needed to represent a number between 0 and n
uint bits(uint n);
// returns e[p..p+len-1], assuming len <= myW
uint bitread(uint *e, uint p, uint len);
// writes e[p..p+len-1] = s, assuming len <= myW
void bitwrite(uint *e, uint p, uint len, uint s);
// writes e[p..p+len-1] = 0, no assumption on len

/**///FARI. WITH ASSUMPTION ON LEN, OR IT CRASHES
//NOt WORKING UPON THE LIMIT OF THE STARTING uint.
void bitzero(uint *e, uint p, uint len);
// reads bit p from e
#define DSbitget(e,p) (((e)[(p)/myW] >> ((p)%myW)) & 1)
// sets bit p in e
#define DSbitset(e,p) ((e)[(p)/myW] |= (1<<((p)%myW)))
// cleans bit p in e
#define DSbitclean(e,p) ((e)[(p)/myW] &= ~(1<<((p)%myW)))

/* bitRead and bitWrite as MACROS */
// returns e[p..p+len-1], assuming len <= myW
//DSbitread (uint returned value, uint *e, uint p, uint len)
#define DSbitread(answ, v, p, len) \
   { uint *e ; \
   	 e=v;\
     e += p/myW; p %= myW; \
     answ = *e >> p; \
     if (len == myW) \
	  { if (p) answ |= (*(e+1)) << (myW-p); \
	  } \
     else { if (p+len > myW) answ |= (*(e+1)) << (myW-p); \
            answ &= (1<<len)-1; \
	  } \
   }

// writes e[p..p+len-1] = s, len <= myW
//void bitwrite (uint *e, uint p, uint len, uint s)
#define mybitwrite(v, p, len, s) \
   { uint *e ; \
   	 e=v; \
    e += p/myW; p %= myW; \
     if (len == myW) \
	  { *e |= (*e & ((1<<p)-1)) | (s << p); \
            if (p) { \
            	e++; \
            	*e = (*e & ~((1<<p)-1)) | (s >> (myW-p)); \
        	} \
	  } \
     else { if (p+len <= myW) \
	       { *e = (*e & ~(((1<<len)-1)<<p)) | (s << p); \
	       } \
	       else { \
	    	*e = (*e & ((1<<p)-1)) | (s << p); \
            e++; len -= myW-p; \
            *e = (*e & ~((1<<len)-1)) | (s >> (myW-p)); \
           } \
	  } \
   } 

#endif
