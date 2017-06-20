/*
 * utils.c
 *
 *  Created on: 29.05.2017
 *      Author: pro
 */

#include "utils.h"
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
int g_pagemap_fd = -1;
void init_pagemap() {
	g_pagemap_fd = open("/proc/self/pagemap", O_RDONLY);
	assert(g_pagemap_fd >= 0);
}



uint64_t rdtsc() {
  uint64_t a, d;
  asm volatile ("mfence");
  asm volatile ("rdtsc" : "=a" (a), "=d" (d));
  a = (d<<32) | a;
  asm volatile ("mfence");
  return a;
}
// Extract the physical page number from a Linux /proc/PID/pagemap entry.
uint64_t frame_number_from_pagemap(uint64_t value) {
	return value & ((1ULL << 54) - 1);
}
uint64_t get_physical_addr(uint64_t virtual_addr) {
	uint64_t value;

	off_t offset = (virtual_addr / 4096) * sizeof(value);
	int got = pread(g_pagemap_fd, &value, sizeof(value), offset);
	assert(got == 8);
	// Check the "page present" flag.
	assert(value & (1ULL << 63));

	uint64_t frame_num = frame_number_from_pagemap(value);
	return (frame_num * 4096) | (virtual_addr & (4095));
}
uint64_t get_cacheset_identifier(uint64_t  addr)
{
	addr = get_physical_addr(addr);
	return (addr & SETMASK);
}
int in_same_cache_setl2(uint64_t virt1, uint64_t virt2)
{
	virt1 = get_physical_addr(virt1);
	virt2 = get_physical_addr(virt2);
	//return virt1 == virt2;
	//If L2 uses simple addressing, cache sets might be seen
	//as 512 cache sets * 64byte cache lines. Thus L2 might use
	//the bottom 15 virtual address bits to determine L2 CS.
	return ((virt1 & SETMASK) == (virt2 & SETMASK));
}

