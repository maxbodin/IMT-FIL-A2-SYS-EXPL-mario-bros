/*
 * memoire.h
 *
 *  Created on: 27 ao�t 2008
 *      Author: jmenaud
 */

#ifndef MEMOIRE_H_
#define MEMOIRE_H_
#include <drivers/Ecran.h>

void mem_setup(void * begin, int size,Ecran *ec);
void *getmem(long unsigned int nbytes);
void freemem(int address,int size);

void* operator new(long unsigned int);
void operator delete(void * object);

#endif /* MEMOIRE_H_ */
