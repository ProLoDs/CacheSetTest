/*
 * utils.h
 *
 *  Created on: 29.05.2017
 *      Author: pro
 */

#ifndef UTILS_H_
#define UTILS_H_
#define MB 1024 * 1024
#define CACHELINE_SIZE 64
#define L2SHIFT 15
#define TAGSHIFT 37
#define CACHESETS 5
#define ADDR_COUNT 25 // 5 for Probe + 16 for the noise + puffer
#define PROBECOUNT 16000
#define MAX_MEASUREMENT_VALUE 2 * MB
#define MAX_NOISE_COUNT MAX_MEASUREMENT_VALUE * 4
#include <stdint.h>
#endif /* UTILS_H_ */

// Keep bit 6 to 15, purge the rest
#define SETMASK (( 1 << L2SHIFT) -1) & ~(( 1 << 6) -1)
int in_same_cache_setl2(uint64_t virt1, uint64_t virt2);
uint64_t get_cacheset_identifier(uint64_t  addr);
uint64_t rdtsc();
void init_pagemap();
uint64_t rdtsc_begin();
uint64_t rdtsc_end();
