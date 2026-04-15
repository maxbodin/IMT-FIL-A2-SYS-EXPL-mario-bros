#include "random.h"

#define RAND_MAX 4294967291

unsigned long int Random::random(void)
{
   static unsigned long int random_seed = 93186752;
   static unsigned long int a = 1588635695, q = 2, r = 1117695901;

   random_seed = a*(random_seed % q) - r*(random_seed / q);
   return random_seed;
};


int Random::nextInt(int max) {
	return random() % max;
};
    
bool Random::nextBool(){
	return random() & 1;
};
