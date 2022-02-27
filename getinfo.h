#ifndef GETINFO_H
#define GETINFO_H

#include "shared.h"


extern json_object *get_mem_info();

#if BATTERIES > 0
extern json_object *get_battery(char *battery);
#endif

#ifdef BATTERYBAR
extern json_object *get_battery_bar(char *battery);
#endif

extern json_object *get_time();
extern json_object *get_fs(char *fs);

#endif
