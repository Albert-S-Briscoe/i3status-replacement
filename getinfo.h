#ifndef GETINFO_H
#define GETINFO_H

#include "shared.h"

#ifdef SHORTBATTERY
	#define BATDIVISOR 14.0
	#define BATSTRLEN 9
#else
	#define BATDIVISOR 10.0
	#define BATSTRLEN 12
#endif

#if BATTERIES > 0
extern void get_batteries(char output[][BAT_STR_SIZE], size_t output_size);
#endif

extern void get_fs(char *fs, char* output, size_t output_size);
extern void get_mem_info(char* output, size_t output_size);
extern json_object *get_time();

#endif
