/*
 * main.c
 *
 *  Created on: 29.05.2017
 *      Author: pro
 */

#include <stdio.h>
#include <stdint.h>
#include "utils.h"
#include <inttypes.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sched.h>
volatile uint64_t * cache_sets [CACHESETS][ADDR_COUNT];

uint64_t eviction[MB * 4];


/*
 * Inits global eviction array
 *
 */
void init_evition_array()
{
	for (uint32_t idx = 0;idx < 4 * MB; idx++){
		eviction[idx] = idx;
	}
}

uint32_t init_cache_sets ()
{

	uint32_t ctr = 1;
	cache_sets[0][0] = &eviction[0];
	// Step 1 get 5 different cache sets
	for(int idx  = 1;idx < CACHESETS ; )
	{
		if(in_same_cache_setl2((uint64_t)cache_sets[idx - 1][0],(uint64_t) & eviction[ctr]))
			ctr++;
		else
			cache_sets[idx++][0] = &eviction[ctr];
	}
	printf("Pointer %p \n", (void *)get_cacheset_identifier((uint64_t)cache_sets[0][0]));
	printf("Pointer %p \n", (void *)get_cacheset_identifier((uint64_t)cache_sets[1][0]));
	printf("Pointer %p \n", (void *)get_cacheset_identifier((uint64_t)cache_sets[2][0]));
	printf("Pointer %p \n", (void *)get_cacheset_identifier((uint64_t)cache_sets[3][0]));
	printf("Pointer %p \n", (void *)get_cacheset_identifier((uint64_t)cache_sets[4][0]));
	return ctr;
}
void fill_cache_lines(uint32_t pos)
{
	for (int cacheset = 0; cacheset < CACHESETS;cacheset++ )
	{
		for (int line = 1;line < ADDR_COUNT;)
		{
			if (in_same_cache_setl2((uint64_t) &eviction[pos],(uint64_t)cache_sets[cacheset][0]))
			{
				cache_sets[cacheset][line++] = &eviction[pos];
			}
			pos++;
		}
	}
}
void print_cache_sets()
{
	for (int cacheset = 0; cacheset < CACHESETS;cacheset++ )
	{
		printf("Cache Set:%d\n",cacheset );
		for (int line = 1;line < ADDR_COUNT;line++)
		{
			printf("Pointer %p \n", (void *)get_cacheset_identifier((uint64_t)cache_sets[cacheset][line]));
		}
	}
}
void log_to_file(size_t *measurement  )
{
	FILE *f = fopen("out.bin", "w");
	if (f == NULL)
	{
	    printf("Error opening file!\n");
	    exit(1);
	}
	 fwrite((const void*) measurement,sizeof(size_t),MAX_MEASUREMENT_VALUE,f);
	 fclose(f);
	 printf("Done!\n");
}
size_t flush_reload(int cacheset,int cachelines, int flush)
{
	if (flush)
	{
		*cache_sets[cacheset][(cachelines+10) % 25];
		*cache_sets[cacheset][(cachelines+11) % 25];
		*cache_sets[cacheset][(cachelines+12) % 25];
		*cache_sets[cacheset][(cachelines+13) % 25];
		*cache_sets[cacheset][(cachelines+14) % 25];
	}
	size_t time = rdtsc();


		*cache_sets[cacheset][cachelines];
		*cache_sets[cacheset][cachelines + 1];
		*cache_sets[cacheset][cachelines + 2];
		*cache_sets[cacheset][cachelines + 3];
		*cache_sets[cacheset][cachelines + 4];

	size_t delta = rdtsc() - time;
	return delta;
}
void do_measurements(int flush)
{
	size_t * measurement =  malloc(MAX_MEASUREMENT_VALUE * sizeof(size_t) );
	int current_line= 0;
	for (int idx = 0;idx<MAX_MEASUREMENT_VALUE;idx++){

		measurement[idx] = flush_reload(idx % CACHESETS,current_line,flush);
		current_line = (current_line + 5) % ADDR_COUNT;
		if(idx % 5 == 0)
			sched_yield();
	}
	printf("Done, writing output to log\n");
	log_to_file(measurement);
}
void create_noise(int cacheset)
{
	for (int line = 0;line < ADDR_COUNT;line++){
		*cache_sets[cacheset][line]++;
	}
	for (int ctr = 0;ctr<MAX_NOISE_COUNT;ctr++)
	{
		*cache_sets[cacheset][ctr % ADDR_COUNT];

	}

}

int main(int argc, char *argv[])
{
	pid_t pid;
    init_evition_array();
    uint32_t pos = init_cache_sets();
    fill_cache_lines(pos);
    //print_cache_sets();
    pid = fork();
    if (pid == 0)
    {
    	printf("Child process\n");
    	do_measurements(1);
    	do_measurements(0);
    }
    else
    {
    	printf("Parent process\n");
    	int status;
    	while (waitpid(pid, &status, WNOHANG) == 0)
    	{
    		//create_noise(2);
    	}
    }
    printf("Done!");
    return 0;
}
