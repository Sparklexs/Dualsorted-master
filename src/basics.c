
// Basics

// #include "basics.h" included later to avoid macro recursion for malloc
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

		// Memory management
	
	void *Malloc (int n)
	
	   { void *p;
	     if (n == 0) return NULL;
	     p = (void*) malloc (n);
	     if (p == NULL)
	        { fprintf (stderr,"Could not allocate %i bytes\n",n);
	          exit(1);
	        }
	     return p;
	   }
	
	void Free (void *p)
	
	   { if (p) free (p); 
	   }
	
	void *Realloc (void *p, int n)
	
	   { if (p == NULL) return Malloc (n);
	     if (n == 0) { Free(p); return NULL; }
	     p = (void*) realloc (p,n);
	     if (p == NULL)
	        { fprintf (stderr,"Could not allocate %i bytes\n",n);
	          exit(1);
	        }
	     return p;
	   }

#include "basics.h"

        // bits needed to represent a number between 0 and n

uint bits (uint n)

   { uint b = 0;
     while (n)
	{ b++; n >>= 1; }
     return b;
   }
   

        // returns e[p..p+len-1], assuming len <= myW

uint bitread (uint *e, uint p, uint len)

   { uint answ;
     e += p/myW; p %= myW;
     answ = *e >> p;
     if (len == myW)
	  { if (p) answ |= (*(e+1)) << (myW-p);
	  }
     else { if (p+len > myW) answ |= (*(e+1)) << (myW-p);
            answ &= (1<<len)-1;
	  }
     return answ;
   }


  	// writes e[p..p+len-1] = s, len <= myW

void bitwrite (register uint *e, register uint p, 
	       register uint len, register uint s)

   { e += p/myW; p %= myW;
     if (len == myW)
	  { *e |= (*e & ((1<<p)-1)) | (s << p);
            if (!p) return;
            e++;
            *e = (*e & ~((1<<p)-1)) | (s >> (myW-p));
	  }
     else { if (p+len <= myW)
	       { *e = (*e & ~(((1<<len)-1)<<p)) | (s << p);
		 return;
	       }
	    *e = (*e & ((1<<p)-1)) | (s << p);
            e++; len -= myW-p;
            *e = (*e & ~((1<<len)-1)) | (s >> (myW-p));
	  }
   }
  	// writes e[p..p+len-1] = 0

void bitzero (register uint *e, register uint p, 
	       register uint len)

   { e += p/myW; p %= myW;
     if (p+len >= myW)
	{ *e &= ~((1<<p)-1);
	  len -= p;
	  e++; p = 0;
	}
     while (len >= myW)
	{ *e++ = 0;
	  len -= myW;
	}
     if (len > 0)
	*e &= ~(((1<<len)-1)<<p);
   }
