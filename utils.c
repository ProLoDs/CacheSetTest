/*
 * utils.c
 *
 *  Created on: 29.05.2017
 *      Author: pro
 */

#include "utils.h"

int in_same_cache_setl2(uint64_t virt1, uint64_t virt2)
{
	//If L2 uses simple addressing, cache sets might be seen
	//as 512 cache sets * 64byte cache lines. Thus L2 might use
	//the bottom 15 virtual address bits to determine L2 CS.
	return ((virt1 & SETMASK) == (virt2 & SETMASK));
}


uint64_t get_cacheset_identifier(uint64_t  addr)
{
	return (addr & SETMASK);
}


uint64_t rdtsc() {
  uint64_t a, d;
  asm volatile ("mfence");
  asm volatile ("rdtsc" : "=a" (a), "=d" (d));
  a = (d<<32) | a;
  asm volatile ("mfence");
  return a;
}

