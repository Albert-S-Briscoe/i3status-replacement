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

extern json_object *get_mem_info();

#if BATTERIES > 0
extern json_object *get_battery(char *battery);
#endif

extern json_object *get_time();
extern json_object *get_fs(char *fs);

#endif
