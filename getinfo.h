#ifndef GETINFO_H
#define GETINFO_H

#include "shared.h"


#ifdef RAMSUMMARY
extern json_object *get_mem_info();
#else
extern void get_mem_info(json_object *output[3]);
#endif

extern json_object *get_time();
extern json_object *get_battery(char *battery);
extern json_object *get_fs(char *fs);

#endif
